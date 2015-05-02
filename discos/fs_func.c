#include "fs_func.h"
#include "fs_helper.h"
#include "fs_helper_nir.h"
#include "fs.h"
#include "thread.h"
#include "output.h"
#include "helper.h"
#include "fs_debug.h"


#define system_init_check() do{if(!system_initialized){rd_reset();}}while(0)

#define inter

#ifdef inter
#define disable_interrupt() asm volatile("cli \n\t")
#define enable_interrupt() asm volatile("sti \n\t")
#endif

#ifdef nointer
#define disable_interrupt() do{}while(0)
#define enable_interrupt() do{}while(0)
#endif

//#define return enable_interrupt();return


int current_value=0;  // We use for giving a unique Fd value in the file descriptor table

int rd_creat(char * pathname){
	disable_interrupt();
	system_init_check();
	//checking the super block for the available number of blocks, if not free blocks or inode return an error
	if(file_system.sb.sb.num_free_blocks == 0 || file_system.sb.sb.num_free_innodes == 0){
		println("Can't create new file. Full");
enable_interrupt();
		return FLAG_ERROR;
	}
	//Going to directory
	index_node * directory_node = NULL;
	char buffer[TEMP_BUFFER_SIZE];
	int flag = go_to_target_directory(pathname, & directory_node, buffer);
	if(flag == FLAG_ERROR){
enable_interrupt();
		return flag;
    //Check if the file already exists
		}

	if(1 && filename_in_directory(buffer, directory_node)){
		println("Error: File already exists");
enable_interrupt();
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
enable_interrupt();
	return FLAG_SUCCESS;
}

int rd_mkdir(char *pathname){
	disable_interrupt();
	system_init_check();
	if(file_system.sb.sb.num_free_blocks == 0 || file_system.sb.sb.num_free_innodes == 0){
		println("Can't create new file. Full");
		enable_interrupt();
		return FLAG_ERROR;
	}
	//Going to directory
	index_node * directory_node;
	char buffer[TEMP_BUFFER_SIZE];
	int flag = go_to_target_directory(pathname, & directory_node, buffer);
	if(flag == FLAG_ERROR){
enable_interrupt();
		return flag;
    //Check if filename already exists
	}

	if(1 && filename_in_directory(buffer, directory_node)){
		println("Error: Directory already exists");
enable_interrupt();
		return FLAG_ERROR;
	} else {
	    //Allocate the inode and entry for the directory
		uint16_t new_node_index = -1;
		index_node * new_index_node = get_and_use_next_unused_node(&new_node_index);
		strcpy_b(new_index_node->type, FILE_TYPE_DIR, 4);
		new_index_node->size = 0;

		entry_dir * new_entry = create_new_entry(directory_node);
		strcpy_b(new_entry->filename, buffer, DIR_FILENAME_SIZE);
		new_entry->filename[DIR_FILENAME_SIZE - 1] = '\0';
		new_entry->index_node_number = new_node_index;
	}
enable_interrupt();
	return FLAG_SUCCESS;
}

int rd_open(char *pathname){
	disable_interrupt();
	system_init_check();

	index_node * directory_node = NULL;
	file_descriptor fd;
	int root=0;
	char buffer[TEMP_BUFFER_SIZE];
	//check if the open file is a root file
	 if(strmatch(pathname,"/")){
		  root=1;
		}
	if(root==1){
    //check if the root is already open
		int rcheck= check_if_inode_exists(0);
			if(rcheck == FLAG_ERROR){
				fd.number = current_value;
				fd.index_node_number = 0;
				fd.offset = 0;
				add_to_table(fd);
				current_value++;
enable_interrupt();
				return fd.number;
			}
			else{
				println("Root already open !");	  
enable_interrupt();
				return rcheck;
			}
		}
	int flag = go_to_target_directory(pathname, & directory_node, buffer);
	if(flag == FLAG_ERROR){
enable_interrupt();
		return FLAG_ERROR;
      //check if the file exists
		}

	if(filename_in_directory(buffer, directory_node)){
		entry_dir * entry = filename_in_directory(buffer, directory_node);
	  int inum;
		inum = entry->index_node_number;
		index_node * new_node = get_index_node_at_index(entry->index_node_number);
		if(strmatch(new_node->type,FILE_TYPE_REG) || strmatch(new_node->type,FILE_TYPE_DIR)){
			//check if the inode exists already in the descriptor table
			int check= check_if_inode_exists(inum);
			if(check == FLAG_ERROR){
				fd.number = current_value;
				fd.index_node_number = inum;
				fd.offset = 0;
				add_to_table(fd);
				current_value++;
enable_interrupt();
				return fd.number;
			}
			else{
				println("File already open !");	  
enable_interrupt();
				return check;
			}
		}
		else{
			println("Opening file of unkown file type");
enable_interrupt();
			return FLAG_ERROR;	
		}  
	} else {
			println("Error: File doesn't  exists");
enable_interrupt();
			return FLAG_ERROR;
	}
enable_interrupt();
	return FLAG_SUCCESS;
}

int rd_close(int fd){
	disable_interrupt();
	system_init_check();
	int x = delete_from_table(fd);
	if(x == 0){
enable_interrupt();
		return FLAG_SUCCESS;
	}

	if(x == FLAG_ERROR){
		println("Error: Fd doesn't exist in the file descriptor table ");	
enable_interrupt();
		return FLAG_ERROR;
	}
enable_interrupt();
	return FLAG_SUCCESS;
}

int rd_read(int fd, char * address, int num_bytes){
	disable_interrupt();
	system_init_check();

	file_descriptor * fdesp = NULL;
	fdesp = file_descriptor_entry(fd);
    //check if the fd exists in the file descriptor table
	if (fdesp == NULL)
	{
			println("The given fd does not exist in the file descriptor table.");
enable_interrupt();
			return FLAG_ERROR;
	}
	 index_node * new_node = get_index_node_at_index(fdesp->index_node_number);
        if(strmatch(new_node->type,FILE_TYPE_DIR)){

			println("Cannot read a directory");
enable_interrupt();
			return FLAG_ERROR;

		}

  int bytes_to_read = num_bytes;
  int copied = 0;
  allocated_block_t * blkp = NULL;

	uint16_t index_node_number = fdesp -> index_node_number;
	index_node * innode = get_index_node_at_index(index_node_number);

  int size = 0;
	while(bytes_to_read > 0){

		size = innode->size;

    if(fdesp->offset >= size){
			break;
		}
    //allocate the block to read
    blkp = get_alloc_block_with_num(innode, (fdesp->offset)/BLOCK_SIZE);

		if(!blkp){
			break;
		}

    int offset = (fdesp->offset ) % BLOCK_SIZE;

    for(int i = offset; i < BLOCK_SIZE; i++)
		{   //read byte by byte
			*((unsigned char *) address++) = *((unsigned char *)blkp->b.block1 + i);
			//terminal_putchar(*(((unsigned char *) address) -1));
			bytes_to_read --;
			fdesp->offset ++;
			copied ++;
			if(bytes_to_read == 0 || fdesp->offset >= size)
			{
enable_interrupt();
				return copied;
			}
		}
	}
enable_interrupt();
	return copied;
}

int rd_write(int fd, char * address, int num_bytes){
	disable_interrupt();
	system_init_check();

	file_descriptor * fdesp = NULL;

	fdesp = file_descriptor_entry(fd);
    //check if the fd exists in the descriptor table
	if (fdesp == NULL)
	{
			println("The given fd does not exist in the file descriptor table.");
enable_interrupt();
			return FLAG_ERROR;
	}

     index_node * new_node = get_index_node_at_index(fdesp->index_node_number);
        if(strmatch(new_node->type,FILE_TYPE_DIR)){

			println("Cannot write a directory");
enable_interrupt();
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
		//allocate blocks as necessary
		blkp = get_alloc_block_with_num(innode, fdesp->offset / BLOCK_SIZE);
		if(!blkp){
enable_interrupt();
			return copied;
		}

	  int offset = fdesp->offset % BLOCK_SIZE;

		for(int i = offset; i < BLOCK_SIZE; i++)
	{
			blkp->b.block1[i] = *(address++);
			//terminal_putchar(*((unsigned char *) address));
			copied++;
			bytes_to_copy --;
			fdesp->offset ++;
			if(fdesp->offset > size){
				set_inode_size_at_inode_index(index_node_number , 1);
				size = innode->size;
			}
			if(bytes_to_copy == 0){
enable_interrupt();
				return copied;
			}
		}
	}
enable_interrupt();
	return FLAG_SUCCESS;
}

int rd_lseek(int fd, int offset){
	disable_interrupt();
	system_init_check();

	file_descriptor * fdesp = NULL;
	fdesp = file_descriptor_entry(fd);
	uint16_t index_node_number = fdesp -> index_node_number;
	index_node * innode = get_index_node_at_index(index_node_number);

	if (fdesp == NULL)
	{
			println("The given fd does not exist in the file descriptor table.");
enable_interrupt();
			return FLAG_ERROR;
	}

  int size = innode->size;

	if(offset >= size)
  {
		println("Offset is greater than the file size");
enable_interrupt();
		return FLAG_ERROR;
	}
	else
	{
		fdesp->offset= offset;
enable_interrupt();
		return FLAG_SUCCESS;
	}
}

int rd_unlink(char * pathname){
	disable_interrupt();
	system_init_check();
	index_node * directory_node = NULL;
	file_descriptor fd;
	char buffer[TEMP_BUFFER_SIZE];
	int flag = go_to_target_directory(pathname, & directory_node, buffer);
	if(flag == FLAG_ERROR){
enable_interrupt();
		return flag;
	}

	if(filename_in_directory(buffer, directory_node)){
		entry_dir * entry = filename_in_directory(buffer, directory_node);
	  int inum;
		inum = entry->index_node_number;
		index_node * to_delete_node = get_index_node_at_index(entry->index_node_number);
		if(strmatch(to_delete_node->type,FILE_TYPE_REG)){
			// If it's a regular file
			// Loop through all the active pointers and delete blocks
			for(int i = 0; i < (to_delete_node->size + BLOCK_SIZE - 1)  / BLOCK_SIZE; i++){
				allocated_block_t **  pt = get_alloc_block_ptr_with_num(to_delete_node, i);
				if( *pt != NULL ){
					remove_block(*pt);
					*pt = NULL;
				}
			}
			reset_index_node(to_delete_node);
			remove_entry_from_parent_directory(entry, directory_node);
enable_interrupt();
			return FLAG_SUCCESS;
		} else if (strmatch(to_delete_node->type,FILE_TYPE_DIR)){
			//If it's a directory file
			if( to_delete_node -> size != 0){
				println("Cannnot delete non-empty directory");
enable_interrupt();
				return FLAG_ERROR;
			} else {
				reset_index_node(to_delete_node);
				remove_entry_from_parent_directory(entry, directory_node);
enable_interrupt();
				return FLAG_SUCCESS;
			}
		} else {
			println("Error: Trying to delete unknown file type");
enable_interrupt();
			return FLAG_ERROR;
		}

	} else {
			println("Error: File doesn't  exists");
enable_interrupt();
			return FLAG_ERROR;
	}
enable_interrupt();
	return FLAG_SUCCESS;
}

int rd_readdir(int fd, char * address){
	enable_interrupt();
	system_init_check();

  file_descriptor * fdesp = NULL;
	fdesp = file_descriptor_entry(fd);

	if (fdesp == NULL)
	{
		println("The given fd does not exist in the file descriptor table.");
enable_interrupt();
		return FLAG_ERROR;
	}

	index_node * new_node = get_index_node_at_index(fdesp->index_node_number);
	//check if its a directory
	if(strmatch(new_node->type,FILE_TYPE_REG)){
		println("rd_readdir can only be applied to Directories");
enable_interrupt();
		return FLAG_ERROR;
	}

	int next_entry_index = fdesp->offset;
	entry_dir * curr_entry;
	curr_entry = walk_along_directory_entry(new_node, &next_entry_index);
	if(curr_entry == NULL){
		println("No more entries left in the directory");
enable_interrupt();
		return 0;
	}
	entry_dir * entry = (entry_dir *)address;
	entry->index_node_number = curr_entry->index_node_number;
	strcpy_b(entry->filename, curr_entry->filename, DIR_FILENAME_SIZE);
    //strcpy_b((char *) address  ,curr_entry->filename,DIR_FILENAME_SIZE);
  //*((unsigned short *)address + 14) = (curr_entry->index_node_number)>>8;
  //*((unsigned short *)address + 14) = curr_entry->index_node_number;
	
//		printn(*(uint16_t *)address);
//		
//		for(int i=2;i<16; i++){
//	   terminal_putchar( *(address +i));
//      }
//      println(" ");
	fdesp->offset++;	
enable_interrupt();
	return FLAG_DONE;
}

