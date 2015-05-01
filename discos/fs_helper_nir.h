#ifndef __H_FILE_SYSTEM_HELPER_NIR__
#define __H_FILE_SYSTEM_HELPER_NIR__
#include "stdint.h"
#include "fs.h"
#include "global_t.h"
#include "thread.h"


int get_inode_size_at_inode_index(uint16_t index);
void set_inode_size_at_inode_index(uint16_t index, uint16_t size);
int check_if_inode_exists(int inode);

void add_to_table(file_descriptor fd); 
int delete_from_table(int fd);
int check_if_fd_exists(int fd);
file_descriptor * file_descriptor_entry(int fd);




#endif
