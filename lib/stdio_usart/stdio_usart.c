#include <stdio_usart.h>

int _write(int file, char *ptr, int len)
{
	(void) file;

	HAL_UART_Transmit(&huart2, (uint8_t*) ptr, len, HAL_MAX_DELAY);

	return len;
}

int _read(int file, char *ptr, int len)
{
	(void) file;

	__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_ORE);
	HAL_UART_Receive(&huart2, (uint8_t*) ptr, len, HAL_MAX_DELAY);

	return len;
}

// Another method of redirecting input-output flows is presented below.
/*
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2, (uint8_t*) &ch, 1, HAL_MAX_DELAY);

	return HAL_OK;
}

int __io_getchar(void)
{
	int ch;

	//	__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_ORE);
	__HAL_UART_CLEAR_OREFLAG(&huart2);

	HAL_UART_Receive(&huart2, (uint8_t*) &ch, 1, HAL_MAX_DELAY);

	return ch;
}
*/

void stdio_usart_init(void)
{
	printf("%s", "");
}
