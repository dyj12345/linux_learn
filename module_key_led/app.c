#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>


int on ;
int ledn;

int main(int argc,char** argv)
{
	printf("app!\n");

	if(argc > 3|| sscanf(argv[2], "%d",&ledn)!=1||sscanf(argv[1],"%d",&on)!=1)
	{
		printf("error: format: ./app on ledn\n");
		return -1;
	}
	int fd = open("/dev/key_led",O_RDWR);
	
	printf("%d-%d\n",on,ledn);
	ioctl(fd, ledn, on);
	
	close(fd);
	return 0;
}



