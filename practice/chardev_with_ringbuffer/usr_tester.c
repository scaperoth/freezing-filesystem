#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void){
	
	//printf("%d\n",count);
	int fd = open("/dev/osqueue", O_RDWR);

	close(fd);

	return 0;
}
