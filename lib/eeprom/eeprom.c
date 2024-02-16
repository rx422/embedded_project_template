#include "eeprom.h"

void Init_24LCxx(void)
{
	// pass
}

HAL_StatusTypeDef Write_To_24LCxx(
									I2C_HandleTypeDef *hi2c,
									uint16_t DevAddress,
									uint16_t MemAddress,
									const uint8_t *pData,
									uint16_t len
								)
{
	HAL_StatusTypeDef returnValue;
	uint8_t *data;

	/* Сначала мы выделяем временный буфер для хранения адреса памяти пункта
	   назначения и данных для сохранения */
	data = (uint8_t *) malloc(sizeof(uint8_t) * (len + 2));

	/* Вычисляем MSB и LSB части адреса памяти */
	data[0] = (uint8_t) ((MemAddress & 0xFF00) >> 8);
	data[1] = (uint8_t) (MemAddress & 0xFF);

	/* копируем содержимое массива pData во временный буфер */
	memcpy(data + 2, pData, len);

	/* Теперь мы готовы передать буфер по шине I2C */
	returnValue = HAL_I2C_Master_Transmit(hi2c, DevAddress, data, len + 2, HAL_MAX_DELAY);
	if (returnValue != HAL_OK)
	{
		return returnValue;
	}
	free(data);

	/* Ждем, пока EEPROM эффективно сохранит данные в памяти */
	while (HAL_I2C_Master_Transmit(hi2c, DevAddress, 0, 0, HAL_MAX_DELAY) != HAL_OK);
	return HAL_OK;
}

HAL_StatusTypeDef Read_From_24LCxx(
									I2C_HandleTypeDef *hi2c,
									uint16_t DevAddress,
									uint16_t MemAddress,
									uint8_t *pData,
									uint16_t len
								)
{
	HAL_StatusTypeDef returnValue;
	uint8_t addr[2];

	/* Вычисляем MSB и LSB части адреса памяти */
	addr[0] = (uint8_t) ((MemAddress & 0xFF00) >> 8);
	addr[1] = (uint8_t) (MemAddress & 0xFF);

	/* Сначала отправляем адрес ячейки памяти, откуда начинаем считывать данные */
	returnValue = HAL_I2C_Master_Transmit(hi2c, DevAddress, addr, 2, HAL_MAX_DELAY);
	if (returnValue != HAL_OK)
	{
		return returnValue;
	}

	/* Далее мы можем получить данные из EEPROM */
	returnValue = HAL_I2C_Master_Receive(hi2c, DevAddress, pData, len, HAL_MAX_DELAY);

	return returnValue;
}

void Test_Write_Read_24LCxx(
					I2C_HandleTypeDef *hi2c,
					uint8_t DevAddress,
					uint16_t MemAddress,
					const uint8_t *wmsg,
					uint8_t *rmsg
				)
{
	Write_To_24LCxx(hi2c, DevAddress, MemAddress, wmsg, strlen((char*)wmsg) + 1);
	Read_From_24LCxx(hi2c, DevAddress, MemAddress, rmsg, strlen((char*)wmsg) + 1);
}
