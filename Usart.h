/*
 * @Author: fuyunyou
 * @Date: 2024-09-24 13:42:12
 * @LastEditors: fuyunyou
 * @LastEditTime: 2024-10-09 14:07:38
 * @Description:
 * @FilePath: \switch\hardware\UART.h
 */
#ifndef __USART_H
#define __USART_H

#include "stm32f10x.h"
#include "Delay.h"
#include <stdio.h>
#include <stdarg.h>

#define USART1_GPIO_CLK    RCC_APB2Periph_USART1
#define USART1_GPIO_PORT   GPIOA
#define USART1_GPIO_RX_PIN GPIO_Pin_10
#define USART1_GPIO_TX_PIN GPIO_Pin_9

#define USART_BAUDRATE     9600

extern uint8_t Rx_Flag;
extern char Rx_Packet[10][20];
extern uint8_t Row_count;

void USART1_Config(void);
void USART_SendString(USART_TypeDef *pUSARTx, char *str);
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch);
void USART_SendNum(USART_TypeDef * pUSARTx,uint32_t num,uint8_t length);
void USART_Printf(USART_TypeDef * pUSARTx,char *format, ...);

#endif
