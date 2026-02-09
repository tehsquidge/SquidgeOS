#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
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

void kprint_int(int num) {
	char buffer[21];
	int i = 0;
	if (num == 0) {
		kputchar('0');
		return;
	}
	if (num < 0) {
		kputchar('-');
		num = -num;
	}
	while (num > 0) {
		buffer[i++] = '0' + (num % 10);
		num /= 10;
	}
	while (i > 0) {
		kputchar(buffer[--i]);
	}
}

void kprint_hex(uint64_t val) {
    char *digits = "0123456789ABCDEF";
    char buffer[16]; // 64-bit hex is 16 chars
    
    // We process from right to left
    for (int i = 15; i >= 0; i--) {
        buffer[i] = digits[val & 0xF];
        val >>= 4;
    }

	kprint(buffer);    

}

void knewline(void) {
	kputchar('\n');
}

void kputs(const char *str) {
	kprint(str);
	knewline();
}

void kprintf(const char *format, ...) {
	va_list args;
	va_start(args, format);

	for(char *p = format; *p != '\0'; p++) {
		if (*p == '%') {
			p++;
			switch (*p) {
				case 's': {
					char *s = va_arg(args, char *);
					kprint(s);
					break;
				}
				case 'd': {
					int d = va_arg(args, int);
					kprint_int(d);
					break;
				}
				case 'x':   // Hex
				case 'p': { // Pointer
					uint64_t x = va_arg(args, uint64_t);
					kputchar('0'); kputchar('x');
					kprint_hex(x);
					break;
				}
				case '%':
					kputchar('%');
					break;
				default:
					kputchar('%');
					kputchar(*p);
			}
		} else {
			kputchar(*p);
		}
	}

}