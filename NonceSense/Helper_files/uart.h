#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

//Initializes USART2 on PA2 (TX) and PA3 (RX) at 115200 baud (8-N-1).
void uart_init(void);

//Transmits a single character (blocking wait on TXE).
void uart_putc(char c);

//Transmits a null-terminated string over UART.
void uart_print(const char *str);

//Non-blocking read of a single received character.out_char = Pointer to store the character if available. returns true if a byte was read, false if the RX buffer is empty.
bool uart_getc_nonblocking(char *out_char);

#endif 