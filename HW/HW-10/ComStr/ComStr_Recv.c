#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include<fcntl.h>

int main(void)
{
    int fd, f_stop = 0;
    char buf[512] = { 0 };
    struct termios opt;

    fd = open("/dev/ttySAC3", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        printf("COM open error!\n");
        return 0;
    }
    //get current configuration of COM
    tcgetattr(fd, &opt);
    tcflush(fd, TCIFLUSH);
    //set speed(115200Hz)
    cfsetispeed(&opt, B115200);//input
    cfsetospeed(&opt, B115200);//output
    //data bits(8bits)
    opt.c_cflag &= ~CSIZE;
    opt.c_cflag |= CS8;
    //set chk
    opt.c_cflag &= ~PARENB;
    opt.c_iflag &= ~INPCK;
    //set stop flag
    opt.c_cflag &= ~CSTOPB;
    //time out(150s)
    opt.c_cc[VTIME] = 150;
    opt.c_cc[VMIN] = 0;
    //write conf to COM
    tcsetattr(fd, TCSANOW, &opt);
    tcflush(fd, TCIFLUSH);

    do
    {
	read(fd, buf, 1);
        if (buf[0] == '!')
        {
            read(fd, buf, 6);
            if (!strncmp(buf, "[QUIT]", 6))
                f_stop = 1;
        }
        else if (buf[0])
        {
            printf("%c", buf[0] + 1);
	    buf[0] = 0;
	}
    } while (!f_stop);
    close(fd);

    return 0;
}