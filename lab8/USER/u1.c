/***********************************
Luke Darrow 11349190
Cpts 460
u1.c
first user
***********************************/
#include "ucode.c"

int color;

main()
{ 
  char name[64];
  int pid, cmd;
  int i, j;

  while(1){
    pid = getpid();
    color = 0x000B + (pid % 5);
       
    printf("----------------------------------------------\n");
    printf("I am proc %d in U mode: running segment=%x\n",getpid(), getcs());
    show_menu();
    printf("Command ? ");
    gets(name); 
    if (name[0]==0) 
        continue;

    //add a LARGE delay loop here

    cmd = find_cmd(name);
    switch(cmd){
           case 0 : getpid();   break;
           case 1 : ps();       break;
           case 2 : chname();   break;
           case 3 : kfork();    break;
           case 4 : kswitch();  break;
           case 5 : wait();     break;
           case 6 : exit();     break;
           case 7 : fork();     break;
           case 8 : exec();     break;

           case 9 : pipe();     break;
           case 10 : pfd();     break;
           case 11 : read_pipe();      break;
           case 12 : write_pipe();     break;
           case 13 : close_pipe();     break;
           case 14 : sleep();     break;

           //print invalid command error message
           default: invalid(name); break;
    }
  }
}




