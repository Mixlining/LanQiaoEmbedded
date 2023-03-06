/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
uchar view = 0;			  // 显示界面
extern struct keys key[]; // 按键
uint frqPA1 = 1000;		  // PA1的输出频率
uint frqPA7 = 1000;		  // PA7的输出频率
uint dutyPA1 = 10;		  // PA1占空比
uint dutyPA7 = 10;		  // PA7占空比
bool model = 0;			  // 控制模式,0为按键控制
#define DELAY_TIME 100	  // LED闪烁时间

extern uint8_t rx;			 // 每次接收的字符
extern char receiveData[30]; // 存储接受的字符串
extern uchar rePoint;		 // 指示位置

char carType[5];
char carData[5];
char carTime[13];

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM2_Init();
	MX_TIM17_Init();
	MX_TIM3_Init();
	MX_USART1_UART_Init();
	/* USER CODE BEGIN 2 */
	// ledDisplay(0x00);//默认全灭
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_All, GPIO_PIN_SET); // 默认全关闭
	LCD_Init();
	HAL_TIM_Base_Start_IT(&htim3);			   // 打开按键定时器
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);  // PA1的pwm输出
	HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1); // PA7的pwm输出
	HAL_UART_Receive_IT(&huart1, &rx, 1);	   // 启用中断,也算是初始化
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */

	LCD_Clear(Black);
	LCD_SetBackColor(Black);
	LCD_SetTextColor(White);

	LCD_DisplayStringLine(Line0, (uint8_t *)"                    ");
	LCD_DisplayStringLine(Line1, (uint8_t *)"                    ");
	LCD_DisplayStringLine(Line2, (uint8_t *)"                    ");
	LCD_DisplayStringLine(Line3, (uint8_t *)"                    ");
	LCD_DisplayStringLine(Line4, (uint8_t *)"                    ");
	LCD_DisplayStringLine(Line5, (uint8_t *)"                    ");
	LCD_DisplayStringLine(Line6, (uint8_t *)"                    ");
	LCD_DisplayStringLine(Line7, (uint8_t *)"                    ");
	LCD_DisplayStringLine(Line8, (uint8_t *)"                    ");
	LCD_DisplayStringLine(Line9, (uint8_t *)"                    ");

	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */

		keyProcess();
		pwmProcess();
		// uart_TX();
		uartProcess();
		displayView();
		ledProcess();
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV3;
	RCC_OscInitStruct.PLL.PLLN = 20;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
	{
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void uartProcess()
{
	// 读取到数据
	if (rePoint >0 )
	{
		if (model == 1)
		{
			// 数据位数正常
			if (receiveData[0] == '@'&&rePoint==1)
				view = 0;
			else if (receiveData[0] == '#'&&rePoint==1)
				view = 1;
			else
			{
				uartTX();
			}
			rePoint = 0;				// 清空
			memset(receiveData, 0, 30); // 清空
		}
		else
		{
			char str[20];

			sprintf(str, "KEY CONTROL\r\n");
			HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 50);
			rePoint = 0;				// 清空
			memset(receiveData, 0, 30); // 清空
		}
	}
}
void ledProcess()
{
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_All, GPIO_PIN_SET);
	if (model == 0)
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);
	if (view == 0)
	{
		if (frqPA1 > frqPA7)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // 开
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
			HAL_Delay(DELAY_TIME);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET); // 灭
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
			HAL_Delay(DELAY_TIME);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); // 开启
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
		}
	}
	else
	{
		if (frqPA7 > frqPA1)
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); // 开
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
			HAL_Delay(DELAY_TIME);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); // 灭
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
			HAL_Delay(DELAY_TIME);
		}
		else
		{
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET);
		}
	}
}

void pwmProcess()
{
	__HAL_TIM_SetCompare(&htim2, TIM_CHANNEL_2, dutyPA1);	  // 设置PA1的占空比,因为重装载值是100,所以比列不需要运算
	__HAL_TIM_SetCompare(&htim17, TIM_CHANNEL_1, dutyPA7);	  // 设置PA7的占空比
	__HAL_TIM_SET_PRESCALER(&htim2, ((800000 / frqPA1) - 1)); // 设置频率 80000000/100/目标频率 -1
	__HAL_TIM_SET_PRESCALER(&htim2, ((800000 / frqPA1) - 1));
}

void keyProcess()
{
	// B1
	if (key[0].flagKey > 0)
	{
		if (view == 0)
		{
			frqPA1 += 1000;
			if (frqPA1 > 10000)
				frqPA1 = 1000;
		}
		else
		{
			frqPA7 += 1000;
			if (frqPA7 > 10000)
				frqPA7 = 1000;
		}
		key[0].flagKey = 0;
	}
	// B2
	if (key[1].flagKey > 0)
	{
		if (view == 0)
		{
			dutyPA1 += 10;
			if (dutyPA1 > 90)
				dutyPA1 = 10;
		}
		else
		{
			dutyPA7 += 10;
			if (dutyPA7 > 90)
				dutyPA7 = 10;
		}
		key[1].flagKey = 0;
	}
	// B3
	if (key[2].flagKey > 0)
	{
		if (model == 0) // 在按键模式下生效
		{
			view = !view;
			LCD_Clear(Black);
		}
		key[2].flagKey = 0;
	}
	// B4
	if (key[3].flagKey > 0)
	{
		model = !model;
		key[3].flagKey = 0;
	}
}
void displayView()
{
	if (view == 0)
	{
		LCD_DisplayStringLine(Line3, (uint8_t *)"        PA1         ");
		char text[30];
		// int i=2000;
		sprintf(text, "    F:%dHz        ", frqPA1);
		LCD_DisplayStringLine(Line5, (uint8_t *)text);

		// i=20;
		sprintf(text, "    D:%d%%        ", dutyPA1);
		LCD_DisplayStringLine(Line7, (uint8_t *)text);
	}
	else
	{
		LCD_DisplayStringLine(Line3, (uint8_t *)"        PA7         ");
		char text[30];
		// int i=1000;
		sprintf(text, "    F:%dHz        ", frqPA7);
		LCD_DisplayStringLine(Line5, (uint8_t *)text);

		// i=10;
		sprintf(text, "    D:%d%%        ", dutyPA7);
		LCD_DisplayStringLine(Line7, (uint8_t *)text);
	}
}
void uartTX()
{
	char str[20];
	// sprintf(str,"frqPA1=%d\r\n",frqPA1);
	sprintf(str, "ERROR\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t *)str, strlen(str), 50);
	// HAL_Delay(500);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	   ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
