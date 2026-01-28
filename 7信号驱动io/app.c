#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <signal.h>

#include "mychar.h"

#define PATH "/dev/mychar"


int fd = -1;

void sig_handler(int sig)
{
	printf("get signal, signum:%d\n",sig);
	char buf[1024] = {0};
	read(fd,buf,sizeof(buf));
	printf("read:%s\n",buf);
}

int main(int argc ,char ** argv){
	
	char * path = PATH;
	if(argc == 2){
		path = argv[1];
	}

	int flg;
	signal(SIGIO,sig_handler);

	fd = open(path,O_RDWR);

	if(fd == -1){
		printf("open /dev/mychar failed\n");
		return -1;
	}

	fcntl(fd,F_SETOWN,getpid());
	flg = fcntl(fd,F_GETFL);
	flg |= FASYNC;
	fcntl(fd,F_SETFL,flg);

	while(1);

	close(fd);
	fd = -1;
}
