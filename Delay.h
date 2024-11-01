/*
 * @Author: fuyunyou
 * @Date: 2024-09-24 13:52:03
 * @LastEditors: fuyunyou
 * @LastEditTime: 2024-09-24 18:26:30
 * @Description: 
 * @FilePath: \switch\user\Delay.h
 */
#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f10x.h"

void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);
void Delay_s(uint32_t s);

#endif
