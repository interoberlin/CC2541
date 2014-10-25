
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main() {
	int ttyfd = open("/dev/ttyAMA0", O_RDWR);
	printf("fd: %d\n", ttyfd);

	struct termios attribs;
    speed_t speed;
    /*
     * Get the current settings. This saves us from
     * having to initialize a struct termios from
     * scratch.
     */
    if(tcgetattr(ttyfd, &attribs) < 0)
    {
        perror("stdin");
        return EXIT_FAILURE;
    }
    /*
     * Set the speed data in the structure
     */
    if(cfsetispeed(&attribs, B9600) < 0)
    {
        perror("invalid baud rate");
        return EXIT_FAILURE;
    }

if(tcsetattr(ttyfd, TCSANOW, &attribs) < 0)
    {
        perror("stdin");
        return EXIT_FAILURE;
    }

        int res, STOP = 0;
	char buf[255];
        while(STOP == 0)
        {
	write(ttyfd, "AT");
            while((res = read(ttyfd,buf,255)) == 0);
            {
                if(res > 0)
                {
                    buf[res]=0;
                    printf("%s:%d\n", buf, res);
                    if(buf[sizeof(buf)]=='\n') break;   
                }
            }
        }
	
	return 0;
}

