# XNU-syscall-hiding-PoC
This proof-of-concept demonstrates how to hide syscalls (system calls) on arm64
XNU (macOS on Apple Silicon Macs and iOS), while also comparing it to a normal
usage of system calls.

This syscall trick works by utilizing an arm64 XNU kernel function, such as
`__mmap`, to jump to its syscall instruction (`SVC 0x80`). As a result, static
analysis of a program using this trick will not show any usage of syscalls;
`__mmap` was chosen for its non-exotic function name that would attract less
attention compared to some of the other functions I tested.

As a happy side effect of my hastily written assembly code, if either syscall
function is decompiled in IDA (7.7, I tested) at least once, any functions
calling it will produce a decompilation error! 🙂

## License
[MIT License](./LICENSE)
