#ifndef UART_H
#define UART_H

#define UART_RBR (volatile uint8_t *)(UART_ADDRESS + 0)
#define UART_LSR (volatile uint8_t *)(UART_ADDRESS + 5)
#define UART_ADDRESS 0x10000000
#define UART_INTERRUPT_ENABLE_REGISTER 0x1001100C
#define UART_INTEN_OFFSET 0x0C
#define UART_TX_OFFSET 0x04
#define UART_INTERRUPT_ENABLE_REGISTER (UART_BASE_ADDR + UART_INTEN_OFFSET)

void uart_init();
void uart_put(size_t base_addr, uint8_t data);
char uart_getc();
void uart_handle_interrupt();

int kputchar(int ch);
void kprint_hex(uint64_t val);
void kprint_int(int num);
void kprintf(const char *format, ...);
void kprint_float(float num);
void knewline();
void kputs(const char *str);
void kprint(const char *str);
#endif