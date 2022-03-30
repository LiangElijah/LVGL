/*
arm-none-linux-gnueabihf-gcc main.c lcd.c ts.c lv_port_disp.c lv_port_indev.c -o main -Wno-packed-bitfield-compat -I./Git/lvgl/out/include -llvgl -L./Git/lvgl/out/lib
*/

#include <stdio.h>
#include <stdlib.h> //strtol
#include <unistd.h> //sleep、usleep
#include <limits.h> //LONG_MIN、LONG_MAX
#include "lcd.h"
#include "ts.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"

void btn_event_cb(lv_event_t *e)
{
    printf("Clicked\n");
}

void lv_port_demo(void)
{
    lv_obj_t *btn = lv_btn_create(lv_scr_act());                    /*Add a button to the current screen*/
    lv_obj_set_pos(btn, (1024-200)/2, (600-100)/2);                 /*Set its position*/
    lv_obj_set_size(btn, 200, 100);                                 /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

    lv_obj_t *label = lv_label_create(btn);                         /*Add a label to the button*/
    lv_label_set_text(label, "Button");                             /*Set the labels text*/
    lv_obj_center(label);                                           /*Align the label to the center*/

    lv_obj_t *label1 = lv_label_create(lv_scr_act());
    lv_label_set_text(label1, "Hello World!");
    lv_obj_align(label1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align_to(btn, label1, LV_ALIGN_OUT_TOP_MID, 0, -10);
}

int main(int argc, char *argv[])
{
#if 0
    int color = 0xFFFFFF;
    int buffer[100*100] = {0};
    if(argc == 2)
    {
        char *endstr = NULL;
        int ret = strtol(argv[1], &endstr, 16);
        if((ret != LONG_MIN) && (ret != LONG_MAX))
            color = ret;
        printf("color:0x%x\n", color);
    } 

    lcd_init();
    //lcd_set_info();
    ts_init();

    lcd_fill_screen(color, RGB24);
    lcd_fill_buffer2(300, 300, 100, 100, (void *)buffer, RGB24);
    lcd_draw_box2(100, 100, 200, 200, 0xFFFFFF-color, RGB24);

    for(int i = 0; i < 50; i++)
    {
        ts_get_event(NULL);
    }

    lcd_deinit();
    ts_deinit();
#else
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    #if LV_VERSION_CHECK(8, 0, 0)
    #if (INPUT_READ_MODE == 0 || INPUT_READ_MODE == 1)
    lv_timer_create((lv_timer_cb_t)ts_default_handler, 0, NULL);
    #elif (INPUT_READ_MODE == 2)
    lv_timer_create((lv_timer_cb_t)ts_poll_handler, 0, NULL);
    #endif
    #else
    #if (INPUT_READ_MODE == 0 || INPUT_READ_MODE == 1)
    lv_task_create(ts_default_handler, 0, LV_TASK_PRIO_HIGHEST, NULL);
    #elif (INPUT_READ_MODE == 2)
    lv_task_create(ts_poll_handler, 0, LV_TASK_PRIO_HIGH, NULL);
    #endif
    #endif

    lv_port_demo();
    while(1)
    {
        lv_task_handler();
        usleep(1000);
        lv_tick_inc(1);
    }
#endif
    return 0;
}
