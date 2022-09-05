#ifndef TLAY2_UART_H
#define TLAY2_UART_H

#include <stdint.h>

void uart_init(void);
intptr_t uart_get_byte(uint8_t *byte);
intptr_t uart_send_byte(uint8_t byte);
uint8_t uart_getnbytes(void);

#endif
