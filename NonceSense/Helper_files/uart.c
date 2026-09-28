//Skeleton UART Driver for STM32F4/F1 series Microntrollers.
//It covers basic initialization, character transmission, string transmission, and non-blocking character reception.
//Use CMSIS Header files for register definitions
#include "uart.h"

#if defined(STM32F4) || defined(STM32F401xE) || defined(STM32F411xE)
  #include "stm32f4xx.h"
#elif defined(STM32F1) || defined(STM32F103xB)
  #include "stm32f1xx.h"
#else
  // Standalone fallback register definitions if building without external device header
  #include <stdint.h>
  #define RCC_AHB1ENR   (*(volatile uint32_t *)0x40023830UL)
  #define RCC_APB1ENR   (*(volatile uint32_t *)0x40023840UL)
  #define GPIOA_MODER   (*(volatile uint32_t *)0x40020000UL)
  #define GPIOA_AFRL    (*(volatile uint32_t *)0x40020020UL)
  #define USART2_SR     (*(volatile uint32_t *)0x40004400UL)
  #define USART2_DR     (*(volatile uint32_t *)0x40004404UL)
  #define USART2_BRR    (*(volatile uint32_t *)0x40004408UL)
  #define USART2_CR1    (*(volatile uint32_t *)0x4000440CUL)
#endif

void uart_init(void) {
#if defined(RCC_AHB1ENR)
    // STM32F4 register setup
    RCC_AHB1ENR |= (1UL << 0);   // Enable GPIOA clock
    RCC_APB1ENR |= (1UL << 17);  // Enable USART2 clock

    // PA2 (TX) and PA3 (RX) -> Alternate Function AF7
    GPIOA_MODER &= ~((3UL << (2 * 2)) | (3UL << (3 * 2)));
    GPIOA_MODER |=  ((2UL << (2 * 2)) | (2UL << (3 * 2)));
    GPIOA_AFRL  &= ~((0xFUL << (2 * 4)) | (0xFUL << (3 * 4)));
    GPIOA_AFRL  |=  ((0x7UL << (2 * 4)) | (0x7UL << (3 * 4)));

    USART2_BRR = 0x008B; // 115200 baud @ 16 MHz APB1
    USART2_CR1 = (1UL << 3) | (1UL << 2) | (1UL << 13); // TE, RE, UE
#endif
}

void uart_putc(char c) {
    while (!(USART2_SR & (1UL << 7))); // Wait until TXE (Transmit Data Register Empty)
    USART2_DR = (uint8_t)c;
}

void uart_print(const char *str) {
    while (*str) {
        uart_putc(*str++);
    }
}

bool uart_getc_nonblocking(char *out_char) {
    if (USART2_SR & (1UL << 5)) { // Check RXNE (Read Data Register Not Empty)
        *out_char = (char)(USART2_DR & 0xFF);
        return true;
    }
    return false;
}