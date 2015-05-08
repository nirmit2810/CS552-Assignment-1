#include "fs_func.h"
#include "fs_helper.h"
#include "fs_helper_nir.h"
#include "fs.h"
#include "thread.h"
#include "output.h"
#include "helper.h"
#include "fs_debug.h"

bool system_initialized = FALSE;

#define system_init_check() do{if(!system_initialized){rd_reset();}}while(0)

int current_value=0;
void rd_reset(){

	prints("File system restting....");
	// Setting up Super block
	// -1 to account for the root node
	file_system.sb.sb.num_free_blocks = ALLOCATED_NUM_BLOCKS;
	file_system.sb.sb.num_free_innodes = NUM_INDEX_NODE;
	
	//Clearing Bit Map
	for(int i = 0; i < NUM_BYTE_FOR_BITMAP; i++) {
		file_system.bmap.byte_maps[i] = 0;
	}

	// Setting up root node
	int error = 0;
	uint16_t dummy;
	index_node * root_node = get_and_use_next_unused_node(&dummy);
	error |= init_index_node_reg(root_node, DIRECTORY);
	if(error)
		println("Error setting up root node");

	println("Done");
	system_initialized = TRUE;

}

int go_to_target_directory(char * pathname, index_node ** target, char* buffer) {
	int position = 0;
	int flag = -3;

	index_node * directory_node = &(file_system.ins[ROOT_IN_INDEX]);

	// PARSING PATH
	flag = next_path_in_str(pathname, &position, buffer);
	while(flag != FLAG_ERROR && flag != FLAG_DONE) {
		int next_entry_index = 0;
		entry_dir * next_level = NULL;
		entry_dir * curr_entry = NULL;
		while(curr_entry = walk_along_directory_entry(directory_node, &next_entry_index)){
			if(strmatch(buffer, curr_entry->filename)){
				if(strmatch((get_index_node_at_index(curr_entry->index_node_number)->type),FILE_TYPE_DIR) == TRUE) {
					next_level = curr_entry;
					directory_node = get_index_node_at_index(next_level->index_node_number);
					break;
				}
			}
		}
		if(!next_level){
			println("Error: Incorrect Path");
			return FLAG_ERROR;
		}
		flag = next_path_in_str(pathname, &position, buffer);
	}
	if(flag==FLAG_ERROR)
		return FLAG_ERROR;
	*target = directory_node;
	return FLAG_SUCCESS;
}

int rd_creat(char * pathname){
	system_init_check();
	if(file_system.sb.sb.num_free_blocks == 0 || file_system.sb.sb.num_free_innodes == 0){
		println("Can't create new file. Full");
		return FLAG_ERROR;
	}
	//Going to directory
	index_node * directory_node = NULL;
	char buffer[TEMP_BUFFER_SIZE];
	int flag = go_to_target_directory(pathname, & directory_node, buffer);
	if(flag == FLAG_ERROR)
		return flag;

	if(1 && filename_in_directory(buffer, directory_node)){
		println("Error: File already exists");
		return FLAG_ERROR;
	} else {
		uint16_t new_node_index = -1;
		index_node * new_index_node = get_and_use_next_unused_node(&new_node_index);
		strcpy_b(new_index_node->type, FILE_TYPE_REG, 4);
		new_index_node->size = 0;

		entry_dir * new_entry = create_new_entry(directory_node);
		strcpy_b(new_entry->filename, buffer, DIR_FILENAME_SIZE);
		new_entry->filename[DIR_FILENAME_SIZE - 1] = '\0';
		new_entry->index_node_number = new_node_index;
	}
}

int rd_mkdir(char *pathname){
	system_init_check();
	if(file_system.sb.sb.num_free_blocks == 0 || file_system.sb.sb.num_free_innodes == 0){
		println("Can't create new file. Full");
		return FLAG_ERROR;
	}
	//Going to directory
	index_node * directory_node;
	char buffer[TEMP_BUFFER_SIZE];
	int flag = go_to_target_directory(pathname, & directory_node, buffer);
	if(flag == FLAG_ERROR)
		return flag;

	if(1 && filename_in_directory(buffer, directory_node)){
		println("Error: Directory already exists");
		return FLAG_ERROR;
	} else {
		uint16_t new_node_index = -1;
		index_node * new_index_node = get_and_use_next_unused_node(&new_node_index);
		strcpy_b(new_index_node->type, FILE_TYPE_DIR, 4);
		new_index_node->size = 0;

		entry_dir * new_entry = create_new_entry(directory_node);
		strcpy_b(new_entry->filename, buffer, DIR_FILENAME_SIZE);
		new_entry->filename[DIR_FILENAME_SIZE - 1] = '\0';
		new_entry->index_node_number = new_node_index;
	}
}

int rd_open(char *pathname){
	system_init_check();

	index_node * directory_node = NULL;
	file_descriptor fd;
	int root=0;
	char buffer[TEMP_BUFFER_SIZE];
	 if(strmatch(pathname,"/")){
		  root=1;
		}
	if(root==1){
		int rcheck= check_if_inode_exists(0);
			if(rcheck == FLAG_ERROR){
				fd.number = current_value;
				fd.index_node_number = 0;
				fd.offset = 0;
				add_to_table(fd);
				current_value++;
				return fd.number;
			}
			else{
				println("Root already open !");	  
				return rcheck;
			}
		}	
	int flag = go_to_target_directory(pathname, & directory_node, buffer);
	if(flag == FLAG_ERROR)
		return FLAG_ERROR;

	if(filename_in_directory(buffer, directory_node)){
		entry_dir * entry = filename_in_directory(buffer, directory_node);
	  int inum;
		inum = entry->index_node_number;
		index_node * new_node = get_index_node_at_index(entry->index_node_number);
		if(strmatch(new_node->type,FILE_TYPE_REG) || strmatch(new_node->type,FILE_TYPE_DIR)){
			int check= check_if_inode_exists(inum);
			if(check == FLAG_ERROR){
				fd.number = current_value;
				fd.index_node_number = inum;
				fd.offset = 0;
				add_to_table(fd);
				current_value++;
				return fd.number;
			}
			else{
				println("File already open !");	  
				return check;
			}
		}
		else{
			println("Opening file of unkown file type");
			return FLAG_ERROR;	
		}  
	} else {
			println("Error: File doesn't  exists");
			return FLAG_ERROR;
	}
}

int rd_close(int fd){
	system_init_check();
	int x = delete_from_table(fd);
	if(x == 0){
		return FLAG_SUCCESS;
	}
	
	if(x == FLAG_ERROR){
		println("Error: Fd doesn't exist in the file descriptor table ");	
		return FLAG_ERROR;
	}
}

int rd_read(int fd, char * address, int num_bytes){
	system_init_check();
 
	file_descriptor * fdesp = NULL;
	fdesp = file_descriptor_entry(fd);

	if (fdesp == NULL)
	{
			println("The given fd does not exist in the file descriptor table.");
			return FLAG_ERROR;
	}
	 index_node * new_node = get_index_node_at_index(fdesp->index_node_number);
        if(strmatch(new_node->type,FILE_TYPE_DIR)){
			
			println("Cannot read a directory");
			return FLAG_ERROR;
		
		}
	
  int bytes_to_read = num_bytes;
  int copied = 0;
  allocated_block_t * blkp=NULL;

	uint16_t index_node_number = fdesp -> index_node_number;
	index_node * innode = get_index_node_at_index(index_node_number);

  int size = 0;
	while(bytes_to_read > 0){

		size = innode->size;	 

    if(fdesp->offset >= size){
			break;
		}
    
    blkp = get_alloc_block_with_num(innode, (fdesp->offset)/BLOCK_SIZE);

		if(!blkp){
			break;
		}
      
    int offset = (fdesp->offset ) % BLOCK_SIZE;

    for(int i = offset; i < BLOCK_SIZE; i++)
		{
			*((unsigned char *) address++) = *((unsigned char *)blkp->b.block1 + i);
			//terminal_putchar(*(((unsigned char *) address) -1));
			bytes_to_read --;
			fdesp->offset ++;
			copied ++;
			if(bytes_to_read == 0 || fdesp->offset >= size)
			{
				return copied;
			}
		}
	}
	return copied;
}

int rd_write(int fd, char * address, int num_bytes){
	system_init_check();
	
	file_descriptor * fdesp = NULL;
	fdesp = file_descriptor_entry(fd);

	if (fdesp == NULL)
	{
			println("The given fd does not exist in the file descriptor table.");
			return FLAG_ERROR;
	}
   
     index_node * new_node = get_index_node_at_index(fdesp->index_node_number);
        if(strmatch(new_node->type,FILE_TYPE_DIR)){
			
			println("Cannot write a directory");
			return FLAG_ERROR;
		
		}
	
	int bytes_to_copy = num_bytes;
	int copied = 0;
	uint16_t index_node_number = fdesp -> index_node_number;
	index_node * innode = get_index_node_at_index(index_node_number);

	allocated_block_t * blkp = NULL;
	int size;
	while(bytes_to_copy > 0){
		//Updating size of index node
		size = innode->size;
						 
		//if(( (fdesp->offset/BLOCK_SIZE) > (size - 1) / BLOCK_SIZE) && size != 0){     	   
		//	blkp = get_last_available_alloc_block(innode);
		//	if(blkp == NULL){
		//		return copied;
		//	}
		//}         		
		blkp = get_alloc_block_with_num(innode, fdesp->offset / BLOCK_SIZE);
		if(!blkp){
	        return copied;
	}
	
	  int offset = fdesp->offset % BLOCK_SIZE;
        
		for(int i = offset; i < BLOCK_SIZE; i++)
	{
			*((unsigned char *) blkp->b.block1 + i) = *((unsigned char *) address++);
			//terminal_putchar(*((unsigned char *) blkp->b.block1 + i));
			copied++;
			bytes_to_copy --;
			fdesp->offset ++;
			if(fdesp->offset > size){
				set_inode_size_at_inode_index(index_node_number , 1);
				size = innode->size;
			}
			if(bytes_to_copy == 0){
				
				return copied;
			}
		}
	}
	return FLAG_SUCCESS;
}

int rd_lseek(int fd, int offset){
	system_init_check();

	file_descriptor * fdesp = NULL;
	fdesp = file_descriptor_entry(fd);
	uint16_t index_node_number = fdesp -> index_node_number;
	index_node * innode = get_index_node_at_index(index_node_number);

	if (fdesp == NULL)
	{
			println("The given fd does not exist in the file descriptor table.");
			return FLAG_ERROR;
	}
   
  int size = innode->size;
  
	if(offset >= size)
  {
		println("Offset is greater than the file size");
		return FLAG_ERROR;
	}
	else
	{
		fdesp->offset= offset;
		return FLAG_SUCCESS;
	}
}

int rd_unlink(char * pathname){
	system_init_check();
	index_node * directory_node = NULL;
	file_descriptor fd;
	char buffer[TEMP_BUFFER_SIZE];
	int flag = go_to_target_directory(pathname, & directory_node, buffer);
	if(flag == FLAG_ERROR){
		return flag;
	}

	if(1 && filename_in_directory(buffer, directory_node)){
		entry_dir * entry = filename_in_directory(buffer, directory_node);
	  int inum;
		inum = entry->index_node_number;
		index_node * to_delete_node = get_index_node_at_index(entry->index_node_number);
		if(strmatch(to_delete_node->type,FILE_TYPE_REG)){
			// If it's a regular file
			// Loop through all the active pointers and delete blocks
			for(int i = 0; i <= to_delete_node->size / BLOCK_SIZE; i++){
				allocated_block_t **  pt = get_alloc_block_ptr_with_num(to_delete_node, i);
				if( *pt != NULL ){
					clear_block_content(*pt);
					uint16_t block_index = index_of_allocated_block(*pt);
					clear_bit_map(block_index);
					*pt = NULL;
				}
			}
			reset_index_node(to_delete_node);
			remove_entry_from_parent_directory(entry, directory_node);
			return FLAG_SUCCESS;
		} else if (strmatch(to_delete_node->type,FILE_TYPE_REG)){
			//If it's a directory file
			if( to_delete_node -> size != 0){
				println("Cannnot delete non-empty directory");
				return FLAG_ERROR;
			} else {
				reset_index_node(to_delete_node);
				remove_entry_from_parent_directory(entry, directory_node);
				return FLAG_SUCCESS;
			}
		} else {
			println("Error: Trying to delete unknown file type");
			return FLAG_ERROR;
		}

	} else {
			println("Error: File doesn't  exists");
			return FLAG_ERROR;
	}

}

int rd_readdir(int fd, char * address){
	system_init_check();
   
   file_descriptor * fdesp = NULL;
	fdesp = file_descriptor_entry(fd);

	if (fdesp == NULL)
	{
			println("The given fd does not exist in the file descriptor table.");
			return FLAG_ERROR;
	}
   
     index_node * new_node = get_index_node_at_index(fdesp->index_node_number);
        if(strmatch(new_node->type,FILE_TYPE_REG)){
			
			println("rd_readdir can only be applied to Directories");
			return FLAG_ERROR;
		
		}
	int next_entry_index = fdesp->offset;
	entry_dir * curr_entry;
	curr_entry = walk_along_directory_entry(get_index_node_at_index(fdesp->index_node_number), &next_entry_index);
	if(curr_entry==NULL){
	println("No more entries left in the directory");
	return 0;
	}
  *((unsigned short *)address) = curr_entry->index_node_number;
    strcpy_b((char *) address +2  ,curr_entry->filename,DIR_FILENAME_SIZE);
	
		printn(*(uint16_t *)address);
		
		for(int i=2;i<16; i++){
	   terminal_putchar( *(address +i));
      }
      println(" ");
	fdesp->offset+=1;	
	return FLAG_DONE;
   


}

