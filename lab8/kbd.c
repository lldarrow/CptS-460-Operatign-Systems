/*************************************
Luke Darrow 11349190
CptS 460 Lab 4 kbd.c
*************************************/

#define KEYBD	         0x60	/* I/O port for keyboard data */
#define PORT_B           0x61   /* port_B of 8255 */
#define KBIT	         0x80	/* bit used to ack characters to keyboard */

#define NSCAN             64	/* Number of scan codes */
#define KBLEN             64    // input buffer size

#define CAPSLOCK         0x3A
#define LSHIFT           0x2A
#define RSHIFT           0x36
#define F1               0x3B
#define F2               0x3C
#define F3               0x3D


#include "keymap.c"

extern int color;

typedef struct kbd{           // data struct representing the keyboard
    char buf[KBLEN];          // input buffer
    int  head, tail;          // CIRCULAR buffer
    SEMAPHORE  data;                // number of keys in bif[ ] 
}KBD;

KBD kbd;

int capslock, shifted;

int kbd_init()
{
  printf("kbinit()\n");

  kbd.head = kbd.tail = 0;
  kbd.data.value = 0;
  kbd.data.queue = 0;

  capslock = shifted = 0;

  printf("kbinit done\n"); 
}

int kbhandler()
{
  int scode, value, c;
  KBD *kp = &kbd;
  
  /* Fetch scan code from the keyboard hardware and acknowledge it. */
  scode = in_byte(KEYBD);	 /* get the scan code of the key */
  value = in_byte(PORT_B);	 /* strobe the keyboard to ack the char */
  out_byte(PORT_B, value | KBIT);/* first, strobe the bit high */
  out_byte(PORT_B, value);	 /* then strobe it low */

  if (scode & 0x80){ //key release check if shift released
     scode &= 0x7F; //get rid of bit 7
     if(scode == LSHIFT || scode == RSHIFT)
         shifted = 0;
     goto out;
  }

  //shift pressed
  if (scode == LSHIFT || scode == RSHIFT){
    shifted = 1; //set flag and go to out
    goto out;
  }

  //capslock pressed
  if (scode == CAPSLOCK){
    capslock = 1 - capslock; //toggle capslock
    goto out;
  }

  //function keys
  if (scode == F1){
    clear_clock();
    printf("F1 pressed\n");

    kps();
    goto out;
  }
  if (scode == F2){
    clear_clock();
    printf("F2 pressed\n");

    printQueue(freeList);
    goto out;
  }
  if (scode == F3){
    clear_clock();
    printf("F3 pressed\n");

    goto out;
  }

  // translate scan code to ASCII; //using shift[ ] table if shifted;
  if(shifted)
    c = shift[scode];
  else
    c = unshift[scode];

  //check if capslock on
  if(capslock){
    if (c >= 'A' && c <= 'Z')
	c += 'a' - 'A';
    else if (c >= 'a' && c <= 'z')
	c -= 'a' - 'A';
  }

  /* store the character in buf[ ] for process to get */
  if (kbd.data.value >= KBLEN){   // buf[ ] already FULL
    printf("%c\n", 0x07);
    goto out;          
  }

  kbd.buf[kbd.head++] = c;
  kbd.head %= KBLEN;

  V(&kbd.data);

out:
  out_byte(0x20, 0x20); // issue EOI to 8259 Interrupt controller
}

/********************** upper half routine ***********************/ 
int getc()
{
  int c;

  P(&kbd.data);

  lock();
  c = kbd.buf[kbd.tail++] & 0x7F;
  kbd.tail %= KBLEN;
  unlock();

  return c;
}
