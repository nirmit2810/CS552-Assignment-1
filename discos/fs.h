#ifndef __H_FILE_SYSTEM__
#define __H_FILE_SYSTEM__
#include "stdint.h"

#define FLAG_SUCCESS 0
#define FLAG_ERROR -1
#define FLAG_DONE 1

#define BLOCK_SIZE 256
#define BIT_IN_BYTE 8
#define POINTER_SIZE 4

//Super block
#define NUM_BLOCK_SUPERBLOCK 1

// Index Node
#define INDEX_NODE_SIZE 64
#define NUM_BLOCK_FOR_INDEX_NODE 256
#define NUM_INDEX_NODE NUM_BLOCK_FOR_INDEX_NODE * BLOCK_SIZE / INDEX_NODE_SIZE
#define NUM_BLOCK_POINTERS 10
#define FILE_TYPE_STR_SIZE 3 + 1
#define FILE_TYPE_DIR "dir"
#define FILE_TYPE_REG "reg"
#define ROOT_IN_INDEX 0
#define INDEX_NODE_USED 1
#define INDEX_NODE_UNUSED 0
#define INDEX_NODE_EMPTY_SPACE_SIZE 15

//Bitmap
#define NUM_BLOCK_FOR_BITMAP 4
#define NUM_BYTE_FOR_BITMAP NUM_BLOCK_FOR_BITMAP * BLOCK_SIZE

//Entries
#define NUM_BYTES_FOR_DIR_ENTRY 16
#define DIR_FILENAME_SIZE 14
#define NUM_DIRECT_POINTER 8
#define NUM_SINGLE_LEVEL_PTR 64
#define NUM_DOUBLE_LEVEL_PTR 4096 
#define MAX_NUM_BLOCKS_PER_FILE NUM_DIRECT_POINTER + NUM_SINGLE_LEVEL_PTR + NUM_DOUBLE_LEVEL_PTR

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
	uint8_t block1[BLOCK_SIZE];
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
	union allocated_block_u * locations[NUM_BLOCK_POINTERS];
	uint8_t assigned;
	uint8_t empty_space[INDEX_NODE_EMPTY_SPACE_SIZE];
} index_node;

typedef struct entry_dir_st {
	char filename[DIR_FILENAME_SIZE];
	uint16_t index_node_number;
} entry_dir;

typedef struct index_block_st {
	union allocated_block_u * block_pointers[NUM_ENTRIES_IN_INDEX_BLOCK];
} index_block;

typedef union allocated_block_u{
	block b;
	entry_dir en_dir[BLOCK_SIZE / NUM_BYTES_FOR_DIR_ENTRY];
	struct index_block_st in_blk;
}allocated_block_t;

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
allocated_block_t alloc_blks[ALLOCATED_NUM_BLOCKS];

} file_system_t;

file_system_t file_system;

#endif
