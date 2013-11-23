#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void){
	
	int count = 40;//strlen(str)+1;
	int i,j;

	


	int integer = 12345;
	int integer2 = 1234567;
	char str[1024]= "";
	sprintf(str, "%d",integer);

	char str2[1024]= "";
	sprintf(str2, "%d",integer2);

	//printf("%d\n",count);
	int fd = open("/dev/osqueue", O_RDWR);

	
	char* l_str = (char*)malloc(count);	
	
	j = read(fd, l_str, count);
	if(j<0){
		printf("error in read: %d\n",j);
	}else
	printf("it worked: %d\n",atoi(l_str));

	i = write(fd,str, count);
	if(i<0){
		printf("error in write: %d\n",i);
	}else printf("1st write: %d\n",atoi(str));

	i = write(fd,str2, count);
	if(i<0){
		printf("error in write: %d\n",i);
	}else printf("2nd write: %d\n",atoi(str2));

	
	j = read(fd, l_str, count);
	if(j<0){
		printf("error in read: %d\n",i);
	}else
	printf("1st read: %d\n",atoi(l_str));

	
	
	
	j = read(fd, l_str, count);
	if(j<0){
		printf("error in read: %d\n",i);
	}else
	printf("2nd read: %d\n",atoi(l_str));


	j = read(fd, l_str, count);
	if(j<0){
		printf("error in read: %d\n",i);
	}else
	printf("3rd read: %d\n",atoi(l_str));

	j = read(fd, l_str, count);
	if(j<=0){
		printf("error in read: %d\n",i);
	}else
	printf("%d\n",atoi(l_str));

	j = read(fd, l_str, count);
	if(j<=0){
		printf("error in read: %d\n",i);
	}else
	printf("%d\n",atoi(l_str));

	return 0;
}
