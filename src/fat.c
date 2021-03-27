#include "fat.h"
#include "sd.h"
#include "rprintf.h"
#include <stdint.h>
#include "serial.h"
#include "clibfuncs.h"
boot_sector *bs;
unsigned char bootSector[512];
unsigned char fat_table[32*SECTOR_SIZE];
unsigned int root_sector;
unsigned char rootEntries[512*32];
unsigned char fatTableChars[8192][2];
fat_table_entry *fatTablePointers[8192];
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
	int fat_table_size = bs->num_sectors_per_fat;
	esp_printf((void *) putc, "FAT TABLE START = %d\nFAT TABLE SIZE: %d\n", fat_table_start, fat_table_size);
	error = sd_readblock(fat_table_start, fat_table, fat_table_size); //read fat-table into fat_table char[]
	if (error == 0){
		esp_printf((void *) putc, "FAT table could not be read\n");
		return error;
	}
	//root sector = reserved sectors + hidden sectors + (num fat tables * sectors per fat) 
	root_sector = (bs->num_fat_tables * bs->num_sectors_per_fat) + bs->num_reserved_sectors + bs->num_hidden_sectors;
	esp_printf((void *) putc, "OEM: ");
	for(int i = 0; i < sizeof(bs->oem_name); i++){
		esp_printf((void *) putc, "%c", bs->oem_name[i]);
	}
	esp_printf((void *) putc, "\n");
	esp_printf((void *) putc, "Bytes per sector: %d\nSectors per Cluster: %d\nNumber of FATs: %d\nFAT Size (Sectors): %d\nROOT DIR SECTOR: %d\n", bs->bytes_per_sector, bs->num_sectors_per_cluster, bs->num_fat_tables, bs->num_sectors_per_fat, root_sector);
	/*for (i = 0; i < 20; i++){
		esp_printf((void *) putc, "%x ", fat_table[i]);
	}*/
	error = sd_readblock(68, rootEntries, 32);
	/*for (i = 0; i < 1000; i++){
		if ((i % 16) == 0){
			esp_printf((void *) putc, "\n");
		}
		esp_printf((void *) putc, "%x ", rootEntries[i]);
	}*/
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
	sd_readblock(root_sector, rootDirectoryEntries, 32);
	int i, index, count;
	index = 0; count = 0;
	for (i = 0; i < 16384; i++){
		if (i != 0 && (i % 32 == 0)){
			count++;
			index = 0;
		}
		rootDirectoryChars[count][index] = rootDirectoryEntries[i];
		index++;
	}
	index = 0; count = 0;
	for (i = 0; i < 16384; i++){
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
	for (i = 0; i < 20; i++){
		esp_printf((void *) putc, "%x\n", fatTablePointers[i]->entry);
	}
}

int fatOpen(file *fle, char *filename){
	char placementFile[20][11];
	pathToFileName(placementFile, filename, strlen(filename));
	char filen[8];
	char ext[3];
	int i;
	for (i = 0; i < 1; i++){
		esp_printf((void *) putc, "%s\n", placementFile[i]);
	}
}

int fatRead(char* buffer, file* fp){
	
}

void pathToFileName(char fn[][11], char *path, int length){
	int i, count, index;
	count = 0;
	index = 0;
	for (i = 0; i < length; i++){
		if (isSeparator(path[i], '/')){
			count++;
			index = 0;
			continue;
		}
		fn[count][index] = path[i];
		index++;
	}
}

void nullCharArray(char arr[]){
	int i;
	for (i = 0; i < sizeof(arr); i++){
		arr[i] = '\0';
	}
}

int isSeparator(char c, char sep){
    if (c == sep){
        return 1;
    } else {
        return 0;
    }
}

void extension(char a[], char b[], unsigned int length){
	int i;
	for (i = 0; i < length; i++){
		a[i] = b[i];
	}
}
