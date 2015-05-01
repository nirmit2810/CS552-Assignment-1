#include "fs_helper.h"
#include "fs_helper_nir.h"
#include "fs.h"
#include "output.h"
#include "helper.h"
#include "fs_debug.h"

#include "thread.h"
#include "output.h"
#include "scheduler.h"

int get_inode_size_at_inode_index(uint16_t index){
  index_node * new_node = get_index_node_at_index(index);
	return new_node->size;
}

void set_inode_size_at_inode_index(uint16_t index, uint16_t size1){
	 	 index_node * node = get_index_node_at_index(index);
		 node->size += size1;
	   return;
}

int check_if_inode_exists(int inode){
		int i;
		for(int i =0; i < MAX_FILE_DESCRIPTOR_NUM; i ++) {
			if(current_pcb->fd[i].index_node_number == inode){
				return current_pcb->fd[i].number;
			}
		}
	 return FLAG_ERROR;
}
