/*
 * MyLinux ARM64 low-level entry helper
 * Architecture: AArch64
 *
 * This is a callable routine, not a kernel boot entry point.
 */

.section .text.arch_init, "ax"
.align  2

.global mylinux_arch_init
.type   mylinux_arch_init, %function

mylinux_arch_init:
    /*
     * AArch64 function entry.
     * Preserve the link register and return to the caller.
     */
    ret

.size mylinux_arch_init, . - mylinux_arch_init

.section .note.GNU-stack, "", %progbits
