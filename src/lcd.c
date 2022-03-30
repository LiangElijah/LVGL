#include "lcd.h"

int lcd_fd = -1;
char *lcd_p = (void *)-1;
int lcd_buffer_size = 0;
struct fb_var_screeninfo fb_var = {0};
struct fb_fix_screeninfo fb_fix = {0};

int lcd_init(void)
{
    if(lcd_p != (void *)-1) 
    {
        printf("mremap lcd device\n");
        munmap(lcd_p, lcd_buffer_size);
        lcd_p = (void *)-1;
    }

    if(lcd_fd != -1) 
    {
        printf("reinit lcd device\n");
        close(lcd_fd);
        lcd_fd = -1;
    }
    
    lcd_fd = open("/dev/fb0", O_RDWR);
    if(lcd_fd == -1)
    {
        perror("open lcd device failed");
        return -1;
    }

    lcd_get_info();

    lcd_buffer_size = fb_var.xres * fb_var.yres * fb_var.bits_per_pixel/8;
    lcd_p = mmap(
        NULL, 
		lcd_buffer_size, 
		PROT_READ|PROT_WRITE,			//映射内存，与LCD有关
		MAP_SHARED, 
		lcd_fd, 
		0
	);
    if(lcd_p == (void *)-1)
    {
        perror("mmap lcd device failed");
        close(lcd_fd);
        lcd_fd = -1;
        return -1;
    }

    return 0;
}

void lcd_deinit(void)
{
    if(lcd_p != (void *)-1) 
    {
        printf("munmap lcd device\n");
        munmap(lcd_p, lcd_buffer_size);
        lcd_p = (void *)-1;
    }

    if(lcd_fd != -1) 
    {
        printf("deinit lcd device\n");
        close(lcd_fd);
        lcd_fd = -1;
    }
}

void lcd_get_info(void)
{
    int ret;

    if(lcd_fd == -1) 
    {
        printf("lcd device not init\n");
        return;
    }

    bzero(&fb_var, sizeof(struct fb_var_screeninfo));
    bzero(&fb_fix, sizeof(struct fb_fix_screeninfo));
    ret = ioctl(lcd_fd, FBIOGET_VSCREENINFO, &fb_var);
    if(ret == -1)
    {
        perror("ioctl lcd device failed [FBIOGET_VSCREENINFO]");
        return;
    }
    ret = ioctl(lcd_fd, FBIOGET_FSCREENINFO, &fb_fix);
    if(ret == -1)
    {
        perror("ioctl lcd device failed [FBIOGET_FSCREENINFO]");
        return;
    }

    printf(
        "分辨率: %d*%d\n" 
        "虚拟分辨率: %d*%d\n" 
        "行偏移: %d 列偏移: %d\n" 
        "像素深度bpp: %d\n" 
        "像素格式: R<%d %d> G<%d %d> B<%d %d>\n", 
        fb_var.xres, fb_var.yres, 
        fb_var.xres_virtual, fb_var.yres_virtual, 
        fb_var.xoffset, fb_var.yoffset, 
        fb_var.bits_per_pixel, 
        fb_var.red.offset, fb_var.red.length, 
        fb_var.green.offset, fb_var.green.length, 
        fb_var.blue.offset, fb_var.blue.length
    );

    printf(
        "一行的字节数: %d\n", 
        fb_fix.line_length
    );
}

int lcd_set_info(void)
{
    int ret;

    if(lcd_fd == -1) 
    {
        printf("lcd device not init\n");
        return -1;
    }

    if(lcd_p != (void *)-1) 
    {
        printf("munmap lcd device\n");
        munmap(lcd_p, lcd_buffer_size);
        lcd_p = (void *)-1;
    }
    
    fb_var.xres_virtual = fb_var.xres;
    fb_var.yres_virtual = fb_var.yres*2;
    fb_var.bits_per_pixel = 24;
    fb_var.red.offset = 16, fb_var.red.length = 8;
    fb_var.green.offset = 8, fb_var.green.length = 8;
    fb_var.blue.offset = 0, fb_var.blue.length = 8;
    
    ret = ioctl(lcd_fd, FBIOPUT_VSCREENINFO, &fb_var);
    if(ret == -1)
    {
        perror("ioctl lcd device failed [FBIOPUT_VSCREENINFO]");
        return -1;
    }

    lcd_get_info();

    lcd_buffer_size = fb_var.xres * fb_var.yres * fb_var.bits_per_pixel/8;
    lcd_p = mmap(
        NULL, 
		lcd_buffer_size, 
		PROT_READ|PROT_WRITE,			//映射内存，与LCD有关
		MAP_SHARED, 
		lcd_fd, 
		0
	);
    if(lcd_p == (void *)-1)
    {
        perror("mmap lcd device failed");
        close(lcd_fd);
        lcd_fd = -1;
        return -1;
    }

    return 0;
}

void lcd_draw_point(uint32_t x, uint32_t y, uint32_t color, COLOR_MODE mode)
{
    char rgb32_r, rgb32_g, rgb32_b, rgb32_a;
    rgb32_r = rgb32_g = rgb32_b = 0xFF;
    rgb32_a = 0;

    if(x >= fb_var.xres || y >= fb_var.yres)
    {
        printf("x,y position over lcd screen\n");
        return;
    }

    if((lcd_p == (void *)-1) || (lcd_fd == -1)) 
    {
        printf("lcd device not init\n");
        return;
    }

    switch(mode)
    {
        case RGB332:
            rgb32_r = (char)((color & 0xE0) >> 5 << 5);
            rgb32_g = (char)((color & 0x1C) >> 2 << 5);
            rgb32_b = (char)((color & 0x03) >> 0 << 6);
            break;
        case BGR233:
            rgb32_b = (char)((color & 0xC0) >> 6 << 6);
            rgb32_g = (char)((color & 0x38) >> 3 << 5);
            rgb32_r = (char)((color & 0x07) >> 0 << 5);
            break;
        case RGB555:
            rgb32_r = (char)((color & 0x7C00) >> 10 << 3);
            rgb32_g = (char)((color & 0x03E0) >>  5 << 3);
            rgb32_b = (char)((color & 0x001F) >>  0 << 3);
            break;
        case BGR555:
            rgb32_b = (char)((color & 0x7C00) >> 10 << 3);
            rgb32_g = (char)((color & 0x03E0) >>  5 << 3);
            rgb32_r = (char)((color & 0x001F) >>  0 << 3);
            break;
        case RGB565:
            rgb32_r = (char)((color & 0xF800) >> 11 << 3);
            rgb32_g = (char)((color & 0x07E0) >>  5 << 2);
            rgb32_b = (char)((color & 0x001F) >>  0 << 3);
            break;
        case BGR565:
            rgb32_b = (char)((color & 0xF800) >> 11 << 3);
            rgb32_g = (char)((color & 0x07E0) >>  5 << 2);
            rgb32_r = (char)((color & 0x001F) >>  0 << 3);
            break;
        case RGB888:
            rgb32_r = (char)((color & 0xFF0000) >> 16);
            rgb32_g = (char)((color & 0x00FF00) >>  8);
            rgb32_b = (char)((color & 0x0000FF) >>  0);
            break;
        case BGR888:
            rgb32_b = (char)((color & 0xFF0000) >> 16);
            rgb32_g = (char)((color & 0x00FF00) >>  8);
            rgb32_r = (char)((color & 0x00FF00) >>  0);
            break;
        case ARGB:
            rgb32_a = (char)((color & 0xFF000000) >> 24);
            rgb32_r = (char)((color & 0xFF0000)   >> 16);
            rgb32_g = (char)((color & 0x00FF00)   >>  8);
            rgb32_b = (char)((color & 0x00FF00)   >>  0);
            break;
        case ABGR:
            rgb32_a = (char)((color & 0xFF000000) >> 24);
            rgb32_b = (char)((color & 0x00FF0000) >> 16);
            rgb32_g = (char)((color & 0x0000FF00) >>  8);
            rgb32_r = (char)((color & 0x000000FF) >>  0);
            break;
        case RGBA:
            rgb32_r = (char)((color & 0xFF000000) >> 24);
            rgb32_g = (char)((color & 0x00FF0000) >> 16);
            rgb32_b = (char)((color & 0x0000FF00) >>  8);
            rgb32_a = (char)((color & 0x000000FF) >>  0);
            break;
        case BGRA:
            rgb32_b = (char)((color & 0xFF000000) >> 24);
            rgb32_g = (char)((color & 0x00FF0000) >> 16);
            rgb32_r = (char)((color & 0x0000FF00) >>  8);
            rgb32_a = (char)((color & 0x000000FF) >>  0);
            break;
        default:
            perror("unknown color mode");
            return;
            break;
    }

    switch(fb_var.bits_per_pixel)
    {
        case 8:                             //332
        {
            rgb8_t rgb8;
            rgb8.y = (rgb32_g >> (8-fb_var.green.length));
            if(fb_var.red.offset > fb_var.blue.offset)      //RGB332
            {
                rgb8.x = (rgb32_b >> (8-fb_var.blue.length));
                rgb8.z = (rgb32_r >> (8-fb_var.red.length));
            }
            else                                            //BGR233
            {
                rgb8.x = (rgb32_r >> (8-fb_var.red.length));
                rgb8.z = (rgb32_b >> (8-fb_var.blue.length));
            }

            rgb8_t *rgb8_p = (rgb8_t *)lcd_p;
            rgb8_p[fb_var.xres*y + x] = rgb8;
            break;
        }
        case 16:                            //555、565
        {
            if(fb_var.green.length == 5)    //555
            {
                rgb15_t rgb15;
                rgb15.k = 0;
                rgb15.y = (rgb32_g >> (8-fb_var.green.length));
                if(fb_var.red.offset > fb_var.blue.offset)  //RGB555
                {
                    rgb15.x = (rgb32_b >> (8-fb_var.blue.length));
                    rgb15.z = (rgb32_r >> (8-fb_var.red.length));
                }
                else                                       //BGR555
                {
                    rgb15.x = (rgb32_r >> (8-fb_var.red.length));
                    rgb15.z = (rgb32_b >> (8-fb_var.blue.length));
                }

                rgb15_t *rgb15_p = (rgb15_t *)lcd_p;
                rgb15_p[fb_var.xres*y + x] = rgb15;
            }
            else                            //565
            {
                rgb16_t rgb16;
                rgb16.y = (rgb32_g >> (8-fb_var.green.length));
                if(fb_var.red.offset > fb_var.blue.offset)  //RGB565
                {
                    rgb16.x = (rgb32_b >> (8-fb_var.blue.length));
                    rgb16.z = (rgb32_r >> (8-fb_var.red.length));
                }
                else                                        //BGR565
                {
                    rgb16.x = (rgb32_r >> (8-fb_var.red.length));
                    rgb16.z = (rgb32_b >> (8-fb_var.blue.length));
                }

                rgb16_t *rgb16_p = (rgb16_t *)lcd_p;
                rgb16_p[fb_var.xres*y + x] = rgb16;
            }
            break;
        }
        case 24:                            //888
        {
            rgb24_t rgb24;
            rgb24.y = (rgb32_g >> (8-fb_var.green.length));
            if(fb_var.red.offset > fb_var.blue.offset)      //RGB888
            {
                rgb24.x = (rgb32_b >> (8-fb_var.blue.length));
                rgb24.z = (rgb32_r >> (8-fb_var.red.length));
            }
            else                                            //BGR888
            {
                rgb24.x = (rgb32_r >> (8-fb_var.red.length));
                rgb24.z = (rgb32_b >> (8-fb_var.blue.length));
            }

            rgb24_t *rgb24_p = (rgb24_t *)lcd_p;
            rgb24_p[fb_var.xres*y + x] = rgb24;
            break;
        }
        case 32:                            //8888
        {
            rgb32_t rgb32;
            if(fb_var.transp.offset > fb_var.green.offset)  //A888
            {
                rgb32.k = (rgb32_a >> (8-fb_var.transp.length));
                rgb32.y = (rgb32_g >> (8-fb_var.green.length));
                if(fb_var.red.offset > fb_var.blue.offset)  //ARGB
                {
                    rgb32.x = (rgb32_b >> (8-fb_var.blue.length));
                    rgb32.z = (rgb32_r >> (8-fb_var.red.length));
                }
                else                                        //ABGR
                {
                    rgb32.x = (rgb32_r >> (8-fb_var.red.length));
                    rgb32.z = (rgb32_b >> (8-fb_var.blue.length));
                }
            }
            else                                            //888A
            {
                rgb32.x = (rgb32_a >> (8-fb_var.transp.length));
                rgb32.z = (rgb32_g >> (8-fb_var.green.length));
                if(fb_var.red.offset > fb_var.blue.offset)  //RGBA
                {
                    rgb32.y = (rgb32_b >> (8-fb_var.blue.length));
                    rgb32.k = (rgb32_r >> (8-fb_var.red.length));
                }
                else                                        //BGRA
                {
                    rgb32.k = (rgb32_r >> (8-fb_var.red.length));
                    rgb32.y = (rgb32_b >> (8-fb_var.blue.length));
                }
            }

            rgb32_t *rgb32_p = (rgb32_t *)lcd_p;
            rgb32_p[fb_var.xres*y + x] = rgb32;
            break;
        }
        default:
            perror("unknown bits_per_pixel");
            return;
            break;
    }
}

void lcd_draw_box(uint32_t x, uint32_t y, uint32_t m, uint32_t n, uint32_t color, COLOR_MODE mode)
{
    char rgb32_r, rgb32_g, rgb32_b, rgb32_a;
    rgb32_r = rgb32_g = rgb32_b = 0xFF;
    rgb32_a = 0;

    if(x >= fb_var.xres || y >= fb_var.yres || x > m || y > n)
    {
        printf("x,y position over lcd screen\n");
        return;
    }

    if((lcd_p == (void *)-1) || (lcd_fd == -1)) 
    {
        printf("lcd device not init\n");
        return;
    }

    switch(mode)
    {
        case RGB332:
            rgb32_r = (char)((color & 0xE0) >> 5 << 5);
            rgb32_g = (char)((color & 0x1C) >> 2 << 5);
            rgb32_b = (char)((color & 0x03) >> 0 << 6);
            break;
        case BGR233:
            rgb32_b = (char)((color & 0xC0) >> 6 << 6);
            rgb32_g = (char)((color & 0x38) >> 3 << 5);
            rgb32_r = (char)((color & 0x07) >> 0 << 5);
            break;
        case RGB555:
            rgb32_r = (char)((color & 0x7C00) >> 10 << 3);
            rgb32_g = (char)((color & 0x03E0) >>  5 << 3);
            rgb32_b = (char)((color & 0x001F) >>  0 << 3);
            break;
        case BGR555:
            rgb32_b = (char)((color & 0x7C00) >> 10 << 3);
            rgb32_g = (char)((color & 0x03E0) >>  5 << 3);
            rgb32_r = (char)((color & 0x001F) >>  0 << 3);
            break;
        case RGB565:
            rgb32_r = (char)((color & 0xF800) >> 11 << 3);
            rgb32_g = (char)((color & 0x07E0) >>  5 << 2);
            rgb32_b = (char)((color & 0x001F) >>  0 << 3);
            break;
        case BGR565:
            rgb32_b = (char)((color & 0xF800) >> 11 << 3);
            rgb32_g = (char)((color & 0x07E0) >>  5 << 2);
            rgb32_r = (char)((color & 0x001F) >>  0 << 3);
            break;
        case RGB888:
            rgb32_r = (char)((color & 0xFF0000) >> 16);
            rgb32_g = (char)((color & 0x00FF00) >>  8);
            rgb32_b = (char)((color & 0x0000FF) >>  0);
            break;
        case BGR888:
            rgb32_b = (char)((color & 0xFF0000) >> 16);
            rgb32_g = (char)((color & 0x00FF00) >>  8);
            rgb32_r = (char)((color & 0x00FF00) >>  0);
            break;
        case ARGB:
            rgb32_a = (char)((color & 0xFF000000) >> 24);
            rgb32_r = (char)((color & 0xFF0000)   >> 16);
            rgb32_g = (char)((color & 0x00FF00)   >>  8);
            rgb32_b = (char)((color & 0x00FF00)   >>  0);
            break;
        case ABGR:
            rgb32_a = (char)((color & 0xFF000000) >> 24);
            rgb32_b = (char)((color & 0x00FF0000) >> 16);
            rgb32_g = (char)((color & 0x0000FF00) >>  8);
            rgb32_r = (char)((color & 0x000000FF) >>  0);
            break;
        case RGBA:
            rgb32_r = (char)((color & 0xFF000000) >> 24);
            rgb32_g = (char)((color & 0x00FF0000) >> 16);
            rgb32_b = (char)((color & 0x0000FF00) >>  8);
            rgb32_a = (char)((color & 0x000000FF) >>  0);
            break;
        case BGRA:
            rgb32_b = (char)((color & 0xFF000000) >> 24);
            rgb32_g = (char)((color & 0x00FF0000) >> 16);
            rgb32_r = (char)((color & 0x0000FF00) >>  8);
            rgb32_a = (char)((color & 0x000000FF) >>  0);
            break;
        default:
            perror("unknown color mode");
            return;
            break;
    }

    switch(fb_var.bits_per_pixel)
    {
        case 8:                            //332
        {
            rgb8_t rgb8;
            rgb8.y = (rgb32_g >> (8-fb_var.green.length));
            if(fb_var.red.offset > fb_var.blue.offset)      //RGB332
            {
                rgb8.x = (rgb32_b >> (8-fb_var.blue.length));
                rgb8.z = (rgb32_r >> (8-fb_var.red.length));
            }
            else                                            //BGR233
            {
                rgb8.x = (rgb32_r >> (8-fb_var.red.length));
                rgb8.z = (rgb32_b >> (8-fb_var.blue.length));
            }

            rgb8_t *rgb8_p = (rgb8_t *)lcd_p;
            for(int j = y; j <= n && j < fb_var.yres; j++)
            {
                for(int i = x; i <= m && i < fb_var.xres; i++)
                {
                    rgb8_p[j*fb_var.xres + i] = rgb8;
                }
            }
            break;
        }
        case 16:                            //555、565
        {
            if(fb_var.green.length == 5)    //555
            {
                rgb15_t rgb15;
                rgb15.k = 0;
                rgb15.y = (rgb32_g >> (8-fb_var.green.length));
                if(fb_var.red.offset > fb_var.blue.offset)  //RGB555
                {
                    rgb15.x = (rgb32_b >> (8-fb_var.blue.length));
                    rgb15.z = (rgb32_r >> (8-fb_var.red.length));
                }
                else                                        //BGR555
                {
                    rgb15.x = (rgb32_r >> (8-fb_var.red.length));
                    rgb15.z = (rgb32_b >> (8-fb_var.blue.length));
                }

                rgb15_t *rgb15_p = (rgb15_t *)lcd_p;
                for(int j = y; j <= n && j < fb_var.yres; j++)
                {
                    for(int i = x; i <= m && i < fb_var.xres; i++)
                    {
                        rgb15_p[j*fb_var.xres + i] = rgb15;
                    }
                }
            }
            else                            //565
            {
                rgb16_t rgb16;
                rgb16.y = (rgb32_g >> (8-fb_var.green.length));
                if(fb_var.red.offset > fb_var.blue.offset)  //RGB565
                {
                    rgb16.x = (rgb32_b >> (8-fb_var.blue.length));
                    rgb16.z = (rgb32_r >> (8-fb_var.red.length));
                }
                else                                        //BGR565
                {
                    rgb16.x = (rgb32_r >> (8-fb_var.red.length));
                    rgb16.z = (rgb32_b >> (8-fb_var.blue.length));
                }

                rgb16_t *rgb16_p = (rgb16_t *)lcd_p;
                for(int j = y; j <= n && j < fb_var.yres; j++)
                {
                    for(int i = x; i <= m && i < fb_var.xres; i++)
                    {
                        rgb16_p[j*fb_var.xres + i] = rgb16;
                    }
                }
            }
            break;
        }
        case 24:                            //888
        {
            rgb24_t rgb24;
            rgb24.y = (rgb32_g >> (8-fb_var.green.length));
            if(fb_var.red.offset > fb_var.blue.offset)      //RGB888
            {
                rgb24.x = (rgb32_b >> (8-fb_var.blue.length));
                rgb24.z = (rgb32_r >> (8-fb_var.red.length));
            }
            else                                            //BGR888
            {
                rgb24.x = (rgb32_r >> (8-fb_var.red.length));
                rgb24.z = (rgb32_b >> (8-fb_var.blue.length));
            }

            rgb24_t *rgb24_p = (rgb24_t *)lcd_p;
            for(int j = y; j <= n && j < fb_var.yres; j++)
            {
                for(int i = x; i <= m && i < fb_var.xres; i++)
                {
                    rgb24_p[j*fb_var.xres + i] = rgb24;
                }
            }
            break;
        }
        case 32:                            //8888
        {
            rgb32_t rgb32;
            if(fb_var.transp.offset > fb_var.green.offset)  //A888
            {
                rgb32.k = (rgb32_a >> (8-fb_var.transp.length));
                rgb32.y = (rgb32_g >> (8-fb_var.green.length));
                if(fb_var.red.offset > fb_var.blue.offset)  //ARGB
                {
                    rgb32.x = (rgb32_b >> (8-fb_var.blue.length));
                    rgb32.z = (rgb32_r >> (8-fb_var.red.length));
                }
                else                                        //ABGR
                {
                    rgb32.x = (rgb32_r >> (8-fb_var.red.length));
                    rgb32.z = (rgb32_b >> (8-fb_var.blue.length));
                }
            }
            else                                            //888A
            {
                rgb32.x = (rgb32_a >> (8-fb_var.transp.length));
                rgb32.z = (rgb32_g >> (8-fb_var.green.length));
                if(fb_var.red.offset > fb_var.blue.offset)  //RGBA
                {
                    rgb32.y = (rgb32_b >> (8-fb_var.blue.length));
                    rgb32.k = (rgb32_r >> (8-fb_var.red.length));
                }
                else                                        //BGRA
                {
                    rgb32.k = (rgb32_r >> (8-fb_var.red.length));
                    rgb32.y = (rgb32_b >> (8-fb_var.blue.length));
                }
            }

            rgb32_t *rgb32_p = (rgb32_t *)lcd_p;
            for(int j = y; j <= n && j < fb_var.yres; j++)
            {
                for(int i = x; i <= m && i < fb_var.xres; i++)
                {
                    rgb32_p[j*fb_var.xres + i] = rgb32;
                }
            }
            break;
        }
        default:
            perror("unknown bits_per_pixel");
            return;
            break;
    }
}

void lcd_draw_box2(uint32_t x, uint32_t y, uint32_t m, uint32_t n, uint32_t color, COLOR_MODE mode)
{
    lcd_draw_box(x, y, x+m-1, y+n-1, color, mode);
}

void lcd_fill_screen(uint32_t color, COLOR_MODE mode)
{
    lcd_draw_box(0, 0, fb_var.xres-1, fb_var.yres-1, color, mode);
}

void lcd_fill_buffer(uint32_t x, uint32_t y, uint32_t m, uint32_t n, void *color, COLOR_MODE mode)
{
    if(mode < COLOR_MODE_8_16_BITS_LINE)
    {
        uint8_t *p = (uint8_t *)color;
        for(int j = y, k = 0; j <= n && j < fb_var.yres; j++, k++)
        {
            for(int i = x, l = 0; i <= m && i < fb_var.xres; i++, l++)
            {
                lcd_draw_point(i, j, p[k*(m-x+1)+l], mode);
            }
        }
    }
    else if(mode < COLOR_MODE_16_32_BITS_LINE)
    {
        uint16_t *p = (uint16_t *)color;
        for(int j = y, k = 0; j <= n && j < fb_var.yres; j++, k++)
        {
            for(int i = x, l = 0; i <= m && i < fb_var.xres; i++, l++)
            {
                lcd_draw_point(i, j, p[k*(m-x+1)+l], mode);
            }
        }
    }
    else
    {
        uint32_t *p = (uint32_t *)color;
        for(int j = y, k = 0; j <= n && j < fb_var.yres; j++, k++)
        {
            for(int i = x, l = 0; i <= m && i < fb_var.xres; i++, l++)
            {
                lcd_draw_point(i, j, p[k*(m-x+1)+l], mode);
            }
        }
    }
}

void lcd_fill_buffer2(uint32_t x, uint32_t y, uint32_t m, uint32_t n, void *color, COLOR_MODE mode)
{
    lcd_fill_buffer(x, y, x+m-1, y+n-1, color, mode);
}

void lcd_direct_fill_buffer(uint32_t x, uint32_t y, uint32_t m, uint32_t n, void *color)
{
    if(fb_var.bits_per_pixel == 8)
    {
        uint8_t *rgb8_p = (uint8_t *)lcd_p;
        uint8_t *p = (uint8_t *)color;
        for(int j = y, k = 0; j <= n && j < fb_var.yres; j++, k++)
        {
            for(int i = x, l = 0; i <= m && i < fb_var.xres; i++, l++)
            {
                rgb8_p[j*fb_var.xres + i] = p[k*(m-x+1)+l];
            }
        }
    }
    else if(fb_var.bits_per_pixel == 16)
    {
        uint16_t *rgb16_p = (uint16_t *)lcd_p;
        uint16_t *p = (uint16_t *)color;
        for(int j = y, k = 0; j <= n && j < fb_var.yres; j++, k++)
        {
            for(int i = x, l = 0; i <= m && i < fb_var.xres; i++, l++)
            {
                rgb16_p[j*fb_var.xres + i] = p[k*(m-x+1)+l];
            }
        }
    }
    else // 24、32
    {
        uint32_t *rgb32_p = (uint32_t *)lcd_p;
        uint32_t *p = (uint32_t *)color;
        for(int j = y, k = 0; j <= n && j < fb_var.yres; j++, k++)
        {
            for(int i = x, l = 0; i <= m && i < fb_var.xres; i++, l++)
            {
                rgb32_p[j*fb_var.xres + i] = p[k*(m-x+1)+l];
            }
        }
    }
}

void lcd_direct_fill_buffer2(uint32_t x, uint32_t y, uint32_t m, uint32_t n, void *color)
{
    lcd_direct_fill_buffer(x, y, x+m-1, y+n-1, color);
}
