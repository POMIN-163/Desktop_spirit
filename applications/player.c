/*
 * @Author: Pomin
 * @Date: 2021-11-25 15:22:23
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-12-01 15:22:48
 * @Description:
 */
#include "app_common.h"
#include <dfs_posix.h>

rt_mq_t spirit_mq;

/* 桌面精灵对象 */
my_spirit spirit = {
    .isplaying = 0,
    .led_status = 0
};
/* 储存歌曲名 */
#define MUSIC_NUM 10
uint8_t file_active = 0;
char filename[MUSIC_NUM][50];
/**
 * @brief 读取音乐文件
 *
**/
static void spirit_read(void) {
    DIR* dirp;
    struct dirent* d;
    uint8_t number = 0;
    dirp = opendir("/music");
    if (dirp == RT_NULL) {
        rt_kprintf("open directory error!\n");
    } else {
        /* 读取目录 */
        while ((d = readdir(dirp)) != RT_NULL) {
            if (d->d_type == 1 && strcmp(".wav", d->d_name + strlen(d->d_name) - 4) == 0) {
                strcpy(filename[number], d->d_name);
                number++;
                rt_kprintf("found wav %d %s\n", number, d->d_name);
            }
        }
        /* 关闭目录 */
        closedir(dirp);
    }
    spirit.isplaying = !spirit.isplaying;
    rt_kprintf("find %d songs\n", number);
    for (size_t i = 0; i < number; i++) {
        rt_kprintf("%d -- %s\n", i + 1, filename[i]);
    }
}
/**
 * @brief 更新显示
 *
**/
static void updata_disp(void) {
    rt_device_t esp = rt_device_find("uart1");
    char tem[50];
    char begin[1] = { '{' };
    extern rt_mq_t oled_mq;
    /* 发送到微信小程序 */
    rt_sprintf(tem, "        {\"url\":\"%s\"}", filename[file_active]);
    rt_device_write(esp, 0, tem, 50);

    rt_kprintf("\n%s\n", tem);
    /* 发送到oled显示 */
    rt_sprintf(tem, "%s", filename[file_active]);
    rt_mq_send(oled_mq, tem, 50);
    spirit.isplaying = (wavplayer_state_get() == PLAYER_STATE_PLAYING);
}
/**
 * @brief 播放一首歌曲
 *
**/
static void play_one_music(void) {
    char tempname[50] = "/music/";
    strcat(tempname, filename[file_active]);
    wavplayer_play(tempname);
    rt_kprintf("num %d url: %s\n", file_active, tempname);
}
/**
 * @brief 播放器线程
 *
 * @param p
**/
static void spirit_thread_entry(void* p) {
    char mq_buff[100];
    char* parse_buff;
    spirit_mq = rt_mq_create("spirit_mq", 100, 3, RT_IPC_FLAG_FIFO);
    while(1) {
        rt_mq_recv(spirit_mq, mq_buff, 100,  RT_WAITING_FOREVER);
        my_msg_t rec_obj = (my_msg_t)mq_buff;
        parse_buff = rec_obj->buff;
        /* esp-01s串口发来的消息 */
        if (rec_obj->type == type_uart) {
            cJSON* obj = (cJSON*)0;
            cJSON* parse = cJSON_Parse(parse_buff);
            if (parse == (cJSON *)0) {
                rt_kprintf("error\n");
            } else {
                obj = cJSON_GetObjectItem(parse, "led");
                if (obj != (cJSON *)0) {
                    rt_kprintf("led: %d\n", obj->valueint);
                    spirit.led_status = obj->valueint;
                }
                obj = cJSON_GetObjectItem(parse, "play");
                if (obj != (cJSON*)0) {
                    rt_kprintf("play: %d\n", obj->valueint);
                    switch (obj->valueint)
                    {
                    case 1:
                        /* 上一曲 */
                        if (file_active == 1) {
                            file_active = 9;
                        } else {
                            file_active--;
                        }
                        play_one_music();
                        break;
                    case 2:
                        /* 播放暂停 */
                        if (wavplayer_state_get() == PLAYER_STATE_STOPED) {
                            play_one_music();
                        }
                        else if (wavplayer_state_get() == PLAYER_STATE_PAUSED) {
                            wavplayer_resume();
                        }
                        else if (wavplayer_state_get() == PLAYER_STATE_PLAYING) {
                            wavplayer_pause();
                        }
                        break;
                    case 3:
                        /* 下一曲 */
                        file_active++;
                        file_active %= 9;
                        play_one_music();
                        break;
                    default:
                        break;
                    }
                   updata_disp();
                }
            }
        /* 红外发来的消息 */
        } else if (rec_obj->type == type_irrx) {
            if (rec_obj->buff[1] == 0x44) {
                /* 上一曲 */
                if (file_active == 0) {
                    file_active = 9;
                } else {
                    file_active--;
                }
                play_one_music();
            } else if (rec_obj->buff[1] == 0x43) {
                /* 播放暂停 */
                if (wavplayer_state_get() == PLAYER_STATE_STOPED) {
                    play_one_music();
                }
                else if (wavplayer_state_get() == PLAYER_STATE_PAUSED) {
                    wavplayer_resume();
                }
                else if (wavplayer_state_get() == PLAYER_STATE_PLAYING) {
                    wavplayer_pause();
                }
            } else if (rec_obj->buff[1] == 0x40) {
                /* 下一曲 */
                file_active++;
                file_active %= 9;
                play_one_music();
            }
            updata_disp();
        }
    }
}
/**
 * @brief 播放器初始化
 *
 * @return int
**/
int spirit_init(void) {
    rt_thread_delay(2000);
    spirit_read();
    file_active = 1;
    updata_disp();
    rt_err_t ret = RT_EOK;
    rt_thread_t thread = rt_thread_create("spirit", spirit_thread_entry, RT_NULL, 2048, 22, 100);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL) {
        rt_thread_startup(thread);
    } else {
        ret = RT_ERROR;
    }

    return ret;
}

INIT_APP_EXPORT(spirit_init);
