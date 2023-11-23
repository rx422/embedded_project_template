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
