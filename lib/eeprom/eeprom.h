#ifndef __EEPROM_INC_EEPROM_H__
#define __EEPROM_INC_EEPROM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx_hal.h"
#include "i2c.h"

#include <stdlib.h>
#include <string.h>

void Init_24LCxx(void);

HAL_StatusTypeDef Write_To_24LCxx(
									I2C_HandleTypeDef *hi2c,
									uint16_t DevAddress,
									uint16_t MemAddress,
									const uint8_t *pData,
									uint16_t len
								);
HAL_StatusTypeDef Read_From_24LCxx(
									I2C_HandleTypeDef * hi2c,
									uint16_t DevAddress,
									uint16_t MemAddress,
									uint8_t *pData,
									uint16_t len
								);
void Test_Write_Read_24LCxx(
								I2C_HandleTypeDef *hi2c,
								uint8_t DevAddress,
								uint16_t MemAddress,
								const uint8_t *wmsg,
								uint8_t *rmsg
							);

#ifdef __cplusplus
}
#endif

#endif /* EEPROM_INC_EEPROM_H_ */
