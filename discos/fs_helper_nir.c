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

void add_to_table(file_descriptor fd1){
	int i;
	for(i = 0; i < MAX_FILE_DESCRIPTOR_NUM; i ++) {
		if(!current_pcb->fd[i].assigned){
			current_pcb->fd[i].number = fd1.number;
			current_pcb->fd[i].index_node_number = fd1.index_node_number;
			current_pcb->fd[i].offset = fd1.offset;
			current_pcb->fd[i].assigned= TRUE;
		       prints("Inserting fd entry with fd= ");
		       printn(current_pcb->fd[i].number);
		       prints("inode = ");
		       printnln(current_pcb->fd[i].index_node_number);
			return;
		}
	}
	println("Fd table full close some file");
}

int delete_from_table(int fd1){
	int i;
		for(int i =0; i < MAX_FILE_DESCRIPTOR_NUM; i ++) {
			if(current_pcb->fd[i].number == fd1){
				current_pcb->fd[i].assigned= FALSE;
				current_pcb->fd[i].number=-1;
                current_pcb->fd[i].index_node_number=-1;			
			    return FLAG_SUCCESS;
			}
		}
     
     return FLAG_ERROR;
}


int check_if_fd_exists(int fd1){
	file_descriptor *entry = file_descriptor_entry(fd1);
	if(entry == NULL)
		return FLAG_ERROR;
	return FLAG_SUCCESS;
}

file_descriptor * file_descriptor_entry(int fd1){

    file_descriptor *entry;
    int i;
    for (i = 0; i < MAX_FILE_DESCRIPTOR_NUM; i++)
    {
        if(current_pcb->fd[i].number == fd1){
				entry= &(current_pcb->fd[i]);
				return entry;
			}
		}
    
    return NULL;
}
