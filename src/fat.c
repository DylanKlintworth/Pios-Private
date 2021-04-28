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
unsigned int fat_table_start;
unsigned char fatTableChars[8192][2];
fat_table_entry *fatTablePointers[8192];

unsigned int root_sector;
unsigned int root_entries_size;
unsigned int data_sector;
unsigned char rootEntries[512*32];

unsigned char rootDirectoryEntries[16384];
unsigned char rootDirectoryChars[512][32];
root_directory_entry *rootDirectoryPointers[512];

unsigned char dataEntries[2048];
unsigned char dataChars[64][32];
root_directory_entry *dataPointers[64];

root_directory_entry rootEntry;

int initFat() {
	int error;
	sd_init();
	esp_printf((void *) putc, "\n----------------------------\nInitializing FAT Filesystem\n----------------------------\n");
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
	/*
	int fat_table_start = bs->num_reserved_sectors;
	fat_table_size = bs->num_sectors_per_fat;
	esp_printf((void *) putc, "FAT TABLE START = %d\nFAT TABLE SIZE: %d\n", fat_table_start, fat_table_size);
	error = sd_readblock(fat_table_start, fat_table, fat_table_size); //read fat-table into fat_table char[]
	if (error == 0){
		esp_printf((void *) putc, "FAT table could not be read\n");
		return error;
	}
	*/
	//root sector = reserved sectors + hidden sectors + (num fat tables * sectors per fat) 
	root_sector = (bs->num_fat_tables * bs->num_sectors_per_fat) + bs->num_reserved_sectors + bs->num_hidden_sectors;
	data_sector = (root_sector + ((bs->num_root_dir_entries * 32) / SECTOR_SIZE));
	esp_printf((void *) putc, "OEM: ");
	for(int i = 0; i < sizeof(bs->oem_name); i++){
		esp_printf((void *) putc, "%c", bs->oem_name[i]);
	}
	esp_printf((void *) putc, "\n");
	esp_printf((void *) putc, "Bytes per sector: %d\nSectors per Cluster: %d\nNumber of FATs: %d\nFAT Size (Sectors): %d\nRoot directory sector: %d\n", bs->bytes_per_sector, bs->num_sectors_per_cluster, bs->num_fat_tables, bs->num_sectors_per_fat, root_sector);
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
	rootEntry.cluster = root_sector;
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
	//int error = sd_readblock(root_sector, rootDirectoryEntries, root_entries_size);
	int error = charArrCpyIndex((char *) rootDirectoryEntries, (char *) disk, (root_sector * SECTOR_SIZE), ((root_sector * SECTOR_SIZE) + 16384));
	if (error == 0){
		return 0;
	}
	fat_table_start = bs->num_reserved_sectors + bs->num_hidden_sectors;
	fat_table_size = bs->num_sectors_per_fat;
	//esp_printf((void *) putc, "FAT TABLE START = %d\nFAT TABLE SIZE: %d\n", fat_table_start, fat_table_size);
	error = charArrCpyIndex((char *) fat_table, (char *) disk, (fat_table_start * SECTOR_SIZE), ((fat_table_start * SECTOR_SIZE) + (fat_table_size * SECTOR_SIZE))); //read fat-table into fat_table char[]
	if (error == 0){
		esp_printf((void *) putc, "FAT table could not be read\n");
		return error;
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
/*
	Return 0 if file has been found and opened.
	Return 1 if directory could not be found in root directory.
	Return 2 if subdirectory could not be found.
	Return 3 if path given does not contain a valid path
*/
int fatOpen(file *fle, char *filename){
	initFatStructs();
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
				fle->parentRde = rootEntry;
				fle->rde = rde;
				fle->start_cluster = rde.cluster;
				foundFile = 1;
				break;
			}
		}
		if (foundFile != 1){
			return 1;
		}
		writeFatTable();
		writeRootDirectory();
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
			int index, count;
			index = 0; count = 0;
			readFromCluster(dataEntries, parentDirectory.cluster);
			for (i = 0; i < 2048; i++){
				if ((i != 0) && (i % 32 == 0)){
					count++;
					index = 0;
				}
				dataChars[count][index] = dataEntries[i];
				index++;
			}
			for (i = 0; i < 64; i++){
				dataPointers[i] = (root_directory_entry *) dataChars[i];
			}
			for (i = 0; i < 64; i++){
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
						fle->parentRde = parentDirectory;
						fle->rde = rde;
						fle->start_cluster = rde.cluster;
						foundSubFile = 1;
						break;
					} else {
						parentDirectory = rde;
						placementFileIndex++;
						break;
					}
				}
			}
			if (i == 64){
				return 2;
			}
		}
		if (foundSubFile != 1){
			return 2;
		}
		writeFatTable();
		writeRootDirectory();
		return 0;
	}
	return 3;
}
/*
	Return 0 if file has been created.
	Return 1 if file is found in root directory.
	Return 2 if subdirectory could not be found.
	Return 3 if file found in subdirectory
	Return 4 if file could not be created for specified path.
	Return 5 if path given is not a valid path
*/
int fatCreate(char *filename, unsigned int isDirectory){
	initFatStructs();
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
		if (!(isDirectory)){
			pointer->attribute = 0x20;
		} else {
			pointer->attribute = 0x10;
		}
		for (i = 0; i < 8192; i++){
			fat_table_entry *fte = fatTablePointers[i];
			if (fte->entry == 0x0){
				pointer->cluster = i;
				fte->entry = 0xfff8;
				break;
			}
		}
		/*
		if (isDirectory){
			unsigned char tempEntries[2048];
			unsigned char tempChars[64][32];
			root_directory_entry *tempPointers[64];
			readFromCluster(tempEntries, pointer->cluster);
			int index, count;
			index = 0; count = 0;
			for (i = 0; i < 2048; i++){
				if ((i != 0) && (i % 32 == 0)){
					count++;
					index = 0;
				}
				tempChars[count][index] = tempEntries[i];
				index++;
			}
			for (i = 0; i < 64; i++){
				tempPointers[i] = (root_directory_entry *) tempChars[i];
			}
			spaceCharArray(tempPointers[0]->file_name, 8);
			spaceCharArray(tempPointers[1]->file_name, 8);
			strcpy(tempPointers[0]->file_name, ".");
			strcpy(tempPointers[1]->file_name, "..");
			tempPointers[0]->attribute = 0x10;
			tempPointers[1]->attribute = 0x10;
			writeDataEntries(*pointer, tempPointers);
		}*/
		pointer->file_size = 0x0;
		writeFatTable();
		writeRootDirectory();
		return 0;
	} else if (fileCount > 1){
		//tracks index of ending file e.g in path "/alan/alan.txt" subdirect index = 1 for "alan.txt"
		int subDirectIndex; 
		for (i = 0; i < 20; i++){
			if (placementFile[i][0] == '\0'){
				subDirectIndex = i-1;
				break;
			}
		}
		root_directory_entry parentDirectory; //used to iterate through directory structure
		int foundSubFile = 0; //the ending file has been found at subDirectIndex
		int createFile = 0; //if in correct parent directory, this turns to 0 if pointer is a null file
		int placementFileIndex = 0; //used to iterate through path ["alan", "alan.txt"]
		/* iterate through root directory to find next directory
		 * if next directory could not be found, return 1
		 * if found next directory, set parentDirectory to the directory found
		 * */
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
		placementFileIndex++; //next part of file path
		//found parent directory
		/*
		 * While placementFileIndex isn't the subdirect index, keep looping
		 * if subFile is found, loop exits as file exists already
		 * if file does not exist at correct subIndex, set createFile to true and break
		 * */
		while ((foundSubFile != 1) && (placementFileIndex <= subDirectIndex) && (createFile != 1)){
			int index, count;
			index = 0; count = 0;
			//read parent directory into dataEntries char array
			readFromCluster(dataEntries, parentDirectory.cluster);
			//create 512 strings consisting of parent directory files
			for (i = 0; i < 2048; i++){
				if ((i != 0) && (i % 32 == 0)){
					count++;
					index = 0;
				}
				dataChars[count][index] = dataEntries[i];
				index++;
			}
			//make pointers to those files
			for (i = 0; i < 64; i++){
				dataPointers[i] = (root_directory_entry *) dataChars[i];
			}
			//loop through each file within parent directory
			for (i = 0; i < 64; i++){
				root_directory_entry rde = *dataPointers[i];
				char *tempFilename = rde.file_name;
				char *tempExtension = rde.file_extension;
				char p[11]; 
				nullCharArray(p, 11);
				extension(filen, tempFilename, 8);
				//if file name is null and correct index, set pointer
				if (filen[0] == '\0' && placementFileIndex == subDirectIndex){
					pointer = dataPointers[i];
					createFile = 1;
					break;
				}
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
						//file already exists
						foundSubFile = 1;
						break;
					} else {
						//directory found, proceed to next directory
						parentDirectory = rde;
						placementFileIndex++;
						break;
					}
				}
			}
			//if looped through all entries and none are equal, return 1
			if (i == 64){
				return 2;
			}
		}
		if (foundSubFile == 1){
			return 3;
		}
		if (createFile != 1){
			return 4;
		}
		char tempName[8];
		char tempExtension[3];
		spaceCharArray(tempName, 8);
		nullCharArray(tempExtension, 3);
		if (hasSeparator(placementFile[placementFileIndex], '.', strlen(placementFile[placementFileIndex])) > -1){
			int index = hasSeparator(placementFile[placementFileIndex], '.', strlen(placementFile[placementFileIndex]));
			int diff = charArrCpy(tempName, placementFile[placementFileIndex], index);
			diff++;
			for (i = diff; i < (diff + 3); i++){
				tempExtension[i - diff] = placementFile[placementFileIndex][i];
			}
		} else {
			charArrCpy(tempName, placementFile[placementFileIndex], strlen(placementFile[placementFileIndex]));
		}
		charArrToUpper(tempName, 8);
		charArrToUpper(tempExtension, 3);
		charArrCpy(pointer->file_name, tempName, 8);
		charArrCpy(pointer->file_extension, tempExtension, 3);
		if (!(isDirectory)){
			pointer->attribute = 0x20;
		} else {
			pointer->attribute = 0x10;
		}
		for (i = 0; i < 8192; i++){
			fat_table_entry *fte = fatTablePointers[i];
			if (fte->entry == 0x0){
				pointer->cluster = i;
				fte->entry = 0xfff8;
				break;
			}
		}
		/*if (isDirectory){
			unsigned char tempEntries[2048];
			unsigned char tempChars[64][32];
			root_directory_entry *tempPointers[64];
			readFromCluster(tempEntries, pointer->cluster);
			int index, count;
			index = 0; count = 0;
			for (i = 0; i < 2048; i++){
				if ((i != 0) && (i % 32 == 0)){
					count++;
					index = 0;
				}
				tempChars[count][index] = tempEntries[i];
				index++;
			}
			for (i = 0; i < 64; i++){
				tempPointers[i] = (root_directory_entry *) tempChars[i];
			}
			spaceCharArray(tempPointers[0]->file_name, 8);
			spaceCharArray(tempPointers[1]->file_name, 8);
			strcpy(tempPointers[0]->file_name, ".");
			strcpy(tempPointers[1]->file_name, "..");
			tempPointers[0]->attribute = 0x10;
			tempPointers[1]->attribute = 0x10;
			writeDataEntries(*pointer, tempPointers);
		}*/
		pointer->file_size = 0x0;
		writeDataEntries(parentDirectory, dataPointers);
		writeFatTable();
		writeRootDirectory();
		return 0;
	}
	return 5;
}
	
int writeFileLength(root_directory_entry parentDirectory, root_directory_entry rde, unsigned int size){
	unsigned char tempEntries[2048];
	unsigned char tempChars[64][32];
	root_directory_entry* tempPointers[64];
	readFromCluster(tempEntries, parentDirectory.cluster);
	char rdeName[11];
	rootDirectoryToFilename(rde, rdeName);
	int i, count, index;
	count = 0; index = 0;
	for (i = 0; i < CLUSTER_SIZE; i++){
		if ((i != 0) && ((i % 32) == 0)){
			count++;
			index = 0;
		}
		tempChars[count][index] = tempEntries[i];
	}
	for (i = 0; i < 64; i++){
		tempPointers[i] = (root_directory_entry *) tempChars[i];
	}
	for (i = 0; i < 64; i++){
		root_directory_entry *tempRde = tempPointers[i];
		if (tempRde->file_name[0] == '\0'){
			continue;
		}
		char buffer[11];
		rootDirectoryToFilename(*tempRde, buffer);
		if (strcmp(buffer, rdeName) == 0){
			tempRde->file_size = size;
			break;
		}
	}
	writeDataEntries(parentDirectory, tempPointers);
	return 0;
}

int fatRead(char* buffer, file* fp, unsigned int length){
	initFatStructs();
	uint16_t fpCluster = fp->start_cluster;
	uint16_t fatValue = fatTablePointers[fpCluster]->entry;
	int i, index;
	index = 0;
	if (fatValue >= 0xfff8){
		unsigned char data[2048];
		readFromCluster(data, fpCluster);
		for (i = 0; i < length; i++){
			buffer[i] = data[i];
		}
	} else if (fatValue < 0xfff8){
		while (fatValue < 0xfff8){
			unsigned char data[2048];
			readFromCluster(data, fpCluster);
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
		readFromCluster(data, fpCluster);
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
/*
 * Write a buffer to a file 
 */
int fatWrite(file *fp, char *buffer){
	initFatStructs();
	// cluster is the file's starting cluster
	uint16_t fpCluster = fp->start_cluster;
	//fatValue is the value of the entry at the cluster location in the FAT table
	uint16_t fatValue = fatTablePointers[fpCluster]->entry;
	unsigned int length = strlen(buffer) + 1;
	unsigned int clustersNeeded = (length / CLUSTER_SIZE) + 1;
	char sectors[clustersNeeded][2048];
	if (clustersNeeded > 1){
		while (fatValue < 0xfff8){
			char tempBuffer[2048];
			nullCharArray(tempBuffer, 2048);
			writeToCluster(tempBuffer, fpCluster, 2048);
			fatTablePointers[fpCluster]->entry = 0x0;
			fpCluster = fatValue;
			fatValue = fatTablePointers[fpCluster]->entry;
		}
		// fatValue now > 0xfff8
		int i, index, count;
		i = 0; index = 0; count = 0;
		while (i < (clustersNeeded * CLUSTER_SIZE)){
			if (((i % 2048) == 0) && (i != 0)){
				count++;
				index = 0;
			}
			sectors[count][index] = buffer[i];
			index++;
			i++;
		}
		for (i = 0; i < clustersNeeded; i++){
			if (i == 0){
				fp->start_cluster = fpCluster;
				fp->rde.cluster = fpCluster;
				int nextCluster = unallocatedFatTableIndex();
				fatTablePointers[fpCluster]->entry = nextCluster;
				char tempBuffer[2048];
				nullCharArray(tempBuffer, 2048);
				writeToCluster(tempBuffer, fpCluster, 2048);
				writeToCluster(sectors[i], fpCluster, (strlen(buffer)+1));
				fpCluster = nextCluster;
			} else if (i == (clustersNeeded-1)) {
				fatTablePointers[fpCluster]->entry = 0xfff8;
				char tempBuffer[2048];
				nullCharArray(tempBuffer, 2048);
				writeToCluster(tempBuffer, fpCluster, 2048);
				writeToCluster(sectors[i], fpCluster, (strlen(buffer)+1));
			} else {
				int nextCluster = unallocatedFatTableIndex();
				fatTablePointers[fpCluster]->entry = nextCluster;
				char tempBuffer[2048];
				nullCharArray(tempBuffer, 2048);
				writeToCluster(tempBuffer, fpCluster, 2048);
				writeToCluster(sectors[i], fpCluster, (strlen(buffer)+1));
				fpCluster = nextCluster;
			}
		}
		writeFileLength(fp->parentRde, fp->rde, (strlen(buffer) + 1));
		writeFatTable();
		writeRootDirectory();
		return 0;
	} else if (clustersNeeded == 1){
		while (fatValue < 0xfff8){
			char tempBuffer[2048];
			nullCharArray(tempBuffer, 2048);
			writeToCluster(tempBuffer, fpCluster, 2048);
			fatTablePointers[fpCluster]->entry = 0x0;
			fpCluster = fatValue;
			fatValue = fatTablePointers[fpCluster]->entry;
		}
		char tempBuffer[2048];
		nullCharArray(tempBuffer, 2048);
		writeToCluster(tempBuffer, fpCluster, 2048);
		writeToCluster(buffer, fpCluster, (strlen(buffer)+1));
		fp->start_cluster = fpCluster;
		fp->rde.cluster = fpCluster;
		writeFileLength(fp->parentRde, fp->rde, (strlen(buffer) + 1));
		fatTablePointers[fpCluster]->entry = 0xfff8;
		writeFatTable();
		writeRootDirectory();
		return 0;
	}
	return 1;
}

// Return index of fat table entry with value of 0
int unallocatedFatTableIndex(){
	int i;
	for (i = 0; i < 8192; i++){
		fat_table_entry *fte = fatTablePointers[i];
		if (fte->entry == 0x0){
			return i;
		}
	}
	return -1;
}

// Read from a specified cluster to data buffer
void readFromCluster(unsigned char data[], uint16_t clusterNum){
	unsigned int dataSector = data_sector + ((clusterNum - 2) * SECTORS_PER_CLUSTER);
	//sd_readblock(dataSector, data, size);
	charArrCpyIndex((char *) data, (char *) disk, (dataSector * SECTOR_SIZE), ((dataSector * SECTOR_SIZE) + (SECTOR_SIZE * SECTORS_PER_CLUSTER)));
}
// Write N length of a data buffer to the specified cluster
void writeToCluster(char data[], uint16_t clusterNum, unsigned int size){
	unsigned int dataSector = data_sector + ((clusterNum - 2) * SECTORS_PER_CLUSTER);
	charArrCpyIndexOpp((char *) disk, (char *) data, (dataSector * SECTOR_SIZE), ((dataSector * SECTOR_SIZE) + size));
}
// Writes the root directory to emulated disk after modification 
void writeRootDirectory(){
	char buffer[bs->num_root_dir_entries][ROOT_DIRECTORY_ENTRY_SIZE];
	char retBuff[bs->num_root_dir_entries * ROOT_DIRECTORY_ENTRY_SIZE];
	int i,j,index;
	for (i = 0; i < bs->num_root_dir_entries; i++){
		char data[32];
		memcpy(data, rootDirectoryPointers[i], sizeof(root_directory_entry));
		memcpy(buffer[i], data, 32);
	}
	index = 0;
	for (i = 0; i < bs->num_root_dir_entries; i++){
		for (j = 0; j < 32; j++){
			retBuff[index] = buffer[i][j];
			index++;
		}
	}
	charArrCpyIndexOpp((char *) disk, (char *) retBuff, (root_sector * SECTOR_SIZE), ((root_sector * SECTOR_SIZE) + 16384));
}
// Writes the FAT Table to emulated disk after modification
void writeFatTable(){
	char buffer[((fat_table_size * SECTOR_SIZE) / 2)][sizeof(fat_table_entry)];
	char retBuff[fat_table_size * SECTOR_SIZE];
	int i,j,index;
	for (i = 0; i < ((fat_table_size * SECTOR_SIZE) / 2); i++){
		char data[2];
		memcpy(data, fatTablePointers[i], sizeof(fat_table_entry));
		memcpy(buffer[i], data, 2);
	}
	index = 0;
	for (i = 0; i < (fat_table_size * SECTOR_SIZE); i++){
		for (j = 0; j < 2; j++){
			retBuff[index] = buffer[i][j];
			index++;
		}
	}
	charArrCpyIndexOpp((char *)disk, (char *) retBuff, (fat_table_start * SECTOR_SIZE), ((fat_table_start * SECTOR_SIZE) +(fat_table_size * SECTOR_SIZE)));
}
// Writes a specified directories directory entries to emulated disk
void writeDataEntries(root_directory_entry parentDirectory, root_directory_entry *entries[]){
	char buffer[64][ROOT_DIRECTORY_ENTRY_SIZE];
	char retBuff[64 * ROOT_DIRECTORY_ENTRY_SIZE];
	int i,j,index;
	for (i = 0; i < 64; i++){
		char data[32];
		memcpy(data, entries[i], sizeof(root_directory_entry));
		memcpy(buffer[i], data, 32);
	}
	index = 0;
	for (i = 0; i < 64; i++){
		for (j = 0; j < 32; j++){
			retBuff[index] = buffer[i][j];
			index++;
		}
	}
	unsigned int dataSector = data_sector + ((parentDirectory.cluster - 2) * SECTORS_PER_CLUSTER);
	charArrCpyIndexOpp((char *) disk, (char *) retBuff, (dataSector * SECTOR_SIZE), ((dataSector * SECTOR_SIZE) + 2048));
}
// Adds filenames within a path to an array of strings
// ex. "/boot/kernel8.elf" -> ["boot", "kernel8.elf"]
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
// Converts a buffer to nulls up to N length
void nullCharArray(char arr[], int length){
	int i;
	for (i = 0; i < length; i++){
		arr[i] = '\0';
	}
}
// Converts a buffer to spaces up to N length
void spaceCharArray(char arr[], int length){
	int i;
	for (i = 0; i < length; i++){
		arr[i] = ' ';
	}
}
// Checks if char is separator
int isSeparator(char c, char sep){
    if (c == sep){
        return 1;
    } else {
        return 0;
    }
}
// Returns the index of a separator char in a string, -1 is it doesn't exist
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
// Converts FAT filename/extension to string compatible format
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
// Prints a buffer
void printCharArray(char arr[], unsigned int length){
	int i;
	for (i = 0; i < length; i++){
		esp_printf((void *) putc, "%c", arr[i]);
	}
}

void rootDirectoryToFilename(root_directory_entry rde, char *buffer){
	char *tempFilename = rde.file_name;
	char *tempExtension = rde.file_extension;
	char path[11];
	char filen[8];
	char ext[3];
	extension(filen, tempFilename, 8);
	extension(ext, tempExtension, 3);
	strncpy(path, filen, 8);
	if (ext[0] != '\0'){
		strcat(path, ".");
		strcat(path, ext);
	}
	charArrCpy(buffer, path, 11);
}
