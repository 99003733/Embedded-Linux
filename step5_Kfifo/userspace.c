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
    int maxlen=11;
    int nbytes,res;
    int fd;

    fd=open("/dev/psample", O_RDWR);
    if(fd<0) 
    {
    perror("open");
    }

    char str[]="I am Aditya";
    nbytes=write(fd,str,11);

    if(nbytes<0) 
    {
    	perror("write");
    }


    nbytes=read(fd,str,maxlen);
    if(nbytes<0) 
    {
    	perror("write");
    }
    write(1,str,maxlen);
    str[nbytes]='\0';
    puts(str);

 

    close(fd);
}
