/*
 * @Author: Pomin
 * @Date: 2021-11-27 17:32:18
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-11-28 18:40:29
 * @Description:
 */
#include "WS2812B.h"
#include "app_common.h"

#define SIGNAL0_H_Time 400 // 0 码 高电平时间
#define SIGNAL0_L_Time 850 // 0 码 低电平时间
#define SIGNAL1_H_Time 850 // 1 码 高电平时间
#define SIGNAL1_L_Time 400 // 1 码 低电平时间

uint8_t rgb_pin;

static void WS2812B_Write0(void) {
	rt_pin_write(rgb_pin, 1);
    // GPIOF_BASE[GPIOx] |= BIT(1);
	// hal_gpio_write(GPIOF_BASE, 1, 1);
	asm("nop");asm("nop");asm("nop");asm("nop");
	rt_pin_write(rgb_pin, 0);
    // GPIOF_BASE[GPIOx] &= ~BIT(1);
	// hal_gpio_write(GPIOF_BASE, 1, 0);
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
}
static void WS2812B_Write1(void) {
    // GPIOF_BASE[GPIOx] |= BIT(1);
	rt_pin_write(rgb_pin, 1);
    // hal_gpio_write(GPIOF_BASE, 1, 1);
    asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");
    // GPIOF_BASE[GPIOx] &= ~BIT(1);
	rt_pin_write(rgb_pin, 0);
    // hal_gpio_write(GPIOF_BASE, 1, 0);
    asm("nop");asm("nop");asm("nop");asm("nop");
}
void WS2812B_Reset(void) {
	GPIOF_BASE[GPIOx] &= ~BIT(1);
}
void WS2812B_Write_Byte(uint8_t byte) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		if (byte & 0x80) {
			WS2812B_Write1();
		}
		else {
			WS2812B_Write0();
		}
		byte <<= 1;
	}
}
void WS2812B_Write_24Bits(uint8_t red, uint8_t green, uint8_t blue) {
	WS2812B_Write_Byte(green);
	WS2812B_Write_Byte(red);
	WS2812B_Write_Byte(blue);
}
void WS2812B_All(uint8_t red, uint8_t green, uint8_t blue) {
	uint8_t i;
	for (i = 0; i < LED_NUM; i++) {
		WS2812B_Write_24Bits(red, green, blue);
	}
}
void WS2812B_Write_24Bits_RGB(RGB *color) {
	WS2812B_Write_Byte(color->G);
	WS2812B_Write_Byte(color->R);
	WS2812B_Write_Byte(color->B);
}
void WS2812B_All_RGB(RGB *color) {
	uint8_t i;
	for (i = 0; i < LED_NUM; i++) {
		WS2812B_Write_24Bits_RGB(color);
	}
}
