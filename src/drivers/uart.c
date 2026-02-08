#include <stdint.h>
#include <stddef.h>
#include <drivers/uart.h>

void uart_put(size_t base_addr, uint8_t data) {
	*(volatile uint8_t *)base_addr = data;
}

int kputchar(int ch) {
	uart_put(UART_ADDRESS, ch);
	return ch;
}

void kprint(const char *str) {
	while (*str) {
		kputchar(*str++);
	}
}

void knewline(void) {
	kputchar('\n');
}

void kputs(const char *str) {
	kprint(str);
	knewline();
}