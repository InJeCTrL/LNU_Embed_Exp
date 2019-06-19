#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<termios.h>
#include<string.h>
#include<fcntl.h>
#include<pthread.h>

void* th_Recv(void *data)
{
    char buf[512] = { 0 };
    int f_stop = 0;

    do
    {
        read(*(int*)data, buf, 1);
	if (buf[0])
	{
            printf("%c", buf[0]);
	    buf[0] = 0;
	}
    } while (1);
    
    return 0;
}
void* th_Send(void *data)
{
    char buf[512] = { 0 };
    int f_stop = 0;

    do
    {
        printf("Input to send(\"![QUIT] to quit\"):\n");
        scanf("%s", buf);
        if (!strncmp(buf, "![QUIT]", 7))
            f_stop = 1;
        else
            write(*(int*)data, buf, strlen(buf));
    } while (!f_stop);

    return 0;
}
int main(void)
{
    int fd;
    struct termios opt;
    pthread_t thread_Recv, thread_Send;
    void *ret;

    fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
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

    pthread_create(&thread_Recv, NULL, th_Recv, &fd);
    pthread_create(&thread_Send, NULL, th_Send, &fd);
    pthread_join(thread_Send, &ret);
    pthread_cancel(thread_Recv);
    
    close(fd);

    return 0;
}