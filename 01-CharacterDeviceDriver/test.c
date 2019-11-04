#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
static char receive[255];
int main()
{
	int ret, fd;
	fd = open("/dev/project02", O_RDWR);           
  	if (fd < 0){
      		perror("Open device failed");
      		return errno;
   	}
	ret = read(fd, receive, 255);        
   	if (ret < 0){
     		perror("Read from device failed");
      		return errno;
   	}
   	printf("Number is: %s\n", receive);

	return 1;
}
