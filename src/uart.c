#include "uart.h"

int uart_fd = -1;
struct termios last_cfg = {0};
struct termios next_cfg = {0};

int uart_init(void)
{
    int ret = 0;

    if(uart_fd != -1) 
    {
        printf("reinit uart device\n");
        close(uart_fd);
        uart_fd = -1;
    }

    uart_fd = open("/dev/ttySTM0", O_RDWR | O_NOCTTY);  //O_NOCTTY 表示该终端不会成为进程的控制终端
    {
        perror("open uart device failed");
        return -1;
    }

    ret = tcgetattr(uart_fd, &last_cfg);
    if(ret == -1)
    {
        printf("get uart device info failed\n");
        close(uart_fd);
        return -1;
    }

    memset(&next_cfg, 0, sizeof(struct termios));

    // 原始模式
    cfmakeraw(&next_cfg);

    // 接收使能
    next_cfg.c_cflag |= CREAD;

    // 波特率
    //cfsetispeed(&next_cfg, B115200);
    //cfsetospeed(&next_cfg, B115200);
    cfsetspeed(&next_cfg, B115200);

    // 数据位
    next_cfg.c_cflag &= ~CSIZE;
    next_cfg.c_cflag |= CS8;

    #if 0   // 奇校验
    next_cfg.c_cflag |= (PARODD | PARENB);
    next_cfg.c_iflag |= INPCK;
    #elif 0 // 奇校验
    next_cfg.c_cflag |= PARENB;
    next_cfg.c_cflag &= ~PARODD;
    next_cfg.c_iflag |= INPCK;
    #else   // 无校验
    next_cfg.c_cflag &= ~PARENB;
    next_cfg.c_iflag &= ~INPCK;
    #endif

    #if 1   // 1位停止位
    next_cfg.c_cflag &= ~CSTOPB;
    #else   // 2位停止位
    next_cfg.c_cflag |= CSTOPB;
    #endif

    next_cfg.c_cc[VTIME] = 0;
    next_cfg.c_cc[VMIN] = 0;

    ret = tcflush(uart_fd, TCIOFLUSH);
    if(ret == -1)
    {
        printf("clear uart device buffer failed\n");
        close(uart_fd);
        return -1;
    }

    ret = tcsetattr(uart_fd, TCSANOW, &next_cfg);
    if(ret == -1)
    {
        printf("set uart device info failed\n");
        close(uart_fd);
        return -1;
    }
}

void uart_deinit(void)
{
    if(uart_fd != -1) 
    {
        printf("deinit uart device\n");
        close(uart_fd);
        uart_fd = -1;
    }
}