#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>
#include <stdlib.h>     //exit
#include <stdint.h>     //uintx_t
#include <sys/types.h>  //open
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>     //close
#include <strings.h>    //perror
#include <string.h>     //memset
#include <termios.h>

int uart_init(void);
void uart_deinit(void);

#endif  /* __UART_H__ */