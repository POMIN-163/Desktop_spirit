/*
 * @Author: Pomin
 * @Date: 2021-11-27 17:02:06
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-11-30 22:18:25
 * @Description:
 */
#include "app_common.h"
#include "WS2812B.h"

/**
  * @brief  rgb_thread_entry
  * @param  parameter
  * @retval None
  */
static void rgb_thread_entry(void* parameter) {
    while(1) {
        // WS2812B_All(0, 50, 0);
        // rt_pin_write(rgb_pin, 1);
        rt_thread_delay(500);
    }
}

/**
  * @brief  thread_rgb
  * @param  None
  * @retval ret
  */
int thread_rgb(void) {
    rt_err_t ret = RT_EOK;
    rgb_pin = (uint8_t)rt_pin_get("PF.1");
    rt_pin_mode(rgb_pin, PIN_MODE_OUTPUT);
    rt_thread_t thread = rt_thread_create("rgb", rgb_thread_entry, RT_NULL, 512, 25, 100);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL) {
        rt_thread_startup(thread);
    } else {
        ret = RT_ERROR;
    }

    return ret;
}

// INIT_APP_EXPORT(thread_rgb);
