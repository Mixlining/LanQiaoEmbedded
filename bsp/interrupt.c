#include "interrupt.h"
#include "usart.h"
struct keys key[4] = {0, 0, 0};
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) // 任何一个定时器中断都会进入
{
	if (htim->Instance == TIM3)
	{
		key[0].staKey = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
		key[1].staKey = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1);
		key[2].staKey = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2);
		key[3].staKey = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0);

		for (int i = 0; i < 4; i++)
		{
			if (key[i].staKey == 0)
			{
				key[i].timesKey++;
			}
			else
			{
				if (key[i].timesKey > 70)
				{
					key[i].flagKey = 2;
					key[i].timesKey = 0;
				}
				else if (key[i].timesKey > 2)
				{
					key[i].flagKey = 1;
					key[i].timesKey = 0;
				}
				else
				{
					key[i].timesKey = 0;
				}
			}
		}
	}
}

uint8_t rx;
char receiveData[30];
uchar rePoint; // 指示位置

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	receiveData[rePoint++] = rx;
	HAL_UART_Receive_IT(&huart1, &rx, 1); // 必须为1,每次接受1个字符
}
