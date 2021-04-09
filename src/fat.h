
#ifndef __FAT_H__
#define __FAT_H__

#include <stdint.h>

#define CLUSTER_SIZE 2048
#define SECTOR_SIZE 512
#define SECTORS_PER_CLUSTER (CLUSTER_SIZE/SECTOR_SIZE)
#define ROOT_DIRECTORY_ENTRY_SIZE 32
#define FILE_ATTRIBUTE_SUBDIRECTORY 0x10

/*
 * Data structure definitions.
 *
 */

/*
 * Boot sector structure for FAT FS. This is the structure of the first sector
 * in the filesystem's partition. The drive also has a boot sector called the
 * MBR which is stored in the first sector on the drive.
 *
 */

struct boot_sector {
    char code[3];
    char oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t num_sectors_per_cluster;
    uint16_t num_reserved_sectors;
    uint8_t num_fat_tables;
    uint16_t num_root_dir_entries;
    uint16_t total_sectors;
    uint8_t media_descriptor;
    uint16_t num_sectors_per_fat;
    uint16_t num_sectors_per_track;
    uint16_t num_heads;
    uint32_t num_hidden_sectors;
    uint32_t total_sectors_in_fs;
    uint8_t logical_drive_num;
    uint8_t reserved;
    uint8_t extended_signature;
    uint32_t serial_number;
    char volume_label[11];
    char fs_type[8];
    char boot_code[448];
    uint16_t boot_signature;
}__attribute__((packed));

typedef struct boot_sector boot_sector;

/*
 * Root directory entry used to store info about a file. These data structures
 * are packed in the root directory.
 *
 */
struct root_directory_entry {
    char file_name[8];
    char file_extension[3];
    uint8_t attribute;
    uint8_t reserved1;
    uint8_t creation_timestamp;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t access_date;
    uint16_t reserved2;
    uint16_t modified_time;
    uint16_t modified_date;
    uint16_t cluster;
    uint32_t file_size;
};

typedef struct root_directory_entry root_directory_entry;

/*
 *
 * Stores info about an open file
 *
 */
struct file {
    struct file *next;
    struct file *prev;
    struct root_directory_entry rde;
    uint16_t start_cluster;
};

typedef struct file file;

struct fat_table_entry {
	uint16_t entry;
};

typedef struct fat_table_entry fat_table_entry;

int fatInit();
int fatOpen(file *fle, char* filename);
int fatRead(char* buffer, file* fp, unsigned int length);
int fat_create(char *filename);
int initFatStructs();
int pathToFileName(char fn[][11], char *path, int length);
void nullCharArray(char arr[], int length);
void spaceCharArray(char arr[], int length);
int isSeparator(char c, char sep);
int hasSeparator(char arr[], char sep, unsigned int length);
void extension(char a[], char b[], unsigned int length);
void oppositeExtension(char a[], char b[], char c[]);
void readFromCluster(unsigned char data[], uint16_t clusterNum, unsigned int size);
void printCharArray(char arr[], unsigned int length);
#endif
