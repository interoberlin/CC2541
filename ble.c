
// http://en.wikibooks.org/wiki/Serial_Programming/termios

#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

struct termios config;

const char *device = "/dev/ttyAMA0";
int fd;
fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
if (fd == -1)
{
    printf("Error: Failed to open port.\n");
}

if (cfsetispeed(&config, B9600) < 0 || cfsetospeed(&config, B9600) < 0)
{
    printf("Error: Setting baud failed.\n");
}

write(fd, "AT");
printf("%s\n", read(fd));

close(fd);
