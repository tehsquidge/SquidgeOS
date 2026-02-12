#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <syscon/syscon.h>
#include <drivers/uart.h>
#include <lib/string.h>

void uart_init()
{
	kprint("UART Drive Init...");
	volatile uint8_t *UART_IER = (uint8_t *)(UART_ADDRESS + 1);
	*UART_IER = 0x01; // Enable "Received Data Available" interrupt
	kputs("OK");
}

void uart_put(size_t base_addr, uint8_t data)
{
	*(volatile uint8_t *)base_addr = data;
}

char uart_getc()
{
	if (*UART_LSR & 0x01)
	{
		return (char)(*UART_RBR);
	}
	return '\0';
}

void uart_handle_interrupt()
{
	char c = uart_getc();
	switch (c)
	{
	case '\r': // enter key
		knewline();
		break;
	case '\0':
		// do nowt
		break;
	default:
		kputchar(c);
		break;
	}
}

int kputchar(int ch)
{
	uart_put(UART_ADDRESS, ch);
	return ch;
}

void kprint(const char *str)
{
	while (*str)
	{
		kputchar(*str++);
	}
}

void kprint_int(int num)
{
	char buffer[21];
	memset(buffer, 0, sizeof(buffer));
	int i = 0;
	if (num == 0)
	{
		kputchar('0');
		return;
	}
	if (num < 0)
	{
		kputchar('-');
		num = -num;
	}
	while (num > 0)
	{
		buffer[i++] = '0' + (num % 10);
		num /= 10;
	}
	while (i > 0)
	{
		kputchar(buffer[--i]);
	}
}

void kprint_float(float num)
{
	if (num < 0)
	{
		kputchar('-');
		num = -num;
	}
	int int_part = (int)num;
	float frac_part = num - int_part;
	kprint_int(int_part);
	kputchar('.');
	for (int i = 0; i < 6; i++)
	{
		frac_part *= 10.0f;
	}
	int fraction = (int)(frac_part + 0.5f);
	kprint_int(fraction);
}

void kprint_hex(uint64_t val)
{
	char *digits = "0123456789ABCDEF";
	char buffer[17]; //+1 for null terminator
	memset(buffer, 0, sizeof(buffer));
	buffer[16] = '\0';

	for (int i = 15; i >= 0; i--)
	{
		buffer[i] = digits[val & 0xF];
		val >>= 4;
	}
	kprint("0x");
	kprint(buffer);
}

void knewline(void)
{
	kputchar('\n');
}

void kputs(const char *str)
{
	kprint(str);
	knewline();
}

void kprintf_internal(const char *format, va_list args)
{
	for (const char *p = format; *p != '\0'; p++)
	{
		if (*p == '%')
		{
			p++;
			if(*p == '\0') break;
			switch (*p)
			{
			case 'c':
			{
				char c = (char)va_arg(args, int);
				kputchar(c);
				break;
			}
			case 's':
			{
				char *s = va_arg(args, char *);
				if (!s) s = "(null)";
				kprint(s);
				break;
			}
			case 'd':
			{
				int d = va_arg(args, int);
				kprint_int(d);
				break;
			}
	        case 'u':
			{ 
				unsigned int u = va_arg(args, unsigned int);
				kprint_int((int)u);
				break;
        	}
			case 'f':
			{
				double f = va_arg(args, double);
				kprint_float((float)f);
				break;
			}
			case 'x': // Hex
			case 'p': // Pointer
			{
				#if UINTPTR_MAX == 0xffffffffULL
					unsigned int x = va_arg(args, unsigned int);
				#else
					uint64_t x = va_arg(args, uint64_t);
				#endif
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
		}
		else
		{
			kputchar(*p);
		}
	}
	knewline();
}

void kprintf(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	kprintf_internal(format, args);
	va_end(args);
}