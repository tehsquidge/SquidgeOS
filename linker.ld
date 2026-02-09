ENTRY(_start)

SECTIONS
{
    . = 0x80000000;

    .text : ALIGN(4K) {
        /* This matches the .section .text.boot in your assembly */
        *(.text.boot) 
        *(.text .text.*)
    }

    .rodata : ALIGN(4K) {
        *(.rodata .rodata.*)
    }

    .data : ALIGN(4K) {
        *(.data .data.*)
    }

    .bss : ALIGN(4K) {
        PROVIDE(_bss_start = .);
        *(.bss .bss.*)
        *(COMMON)
        PROVIDE(_bss_end = .);
    }

    /* Keep the stack at the very end to prevent it from overwriting code if it overflows */
    . = ALIGN(16);
    PROVIDE(_stack_bottom = .);
    . += 4096; 
    PROVIDE(stack_top = .);

   . = ALIGN(4K);
   _heap_start =.;
}
