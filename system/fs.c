#include <xinu.h>
#include <kernel.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#ifdef FS
#include <fs.h>

static struct fsystem fsd;
int dev0_numblocks;
int dev0_blocksize;
char *dev0_blocks;

extern int dev0;

char block_cache[512];

#define SB_BLK 0
#define BM_BLK 1
#define RT_BLK 2

#define NUM_FD 16
struct filetable oft[NUM_FD]; //open file table
int next_open_fd = 0;


#define INODES_PER_BLOCK (fsd.blocksz / sizeof(struct inode))
#define NUM_INODE_BLOCKS (( (fsd.ninodes % INODES_PER_BLOCK) == 0) ? fsd.ninodes / INODES_PER_BLOCK : (fsd.ninodes / INODES_PER_BLOCK) + 1)
#define FIRST_INODE_BLOCK 2

int fs_fileblock_to_diskblock(int dev, int fd, int fileblock) {
  int diskblock;

  if (fileblock >= INODEBLOCKS - 2) {
    printf("No indirect block support\n");
    return SYSERR;
  }

  diskblock = oft[fd].in.blocks[fileblock]; //get the logical block address

  return diskblock;
}

/* read in an inode and fill in the pointer */
int
fs_get_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;
  int inode_off;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_get_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  inode_off = inn * sizeof(struct inode);

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  printf("inn*sizeof(struct inode): %d\n", inode_off);
  */

  bs_bread(dev0, bl, 0, &block_cache[0], fsd.blocksz);
  memcpy(in, &block_cache[inode_off], sizeof(struct inode));

  return OK;

}

int
fs_put_inode_by_num(int dev, int inode_number, struct inode *in) {
  int bl, inn;

  if (dev != 0) {
    printf("Unsupported device\n");
    return SYSERR;
  }
  if (inode_number > fsd.ninodes) {
    printf("fs_put_inode_by_num: inode %d out of range\n", inode_number);
    return SYSERR;
  }

  bl = inode_number / INODES_PER_BLOCK;
  inn = inode_number % INODES_PER_BLOCK;
  bl += FIRST_INODE_BLOCK;

  /*
  printf("in_no: %d = %d/%d\n", inode_number, bl, inn);
  */

  bs_bread(dev0, bl, 0, block_cache, fsd.blocksz);
  memcpy(&block_cache[(inn*sizeof(struct inode))], in, sizeof(struct inode));
  bs_bwrite(dev0, bl, 0, block_cache, fsd.blocksz);

  return OK;
}

int fs_mkfs(int dev, int num_inodes) {
  int i;

  if (dev == 0) {
    fsd.nblocks = dev0_numblocks;
    fsd.blocksz = dev0_blocksize;
  }
  else {
    printf("Unsupported device\n");
    return SYSERR;
  }

  if (num_inodes < 1) {
    fsd.ninodes = DEFAULT_NUM_INODES;
  }
  else {
    fsd.ninodes = num_inodes;
  }

  i = fsd.nblocks;
  while ( (i % 8) != 0) {i++;}
  fsd.freemaskbytes = i / 8;

  if ((fsd.freemask = getmem(fsd.freemaskbytes)) == (void *)SYSERR) {
    printf("fs_mkfs memget failed.\n");
    return SYSERR;
  }

  /* zero the free mask */
  for(i=0;i<fsd.freemaskbytes;i++) {
    fsd.freemask[i] = '\0';
  }

  fsd.inodes_used = 0;

  /* write the fsystem block to SB_BLK, mark block used */
  fs_setmaskbit(SB_BLK);
  bs_bwrite(dev0, SB_BLK, 0, &fsd, sizeof(struct fsystem));

  /* write the free block bitmask in BM_BLK, mark block used */
  fs_setmaskbit(BM_BLK);
  bs_bwrite(dev0, BM_BLK, 0, fsd.freemask, fsd.freemaskbytes);

  return 1;
}

void
fs_print_fsd(void) {

  printf("fsd.ninodes: %d\n", fsd.ninodes);
  printf("sizeof(struct inode): %d\n", sizeof(struct inode));
  printf("INODES_PER_BLOCK: %d\n", INODES_PER_BLOCK);
  printf("NUM_INODE_BLOCKS: %d\n", NUM_INODE_BLOCKS);
}

/* specify the block number to be set in the mask */
int fs_setmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  fsd.freemask[mbyte] |= (0x80 >> mbit);
  return OK;
}

/* specify the block number to be read in the mask */
int fs_getmaskbit(int b) {
  int mbyte, mbit;
  mbyte = b / 8;
  mbit = b % 8;

  return( ( (fsd.freemask[mbyte] << mbit) & 0x80 ) >> 7);
  return OK;

}

/* specify the block number to be unset in the mask */
int fs_clearmaskbit(int b) {
  int mbyte, mbit, invb;
  mbyte = b / 8;
  mbit = b % 8;

  invb = ~(0x80 >> mbit);
  invb &= 0xFF;

  fsd.freemask[mbyte] &= invb;
  return OK;
}

/* This is maybe a little overcomplicated since the lowest-numbered
   block is indicated in the high-order bit.  Shift the byte by j
   positions to make the match in bit7 (the 8th bit) and then shift
   that value 7 times to the low-order bit to print.  Yes, it could be
   the other way...  */
void fs_printfreemask(void) {
  int i,j;

  for (i=0; i < fsd.freemaskbytes; i++) {
    for (j=0; j < 8; j++) {
      printf("%d", ((fsd.freemask[i] << j) & 0x80) >> 7);
    }
    if ( (i % 8) == 7) {
      printf("\n");
    }
  }
  printf("\n");
}


int fs_open(char *filename, int flags) {

  int oftin = -1;
  int status = -10;
  struct inode node;


  //check flags; if not O_RDWR, O_RDONLY, or O_WRONLY, return error.
  if(flags != O_RDWR || flags != O_RDONLY || flags != O_WRONLY){
    fprintf(stderr, "Incorrect flags given\n");
    return SYSERR;
  }

  //check filename against names of files in directory
  int i = 0;
  int j = 0;
  while(i<fsd.root_dir.numentries && strcmp(fsd.root_dir.entry[i].name, filename) != 0){
      //do nothing
      i++;
  }

  //check i to see if file exists
  //if it doesn't, for now, just return an error
  if(i >= fsd.root_dir.numentries){
    fprintf(stderr, "File %s not found\n", filename);
    return SYSERR;
  }

  //else the file does exist
  //first check if file already open
  while(j<NUM_FD && oft[j].in.id != fsd.root_dir.entry[i].inode_num){
    //if it's an empty entry in the table
    if(oft[j].state == FSTATE_CLOSED && oftin == -1){
      oftin = j;
    }
    j++;
  }

  //if the file is in the open table, check its state
  if(oft[j].in.id == fsd.root_dir.entry[i].inode_num){
    if(oft[j].state == FSTATE_OPEN){
      //if the file's open, return an error
      fprintf(stderr, "File %s already open\n", filename);
      return SYSERR;
    }else if(oft[j].state == FSTATE_CLOSED){
      oftin = j;
    }
  }

  //if open file table full, return error
  if(oftin == -1){
    fprintf(stderr, "Open file table full; could not open file\n");
    return SYSERR;
  }

  //now we get to opening the file
  //ok so the file is really the inode << that's where the data is (or will be)
  //so get the inode
  status = fs_get_inode_by_num(0, fsd.root_dir.entry[i].inode_num, &node
  if(status) == SYSERR){
    fprintf(stderr, "Error retrieving inode block; could not open file\n");
    return SYSERR;
  }
  //and return... the id??? the pointer to the node???

  //make open file table entry and return the index
  oft[oftin].state = FSTATE_OPEN;
  oft[oftin].fileptr = 0;
  oft[oftin].de = &fsd.root_dir.entry[i];
  oft[oftin].in = node;

  return oftin;
}

int fs_close(int fd) {
  return SYSERR;
}

int fs_create(char *filename, int mode) {
  //create file with filename in given mode
  //what does the mode do??

  int index = -1;
  int status = -1;
  //if mode is anything but O_CREAT return an error
  if(mode != O_CREAT){
    fprintf(stderr, "Incorrect mode input given\n");
    return SYSERR;
  }
  //check filename -- if it exists, return an error
  int i;
  for(i=0;i<fsd.root_dir.numentries;i++){
    if(strcmp(fsd.root_dir.entry[i].name, filename) == 0){
      fprintf(stderr, "File already exists\n");
      return SYSERR;
    }
  }

  //check that there are inodes available
  //if there are none, return an error
  if(fsd.ninodes <= fsd.inodes_used){
    fprintf(stderr, "All inodes used; could not create file\n");
    return SYSERR;
  }

  //else get an inode
  //find a free one
  i = 0;
  while(i < fsd.ninodes && )

  status = fs_get_inode_by_num(0, fsd.root_dir.entry[i].inode_num, &node
  if(status) == SYSERR){
    fprintf(stderr, "Error retrieving inode block; could not open file\n");
    return SYSERR;
  }

  //get block for new file
  //check bitmask

  //create directory entries and structures

  //returns the file index of the file?
  //yes. yes it does.
  return SYSERR;
}

int fs_seek(int fd, int offset) {
  //search in file by offest (move offset bytes back)

  //find file
  //open file
  //move pointer back/forward by offset
  //check that offset+pointerplace is valid (within the file)
  return SYSERR;
}

int fs_read(int fd, void *buf, int nbytes) {
  //read nbytes bytes through file at fd to the buffer pointed to by *buf

  //find file
  //open file
  //read file
  return SYSERR;
}

int fs_write(int fd, void *buf, int nbytes) {
  //write nbytes from *buf to the file at index fd.
  return SYSERR;
}

#endif /* FS */
