/*
 * @Author: fuyunyou
 * @Date: 2024-09-24 11:52:45
 * @LastEditors: fuyunyou
 * @LastEditTime: 2024-10-19 16:06:24
 * @Description: 
 * @FilePath: \switch\src\main.c
 */

#include "stm32f10x.h"
#include "Usart.h"
#include "Delay.h"

int main()
{
    USART1_Config();
    USART2_Config();
    USART3_Config();
    UART5_Config(); 
    ADC_Config();
    HSD_OUT_GPIO_Config();
    HDC1080_Config();

    while (1)
    {
        //接收数据帧调试
        //USART_SendString(USART1,"----------数据帧调试----------\n");
        // for(int i=0;i<=Row_count;i++)
        // {
        //     USART_SendString(USART1,Rx_Packet[i]);
        //     USART_Printf(USART1,"\n");
        // }

        //串口调试
        //USART_SendString(USART1,"----------串口调试----------\n");
        //USART_SendNum(USART1,1000,4);
		//Delay_ms(1);
        //USART_Printf(USART1,"testargs:%d \n",Voltage);
        //Delay_ms(100);
        //USART_SendString(USART1,"a");

        //Delay_ms(200);

    }
}
