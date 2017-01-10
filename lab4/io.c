/********************************
Luke Darrow 11349190
CptS 460 Lab 2

helper functions
********************************/

typedef unsigned int u16;
typedef unsigned long u32;

char *ctable = "0123456789ABCDEF";
u16 BASE = 10; // for decimal numbers

int rgets(char buf[])
{
    int i = 0;
    char c = getc();
    while (c != '\r')
    {
        buf[i] = c;
        putc(c);
        c = getc();
        i++;
    }
    buf[i] = 0;
}

int rpu(u16 x)
{
  char c;
  if (x)
  {
    c = ctable[x % BASE];
    rpu(x / BASE);
    putc(c);
  }
}

int printu(u32 x)
{
  if (x==0)
     putc('0');
  else
     rpu(x);
  putc(' ');
}

void prints(char *s)
{
  while(*s)
  {
    putc(*s);
    s++;
  }
}

int printd(int x)
{
    char c;

  if(x == 0)
  {
    putc('0');
    return 0;
  }
  if (x < 0)
  {
    x = x*-1;
    prints("-");
  }
  if (x){
     c = ctable[x % BASE];
     rpu(x);
  }
}

int printf(char *fmt, ...) // some C compiler requires the three dots
{
 char *cp = fmt; // cp points to the fmt string
 u16 *ip = (u16 *)&fmt + 1; // ip points to first item
 u32 *up; // for accessing long parameters on stack
 while (*cp)
 { // scan the format string
   if (*cp != '%')
   { // spit out ordinary chars
     putc(*cp);
     if (*cp=='\n') // for each ‘\n’
       putc('\r'); // print a ‘\r’
     cp++; continue;
   }
 cp++; // print item by %FORMAT symbol
 switch(*cp){
  case 'c' : putc(*ip); break;
  case 's' : prints(*ip); break;
  case 'u' : printu(*ip); break;
  case 'd' : printd(*ip); break;
  }
 cp++; ip++; // advance pointers
 }
}
