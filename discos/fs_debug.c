#include "fs_debug.h"
#include "output.h"
#include "helper.h"
#include "fs_helper.h"

void print_fs_in(index_node * node){
	prints("Index Node->type: ");
	prints(node->type);
	prints(" ");
	prints("size: ");
	printn(node->size);
	prints(" ");
	prints("used: ");
	printn(node->assigned);
	prints(": ");
	if(strmatch(node->type, FILE_TYPE_DIR) == TRUE){
		int next_entry_index = 0;
		entry_dir * curr_entry;
		while(curr_entry = walk_along_directory_entry(node, &next_entry_index)){
			prints(curr_entry->filename);
			prints(" ");
		}
	}
	println("");
}
void print_fs_indexid(index_node * node){
	unsigned int add = (unsigned int) node;
	unsigned int start = (unsigned int) file_system.ins;
	int answer = (add - start) / INDEX_NODE_SIZE;
	printnln(answer);
}

void print_fs_blockid(allocated_block_t * block){
	unsigned int add = (unsigned int) block;
	unsigned int start = (unsigned int) file_system.alloc_blks;
	int answer = (add - start) / BLOCK_SIZE;
	printnln(answer);
}
