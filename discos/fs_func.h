#ifndef __H_FILE_SYSTEM_FUNC__
#define __H_FILE_SYSTEM_FUNC__

///////////////////////////////////////////////////////////////////
/*
File system functions
*/
/////////////////////////////////////////////////////////////////

int rd_creat(char * pathname);
int rd_mkdir(char *pathname);

//NIRMIT
int rd_open(char *pathname);
int rd_close(int fd);
int rd_read(int fd, char * address, int num_bytes);
int rd_write(int fd, char * address, int num_bytes);
int rd_lseek(int fd, int offset);

// NOTE :  MAke sure that you set that index node assigned field to unused
//Make sure you update super block for both free node
//Make sure you set the block pointers back to NULL
int rd_unlink(char * pathname);
int rd_readdir(int fd, char * address);

#endif
