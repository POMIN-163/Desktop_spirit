/*
 * @Author: Pomin
 * @Date: 2021-11-16 09:33:13
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-12-01 15:17:08
 * @Description: esp-01s 串口交互
 */
#include "app_common.h"

#define SAMPLE_UART_NAME                 "uart1"
struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */

/* 用于接收消息的信号量 */
static rt_sem_t uart_sem;
static rt_device_t serial;

rt_thread_t esp_thread;

/* 串口消息对象 */
my_msg uart_msg = {
    .type = type_uart,
};
uint8_t* uart_buff;

/* 接收状态 */
typedef enum {
    rec_none = 0,
    rec_isrec,
} rec_status_t;

/**
  * @brief  接收数据回调函数
  * @param  dev
  *         size
  * @retval RT_EOK
  */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size) {
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(uart_sem);
    return RT_EOK;
}
/**
 * @brief 串口接收线程
 *
 * @param parameter
**/
static void serial_thread_entry(void* parameter) {
    char uart_ch;
    uint8_t rec_status = 0;
    uint8_t rec_active = 0;
    uart_buff = uart_msg.buff;
    while (1) {
        /* 从串口读取数据，没有读取到则等待接收信号量 */
        while (rt_device_read(serial, -1, &uart_ch, 1) != 1) {
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(uart_sem, RT_WAITING_FOREVER);
        }
        /* json 消息接收状态机 */
        switch (rec_status) {
        case rec_none:
            if (uart_ch == '{') {
                rec_active = 0;
                uart_buff[rec_active] = uart_ch;
                rec_status = rec_isrec;
            } else {
                rec_active = 0;
            }
            break;
        case rec_isrec:
            rec_active++;
            uart_buff[rec_active] = uart_ch;
            if (uart_ch == '}') {
                for (size_t i = rec_active + 1; i < 100; i++) {
                    uart_buff[i] = '\0';
                }
                rec_status = rec_none;
                /* 发送消息到消息队列 */
                rt_kprintf("rec : %s\n", uart_buff);
                rt_mq_send_wait(spirit_mq, &uart_msg, 100, 1000);
            }
            break;
        default:
            break;
        }
    }
}

/**
  * @brief  esp-01s 串口初始化
  * @param  None
  * @retval ret
  */
int esp_init(void) {
    rt_err_t ret = RT_EOK;
    char uart_name[RT_NAME_MAX];
    // char str[] = "hello RT-Thread!\r\n";

    rt_strncpy(uart_name, SAMPLE_UART_NAME, RT_NAME_MAX);

    /* 查找系统中的串口设备 */
    serial = rt_device_find(uart_name);
    if (!serial) {
        rt_kprintf("find %s failed!\n", uart_name);
        return RT_ERROR;
    }
    /* 修改串口配置参数 */
    config.baud_rate = BAUD_RATE_9600;     // 修改波特率
    config.data_bits = DATA_BITS_8;        // 数据位 8
    config.stop_bits = STOP_BITS_1;        // 停止位 1
    config.bufsz     = 64;                 // 修改缓冲区 buff size 为 64
    config.parity    = PARITY_NONE;        // 无奇偶校验位

    /* 控制串口设备。通过控制接口传入命令控制字，与控制参数 */
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);
    /* 初始化信号量 */
    uart_sem = rt_sem_create("uart_sem", 0, RT_IPC_FLAG_FIFO);
    /* 以中断接收及轮询发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_input);
    /* 发送字符串 */
    // rt_device_write(serial, 0, str, (sizeof(str) - 1));
    /* 创建线程 */
    esp_thread = rt_thread_create(
        "serial",
        serial_thread_entry,
        RT_NULL,
        1024,
        24,
        10
    );
    // rt_thread_t thread_cjson = rt_thread_create("cjson", serial_cjson_entry, RT_NULL, 1024, 18, 10);
    /* 创建成功则启动线程 */
    if (esp_thread != RT_NULL/*  && thread_cjson != RT_NULL */) {
        rt_thread_startup(esp_thread);
        // rt_thread_startup(thread_cjson);
    } else {
        ret = RT_ERROR;
    }

    return ret;
}
INIT_APP_EXPORT(esp_init);




/**
  * @brief  serial_cjson_entry
  * @param  parameter
  * @retval None
  */
// static void serial_cjson_entry(void* parameter) {
//     while (1) {
//         rt_thread_delay(100);
//         if (is_rec_ok) {
            // cJSON* obj = (cJSON*)0;
            // cJSON* parse = cJSON_Parse(uart_buff);
            // if (parse == (cJSON *)0) {
            //     rt_kprintf("error\n");
            // } else {
            //     // test = cJSON_Parse("{\"tem\"  : 25,\"hum\"  : 69,\"light\": 666,\"led\"  : true,\"beep\" : false}");
            //     obj = cJSON_GetObjectItem(parse, "led");
            //     rt_kprintf("led: %d\n", obj->valueint);
            //     if (obj != (cJSON *)0) {
            //         spirit.led_status = obj->valueint;
            //     }
            //     if (obj != (cJSON*)0) {
            //         obj = cJSON_GetObjectItem(parse, "play");
            //         rt_kprintf("play: %d\n", obj->valueint);
            //         if (obj->valueint) {
            //             if (wavplayer_state_get() == PLAYER_STATE_STOPED) {
            //                 wavplayer_play("/music/tizzy.wav");
            //             }
            //             if (wavplayer_state_get() == PLAYER_STATE_PAUSED) {
            //                 wavplayer_resume();
            //             }
            //         } else {
            //             if (wavplayer_state_get() == PLAYER_STATE_PLAYING) {
            //                 wavplayer_pause();
            //             }
            //         }
            //         spirit.isplaying = wavplayer_state_get();
            //     }
            // }
            // for (size_t i = 0; i < 100; i++) {
            //     uart_buff[i] = '\0';
            // }
            // rt_free(parse);
            // rt_free(obj);
            // is_rec_ok = 0;
//         }
//     }
// }

