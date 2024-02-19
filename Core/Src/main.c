/* USER CODE BEGIN Header */
/******************************************************************************
 embedded_project_template - template for Embedded C projects
 that are developed for the NUCLEO-G071RB and use the HAL layer
 Copyright (C) 2023  rx422
 ******************************************************************************
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
 ******************************************************************************/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "stdio_usart.h"
#include "motor.h"
#include "eeprom.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUTTON_READ_RECURENCE 200
#define LED_GREEN_GPIO_Port MOTOR_A_GPIO_Port
#define LED_GREEN_Pin MOTOR_A_Pin

#define JOSTICK_X_RAW adc_buffer[0]
#define JOSTICK_Y_RAW adc_buffer[1]
#define TEMPERATURE_RAW adc_buffer[2]

#define READ_JOSTYCK_RECURENCE_FAST 100
#define READ_JOSTYCK_RECURENCE_SLOW 500
#define BUTTON_DEBOUNCE_TIME 200

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint16_t adc_buffer[6];

enum temperature_limits
{
	TEMPERATURE_MIN = 16,
	TEMPERATURE_MAX = 30
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

void changeOnceIfButtonPressed(void);
void changeIfButtonPressed(void);
void motorControl(motor_t *motor, const char *command);
uint8_t needReadUsartInputBuff(void);

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
	// No buffers will be used for input and output streams.
	setvbuf(stdin, NULL, _IONBF, 0);
	setvbuf(stdout, NULL, _IONBF, 0);

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
	stdio_usart_init();

	char output_data_buff[20] = "Data from EEPROM";
	char input_data_buff[20] = {0};

	printf("Output: %s\n", output_data_buff);
	// Test_Write_Read_24LCxx(
	// 				&hi2c1,
	// 				0xA0,
	// 				0x0000,
	// 				(uint8_t*) output_data_buff,
	// 				(uint8_t*) input_data_buff
	// 			);
	printf("Input: %s\n", input_data_buff);

	static motor_t motor;

	const uint8_t FIRST_MOTOR_ID = 1;

	motor_init(
		&motor,
		FIRST_MOTOR_ID,
		0u,
		MOTOR_A_Pin,
		MOTOR_B_Pin,
		MOTOR_A_GPIO_Port);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buffer, 6);

	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
		while (1)
		{
			// static uint32_t next_show_time = 0;
			static uint32_t read_jostick_time = 0;
			static uint32_t set_angle_time = 0;
			static uint32_t read_button_time = 0;

			static int8_t wanted_temperature = 25;
			static int8_t temporary_wanted_temperature = 25;

			static uint8_t need_show_temperature = true;
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

			if (needReadUsartInputBuff())
			{
				scanf("%20s", input_data_buff);
				printf("\a");					   // Bip sound
				printf("\f%s\n", input_data_buff); // Refresh display and print input_data_buff
				motorControl(&motor, input_data_buff);
			}

			if (need_show_temperature)
			{
				printf(
					"\fWant: %d\tSet: %d\n",
					temporary_wanted_temperature,
					wanted_temperature);
				need_show_temperature = false;
			}

			if (HAL_GPIO_ReadPin(JOSTICK_SW_GPIO_Port, JOSTICK_SW_Pin) == GPIO_PIN_RESET)
			{
				if (read_button_time < HAL_GetTick())
				{
					wanted_temperature = temporary_wanted_temperature;
					need_show_temperature = true;
					printf("\a");
				}
				read_button_time = HAL_GetTick() + BUTTON_DEBOUNCE_TIME;
			}

			if (read_jostick_time < HAL_GetTick())
			{
				if (JOSTICK_Y_RAW > 2200)
				{
					read_jostick_time = HAL_GetTick() + (JOSTICK_Y_RAW > 3200 ? READ_JOSTYCK_RECURENCE_FAST : READ_JOSTYCK_RECURENCE_SLOW);

					if (temporary_wanted_temperature < TEMPERATURE_MAX)
					{
						temporary_wanted_temperature++;
						need_show_temperature = true;
					}
				}

				if (JOSTICK_Y_RAW < 1800)
				{
					read_jostick_time = HAL_GetTick() + (JOSTICK_Y_RAW < 800 ? READ_JOSTYCK_RECURENCE_FAST : READ_JOSTYCK_RECURENCE_SLOW);

					if (temporary_wanted_temperature > TEMPERATURE_MIN)
					{
						temporary_wanted_temperature--;
						need_show_temperature = true;
					}
				}
			}

			static uint16_t angle = 300;

			if (set_angle_time < HAL_GetTick())
			{
				static uint16_t last_angle = 300;

				if (JOSTICK_X_RAW > 2200)
				{
					if (angle < 1200)
					{
						angle += 10;
					}
				}

				if (JOSTICK_X_RAW < 1800)
				{
					if (angle > 230)
					{
						angle -= 10;
					}
				}

				if (last_angle != angle)
				{				
					__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, angle);
					last_angle = angle;
				}
				printf("\fAngle: %d\n", angle);
				set_angle_time = HAL_GetTick() + 10;
			}

			

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void changeOnceIfButtonPressed(void)
{
	static uint32_t next_show_time = 0;

	if (next_show_time < HAL_GetTick())
	{
		uint8_t buttonState = !HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);

		if (buttonState)
		{
			HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

			next_show_time = HAL_GetTick() + BUTTON_READ_RECURENCE;
		}
	}
}

void changeIfButtonPressed(void)
{
	static uint32_t next_show_time = 0;

	uint8_t buttonState = !HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);

	if (buttonState)
	{
		if (next_show_time < HAL_GetTick())
		{
			HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
		}
		next_show_time = HAL_GetTick() + BUTTON_READ_RECURENCE;
	}
}

void motorControl(motor_t *motor, const char *command)
{
	if (!strcmp(command, "motorRight"))
	{
		motor_set_direction(motor, RIGHT);
		motor_start(motor);
	}

	if (!strcmp(command, "motorLeft"))
	{
		motor_set_direction(motor, LEFT);
		motor_start(motor);
	}

	if (!strcmp(command, "motorStart"))
	{
		motor_start(motor);
	}

	if (!strcmp(command, "motorStop"))
	{
		motor_stop(motor);
	}
}

inline uint8_t needReadUsartInputBuff(void)
{
	return (__HAL_UART_GET_FLAG(&huart2, UART_FLAG_RXNE) != RESET);
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
