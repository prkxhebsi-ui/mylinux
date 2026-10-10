/*
 * MyLinux ARM64 Image source
 * Target: Samsung Galaxy Tab A 8.0 (SM-T295)
 * Codename: gto
 *
 * Prototype entry helper.
 * This is not the actual Linux ARM64 boot entry.
 */

.section .text.mylinux_entry, "ax"
.align  2

.global mylinux_image_entry
.type   mylinux_image_entry, %function

mylinux_image_entry:
    /* Return to the caller. */
    ret

.size mylinux_image_entry, . - mylinux_image_entry

.section .note.GNU-stack, "", %progbits
