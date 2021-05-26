#!/bin/sh

cd kernel
nasm -felf boot.asm
nasm -felf x86/gdthelper.asm
cp x86/gdthelper.o ./
gcc -m32 -fno-pie -fno-pic -mno-red-zone kernel.c drivers/*c util/*.c libk/*.c x86/*.c mem/*.c -c -fno-builtin -DPRINTF_DISABLE_SUPPORT_LONG_LONG
ld -T link.ld *.o -o kernel.elf -melf_i386
cp kernel.elf ../iso/boot
cd ../
grub-mkrescue iso -o rynix.iso