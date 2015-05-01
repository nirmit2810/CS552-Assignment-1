#ifndef __H_FS_DEBUG__
#define __H_FS_DEBUG__
#include "fs.h"

void print_fs_in(index_node * node);
void print_fs_indexid(index_node * node);
void print_fs_blockid(allocated_block_t * block);
#endif
