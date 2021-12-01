/*
 * @Author: Pomin
 * @Date: 2021-11-24 10:33:20
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-12-01 14:55:45
 * @Description: 红外遥控
 */
#include "app_common.h"

#ifdef BSP_USING_IRRX

rt_thread_t irrx_thread;

/* 红外接收消息对象 */
my_msg irrx_msg = {
    .type = type_irrx,
};
uint8_t* irrx_buff;

uint8_t ab32_get_irkey(uint16_t* addr, uint16_t* cmd);
/**
 * @brief 红外接收线程
 *
 * @param p
**/
void irrx_thread_entry(void* p) {
    uint16_t addr = 0;
    uint16_t cmd = 0;
    irrx_buff = irrx_msg.buff;
    while (1) {
        if (ab32_get_irkey(&addr, &cmd)) {
            rt_kprintf("get_irkey: %04x, %04x\n", addr, cmd);
            irrx_buff[0] = addr;
            irrx_buff[1] = cmd;
            /* 发送到队列 */
            rt_mq_send(spirit_mq, &irrx_msg, 100);
        }
        rt_thread_delay(500);
    }
}
/**
 * @brief 红外接收初始化
 *
 * @return int
**/
int irrx_init(void) {
    irrx_thread = rt_thread_create(
        "irrx",
        irrx_thread_entry,
        RT_NULL,
        512,
        25,
        5
    );
    if (irrx_thread != RT_NULL) {
        rt_thread_startup(irrx_thread);
        rt_kprintf("irrx thread start\n");
    } else {
        rt_kprintf("irrx thread error\n");
    }
}
INIT_APP_EXPORT(irrx_init);

#endif
