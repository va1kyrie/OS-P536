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
  if(flags != O_RDWR && flags != O_RDONLY && flags != O_WRONLY){
    fprintf(stderr, "Incorrect flags given\n");
    return SYSERR;
  }

  //check filename against names of files in directory
  int i = 0;
  int j = 0;
  while(i<fsd.root_dir.numentries && strncmp(fsd.root_dir.entry[i].name, filename, FILENAMELEN) != 0){
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
  status = fs_get_inode_by_num(0, fsd.root_dir.entry[i].inode_num, &node);
  if(status == SYSERR){
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
  //first check if fd is valid
  if(fd >= NUM_FD || fd < 0){
    fprintf(stderr, "Invalid index given; cannot close file.\n");
    return SYSERR;
  }

  //check that file is actually open
  if(oft[fd].state == FSTATE_OPEN){
    oft[fd].state = FSTATE_CLOSED;
    oft[fd].fileptr = 0;
    return OK;
  }else if(oft[fd].state == FSTATE_CLOSED){
    //else if the file is closed just exit
    return OK;
  }
  //if we're still here, there's something wrong.
  return SYSERR;
}

int fs_create(char *filename, int mode) {
  //create file with filename in given mode
  //what does the mode do??

  int index = -1;
  int status = -1;
  struct inode node;
  //if mode is anything but O_CREAT return an error
  if(mode != O_CREAT){
    fprintf(stderr, "Incorrect mode input given\n");
    return SYSERR;
  }
  //check filename -- if it exists, return an error
  int i;
  for(i=0;i<fsd.root_dir.numentries;i++){
    if(strncmp(fsd.root_dir.entry[i].name, filename, FILENAMELEN) == 0){
      fprintf(stderr, "Filename already exists\n");
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

  status = fs_get_inode_by_num(0, fsd.inodes_used+1, &node);
  if(status == SYSERR){
    fprintf(stderr, "Error retrieving inode block; could not open file\n");
    return SYSERR;
  }

  //increment inodes_used if status passed
  fsd.inodes_used++;

  //create directory entries and structures
  fsd.root_dir.numentries++;
  fsd.root_dir.entry[fsd.root_dir.numentries-1].inode_num = fsd.inodes_used;
  strncpy(fsd.root_dir.entry[fsd.root_dir.numentries-1].name, filename, FILENAMELEN);

  //now openfiletable entry
  //this is just opening the file i think
  index = fs_open(filename, O_RDWR);

  //get the inode stuff set up
  oft[index].in.type = INODE_TYPE_FILE;
  oft[index].in.device = 0;

  //returns the file index of the file?
  //yes. yes it does.
  //to be specific, it returns the open file table entry.
  return index;
}

int fs_seek(int fd, int offset) {
  //search in file by offest (move offset bytes back)

  //check if fd is valid
  if(fd < 0 || fd >= NUM_FD){
    printf("Invalid file descriptor given; cannot seek through file\n");
    return SYSERR;
  }

  //find file
  //fd is the entry in the open file table (i think)
  if(oft[fd].state == FSTATE_OPEN){
    fprintf(stderr, "File must be open in order to seek through it\n");
    return SYSERR;
  }
  //check offset
  if(oft[fd].fileptr+offset < 0){
    fprintf(stderr, "File pointer may not point before the start of the file\n");
    return SYSERR;
  }

  //oh jeez how do you check the size of the file??? i don't think you can
  //well that is a huge bug but i'm not fixing it right now

  //move pointer back/forward by offset
  oft[fd].fileptr += offset;

  //return the new file pointer
  return oft[fd].fileptr;
}

int fs_read(int fd, void *buf, int nbytes) {
  //read nbytes bytes through file at fd to the buffer pointed to by *buf

  //find file
  if(fd >= NUM_FD || fd < 0){
    fprintf(stderr, "Invalid table index given; cannot read file\n");
    return SYSERR;
  }

  //check nbytes is valid
  if(nbytes < 1){
    fprintf(stderr, "Cannot read less than 1 byte from file; invalid number of bytes to read given\n");
    return SYSERR;
  }

  //get filename so we can open the file
  //this is assuming so many things but I have no idea how else to do this

  //open file
  if(oft[fd].state != FSTATE_OPEN){
    fprintf(stderr, "fs_read: File not open or not available\n");
    return SYSERR;
  }
  //read file
  //get blocks to read
  int blocksread = (nbytes + oft[fd].fileptr) / MDEV_BLOCK_SIZE;

  if((nbytes + oft[fd].fileptr) % MDEV_BLOCK_SIZE != 0){
    blocksread++;
  }

  //find starting block
  int ind = oft[fd].fileptr / MDEV_BLOCK_SIZE;
  int blind;
  int status = -1;
  int bytesr = 0;
  int tmp = 0;

  while(ind < blocksread){
    blind = oft[fd].fileptr % MDEV_BLOCK_SIZE;
    memset(block_cache, NULL, MDEV_BLOCK_SIZE);
    status = bs_bread(0, oft[fd].in.blocks[ind], blind, block_cache, MDEV_BLOCK_SIZE-blind);
    if(status == SYSERR){
      fprintf(stderr, "Error in reading file\n");
      return SYSERR;
    }

    strncpy((buf+tmp), block_cache, MDEV_BLOCK_SIZE);
    tmp = strlen(block_cache);
    bytesr += tmp;

    ind++;
  }

  oft[fd].fileptr = bytesr;
  return bytesr;
}

int min(int x, int y){
  //return the smaller value. if they are equal, the second value is returned.
  if(x < y){
    return x;
  }
  return y;
}

int fs_write(int fd, void *buf, int nbytes) {
  //write nbytes from *buf to the file at index fd.
  if(fd >= NUM_FD || fd < 0){
    fprintf(stderr, "Invalid index given; cannot write to file\n");
    return SYSERR;
  }

  //check nbytes is valid
  if(nbytes < 1){
    fprintf(stderr, "Invalid number of bytes to write given; cannot write less than 1 byte\n");
    return SYSERR;
  }

  //create opens the file so we're assuming it's already open
  if(oft[fd].state != FSTATE_OPEN){
    fprintf(stderr, "fs_write: File not open or not available\n");
    return SYSERR;
  }

  int blockwrite = nbytes / MDEV_BLOCK_SIZE;
  if(nbytes % MDEV_BLOCK_SIZE != 0){
    blockwrite++;
  }

  int status = -1;
  int bytesw = 0;
  int blind = FIRST_INODE_BLOCK + NUM_INODE_BLOCKS;
  int i = 0;
  int minb;
  int indb = nbytes;

  while(i<blockwrite && blind<MDEV_BLOCK_SIZE){
    if(fs_getmaskbit(blind) == 0){
      memset(block_cache, NULL, MDEV_BLOCK_SIZE);
      status = bs_bwrite(0, blind, 0, block_cache, MDEV_BLOCK_SIZE);
      if(status == SYSERR){
        fprintf(stderr, "Unable to free block; could not write to file\n");
        return SYSERR;
      }

      minb = min(MDEV_BLOCK_SIZE, blockwrite);
      memcpy(block_cache, buf, minb);

      //write data to data block
      status = bs_bwrite(0, blind, 0, block_cache, MDEV_BLOCK_SIZE);
      if(status == SYSERR){
        fprintf(stderr, "Error writing to block; could not write to file\n");
        return SYSERR;
      }

      buf = (char *) buf + minb;
      indb = indb - minb;
      fs_setmaskbit(blind);
      //keep track of data blocks in inode
      oft[fd].in.blocks[i++] = blind;
    }
  }

  status = fs_put_inode_by_num(0, oft[fd].in.id, &oft[fd].in);
  if(status == SYSERR){
    fprintf(stderr, "Error wriing inode; could not write to file\n");
    return SYSERR;
  }

  //same as read but with O_WRONLY flag

  return nbytes;
}

#endif /* FS */
