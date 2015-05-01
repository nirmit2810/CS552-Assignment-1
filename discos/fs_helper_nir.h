#ifndef __H_FILE_SYSTEM_HELPER_NIR__
#define __H_FILE_SYSTEM_HELPER_NIR__
#include "stdint.h"
#include "fs.h"
#include "global_t.h"
#include "thread.h"


int get_inode_size_at_inode_index(uint16_t index);
void set_inode_size_at_inode_index(uint16_t index, uint16_t size);
int check_if_inode_exists(int inode);




#endif
