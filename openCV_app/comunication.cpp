#include "comunication.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

static char *portname = "/dev/ttyACM0";

int Comunication::set_interface_attribs (int fd, int speed, int parity){
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if(tcgetattr(fd, &tty) != 0){
                printf("error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed(&tty, speed);
        cfsetispeed(&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
        tty.c_iflag &= ~IGNBRK;
        tty.c_lflag = 0;

        tty.c_oflag = 0;
        tty.c_cc[VMIN]  = 0;
        tty.c_cc[VTIME] = 5;

        tty.c_iflag &= ~(IXON | IXOFF | IXANY);

        tty.c_cflag |= (CLOCAL | CREAD);

        tty.c_cflag &= ~(PARENB | PARODD);
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0){
                printf("error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void Comunication::set_blocking (int fd, int should_block){
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0){
                printf("error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                printf("error %d setting term attributes", errno);
}

int Comunication::send(int a)
{
    int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0){
        printf("error %d opening %s: %s", errno, portname, strerror (errno));
        return -1;
    }

    set_interface_attribs (fd, 115200, 0);
    set_blocking (fd, 0);

    char* sendData = (char *)malloc((1)*sizeof(char));
    if(a>255)
        a=255;
    std::string s = intToStr(a);
    sendData[0] = atoi(s.c_str());
    write (fd, sendData,1);
    return 1;
}

std::string Comunication::intToStr(int n)
{
     std::string tmp;
     if(n < 0) {
      tmp = "-";
      n = -n;
     }
     if(n > 9)
      tmp += intToStr(n / 10);
     tmp += n % 10 + 48;
     return tmp;
}


