/*
 * @Author: Pomin
 * @Date: 2021-11-14 22:28:39
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-12-01 15:05:13
 * @Description:
 */
#include "app_common.h"
#include "easyblink.h"

#define LED_B ((uint8_t)rt_pin_get("PA.1"))

int main(void) {
    ebled_t board_led = easyblink_init_led(LED_B, 0);
    rt_pin_mode(LED_B, PIN_MODE_OUTPUT);
    int last_status = 0;
    easyblink(board_led, -1, 500, 1000);

    while (1) {
        if (last_status != spirit.isplaying && spirit.isplaying == 1) {
            last_status = spirit.isplaying;
            easyblink(board_led, -1, 1000, 2000);
        } else if (last_status != spirit.isplaying && spirit.isplaying == 0) {
            last_status = spirit.isplaying;
            easyblink(board_led, -1, 500, 1000);
        }
        rt_thread_delay(500);
    }
}


