#include<linux/fs.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

 

int main()
{
    int maxlen=64;
    int nbytes;
    int fd;

 


    fd=open("/dev/psample", O_RDWR);
    if(fd<0) 
    {
    perror("open");
    }

 


    char str[]="abcdxyz";
    nbytes=write(fd,str,8);
    printf("nbytes %d", nbytes);

 

    if(nbytes<0) 
    {
    perror("write");
    }

 

    nbytes=read(fd,str,maxlen);
    printf("nbytes %d", nbytes);
    if(nbytes<0) 
    {
    perror("write");
    }
    write(1,str,maxlen);
    str[nbytes]='\0';
    puts(str);

 

    close(fd);
}
