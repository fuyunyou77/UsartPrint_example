/*
 * @Author: fuyunyou
 * @Date: 2024-09-24 13:42:35
 * @LastEditors: fuyunyou
 * @LastEditTime: 2024-10-18 11:47:43
 * @Description: 串口驱动，按照指定数据格式收发数据
 * 数据格式：帧头'['+命令段'command'+分隔标识符':'+数据段'data'+帧尾']'
 * 命令和数据长度最大均为50字节
 * @FilePath: \switch\hardware\UART.c
 */
#include "UART.h"

char Rx_Packet[10][20];
uint8_t Rx_Flag=0;
uint8_t Row_count=0;

static void NVIC_Config(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    // NVIC中断向量组选择
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;

    NVIC_Init(&NVIC_InitStructure);
}

void USART1_Config(void)
{

    RCC_APB2PeriphClockCmd(USART1_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    // 接收引脚初始化为上拉模式
    GPIO_InitStructure.GPIO_Pin  = USART1_GPIO_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);

    // 发送引脚初始化为复用推挽输出
    GPIO_InitStructure.GPIO_Pin  = USART1_GPIO_TX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(USART1_GPIO_PORT, &GPIO_InitStructure);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate            = USART_BAUDRATE;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode                = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity              = USART_Parity_No;
    USART_InitStructure.USART_StopBits            = USART_StopBits_1;
    USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStructure);

    // 开启串口中断
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_Config();

    USART_Cmd(USART1, ENABLE);
}


/**
 * @description: 串口发送一个字节
 * @param {USART_TypeDef} *pUSARTx
 * @param {uint8_t} ch
 * @retval: null
 */
void USART_SendByte(USART_TypeDef *pUSARTx, uint8_t ch)
{
    USART_SendData(pUSARTx, ch);

    while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/**
 * @description: 串口发送任意长度字符串
 * @param {USART_TypeDef} *pUSARTx
 * @param {char} *str
 * @retval:  null
 */
void USART_SendString( USART_TypeDef * pUSARTx, char *str)
{
    unsigned int k=0;
    do {
        USART_SendByte( pUSARTx, *(str + k) );
        k++;
    } while (*(str + k)!='\0');

    /* 等待发送完成 */
    while (USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET) {
    }
}

/**
 * @description: 求X的Y次方
 * @param {uint32_t} X
 * @param {uint32_t} Y
 * @retval:  X的Y次方的值
 */
uint32_t USART_Pow(uint32_t X,uint32_t Y)
{
    uint32_t result =1;

    while(Y--)
    {
        result*=X;
    }

    return result;
}

/**
 * @description: 串口发送数字
 * @param {USART_TypeDef *} pUSARTx
 * @param {uint32_t} num:   要发送的数字，范围：0-4294967295
 * @param {uint8_t} length: 发送数字的长度，范围：0-10
 * @retval:  null
 */
void USART_SendNum(USART_TypeDef * pUSARTx,uint32_t num,uint8_t length)
{
    uint8_t i,temp;
    for(i=0;i<length;i++)
    {
        temp=num/USART_Pow(10,length-i-1)%10+'0';
        USART_SendByte(pUSARTx,temp);
    }
}

/**
 * @description: 使用可变参数函数实现的串口打印函数，功能与printf类似
 * @param {USART_TypeDef *} pUSARTx：选择输出的串口
 * @param {char} *format：从这个参数开始，使用方法与printf类似
 * @retval:  null
 */
void USART_Printf(USART_TypeDef * pUSARTx,char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	USART_SendString(pUSARTx,String);		//串口发送字符数组（字符串）
}

/**
 * @description: 串口中断处理函数，可以判断是哪个串口触发的中断，使用状态机实现了简单的接收数据帧
 * @retval:  null
 */
void USART1_IRQHandler(void)
{ 
    
    static uint8_t state=0;
    static uint8_t pRx_Packet_Col=0;
    static uint8_t pRx_Packet_Row=0;

    if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
    {
        uint8_t Rx_data=USART_ReceiveData(USART1);

        //状态0，接收包头
        if(state==0)
        {
            if(Rx_data=='[')
            {
                state=1;
                pRx_Packet_Col=0;
                pRx_Packet_Row=0;
                Row_count=0;
            }
        }
        //状态1，接收命令
        else if (state==1)
        {
            if(Rx_data==',')
            {
                pRx_Packet_Row++;
                pRx_Packet_Col=0;
                Rx_Packet[pRx_Packet_Row][pRx_Packet_Col++]='\0';
            }
            else if(Rx_data==']')
            {
                Rx_Packet[pRx_Packet_Row][pRx_Packet_Col++]='\0';
                state=0;
                Row_count=pRx_Packet_Row;
            }
            else 
            {
               Rx_Packet[pRx_Packet_Row][pRx_Packet_Col++]=Rx_data;
                
            }
        }

        USART_ClearITPendingBit(USART1,USART_IT_RXNE);
    }
}