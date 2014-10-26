#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
int main(int argc,char argv[])
{
	int fd;
	fd = open("/dev/key_led",O_RDWR);
	if(fd<0)
		{
			perror("open dev fail!\n");
			return -1;
		}
	close(fd);
	return 0;
}






