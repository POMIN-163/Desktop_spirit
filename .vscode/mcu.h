/*
 * @Author: Pomin
 * @Date: 2021-11-14 23:09:25
 * @Github: https://github.com/POMIN-163
 * @LastEditTime: 2021-11-27 16:53:26
 * @Description:
 */
/**
 * @file mcu.h
 * @author Pomin
 * @brief mcu 种类选择
 * @date 2021-08-03
 *
 * @copyright Copyright (c) 2021
 *
**/
#ifndef M_C_U___
#define M_C_U___

#ifdef USE_VSCODE
    #include <stdint-gcc.h>
    #undef DOUBLE_CLICK
    #define DOUBLE_CLICK 0
    #ifdef stm32
        #define __CC_ARM
        #define STM32F10X_MD // F1
        // #define STM32F103xB
        // #define STM32F40_41xxx // F4
        #define STM32F407xx // F407
        #define USE_STDPERIPH_DRIVER // 标准库
        // #define USE_HAL_DRIVER // HAL 库
    #endif

    #ifdef c51
            #define __C51__
            #define __VSCODE_C51__
            #define reentrant
            #define compact
            #define small
            #define large
            #define data
            #define idata
            #define pdata
            #define bdata
            #define xdata
            #define code
            #define bit char
            #define sbit char
            #define sfr char
            #define sfr16 int
            #define sfr32 int
            #define interrupt
            #define using
            #define _at_
            #define _priority_
            #define _task_
    #endif

    #ifdef tcxxx
        #define __TASKING__
        #define __attribute__(n)
        #define __interrupt(n)
        #define __vector_table(vectabNum)
        #define __sfrbit16 short
        #define __sfrbit32 int
        #define __fract
        #define __sfract
        #define __laccum
    #endif

    #ifdef rt1064
        #define CPU_MIMXRT1064DVL6A
        #define SKIP_SYSCLK_INIT
        #define XIP_EXTERNAL_FLASH 1
        #define XIP_BOOT_HEADER_DCD_ENABLE  1
        #define XIP_BOOT_HEADER_ENABLE  1
        #define PRINTF_FLOAT_ENABLE 1
        #define SCANF_FLOAT_ENABLE 1
        #define PRINTF_ADVANCED_ENABLE 1
        #define SCANF_ADVANCED_ENABLE 1
        #define FSL_DRIVER_TRANSFER_DOUBLE_WEAK_IRQ 0
        #define USB_STACK_BM
    #endif
#endif

#endif