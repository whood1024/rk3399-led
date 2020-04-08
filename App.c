#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <string.h> 

int main(int argc, char **argv) 
{ 
	int fd; 
	char status;
	/* 1. 判断参数  */ 
	if (argc < 2) 
	{ 
		  printf("Usage: %s on: led on\n", argv[0]); 
		  printf("       %s off: led off\n", argv[0]); 
		  return -1; 
	}

	//打开设备文件
	fd = open("/dev/hello_gpio", O_RDWR);
	if(fd == -1)
	{
		printf("can not open file /dev/hello_gpio\n"); 
		return -1; 
	}
	/* 3. 写文件或读文件  */ 
	if((0 == strcmp(argv[1], "on")) && (argc == 2))
	{ 
		status = 1;
		write(fd, &status, 1); 
	}
	else if((0 == strcmp(argv[1], "off")) && (argc == 2))
	{ 
		status = 0;
		write(fd, &status, 1); 
	}
	else
	{
		printf("Input error, %s %s LINE:%d", __FILE__, __FUNCTION__, __LINE__);
		close(fd);
		return -1;
	}
	close(fd); 
	return 0; 
}