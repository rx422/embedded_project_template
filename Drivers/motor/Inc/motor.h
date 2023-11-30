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

#ifndef MOTOR_H
#define MOTOR_H

#include "stm32g0xx_hal.h"
#include <stdint.h>

/*
 * A	B	DIR
 * 0    1   RIGHT
 * 1	0	LEFT
 * 0	0	STOP
 */

#define MASK_PIN_A	0b00000010u
#define MASK_PIN_B	0b00000001u

typedef enum MOTOR_DIRECTIONS
{
	STOP = 0u,
	RIGHT,
	LEFT,
} motor_dir_t;

typedef struct Motor
{
	uint8_t motor_id;
	int16_t motor_power;
	motor_dir_t direction;
	uint8_t motor_enable_pin;
	uint8_t motor_pin_a;
	uint8_t motor_pin_b;
	GPIO_TypeDef *GPIO_Motor_port;
} motor_t;

void motor_init(motor_t *motor,
				const uint8_t motor_id,
				const uint8_t motor_enable_pin,
				const uint8_t motor_pin_a,
				const uint8_t motor_pin_b,
				GPIO_TypeDef *GPIO_Motor_port);
void motor_set_direction(motor_t *motor, motor_dir_t direction);
void motor_start(motor_t *motor);
void motor_stop(motor_t *motor);

#endif
