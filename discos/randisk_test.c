#include "fs_func.h"
#include "output.h"
#include "fs_helper.h"
#include "fs_helper_nir.h"
#include "thread.h"
 /*
   -- template test file for RAMDISK Filesystem Assignment.
   -- include a case for:
   -- two processes
   -- largest number of files (should be 1024 max)
   -- largest single file (start with direct blocks [2048 bytes max],
   then single-indirect [18432 bytes max] and finally double
   indirect [1067008 bytes max])
   -- creating and unlinking files to avoid memory leaks
   -- each file operation
   -- error checking on invalid inputs
*/

// #define's to control what tests are performed,
// comment out a test if you do not wish to perform it

//#define TEST1
#define TEST2
//#define TEST3
//#define TEST4
//#define TEST5


#define USE_RAMDISK

#ifdef USE_RAMDISK
#define CREAT   rd_creat
#define OPEN    rd_open
#define WRITE   rd_write
#define READ    rd_read
#define UNLINK  rd_unlink
#define MKDIR   rd_mkdir
#define READDIR rd_readdir
#define CLOSE   rd_close
#define LSEEK   rd_lseek

#else
#define CREAT(file)   creat(file, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define OPEN(file)    open(file, O_RDWR)
#define WRITE   write
#define READ    read
#define UNLINK  unlink
#define MKDIR(path)   mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#define READDIR my_readdir
#define CLOSE   close
#define LSEEK(fd, offset)   lseek(fd, offset, SEEK_SET)

int my_readdir(int fd, char* str)
{
  strcpy(str, "blah");
  return 1;
}

#endif
#define EXIT_FAILURE 0


// #define's to control whether single indirect or
// double indirect block pointers are tested

#define TEST_SINGLE_INDIRECT
#define TEST_DOUBLE_INDIRECT

#define MAX_FILES 1023
#define BLK_SZ 256		/* Block size */
#define DIRECT 8		/* Direct pointers in location attribute */
#define PTR_SZ 4		/* 32-bit [relative] addressing */
#define PTRS_PB  (BLK_SZ / PTR_SZ) /* Pointers per index block */

static char pathname[80];

static char data1[DIRECT*BLK_SZ]; /* Largest data directly accessible */
static char data2[PTRS_PB*BLK_SZ];     /* Single indirect data size */
static char data3[PTRS_PB*PTRS_PB*BLK_SZ+1]; /* Double indirect data size */
static char addr[PTRS_PB*PTRS_PB*BLK_SZ+1]; /* Scratchpad memory */

void my_memset(void * b, char c, int len)
{
  int i;
  unsigned char *p = b;
  i = 0;
  while(len > 0)
	{
		*p = c;
		p++;
		len--;
	}
}

int atoi(char *str)
{
    int res = 0; // Initialize result
 
    // Iterate through all characters of input string and update result
    for (int i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';
 
    // return result.
    return res;
}

void my_exit(int i) {
asm volatile("hlt\n\t");
}

void set_pathname(char * s, int index){
	s[0] = '/';
	s[1] = 'f';
	s[2] = 'i';
	s[3] = 'l';
	s[4] = 'e';
	char temp[20];
	itoa(index, temp, 10);
	int i;
	for(i = 5; i < 80; i++){
		char e = temp[i - 5];
		s[i] = e;
		if(e == '\0')
			break;
	}
}

int fs_test1 () {

  int retval, i;
  int fd;
  int index_node_number;

  /* Some arbitrary data for our files */
  my_memset (data1, '1', sizeof (data1));
  my_memset (data2, '2', sizeof (data1));
  my_memset (data3, '3', sizeof (data1));


#ifdef TEST1

  /* ****TEST 1: MAXIMUM file creation**** */

  /* Generate MAXIMUM regular files */
  for (i = 0; i < MAX_FILES + 1; i++) { // go beyond the limit
		set_pathname(pathname, i);

    retval = CREAT (pathname);

    if (retval < 0) {
      println ("creat: File creation error! status: "
	       );
      if (i != MAX_FILES)
				my_exit(EXIT_FAILURE);
    }

    my_memset (pathname, 0, 80);
  }

  /* Delete all the files created */
  for (i = 0; i < MAX_FILES; i++) {
		set_pathname(pathname, i);

    retval = UNLINK (pathname);

    if (retval < 0) {
      println ("unlink: File deletion error! status:");

      my_exit(EXIT_FAILURE);
    }

    my_memset (pathname, 0, 80);
  }

#endif // TEST1

#ifdef TEST2

  /* ****TEST 2: LARGEST file size**** */

  /* Generate one LARGEST file */
  retval = CREAT ("/bigfile");

  if (retval < 0) {
    println ("creat: File creation error! status:");

    my_exit(EXIT_FAILURE);
  }

  retval =  OPEN ("/bigfile"); /* Open file to write to it */

  if (retval < 0) {
    println ("open: File open error! status:");

    my_exit(EXIT_FAILURE);
  }

  fd = retval;			/* Assign valid fd */

  /* Try writing to all direct data blocks */
  retval = WRITE (fd, data1, sizeof(data1));
  printnln(get_inode_size_at_inode_index(1));
  if (retval < 0) {
    println ("write: File write STAGE1 error! status");

    my_exit(EXIT_FAILURE);
  }

#ifdef TEST_SINGLE_INDIRECT

  /* Try writing to all single-indirect data blocks */
  retval = WRITE (fd, data2, sizeof(data2));
   printnln(get_inode_size_at_inode_index(1));
  if (retval < 0) {
    println ("write: File write STAGE2 error! status: ");

    my_exit(EXIT_FAILURE);
  }

#ifdef TEST_DOUBLE_INDIRECT

  /* Try writing to all double-indirect data blocks */
  retval = WRITE (fd, data3, sizeof(data3));
   printnln(get_inode_size_at_inode_index(1));
  if (retval < 0) {
    println ("write: File write STAGE3 error! status: ");

    my_exit(EXIT_FAILURE);
  }

#endif // TEST_DOUBLE_INDIRECT

#endif // TEST_SINGLE_INDIRECT

#endif // TEST2

#ifdef TEST3

  /* ****TEST 3: Seek and Read file test**** */
  retval = LSEEK (fd, 0);	/* Go back to the beginning of your file */

  if (retval < 0) {
    println ("lseek: File seek error! status: ");

    my_exit(EXIT_FAILURE);
  }

  /* Try reading from all direct data blocks */
  retval = READ (fd, addr, sizeof(data1));

  if (retval < 0) {
    println ("read: File read STAGE1 error! status: ");

    my_exit(EXIT_FAILURE);
  }

  /* Should be all 1s here... */
  prints ("Data at addr: ");
	//println(addr);
	char * b = addr;
	int fail = 0;
	while( *b != '\0'){
		if (*b != '1')
			fail = 1;
		b++;
	}
	if(fail == 1)
		println("Test all 1 fails");
	else
		println("Test all 1 passes");


#ifdef TEST_SINGLE_INDIRECT

  /* Try reading from all single-indirect data blocks */
  retval = READ (fd, addr, sizeof(data2));

  if (retval < 0) {
    println ("read: File read STAGE2 error! status: ");

    my_exit(EXIT_FAILURE);
  }
  /* Should be all 2s here... */
  prints ("Data at addr: ");
	//println(addr);
	b = addr;
	fail = 0;
	while( *b != '\0'){
		if (*b != '2')
			fail = 1;
		b++;
	}
	if(fail == 1)
		println("Test all 2 fails");
	else
		println("Test all 2 passes");

#ifdef TEST_DOUBLE_INDIRECT

  /* Try reading from all double-indirect data blocks */
  retval = READ (fd, addr, sizeof(data3));
  //retval = READ (3d, addr, 3);
	char c[2] ;
	c[2] = file_system.alloc_blks[20].b.block1[1];
	c[1] = '\0';
	println(c);
	println(&(file_system.alloc_blks[4].b.block1[0]));

  if (retval < 0) {
    println ("read: File read STAGE3 error! status: ");

    my_exit(EXIT_FAILURE);
  }
  /* Should be all 3s here... */
  prints ("Data at addr: ");
	println(addr);
	b = addr;
	fail = 0;
	while( *b != '\0'){
		if (*b != '3')
			fail = 1;
		b++;
	}
	if(fail == 1)
		println("Test all 3 fails");
	else
		println("Test all 3 passes");
	printnln(current_pcb->fd[0].offset);
return;

#endif // TEST_DOUBLE_INDIRECT

#endif // TEST_SINGLE_INDIRECT

  /* Close the bigfile */
  retval = CLOSE(fd);

  if (retval < 0) {
    println ("close: File close error! status: ");

    my_exit(EXIT_FAILURE);
  }

  /* Remove the biggest file */

  retval = UNLINK ("/bigfile");

  if (retval < 0) {
    println ("unlink: /bigfile file deletion error! status: ");

    my_exit(EXIT_FAILURE);
  }

#endif // TEST3

#ifdef TEST4

  /* ****TEST 4: Make directory and read directory entries**** */
  retval = MKDIR ("/dir1");

  if (retval < 0) {
    println ("mkdir: Directory 1 creation error! status: ");

    my_exit(EXIT_FAILURE);
  }

  retval = MKDIR ("/dir1/dir2");

  if (retval < 0) {
    println ("mkdir: Directory 2 creation error! status: ");

    my_exit(EXIT_FAILURE);
  }

  retval = MKDIR ("/dir1/dir3");

  if (retval < 0) {
    println ("mkdir: Directory 3 creation error! status: ");

    my_exit(EXIT_FAILURE);
  }

  retval =  OPEN ("/dir1"); /* Open directory file to read its entries */

  if (retval < 0) {
    println ("open: Directory open error! status: ");

    my_exit(EXIT_FAILURE);
  }

  fd = retval;			/* Assign valid fd */

  my_memset (addr, 0, sizeof(addr)); /* Clear scratchpad memory */

  while ((retval = READDIR (fd, addr))) { /* 0 indicates end-of-file */

    if (retval < 0) {
      println ("readdir: Directory read error! status: ");
      my_exit(EXIT_FAILURE);
    }

    index_node_number = atoi(&addr[14]);
    prints ("Contents at addr: [");
		prints(addr);
		printnln(index_node_number);
  }

#endif // TEST4

#ifdef TEST5

  /* ****TEST 5: 2 process test**** */

 // if((retval = fork())) {

 //   if(retval == -1) {
 //     println("Failed to fork\n");
 //     my_exit(EXIT_FAILURE);
 //   }

 //   /* Generate 300 regular files */
 //   for (i = 0; i < 300; i++) {

 //			set_pathname(pathname, i);
 //     retval = CREAT (pathname);

 //     if (retval < 0) {
 // println ("(Parent) create: File creation error! status: ");

 // my_exit(EXIT_FAILURE);
 //     }

 //     my_memset (pathname, 0, 80);
 //   }

 // }
 // else {
 //   /* Generate 300 regular files */
 //   for (i = 0; i < 300; i++) {
 //			set_pathname(pathname, i);

 //     retval = CREAT (pathname);

 //     if (retval < 0) {
 // println ("(Child) create: File creation error! status: ");

 // my_exit(EXIT_FAILURE);
 //     }

 //     my_memset (pathname, 0, 80);
 //   }
 // }

#endif // TEST5

  println("Congratulations, you have passed all tests!!\n");

  return 0;
}
