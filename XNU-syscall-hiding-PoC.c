/*
 * Copyright 2022, Nick Botticelli. <nick.s.botticelli@gmail.com>
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

#if (!defined(__arm64__))
    #error "Must be arm64 XNU!"
#endif

void *__mmap(void *addr, size_t len, int prot, int flags, int fildes, off_t off);

extern void *(*__mmap_ptr)(void *addr, size_t len, int prot, int flags, int fildes, off_t off);

extern int errno_orig;

// Do not use more than 4 additional arguments or bad things might happen
int syscall_orig(int number, ...);
__asm__(".balign 8\n"
        ".text\n"
        ".globl syscall_orig\n"
        "_syscall_orig:\n"
        "mov     x16, x0\n"
        "ldp     x0, x1, [sp, #0x00]\n"
        "ldp     x2, x3, [sp, #0x10]\n"
        "ldp     x4, x5, [sp, #0x20]\n"
        "ldp     x6, x7, [sp, #0x30]\n"
        "svc     #0x80\n"
        "b.lo    #0x14\n" // ret
        "adrp    x8, #_errno_orig@PAGE\n"
        "str     w0, [x8, #_errno_orig@PAGEOFF]\n"
        "movn    x0, #0\n"
        "movn    x1, #0\n"
        "ret\n"
        ".data\n"
        "_errno_orig: .zero 4");

// Do not use more than 4 additional arguments or bad things might happen
int syscall_hidden(int number, ...);
__asm__(".balign 8\n"
        ".text\n"
        ".globl syscall_hidden\n"
        "_syscall_hidden:\n"
        "mov     x16, x0\n"
        "ldp     x0, x1, [sp, #0x00]\n"
        "ldp     x2, x3, [sp, #0x10]\n"
        "ldp     x4, x5, [sp, #0x20]\n"
        "ldp     x6, x7, [sp, #0x30]\n"
        "adrp    x8, #___mmap_ptr@PAGE\n"
        "ldr     x8, [x8, #___mmap_ptr@PAGEOFF]\n"
        "br      x8\n"
        // Could probably sneak in some errno support with bl vs. br, oh well
        ".data\n"
        "___mmap_ptr: .zero 8");

int main(int argc, char *argv[])
{
    int result;
    
    puts("=== XNU-syscall-hiding-PoC ===\n");
    
    // Get pointer to address of syscall instruction inside of __mmap
    __mmap_ptr = (__mmap + (intptr_t) 4);
    
    // Test normal syscall
    result = syscall_orig(SYS_open, "/dev/console", O_WRONLY);
    printf("syscall result: %d\n", result);
    printf("errno = %d\n\n", errno_orig);
    
    // Test normal syscall (produce error)
    result = syscall_orig(SYS_open, "/dev/console_", O_WRONLY);
    printf("syscall result (error): %d\n", result);
    printf("errno = %d\n\n", errno_orig);
    
    // Test syscall hiding
    result = syscall_hidden(SYS_open, "/dev/console", O_WRONLY);
    printf("syscall hiding result: %d\n\n", result);
    
    // Tests syscall hiding (produce error)
    result = syscall_hidden(SYS_open, "/dev/console_", O_WRONLY);
    printf("syscall hiding result (error): %d\n", result);
    
    return EXIT_SUCCESS;
}
