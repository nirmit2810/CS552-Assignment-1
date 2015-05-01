#include "fs_helper.h"
#include "fs.h"
#include "output.h"
#include "helper.h"
#include "fs_debug.h"

int next_path_in_str(const char * src, int * position, char * buffer){

	int pos = *position;
	//Filtering beginning
	if (!pos) {
		if(*src == '/'){
			pos ++;
		} else {
			println("Error: Path not starting with /");
			return FLAG_ERROR;
		}
	}

	char temp = src[pos];
	while(temp != '\0') {
		if (temp == '/') {
			pos++;
			*position = pos;
			*(buffer++) = '\0';
			if (src[pos] == '\0'){
				return FLAG_DONE;
			}else
				return FLAG_SUCCESS;
		} else {
			*buffer = temp;
			temp = src[++pos];
			buffer++;
		}
	}
	*(buffer++) = '\0';
	return FLAG_DONE;
}

//Index Node
int init_index_node_reg(index_node * innode, file_type ft) {
	//TYPE
	if (ft == DIRECTORY){
		strcpy_b(innode->type, FILE_TYPE_DIR, FILE_TYPE_STR_SIZE);
	} else if (ft == REGULAR) {
		strcpy_b(innode->type, FILE_TYPE_REG, FILE_TYPE_STR_SIZE);
	} else {
		println("Erorr innode type initialization\n");
		return FLAG_ERROR;
	}
	innode->size = 0;
	innode->assigned = TRUE;
	return FLAG_SUCCESS;
}

index_node * get_index_node_at_index(uint16_t index){
	if(index >= NUM_INDEX_NODE){
		println("Invalid index node index");
		return NULL;
	}
	return &(file_system.ins[index]);
}

index_node * get_and_use_next_unused_node(uint16_t * index){
	for(int i = 0; i < NUM_INDEX_NODE; i ++ ){
		if(file_system.ins[i].assigned == INDEX_NODE_UNUSED){
			file_system.ins[i].assigned = INDEX_NODE_USED;
			*index = i;
			return &(file_system.ins[i]);
		}
	}
	println("Error: All index blocks used up");
	return NULL;
}

//Bit mask
uint16_t get_next_available_bit(){
	uint16_t result = 0;
	bool found = FALSE;
	for(int i = 0; i < NUM_BYTE_FOR_BITMAP; i++) {
		if(file_system.bmap.byte_maps[i] == 0xFF){
			result += BIT_IN_BYTE;
			continue;
		}
		found = TRUE;	
		uint8_t current_byte = file_system.bmap.byte_maps[i];
		uint8_t offset = 0;
		uint8_t mask;
		while(1) {
			mask = 1 << offset;
			if(!(current_byte & mask))
				break;
			result++;
			offset++;
		}
		break;
	}
	if(found == TRUE){
		return result;
	}
	return FLAG_ERROR;
}

int set_bit_map(uint16_t index) {
	if ( index >= NUM_BYTES_FOR_DIR_ENTRY * BIT_IN_BYTE) {
		println("Invalid bit setting index");
		return  FLAG_ERROR;
	}
	int offset = index % 8;
	int byte = index / 8;
	uint8_t mask = 1 << offset;
	if ((uint8_t)(file_system.bmap.byte_maps[byte] & mask) != 0){
		println("Setting bit map that is already set");
		return FLAG_DONE;
	}
	file_system.bmap.byte_maps[byte] |= mask;
	file_system.sb.sb.num_free_blocks--;
	return FLAG_SUCCESS;
}

int clear_bit_map(uint16_t index) {
	if (index >= NUM_BYTES_FOR_DIR_ENTRY * BIT_IN_BYTE) {
		println("Invalid bit setting index");
		return  FLAG_ERROR;
	}
	int offset = index % 8;
	int byte = index / 8;
	uint8_t mask = 1 << offset;
	// flip all bits
	if (file_system.bmap.byte_maps[byte] & mask == 0){
		println("Clearing bit map that is already cleared");
		return FLAG_DONE;
	}
	mask ^= 0xFF;
	file_system.bmap.byte_maps[byte] &= mask;
	file_system.sb.sb.num_free_blocks++;
	return FLAG_SUCCESS;
}

//Blocks
allocated_block_t ** get_alloc_block_ptr_with_num(index_node * node, int num) {
	if(num < NUM_DIRECT_POINTER) {
		if(node->locations[num] == NULL)
			node->locations[num] = alloc_new_block();
		return &(node->locations[num]);
	}else if (num < NUM_DIRECT_POINTER + NUM_SINGLE_LEVEL_PTR){
		num -= NUM_DIRECT_POINTER;
		allocated_block_t * first =  node->locations[NUM_DIRECT_POINTER];
		if(first == NULL){
			node->locations[NUM_DIRECT_POINTER] = alloc_new_block();
			first =  node->locations[NUM_DIRECT_POINTER];
		}
		if(first->in_blk.block_pointers[num] == NULL){
			first->in_blk.block_pointers[num] = alloc_new_block();
		}
		return &(first->in_blk.block_pointers[num]);
	} else if(num < NUM_DIRECT_POINTER + NUM_SINGLE_LEVEL_PTR + NUM_DOUBLE_LEVEL_PTR) {
		num -= NUM_DIRECT_POINTER;
		num -= NUM_SINGLE_LEVEL_PTR;
		allocated_block_t * first =  node->locations[NUM_DIRECT_POINTER + 1];
		if(first == NULL){
			node->locations[NUM_DIRECT_POINTER + 1] = alloc_new_block();
			first =  node->locations[NUM_DIRECT_POINTER + 1];
		}
		int index = num / NUM_ENTRIES_IN_INDEX_BLOCK;
		int offset = num % (uint8_t)(NUM_ENTRIES_IN_INDEX_BLOCK);
		if(1){
		printn(num);
		prints(" ");
		printn(index);
		prints(" ");
		printn(offset);
		println("");
		}
		allocated_block_t * second =  first->in_blk.block_pointers[index];
		if(first->in_blk.block_pointers[index] == NULL){
			first->in_blk.block_pointers[index] = alloc_new_block();
			second =  first->in_blk.block_pointers[index];
		}
		if(second->in_blk.block_pointers[offset] == NULL) {
			second->in_blk.block_pointers[offset] = alloc_new_block();
		}
		return &(second->in_blk.block_pointers[offset]);
	} else {
		println("Erorr: trying to access memory block that is greater than possible");
		return NULL;
	}
}

allocated_block_t * get_alloc_block_with_num(index_node * node, int num) {
	if(
		 num >  (node->size / BLOCK_SIZE)
		){
		println("Erorr: Trying to access memory outside the size of node");
		return NULL;
	}
	return *(get_alloc_block_ptr_with_num(node, num));
}
allocated_block_t  * get_last_available_alloc_block(index_node * node){
	uint32_t file_size = node->size;
	int block_index = file_size / BLOCK_SIZE;
	if(block_index >= MAX_NUM_BLOCKS_PER_FILE) {
		println("Error: Trying to allocate too many blocks for one single file");
		return NULL;
	}
	return get_alloc_block_with_num(node, block_index);
}

void clear_block_content(allocated_block_t * blk) {
	for(int i = 0; i < BLOCK_SIZE; i ++ ){
	 blk->b.block[i] = 0;
	}
}

allocated_block_t * alloc_new_block(){
	uint16_t next_available_block_index = get_next_available_bit();
	//Set everything in new block to 0
	clear_block_content(&(file_system.alloc_blks[next_available_block_index]));
	
	//Setting bit map
	set_bit_map(next_available_block_index);
	return &(file_system.alloc_blks[next_available_block_index]);
}

//Directory
entry_dir * filename_in_directory(const char * filename, index_node * node){
	int next_entry_index = 0;
	entry_dir * curr_entry;
	while(curr_entry = walk_along_directory_entry(node, &next_entry_index)){
		if(strmatch(filename, curr_entry->filename))
			return curr_entry;
	}
	return NULL;
}

entry_dir * walk_along_directory_entry(index_node * node, int * next_entry_index){
	const uint32_t size = node->size;
	const int entry_size = sizeof(char) * DIR_FILENAME_SIZE + sizeof(uint16_t);
	const int entries_per_block = BLOCK_SIZE / NUM_BYTES_FOR_DIR_ENTRY;
	const int entries_filled = size / entry_size;
	if ((*next_entry_index) >= entries_filled)
		return NULL;
	const int block_index = (*next_entry_index) / entries_per_block;
	const int block_offset = (*next_entry_index) % entries_per_block;
	allocated_block_t * block = get_alloc_block_with_num(node, block_index);
	if(!block){
		println("Not returning a block to walk through directory");
		return NULL;
	}
	(*next_entry_index)++;
	return &(block->en_dir[block_offset]);
}

entry_dir * create_new_entry(index_node * node){
	const uint32_t size = node->size;
	const int entry_size = sizeof(char) * DIR_FILENAME_SIZE + sizeof(uint16_t);
	const int entries_per_block = BLOCK_SIZE / NUM_BYTES_FOR_DIR_ENTRY;
	const int entries_filled = size / entry_size;
	allocated_block_t * target = get_last_available_alloc_block(node);
	if(target){
		node->size += entry_size;
		return &(target->en_dir[((entries_filled) % entries_per_block)]);
	} else {
		println("Erorr: can't create_new_entry");
		return NULL;
	}
}
