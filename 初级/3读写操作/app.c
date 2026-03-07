#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


#define PATH "/dev/mychar"


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
	
	write(fd,"hello",5);
	write(fd," world",6);

	read(fd,buf,100);
	printf("read:%s\n",buf);


	close(fd);
	fd = -1;
}
