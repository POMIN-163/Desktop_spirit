/*
 * @Author: Pomin
 * @Date: 2021-11-15 00:02:11
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-12-01 15:01:24
 * @Description: 呼吸灯
 */
#include "app_common.h"

#define PWM_DEV_NAME        "lpwm0" /* PWM设备名称 */
#define PWM_DEV_CHANNEL     0       /* PWM通道 */
struct rt_device_pwm* pwm_dev;      /* PWM设备句柄 */

rt_thread_t breath_thread = RT_NULL;

#define BREATH_T     (100000000)      /* PWM 周期 */
#define BREATH_UNIT  (BREATH_T / 100)
#define BREATH_SPEED 5                /* 呼吸速度 0 ~ 100 */
#define LIGHT_MAX    60               /* 最大亮度 */
/**
 * @brief 呼吸灯线程
 *
 * @param p
**/
void breath_thread_entry(void* p) {
    uint8_t duty = 0; // 0 ~ 100
    int8_t dir = 1;
    /* 查找 PWM 设备 */
    pwm_dev = (struct rt_device_pwm*)rt_device_find(PWM_DEV_NAME);
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, BREATH_T, BREATH_T);
    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
    while (1) {
        if (spirit.isplaying) {
            if (duty > LIGHT_MAX) dir = -1;
            if (duty < 5) dir = 1;
            duty += dir;
            rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, BREATH_T, duty * BREATH_UNIT);
            rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
            rt_thread_delay(100 / BREATH_SPEED);
        } else {
            rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, BREATH_T, 0);
            rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
            rt_thread_delay(100 / BREATH_SPEED);
        }
    }
}
/**
 * @brief 呼吸灯初始化
 *
 * @return int
**/
int breath_init(void) {
    breath_thread = rt_thread_create(
        "breath",
        breath_thread_entry,
        RT_NULL,
        512,
        25,
        5
    );
    if (breath_thread != RT_NULL) {
        rt_thread_startup(breath_thread);
        rt_kprintf("breath thread start\n");
    } else {
        rt_kprintf("breath thread error\n");
    }
}
INIT_APP_EXPORT(breath_init);
