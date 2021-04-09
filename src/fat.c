#include "fat.h"
#include "sd.h"
#include "rprintf.h"
#include <stdint.h>
#include "serial.h"
#include "clibfuncs.h"
#include "string.h"

boot_sector *bs;
unsigned char bootSector[512];

unsigned char disk[16777216];

unsigned char fat_table[32*SECTOR_SIZE];
unsigned int fat_table_size;
unsigned char fatTableChars[8192][2];
fat_table_entry *fatTablePointers[8192];

unsigned int root_sector;
unsigned int root_entries_size;
unsigned int data_sector;
unsigned char rootEntries[512*32];

unsigned char rootDirectoryEntries[16384];
unsigned char rootDirectoryChars[512][32];
root_directory_entry *rootDirectoryPointers[512];


int fatInit() {
	int error;
	sd_init();
	error = sd_readblock(0, bootSector, 1);
	if (error == 0){
		esp_printf((void *) putc, "Boot sector could not be read\n");
		return 0;
	}
	bs = (boot_sector *) bootSector;
	//validate boot signature
	if (bs->boot_signature != 0xaa55) {
		esp_printf((void *) putc, "Boot signature is invalid.\n");
		return 0;
	}
	//validate fs_type - checks if first 5 characters = FAT12
	int i;
	char type[] = {'F','A','T','1','6'};
	for (i = 0; i < (sizeof(type) / sizeof(char)); i++){
		if (bs->fs_type[i] != type[i]){
			error = 0;
			break;
		}
	}
	if (bs->fs_type[5] != ' '){ //checks if fs_type has more than 5 chars, making it nonequal
		error = 0;
	}
	if (error == 0){
		esp_printf((void *) putc, "File system type invalid\n");
		return error;
	} else {
		esp_printf((void *) putc, "File system type valid\n");
	}
	int fat_table_start = bs->num_reserved_sectors;
	fat_table_size = bs->num_sectors_per_fat;
	esp_printf((void *) putc, "FAT TABLE START = %d\nFAT TABLE SIZE: %d\n", fat_table_start, fat_table_size);
	error = sd_readblock(fat_table_start, fat_table, fat_table_size); //read fat-table into fat_table char[]
	if (error == 0){
		esp_printf((void *) putc, "FAT table could not be read\n");
		return error;
	}
	//root sector = reserved sectors + hidden sectors + (num fat tables * sectors per fat) 
	root_sector = (bs->num_fat_tables * bs->num_sectors_per_fat) + bs->num_reserved_sectors + bs->num_hidden_sectors;
	data_sector = (root_sector + ((bs->num_root_dir_entries * 32) / SECTOR_SIZE));
	esp_printf((void *) putc, "OEM: ");
	for(int i = 0; i < sizeof(bs->oem_name); i++){
		esp_printf((void *) putc, "%c", bs->oem_name[i]);
	}
	esp_printf((void *) putc, "\n");
	esp_printf((void *) putc, "Bytes per sector: %d\nSectors per Cluster: %d\nNumber of FATs: %d\nFAT Size (Sectors): %d\nROOT DIR SECTOR: %d\n", bs->bytes_per_sector, bs->num_sectors_per_cluster, bs->num_fat_tables, bs->num_sectors_per_fat, root_sector);
	/*for (i = 0; i < 20; i++){
		esp_printf((void *) putc, "%x ", fat_table[i]);
	}*/
	root_entries_size = ((bs->num_root_dir_entries * 32)/SECTOR_SIZE);
	error = sd_readblock(root_sector, rootEntries, root_entries_size);
	/*for (i = 0; i < 1000; i++){
		if ((i % 16) == 0){
			esp_printf((void *) putc, "\n");
		}
		esp_printf((void *) putc, "%x ", rootEntries[i]);
	}*/
	error = sd_readblock(0, disk, bs->total_sectors);
	return 1;
}

/*
 *  Bytes per sector: 512
 *  Sectors per cluster: 4
 *  Reserved area size: 4
 *  Number of FATs: 2
 *  Max files in RDE: 512
 *  Sectors in file system: 32768
 *  Size in sectors of FAT: 32
 *  RDE Sector = 4 (reserved sectors) + 0 (hidden sectors) + (2 (FATs) * 32 (FAT size))
 *  RDE Sector = 68
 */

int initFatStructs(){
	int error = sd_readblock(root_sector, rootDirectoryEntries, root_entries_size);
	if (error == 0){
		return 0;
	}
	int i, index, count;
	index = 0; count = 0;
	for (i = 0; i < (root_entries_size * SECTOR_SIZE); i++){
		if (i != 0 && (i % 32 == 0)){
			count++;
			index = 0;
		}
		rootDirectoryChars[count][index] = rootDirectoryEntries[i];
		index++;
	}
	index = 0; count = 0;
	for (i = 0; i < (fat_table_size * SECTOR_SIZE); i++){
		if (i != 0 && (i % 2 == 0)){
			count++;
			index = 0;
		}
		fatTableChars[count][index] = fat_table[i];
		index++;
	}
	index = 0; count = 0;
	for (i = 0; i < 512; i++){
		rootDirectoryPointers[i] = (root_directory_entry *) rootDirectoryChars[i];
	}
	for (i = 0; i < 8192; i++){
		fatTablePointers[i] = (fat_table_entry *) fatTableChars[i];
	}
	return 1;
}

int fatOpen(file *fle, char *filename){
	char placementFile[20][11];
	int i, fileCount;
	for (i = 0; i < 20; i++){
		nullCharArray(placementFile[i], 11);
	}
	fileCount = pathToFileName(placementFile, filename, strlen(filename));
	char filen[8];
	char ext[3];
	int foundFile = 0;
	if (fileCount == 1){
		for (i = 0; i < 512; i++){
			root_directory_entry rde = *rootDirectoryPointers[i];
			char *tempFilename = rde.file_name;
			char *tempExtension = rde.file_extension;
			char path[11];
			extension(filen, tempFilename, 8);
			extension(ext, tempExtension, 3);
			strncpy(path, filen, 8);
			if (ext[0] != '\0'){
				strcat(path, ".");
				strcat(path, ext);
			}
			int c;
			c = strcmp(placementFile[0], path);
			if (c == 0){
				fle->rde = rde;
				fle->start_cluster = rde.cluster;
				break;
				foundFile = 1;
			}
		}
		if (foundFile != 1){
			return 0;
		}
	} else if (fileCount > 1){
		int subDirectIndex;
		for (i = 0; i < 20; i++){
			if (placementFile[i][0] == '\0'){
				subDirectIndex = i-1;
				break;
			}
		}
		root_directory_entry parentDirectory;
		int foundSubFile = 0;
		int placementFileIndex = 0;
		for (i = 0; i < 512; i++){
			root_directory_entry rde = *rootDirectoryPointers[i];
			char *tempFilename = rde.file_name;
			char *tempExtension = rde.file_extension;
			char path[11];
			extension(filen, tempFilename, 8);
			extension(ext, tempExtension, 3);
			strncpy(path, filen, 8);
			if (ext[0] != '\0'){
				strcat(path, ".");
				strcat(path, ext);
			}
			int c;
			c = strcmp(placementFile[placementFileIndex], path);
			if (c == 0){
				parentDirectory = rde;
				foundFile = 1;
				break;
			}
		}
		if (foundFile != 1){
			return 0;
		}
		placementFileIndex++;
		while ((foundSubFile != 1) && (placementFileIndex <= subDirectIndex)){
			unsigned char dataEntries[16384];
			unsigned char dataChars[512][32];
			root_directory_entry *dataPointers[512];
			int index, count;
			index = 0; count = 0;
			readFromCluster(dataEntries, parentDirectory.cluster, 32);
			for (i = 0; i < 512; i++){
				if ((i != 0) && (i % 32 == 0)){
					count++;
					index = 0;
				}
				dataChars[count][index] = dataEntries[i];
				index++;
			}
			for (i = 0; i < 512; i++){
				dataPointers[i] = (root_directory_entry *) dataChars[i];
			}
			for (i = 0; i < 512; i++){
				root_directory_entry rde = *dataPointers[i];
				char *tempFilename = rde.file_name;
				char *tempExtension = rde.file_extension;
				char p[11];
				nullCharArray(p, 11);
				extension(filen, tempFilename, 8);
				extension(ext, tempExtension, 3);
				strncpy(p, filen, 8);
				if (ext[0] != '\0'){
					strcat(p, ".");
					strcat(p, ext);
				}
				int c;
				c = strcmp(placementFile[placementFileIndex], p);
				if (c == 0){
					if (placementFileIndex == subDirectIndex){
						fle->rde = rde;
						fle->start_cluster = rde.cluster;
						foundSubFile = 1;
						return 1;
						break;
					} else {
						parentDirectory = rde;
						placementFileIndex++;
						break;
					}
				}
			}
		}
		return 0;
	}
	return 1;
}

int fat_create(char *filename){
	char placementFile[20][11];
	int i, fileCount;
	for (i = 0; i < 20; i++){
		nullCharArray(placementFile[i], 11);
	}
	fileCount = pathToFileName(placementFile, filename, strlen(filename));
	char filen[8];
	char ext[3];
	int foundFile = 0;
	int count = 0;
	root_directory_entry *pointer;
	if (fileCount == 1){
		for (i = 0; i < 512; i++){
			count++;
			root_directory_entry rde = *rootDirectoryPointers[i];
			char *tempFilename = rde.file_name;
			char *tempExtension = rde.file_extension;
			char path[11];
			extension(filen, tempFilename, 8);
			if (filen[0] == '\0'){
				pointer = rootDirectoryPointers[i];
				break;
			}
			extension(ext, tempExtension, 3);
			strncpy(path, filen, 8);
			if (ext[0] != '\0'){
				strcat(path, ".");
				strcat(path, ext);
			}
			int c;
			c = strcmp(placementFile[0], path);
			if (c == 0){
				foundFile = 1;
				break;
			}
		}
		if (foundFile == 1){
			return 1;
		}
		char tempName[8];
		char tempExtension[3];
		spaceCharArray(tempName, 8);
		nullCharArray(tempExtension, 3);
		if (hasSeparator(filename, '.', strlen(filename)) > -1){
			int index = hasSeparator(filename, '.', strlen(filename));
			int diff = charArrCpy(tempName, filename, index);
			diff++;
			for (i = diff; i < (diff + 3); i++){
				tempExtension[i - diff] = filename[i];
			}
		} else {
			charArrCpy(tempName, filename, strlen(filename));
		}
		charArrToUpper(tempName, 8);
		charArrToUpper(tempExtension, 3);
		charArrCpy(pointer->file_name, tempName, 8);
		charArrCpy(pointer->file_extension, tempExtension, 3);
		pointer->attribute = 0x20;
		for (i = 0; i < 8192; i++){
			fat_table_entry *fte = fatTablePointers[i];
			if (fte->entry == 0x0){
				pointer->cluster = i;
				fte->entry = 0xfff8;
				break;
			}
		}
		pointer->file_size = 0x0;
		return 0;
	} else if (fileCount > 1){
		int subDirectIndex;
		for (i = 0; i < 20; i++){
			if (placementFile[i][0] == '\0'){
				subDirectIndex = i-1;
				break;
			}
		}
		root_directory_entry parentDirectory;
		int foundSubFile = 0;
		int placementFileIndex = 0;
		for (i = 0; i < 512; i++){
			root_directory_entry rde = *rootDirectoryPointers[i];
			char *tempFilename = rde.file_name;
			char *tempExtension = rde.file_extension;
			char path[11];
			extension(filen, tempFilename, 8);
			extension(ext, tempExtension, 3);
			strncpy(path, filen, 8);
			if (ext[0] != '\0'){
				strcat(path, ".");
				strcat(path, ext);
			}
			int c;
			c = strcmp(placementFile[placementFileIndex], path);
			if (c == 0){
				parentDirectory = rde;
				foundFile = 1;
				break;
			}
		}
		if (foundFile != 1){
			return 1;
		}
		placementFileIndex++;
		while ((foundSubFile != 1) && (placementFileIndex <= subDirectIndex)){
			unsigned char dataEntries[16384];
			unsigned char dataChars[512][32];
			root_directory_entry *dataPointers[512];
			int index, count;
			index = 0; count = 0;
			readFromCluster(dataEntries, parentDirectory.cluster, 32);
			for (i = 0; i < 512; i++){
				if ((i != 0) && (i % 32 == 0)){
					count++;
					index = 0;
				}
				dataChars[count][index] = dataEntries[i];
				index++;
			}
			for (i = 0; i < 512; i++){
				dataPointers[i] = (root_directory_entry *) dataChars[i];
			}
			for (i = 0; i < 512; i++){
				root_directory_entry rde = *dataPointers[i];
				char *tempFilename = rde.file_name;
				char *tempExtension = rde.file_extension;
				char p[11];
				nullCharArray(p, 11);
				extension(filen, tempFilename, 8);
				extension(ext, tempExtension, 3);
				strncpy(p, filen, 8);
				if (ext[0] != '\0'){
					strcat(p, ".");
					strcat(p, ext);
				}
				int c;
				c = strcmp(placementFile[placementFileIndex], p);
				if (c == 0){
					if (placementFileIndex == subDirectIndex){
						foundSubFile = 1;
						break;
					} else {
						parentDirectory = rde;
						placementFileIndex++;
						break;
					}
				}
			}
		}
		if (foundSubFile == 1){
			return 1;
		}
		char tempName[8];
		char tempExtension[3];
		spaceCharArray(tempName, 8);
		nullCharArray(tempExtension, 3);
		if (hasSeparator(filename, '.', strlen(filename)) > -1){
			int index = hasSeparator(filename, '.', strlen(filename));
			int diff = charArrCpy(tempName, filename, index);
			diff++;
			for (i = diff; i < (diff + 3); i++){
				tempExtension[i - diff] = filename[i];
			}
		} else {
			charArrCpy(tempName, filename, strlen(filename));
		}
		charArrToUpper(tempName, 8);
		charArrToUpper(tempExtension, 3);
		charArrCpy(pointer->file_name, tempName, 8);
		charArrCpy(pointer->file_extension, tempExtension, 3);
		pointer->attribute = 0x20;
		for (i = 0; i < 8192; i++){
			fat_table_entry *fte = fatTablePointers[i];
			if (fte->entry == 0x0){
				pointer->cluster = i;
				fte->entry = 0xfff8;
				break;
			}
		}
		pointer->file_size = 0x0;
		return 0;
	}
}
	

int fatRead(char* buffer, file* fp, unsigned int length){
	uint16_t fpCluster = fp->start_cluster;
	uint16_t fatValue = fatTablePointers[fpCluster]->entry;
	int i, index;
	index = 0;
	if (fatValue >= 0xfff8){
		unsigned char data[2048];
		readFromCluster(data, fpCluster, SECTORS_PER_CLUSTER);
		for (i = 0; i < length; i++){
			buffer[i] = data[i];
		}
	} else if (fatValue < 0xfff8){
		while (fatValue < 0xfff8){
			unsigned char data[2048];
			readFromCluster(data, fpCluster, SECTORS_PER_CLUSTER);
			for (i = 0; i < 2048; i++){
				if (index == length){
					return 1;
				}
				buffer[index] = data[i];
				index++;
			}
			fpCluster = fatValue;
			fatValue = fatTablePointers[fpCluster]->entry;
		}
		unsigned char data[2048];
		readFromCluster(data, fpCluster, SECTORS_PER_CLUSTER);
		for (i = 0; i < 2048; i++){
			if (index == length){
				return 1;
			}
			buffer[index] = data[i];
			index++;
		}
	}
	return 1;
}

void readFromCluster(unsigned char data[], uint16_t clusterNum, unsigned int size){
	unsigned int dataSector = data_sector + ((clusterNum - 2) * SECTORS_PER_CLUSTER);
	sd_readblock(dataSector, data, size);
}

int pathToFileName(char fn[][11], char *path, int length){
	int i, count, index;
	count = 0;
	index = 0;
	for (i = 0; i < length; i++){
		if (isSeparator(path[i], '/') && i == 0){
			index = 0;
			continue;
		} else if (isSeparator(path[i], '/') && (i != 0)){
			fn[count][index] = '\0';
			index = 0;
			count++;
			continue;
		}
		fn[count][index] = toupper(path[i]);
		index++;
	}
	return count + 1;
}	

void nullCharArray(char arr[], int length){
	int i;
	for (i = 0; i < length; i++){
		arr[i] = '\0';
	}
}

void spaceCharArray(char arr[], int length){
	int i;
	for (i = 0; i < length; i++){
		arr[i] = ' ';
	}
}

int isSeparator(char c, char sep){
    if (c == sep){
        return 1;
    } else {
        return 0;
    }
}

int hasSeparator(char arr[], char sep, unsigned int length){
	int i;
	int ret = -1;
	for (i = 0; i < length; i++){
		if (arr[i] == sep){
			ret = i;
			break;
		}
	}
	return ret;
}

void extension(char a[], char b[], unsigned int length){
	int i;
	for (i = 0; i < length; i++){
		if (b[i] == ' '){
			a[i] = '\0';
		} else {
			a[i] = b[i];
		}
	}
}

void printCharArray(char arr[], unsigned int length){
	int i;
	for (i = 0; i < length; i++){
		esp_printf((void *) putc, "%c", arr[i]);
	}
}
