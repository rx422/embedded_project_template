/*
 ******************************************************************************
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
 ******************************************************************************
 */

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

