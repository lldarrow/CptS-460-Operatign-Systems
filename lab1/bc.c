/*******************************************
Luke Darrow 11349190
CptS 460 Lab 1
*******************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define  GREEN  10         // color byte for putc()
#define  CYAN   11
#define  RED    12
#define  MAG    13
#define  YELLOW 14

struct partition {         // Partition table entry in MBR
       u8  drive;          // 0x80 - active 
       u8  head;	   // starting head 
       u8  sector;	   // starting sector 
       u8  cylinder;       // starting cylinder 
       u8  type;	   // partition type 
       u8  end_head;       // end head 
       u8  end_sector;	   // end sector 
       u8  end_cylinder;   // end cylinder 
       u32 start_sector;   // starting sector counting from 0 
       u32 nr_sectors;     // nr of sectors in partition 
};

struct dap{                // DAP for extended INT 13-42  
       u8   len;           // dap length=0x10 (16 bytes)
       u8   zero;          // must be 0  
       u16  nsector;       // number of sectors to read: 1 to 127
       u16  addr;          // memory address = (segment:addr)
       u16  segment;    
       u32  s1;            // low  4 bytes of sector#
       u32  s2;            // high 4 bytes of sector#
};

struct dap dap, *dp;       // global dap struct
u16 color = RED;           // initial color for putc()

#define  BOOTSEG 0x9000

#include "io.c" //<========= WRITE YOUR OWN io.c file !!!!

char mbr[512];
char ans[64];
int partition, bsector;

// load a disk sector to (DS, addr), where addr is an offset in segment
int getSector(u32 sector, char *addr)
{
  dp->addr    = addr;
  dp->s1      = (u32)sector;
  diskr();    // call int13-43 in assembly
}

int main()
{
  int i;
  struct partition *p;
  printf("booter start in main()\n");

  // initialize the dap struct
  dp = &dap;
  dp->len  = 0x10;        // must be 16
  dp->zero = 0;           // must be 0
  dp->nsector = 1;        // load one sector
  dp->addr = 0;           // will set to addr              
  dp->segment = BOOTSEG;  // load to which segment
  dp->s1 = 0;             // will set to LBA sector#
  dp->s2 = 0;             // high 4-byte address s2=0

  getSector((u32)0, (u16)mbr); // get MBR
  //==========================================================
  //In the MBR, partition table begins at byte offset 0x1BE
  // DO #1:  WRITE C CODE TO PRINT THE PARTITIONS
  //==========================================================

  //start of print partitions
  printf("show partition table\n");
  p = (struct partition *)(&mbr[0x1be]);//get first partition from byte offset 0x1BE
  printf("p#  type    start_sector  nr_sectors\n");
  printf("------------------------------------\n");

  //for each partition print partition number, type, start sector, and the number of sectors in partition
  for (i=1; i<=4; i++){
    printf("%d  %x  %l  %l\n", i, p->type, p->start_sector, p->nr_sectors);
    p++;//next partition
  }
  printf("------------------------------------\n");

  //end of print partitions

  color = CYAN;

  while(1)
  {
    printf("what's your name? ");
    gets(ans);
    if (strcmp(ans, "quit")==0)
    {
      printf("\nexit main()\n");
      break;
    }
    printf("\nWelcome %s!\n", ans);
  }
}

/*====================================================================
DO #2: The bs.s file contains
          char getc()         : return a char from keyboard
          int  putc(char c)   : display a char to screen  
   which are supported by BIOS. There are NO OTHER terminal I/O functions!!!

   Write your OWN gets(char s[ ]) fucntion to get a string.
   Write your OWN printf(char *fmt, ...) as in CS360

Then, include YOUR io.c file in bc.c. 
      Do NOT use KCW's io.o in linking step.
===================================================================*/
