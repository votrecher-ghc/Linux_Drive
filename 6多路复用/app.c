#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/select.h>

#include "mychar.h"

#define PATH "/dev/mychar"


int main(int argc ,char ** argv){
	
	char * path = PATH;
	if(argc == 2){
		path = argv[1];
	}

	char buf[1024] = {0};
	int fd = -1;

	fd_set rfds;
	int ret = -1;

	fd = open(path,O_RDWR);

	if(fd == -1){
		printf("open /dev/mychar failed\n");
		return -1;
	}

	while(1){
		FD_ZERO(&rfds);
		FD_SET(fd,&rfds);
		ret = select(fd+1,&rfds,NULL,NULL,NULL);
		if(ret <0){
			if(errno = EINTR){
				continue;
			}
			else{
				printf("select error\n");
				break;
			}
		}

		if(FD_ISSET(fd,&rfds)){
			read(fd,buf,sizeof(buf));
			printf("buf = %s\n",buf);
		}
	}

	close(fd);
	fd = -1;
}
