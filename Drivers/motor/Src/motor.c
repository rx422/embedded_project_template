#include "motor.h"

void motor_init(motor_t *motor,
				const uint8_t motor_id,
				const uint8_t motor_enable_pin,
				const uint8_t motor_pin_a,
				const uint8_t motor_pin_b,
				GPIO_TypeDef *GPIO_Motor_port)
{
	motor->motor_id = motor_id;
	motor->motor_power = 0;
	motor->direction = STOP;
	motor->motor_enable_pin = motor_enable_pin;
	motor->motor_pin_a = motor_pin_a;
	motor->motor_pin_b = motor_pin_b;
	motor->GPIO_Motor_port = GPIO_Motor_port;
}

void motor_set_direction(motor_t *motor, motor_dir_t direction)
{
	motor->direction = direction;
}

void motor_start(motor_t *motor)
{
	HAL_GPIO_WritePin(	motor->GPIO_Motor_port,
						motor->motor_pin_a,
						motor->direction & MASK_PIN_A);

	HAL_GPIO_WritePin(	motor->GPIO_Motor_port,
						motor->motor_pin_b,
						motor->direction & MASK_PIN_B);
}

void motor_stop(motor_t *motor)
{
	HAL_GPIO_WritePin(	motor->GPIO_Motor_port,
						motor->motor_pin_a,
						GPIO_PIN_RESET);

	HAL_GPIO_WritePin(	motor->GPIO_Motor_port,
						motor->motor_pin_b,
						GPIO_PIN_RESET);
}

