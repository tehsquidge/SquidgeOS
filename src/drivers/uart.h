#ifndef UART_H
#define UART_H

#define UART_ADDRESS 0x10000000
void uart_put(size_t base_addr, uint8_t data);
int kputchar(int ch);
void knewline();
void kputs(const char *str);
void kprint(const char *str);
#endif