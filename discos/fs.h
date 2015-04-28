#ifndef __H_FILE_SYSTEM__
#define __H_FILE_SYSTEM__
#include "stdint.h"

#define BLOCK_SIZE 256
#define BIT_IN_BYTE 8
#define POINTER_SIZE 4

//Super block
#define NUM_BLOCK_SUPERBLOCK 1

// Index Node
#define INDEX_INDEX_NODE_SIZE 64
#define NUM_BLOCK_FOR_INDEX_NODE 256
#define NUM_INDEX_NODE NUM_BLOCK_FOR_INDEX_NODE * BLOCK_SIZE / INDEX_INDEX_NODE_SIZE
#define INDEX_NODE_EMPTY_SPACE_SIZE 16
#define NUM_BLOCK_POINTERS 10
#define FILE_TYPE_STR_SIZE 3 + 1

//Bitmap
#define NUM_BLOCK_FOR_BITMAP 4
#define NUM_BYTE_FOR_BITMAP NUM_BLOCK_FOR_BITMAP * BLOCK_SIZE

//Entries
#define NUM_BYTES_FOR_DIR_ENTRY 16
#define DIR_FILENAME_SIZE 14

//Allocated space
#define TOTAL_NUM_MEMORY (1 << (20 + 1))  // 2MB
#define TOTAL_NUM_BLOCKS TOTAL_NUM_MEMORY / BLOCK_SIZE  // 2MB
#define ALLOCATED_NUM_BLOCKS 	TOTAL_NUM_BLOCKS - \
															NUM_BLOCK_SUPERBLOCK -\
															NUM_BLOCK_FOR_INDEX_NODE -\
															NUM_BLOCK_FOR_BITMAP
// INDEX BLOCK
#define NUM_ENTRIES_IN_INDEX_BLOCK BLOCK_SIZE / POINTER_SIZE

typedef struct block_st {
	uint8_t block[BLOCK_SIZE];
} block;

typedef struct superblock_st {
	uint32_t num_free_blocks;
	uint32_t num_free_innodes;
} superblock;

typedef union suberblock_u{
	block b;
	superblock sb;
}superblock_t;

typedef struct bitmap_st {
	uint8_t byte_maps[NUM_BYTE_FOR_BITMAP];
} bitmap;

typedef struct index_node_st {
	char type[FILE_TYPE_STR_SIZE];
	uint32_t size;
	uint32_t locations[NUM_BLOCK_POINTERS];
	uint8_t empty_space[INDEX_NODE_EMPTY_SPACE_SIZE];
} index_node;

typedef struct entry_dir_st {
	char filename[DIR_FILENAME_SIZE];
	uint16_t index_node_number;
} entry_dir;

typedef struct index_block_st {
	uint32_t block_pointers[NUM_ENTRIES_IN_INDEX_BLOCK];
} index_block;

typedef union allocated_blocks_u{
	block b;
	entry_dir en_dir[BLOCK_SIZE / NUM_BYTES_FOR_DIR_ENTRY];
	index_block in_blk;
}allocated_blocks_t;

///////////////////////////////////////////////////////////////////
/*
File System Memory (2MB)
*/
/////////////////////////////////////////////////////////////////

typedef struct file_system_st {

// SUPER BLOCK
superblock_t sb;

// INDEX_NODE
index_node ins[NUM_INDEX_NODE];

// BITMAP
bitmap bmap;

// allocated blocks
allocated_blocks_t alloc_blks[ALLOCATED_NUM_BLOCKS];

} file_system_t;


///////////////////////////////////////////////////////////////////
/*
File system functions
*/
/////////////////////////////////////////////////////////////////

int rd_create(char * pathname);
int rd_mkdir(char *pathname);
int rd_open(char *pathname);
int rd_close(int fd);
int rd_read(int fd, char * address, int num_bytes);
int rd_write(int fd, char * address, int num_bytes);
int rd_lseek(int fd, int offset);
int rd_unlink(char * pathname);
int rd_readdir(int fd, char * address);


#endif
