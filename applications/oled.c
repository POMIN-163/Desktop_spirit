/*
 * @Author: Pomin
 * @Date: 2021-12-01 13:06:37
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-12-01 15:09:48
 * @Description: oled 显示
 */
#include "app_common.h"
#include "ssd1306.h"

/* 利用变参函数来实现对 OLED 的格式化输出 */
char disp_temp[50];
#define oled_printf(x, y, font, ...) do { \
    rt_sprintf(disp_temp, ##__VA_ARGS__); \
    ssd1306_SetCursor(x, y); \
    ssd1306_WriteString(disp_temp, font, White); \
} while(0)

rt_thread_t oled_thread = RT_NULL;
rt_mq_t oled_mq;

/**
 * @brief oled线程
 *
 * @param p
**/
void oled_thread_entry(void* p) {
    uint8_t disp_pos;
    char oled_buff[50];
    oled_mq = rt_mq_create("oled_mq", 50, 3, RT_IPC_FLAG_FIFO);
    while (1) {
        rt_mq_recv(oled_mq, oled_buff, 50,  RT_WAITING_FOREVER);
        ssd1306_Fill(Black);
        if (spirit.isplaying) {
            oled_printf(25, 5, Font_11x18, "Playing");
        } else {
            oled_printf(25, 5, Font_11x18, " Pause ");
        }
        ssd1306_DrawRectangle(1, 1, 126, 30, White);
        /* 限制文件名长度 */
        if (rt_strlen(oled_buff) > 18) {
            oled_buff[18] = '\0';
        }
        /* 计算显示位置 (居中显示) */
        disp_pos = 64 - 7 * rt_strlen(oled_buff) / 2;
        oled_printf(disp_pos, 45, Font_7x10,  "%s", oled_buff);
        ssd1306_UpdateScreen();
    }
}
/**
 * @brief oled初始化
 *
 * @return int
**/
int oled_init(void) {
    ssd1306_Init();
    oled_thread = rt_thread_create(
        "oled",
        oled_thread_entry,
        RT_NULL,
        1024,
        25,
        5
    );
    if (oled_thread != RT_NULL) {
        rt_thread_startup(oled_thread);
        rt_kprintf("oled thread start\n");
    } else {
        rt_kprintf("oled thread error\n");
    }
}
INIT_APP_EXPORT(oled_init);
