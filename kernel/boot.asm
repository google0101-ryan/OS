FLAGS       equ  0x00000003
MAGIC       equ  0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
; Our initial stack
section .initial_stack, nobits
align 4
stack_bottom:
    ; 1mb of uninitialized data(1024*1024=104856)
    resb 104856
stack_top:

; Kernel entry
section .text
global _start
extern kmain
_start:
    cli
    mov esp, stack_top

    extern mbi
    mov ebx, mbi
    call kmain

    jmp $