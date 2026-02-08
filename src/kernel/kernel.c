#include <stdint.h>
#include <stddef.h>

void kmain() {
    char *uart = (char *)0x10000000;
    char *msg = "Hello, OS World!\n";
    
    for (int i = 0; msg[i] != '\0'; i++) {
        *uart = msg[i]; // Write each character to the UART
    }
}
