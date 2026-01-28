#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "mychar.h"

#define PATH "/dev/mychar"

void my_ioctl(int fd){
	int max = 0;
	int cur = 0;
	ioctl(fd,MY_CHAR_IOCTL_GET_MAXLEN,&max);
	ioctl(fd,MY_CHAR_IOCTL_GET_CURLEN,&cur);
	printf("max len:%d,cur len:%d\n",max,cur);
}


int main(int argc ,char ** argv){
	
	char * path = PATH;
	if(argc == 2){
		path = argv[1];
	}


	char buf[1024] = {0};
	int fd = -1;
	fd = open(path,O_RDWR);
	if(fd == -1){
		printf("open /dev/mychar failed\n");
		return -1;
	}
	my_ioctl(fd);
	write(fd,"hello",5);
	my_ioctl(fd);
	write(fd," world",6);
	my_ioctl(fd);

	read(fd,buf,100);
	printf("read:%s\n",buf);

	my_ioctl(fd);

	close(fd);
	fd = -1;
}
