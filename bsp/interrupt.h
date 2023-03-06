#ifndef _INTERRUPT_H
#define _INTERRUPT_H
#include "main.h"
struct keys
{
	bool staKey;//读取后,0为打开,1为松手
	uchar timesKey;//按下的次数
	uchar flagKey;//0表示没有按下,1表示短按,2表示长按
	
};

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);//按键
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);//串口接收
#endif