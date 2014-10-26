
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>

/* baudrate settings are defined in <asm/termbits.h>, which is
included by <termios.h> */
#define BAUDRATE B9600          
#define DEVICE "/dev/ttyAMA0"
#define _POSIX_SOURCE 1

#define FALSE 0
#define TRUE 1

volatile int STOP=FALSE; 

main()
{
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

    fd = open(DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK); 
    if (fd < 0)
    {
        return -1;
    }
    
    tcgetattr(fd,&oldtio); /* save current serial port settings */
    bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
 
    newtio.c_iflag = IGNPAR;
 
    newtio.c_oflag = IGNPAR;
 
    newtio.c_lflag = 0;
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
    newtio.c_cc[VTIME]    = 10;   /* inter-character timer unused */

    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

    while (STOP == FALSE)
    {
        printf("write\n");
        write(fd, "AT+BAUD?");
        printf("blocked.\n");
        sleep(1);
        res = read(fd, buf, 255);
        printf("unblocked\n");
        if (res > -1)
        {
            buf[res]=0;             /* set end of string, so we can printf */
            printf(":%s:%d\n", buf, res);
            if (buf[0]=='z') STOP=TRUE;
        } 
    }

    tcsetattr(fd,TCSANOW,&oldtio);
};
