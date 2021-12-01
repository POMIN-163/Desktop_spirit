/*
 * @Author: Pomin
 * @Date: 2021-11-25 18:09:07
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-12-01 15:06:10
 * @Description: 共同头文件
 */
#ifndef _GLOABL_H
#define _GLOABL_H

#include <rtthread.h>
#include "board.h"
#include "cJSON.h"
#include "wavplayer.h"

typedef enum {
    type_uart = 0,
    type_irrx,
} my_type;
/**
 * @brief 消息包 [0]: 消息来源 ID, [1 - 99]: json 消息包 / 红外码
 *
**/
typedef struct my_msg {
    uint8_t type;
    uint8_t buff[99];
} my_msg;
typedef struct my_msg *my_msg_t;
/**
 * @brief
 *
**/
typedef struct my_spirit {
    char file_path[100];
    uint8_t isplaying;
    uint8_t led_status;
} my_spirit;
typedef struct my_spirit *my_spirit_t;

extern my_spirit spirit;
extern rt_mq_t spirit_mq;

#endif	// _GLOABL_H
