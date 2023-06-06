
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "CLCD_I2C.h"
#include "MPU6050.h"
#include <stdio.h>

I2C_HandleTypeDef hi2c1;   // 

/* USER CODE BEGIN PV */
CLCD_I2C_Name LCD1;
uint32_t volatile state = 0, resetCounter = 0;
extern uint32_t volatile delayLED;
extern unsigned int stepCount;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);


/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void EXTI9_5_IRQHandler(void)
{

  /* USER CODE END EXTI9_5_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
  /* USER CODE BEGIN EXTI9_5_IRQn 1 */
	if((GPIOA->IDR & (1u<<8)) == 0) {
		state++;
		
		for(int i = 0; i < 500000; i++);
		HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
	}
	
	if((GPIOA->IDR & (1u<<9)) == 0) {
		resetCounter = 1;
		stepCount = 0;
		
		for(int i = 0; i < 500000; i++);
		HAL_NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
	}
	
}
/* USER CODE END 0 */


int main(void)
{
  /* USER CODE BEGIN 1 */
  SysTick->LOAD = SystemCoreClock / 1000;
	SysTick->CTRL = 0x7;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();


  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	MPU6050_Init();
	CLCD_I2C_Init(&LCD1,&hi2c1,0x4E,16,2);
	char buf[4];
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		// kiem tra yeu cau resetCounter
		if(resetCounter == 1) {
			resetCounter = 0;
			
			CLCD_I2C_Clear(&LCD1);							// xoa man hinh
			CLCD_I2C_SetCursor(&LCD1,1,0);			// dat con tro LCD tai hang 1 cot 0
		  CLCD_I2C_WriteString(&LCD1, "So buoc chan");	// ghi chuoi "so buoc chan"
			CLCD_I2C_SetCursor(&LCD1,5,1);								//dat con tro LCD tai hang 2 cot 5
		  CLCD_I2C_WriteString(&LCD1, "0");							//Ghi chuoi
		}
		
		if(state % 2 == 0) {
				HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET); // tat led chan GPIO_PIN_6
			if(delayLED >=1000) {					// kiem tra delay
				delayLED = 0;								
				HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_13);		// nhap nhay den LED tren chan GPIOC_PIN_13
			}	
			
			CLCD_I2C_SetCursor(&LCD1,1,0);
			CLCD_I2C_WriteString(&LCD1, "So buoc chan");
			
			sprintf(buf, "%d", MPU6050_Counter());   // chuyen doi gia tri MPU6050_Counter() thành chuõi buf
			CLCD_I2C_SetCursor(&LCD1,5,1);		//dat con tro LCD tai hang 2 cot 5
			CLCD_I2C_WriteString(&LCD1, buf);				// ghi gia tri cua chuoi len LCD
			
			HAL_Delay(50);
		} else {
			HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);			// bat LED trên chân 
			HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;    //thiet lap loai dong ho CPU,
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;											// S? d?ng HSE làm ngu?n SYSCLK
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)					// cau hinh tan so dong ho
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;								// chon I2c1
  hi2c1.Init.ClockSpeed = 100000;				// toc do truyen thong 100kHZ
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2; 				// Chu ky nhiem vu cua I2c là 2
  hi2c1.Init.OwnAddress1 = 0;											//dia chi rieng cua thiet bi off
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;	// su dung che do dia chi 7_bit
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;	// vo hieu hoa che do dia chi kep
  hi2c1.Init.OwnAddress2 = 0;						// dia chi rieng cua rieng biet 
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;			// Vo hieu che do khong keo dai
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)				// khoi tao I2c1
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

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

#ifdef  USE_FULL_ASSERT
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
