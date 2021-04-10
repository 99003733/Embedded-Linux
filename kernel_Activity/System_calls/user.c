#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/syscall.h>
#include<stdlib.h>

int main()
{
  char stri [256] ="this is system call string";
printf("the syscall is %ld \n", syscall(398, stri));
return 0;

}
