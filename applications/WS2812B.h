/*
 * @Author: Pomin
 * @Date: 2021-11-27 17:32:18
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-11-28 18:18:40
 * @Description:
 */
#ifndef __WS2812B_H
#define __WS2812B_H

#include "stdint.h"
#define LED_NUM 3
// 灯珠个数
extern uint8_t rgb_pin;

typedef struct {
    unsigned char R;
    unsigned char G;
    unsigned char B;
} RGB;

void WS2812B_Init(void);
void WS2812B_Reset(void);
void WS2812B_Write_24Bits(uint8_t red, uint8_t green, uint8_t blue);
void WS2812B_All(uint8_t red, uint8_t green, uint8_t blue);

void WS2812B_Write_24Bits_RGB(RGB *color);
void WS2812B_All_RGB(RGB *color);

#endif




