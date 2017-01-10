int printf(char *fmt, ...) // some C compiler requires the three dots
{
  char *cp = fmt;             // cp points to the fmt string
  u16  *ip = (u16 *)&fmt + 1; // ip points to first item
  u32  *up;             // for accessing long parameters on stack
  while (*cp){                // scan the format string
    if (*cp != '%'){         // spit out ordinary chars
      putc(*cp);
      if (*cp=='\n')       // for each ‘\n’
        putc('\r');       // print a ‘\r’
      cp++; continue;
    }
    cp++;                    // print item by %FORMAT symbol         
    switch(*cp){
      case 'c' :   putc(*ip);  break;
      case 's' : prints(*ip);  break;
      case 'u' : printu(*ip);  break;
      case 'd' : printd(*ip);  break;
 //     case 'x' : printx(*ip);  break;
 //     case 'l' : printl(*(u32 *)ip++);  break; 
 //     case 'X' : printX(*(u32 *)ip++);  break; 
    }
    cp++; ip++;              // advance pointers
  }
}

char *ctable = "0123456789ABCDEF";
u16 BASE = 10; // for decimal numbers  
int rpu(u16 x)
{   char c;
  if (x){ 
    c = ctable[x % BASE]; 
    rpu(x / BASE);
    putc(c);
  }
}

int prints(char *x)
{
  while(*x)
  {
    putc(*x);
    x++;
  }
}
int printu(u16 x)
{  
  (x==0)? putc('0') : rpu(x);  
  putc(' ');
} 

int printd(u16 x)
{
  if(x == 0)
  {
    putc('0');
    return 0;
  }
  if (x<0)
  {
    putc('-');
    x *= -1;
  }
  else
  {
    rpu(x);
  }
  putc(' ');

}

int printc(u16 x)
{
  if(x == 0)
  {
    putc('0');
    return 0;
  }
  if (x<0)
  {
    putc('-');
    x *= -1;
  }
  else
  {
    rpu(x);
  }
  putc(' ');

}

int gets(char buf[])
{
	int i = 0;
	char c = getc();
	while(c != '\r')
	{
		buf[i] = c;
		putc(c);
		c = getc();
 		i++;
	}
	buf[i] = 0;
}


int stoi(char *ch)
{
	int total = 0, i = 0;
	while(*ch != 0)
	{
		total = total * 10;
		for(i = 0; i < 10; i++)
		{
			if(ctable[i] == *ch)
			{
				total = total + i;
			}
		}
		ch++;
	}
	return total;
}

