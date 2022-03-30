#include "ts.h"

int ts_fd = -1;
struct input_event event = {0};
struct focus_t last = {0};
struct focus_t next = {0};

#if (INPUT_READ_MODE == 2)
nfds_t nfds;
struct pollfd mpollfd;
#elif (INPUT_READ_MODE == 3)
int flags;
#endif

int ts_init(void)
{
    if(ts_fd != -1) 
    {
        printf("reinit ts device\n");
        close(ts_fd);
        ts_fd = -1;
    }

    #if (INPUT_READ_MODE == 0)
    ts_fd = open("/dev/input/event0", O_RDWR | O_NDELAY | O_NONBLOCK);
    printf("Successfully open device in nonblock mode.\n");
    #else
    ts_fd = open("/dev/input/event0", O_RDWR);
    printf("Successfully open device in normal mode.\n");
    #endif
    
    if(ts_fd == -1)
    {
        perror("open ts device failed");
        return -1;
    }

    #if (INPUT_READ_MODE == 2)
    nfds = 1;
    mpollfd.fd = ts_fd;
    mpollfd.events = POLLIN;
    mpollfd.revents = 0;
    printf("Successfully run in poll mode.\n");
    #elif (INPUT_READ_MODE == 3)
    signal(SIGIO, ts_sig_handler);
    fcntl(ts_fd, F_SETOWN, getpid());
    flags = fcntl(ts_fd, F_GETFL);
    fcntl(ts_fd, F_SETFL, flags | FASYNC | O_NONBLOCK);
    printf("Successfully run in async mode.\n");
    #endif
}

void ts_deinit(void)
{
    if(ts_fd != -1) 
    {
        printf("deinit ts device\n");
        close(ts_fd);
        ts_fd = -1;
    }
}

#if (INPUT_READ_MODE == 0 || INPUT_READ_MODE == 1)
/**
 * query or sleep default handler
 * @param
 * @return
 */
void ts_default_handler(void *arg)
{
#if (INPUT_READ_MODE == 0 )
    while(read(ts_fd, &event, sizeof(struct input_event)) > 0)
#else
	if(read(ts_fd, &event, sizeof(struct input_event)) > 0)
#endif			
    {
        ts_probe_event();
    }
}

#elif (INPUT_READ_MODE == 2)
/**
 * poll handler
 * @param
 * @return
 */
void ts_poll_handler(void *arg)
{
    int len = poll(&mpollfd, nfds, 0);
    if(len > 0)         /* There is data to read */
    {
        if(read(ts_fd, &event, sizeof(struct input_event)) > 0)
        {
            ts_probe_event();
        }
    }
    else if(len == 0)   /* Time out */
    {
        // Do nothing
    }
    else                /* Error */
    {
        perror("poll error");
    }
}

#elif (INPUT_READ_MODE == 3)
/**
 * async signal handler
 * @param
 * @return
 */
void ts_sig_handler(int signal)
{
    while(read(ts_fd, &event, sizeof(struct input_event)) > 0)
    {
        ts_probe_event();
    }
}

#endif

void ts_probe_event(void)
{
    // printf("type:%d code:%d value:%d\n", ev.type, ev.code, ev.value);

    if(event.type == EV_KEY)
    {
        if(event.code == BTN_TOUCH)
            next.value = event.value;
    }
    else if(event.type == EV_ABS)
    {
        if(event.code == ABS_X || event.code == ABS_MT_POSITION_X)
            next.x = event.value;
        else if(event.code == ABS_Y || event.code == ABS_MT_POSITION_Y)
            next.y = event.value;
    }
    else if(event.type == EV_SYN)
        last = next;
}

void ts_get_focus(struct focus_t *f)
{
    if(f != NULL)
    {
        *f = last;
    }
}
