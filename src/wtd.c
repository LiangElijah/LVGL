#include "wtd.h"

int wtd_fd = -1;
struct watchdog_info wtd_info;
WTD_STU wtd_stu = WTD_DISABLE;

int wtd_init(void)
{
    int ret;

    if(wtd_fd != -1) 
    {
        printf("reinit wtd device\n");
        close(wtd_fd);
        wtd_fd = -1;
    }
    
    wtd_fd = open("/dev/watchdog0", O_RDWR);
    if(wtd_fd == -1)
    {
        perror("open wtd device failed");
        return -1;
    }

    wtd_get_info();
    
    ret = wtd_set_status(WTD_DISABLE);
    if(ret == -1)
    {
        printf("set wtd disable failed\n");
        close(wtd_fd);
        wtd_fd = -1;
        return -1;
    }

    return 0;
}

void wtd_deinit(void)
{
    if(wtd_fd != -1) 
    {
        printf("deinit wtd device\n");
        close(wtd_fd);
        wtd_fd = -1;
    }
}

void wtd_get_info(void)
{
    int ret;

    if(wtd_fd == -1) 
    {
        printf("wtd device not init\n");
        return;
    }

    ret = ioctl(wtd_fd, WDIOC_GETSUPPORT, &wtd_info);
    if(ret == -1)
    {
        perror("ioctl wtd device failed [WDIOC_GETSUPPORT]");
        return;
    }

    printf("ID:%s\n", wtd_info.identity);
    printf("版本:%u\n", wtd_info.firmware_version);

    if((wtd_info.options & WDIOF_KEEPALIVEPING) == 0)
    {
        printf("设备不支持喂狗\n");
    }
    if((wtd_info.options & WDIOF_SETTIMEOUT) == 0)
    {
        printf("设备不支持定时\n");
    }
}

int wtd_set_time(int time)
{
    int ret;

    if(wtd_fd == -1) 
    {
        printf("wtd device not init\n");
        return -1;
    }

    ret = ioctl(wtd_fd, WDIOC_SETTIMEOUT, &time);
    if(ret == -1)
    {
        perror("ioctl wtd device failed [WDIOC_SETTIMEOUT]");
        return -1;
    }

    printf("当前定时时间：%d\n", time);

    return 0;
}

void wtd_get_time(int *time)
{
    int ret;

    if(wtd_fd == -1) 
    {
        printf("wtd device not init\n");
        return;
    }

    ret = ioctl(wtd_fd, WDIOC_GETTIMEOUT, time);
    if(ret == -1)
    {
        perror("ioctl wtd device failed [WDIOC_GETTIMEOUT]");
        return;
    }
}

int wtd_set_status(WTD_STU status)
{
    int ret;
    int options;

    if(wtd_fd == -1) 
    {
        printf("wtd device not init\n");
        return -1;
    }

    switch (status)
    {
    case WTD_ENABLE:
        options = WDIOS_ENABLECARD;
        break;
    case WTD_DISABLE:
        options = WDIOS_DISABLECARD;
        break;
    default:
        printf("unknown watchdog status\n");
        return -1;
    }

    ret = ioctl(wtd_fd, WDIOC_SETOPTIONS, &options);
    if(ret == -1)
    {
        perror("ioctl wtd device failed [WDIOC_SETOPTIONS]");
        return -1;
    }

    wtd_stu = status;

    return 0;
}

void wtd_get_status(WTD_STU *status)
{
    *status = wtd_stu;
}

void wtd_keepalive(void)
{
    int ret;

    if(wtd_fd == -1) 
    {
        printf("wtd device not init\n");
        return;
    }

    ret = ioctl(wtd_fd, WDIOC_KEEPALIVE, NULL);
    if(ret == -1)
    {
        perror("ioctl wtd device failed [WDIOC_KEEPALIVE]");
        return;
    }
}