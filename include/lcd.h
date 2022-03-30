#ifndef __LCD_H__
#define __LCD_H__

#include <stdio.h>
#include <stdlib.h>     //exit
#include <stdint.h>     //uintx_t
#include <sys/types.h>  //open
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>     //close
#include <sys/ioctl.h>  //ioctl
#include <sys/mman.h>   //mmap munmap
#include <strings.h>    //perror
#include <linux/fb.h>

typedef enum
{
    RGB332,
    BGR233,
    COLOR_MODE_8_16_BITS_LINE,
    RGB555,
    BGR555,
    RGB565,
    BGR565,
    COLOR_MODE_16_32_BITS_LINE,
    RGB888,
    BGR888,
    ARGB,
    ABGR,
    RGBA,
    BGRA,
}COLOR_MODE;
#define RGB8  RGB332
#define BGR8  BGR233
#define RGB15 RGB555
#define BGR15 BGR555
#define RGB16 RGB565
#define BGR16 BGR565
#define RGB24 RGB888
#define BGR24 BGR888
#define RGB32 ARGB
#define BGR32 BGRA

typedef struct rgb8_t
{
    uint8_t x:3;
    uint8_t y:3;
    uint8_t z:2;
}__attribute__((packed)) rgb8_t;

typedef struct rgb15_t
{
    uint8_t x:5;
    uint8_t y:5;
    uint8_t z:5;
    uint8_t k:1;
}__attribute__((packed)) rgb15_t;

typedef struct rgb16_t
{
    uint8_t x:5;
    uint8_t y:6;
    uint8_t z:5;
}__attribute__((packed)) rgb16_t;

typedef struct rgb24_t
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
}__attribute__((packed)) rgb24_t;

typedef struct rgb32_t
{
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t k;
}__attribute__((packed)) rgb32_t;

int lcd_init(void);
void lcd_deinit(void);
void lcd_get_info(void);
int lcd_set_info(void);

void lcd_draw_point(uint32_t x, uint32_t y, uint32_t color, COLOR_MODE mode);
void lcd_draw_box(uint32_t x, uint32_t y, uint32_t m, uint32_t n, uint32_t color, COLOR_MODE mode);
void lcd_draw_box2(uint32_t x, uint32_t y, uint32_t m, uint32_t n, uint32_t color, COLOR_MODE mode);
void lcd_fill_screen(uint32_t color, COLOR_MODE mode);
void lcd_fill_buffer(uint32_t x, uint32_t y, uint32_t m, uint32_t n, void *color, COLOR_MODE mode);
void lcd_fill_buffer2(uint32_t x, uint32_t y, uint32_t m, uint32_t n, void *color, COLOR_MODE mode);
void lcd_direct_fill_buffer(uint32_t x, uint32_t y, uint32_t m, uint32_t n, void *color);
void lcd_direct_fill_buffer2(uint32_t x, uint32_t y, uint32_t m, uint32_t n, void *color);

#endif  /* __LCD_H__ */