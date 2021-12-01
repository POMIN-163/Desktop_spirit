/*
 * @Author: Pomin
 * @Date: 2021-11-15 00:02:11
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-12-01 15:04:17
 * @Description: 台灯
 */
#include "app_common.h"

#define LED_DEV_NAME     "t5pwm"  /* LED 设备名称 */
#define LED_DEV_CHANNEL  1        /* LED 通道 */
#define LED_LIGHT_MAX    80       /* LED 最大亮度 */
struct  rt_device_pwm*   led_dev; /* LED 设备句柄 */

rt_thread_t led_thread = RT_NULL;
/**
 * @brief LED 控制线程
 *
 * @param p
**/
void led_thread_entry(void* p) {
    /* 查找 PWM 设备 */
    led_dev = (struct rt_device_pwm*)rt_device_find(LED_DEV_NAME);

    rt_pwm_set(led_dev, LED_DEV_CHANNEL, 100000000, 0);
    rt_pwm_enable(led_dev, LED_DEV_CHANNEL);
    while (1) {
        if (spirit.led_status == 1) {
            rt_pwm_set(led_dev, LED_DEV_CHANNEL, 100000000, LED_LIGHT_MAX * 1000000);
            rt_pwm_enable(led_dev, LED_DEV_CHANNEL);
        } else {
            rt_pwm_disable(led_dev, LED_DEV_CHANNEL);
        }
        rt_thread_delay(200);
    }
}
/**
 * @brief LED 初始化
 *
 * @return int
**/
int led_init(void) {
    led_thread = rt_thread_create(
        "led",
        led_thread_entry,
        RT_NULL,
        512,
        25,
        5
    );
    if (led_thread != RT_NULL) {
        rt_thread_startup(led_thread);
        rt_kprintf("led thread start\n");
    } else {
        rt_kprintf("led thread error\n");
    }
}
INIT_APP_EXPORT(led_init);
