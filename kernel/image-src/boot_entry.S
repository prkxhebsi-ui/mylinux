/*
 * MyLinux ARM64 boot entry symbols
 * Target: Samsung Galaxy Tab A 8.0 (SM-T295), gto
 *
 * IMPORTANT:
 * This is a source prototype, not a bootable Linux entry point.
 * Real Linux ARM64 entry code must follow the Linux boot protocol
 * and use the matching kernel's required register and CPU state.
 */

.section .text.head, "ax"
.balign 4

/*
 * Public entry symbols
 */
.global mylinux_image_entry
.global mylinux_primary_entry
.global mylinux_secondary_entry
.global mylinux_boot_protocol_entry
.global mylinux_boot_handoff
.global mylinux_image_return

/*
 * External symbols supplied by the final linker script.
 */
.extern mylinux_early_setup
.extern mylinux_primary_continue
.extern mylinux_secondary_continue

/*
 * Image entry point.
 *
 * This prototype assumes the entry is called with a valid stack.
 * A real kernel entry cannot assume that unless its boot protocol
 * explicitly guarantees it.
 */
.type mylinux_image_entry, %function
mylinux_image_entry:
    b mylinux_primary_entry
.size mylinux_image_entry, . - mylinux_image_entry

/*
 * Primary CPU entry.
 */
.type mylinux_primary_entry, %function
mylinux_primary_entry:
    /*
     * Do not enable the MMU, interrupts, or caches here.
     * Those operations require verified platform-specific setup.
     */
    bl mylinux_early_setup
    b mylinux_primary_continue
.size mylinux_primary_entry, . - mylinux_primary_entry

/*
 * Secondary CPU entry.
 *
 * The actual secondary CPU release mechanism is platform-specific.
 */
.type mylinux_secondary_entry, %function
mylinux_secondary_entry:
    b mylinux_secondary_continue
.size mylinux_secondary_entry, . - mylinux_secondary_entry

/*
 * Boot protocol entry alias.
 */
.type mylinux_boot_protocol_entry, %function
mylinux_boot_protocol_entry:
    b mylinux_image_entry
.size mylinux_boot_protocol_entry, . - mylinux_boot_protocol_entry

/*
 * Boot handoff symbol.
 */
.type mylinux_boot_handoff, %function
mylinux_boot_handoff:
    b mylinux_primary_entry
.size mylinux_boot_handoff, . - mylinux_boot_handoff

/*
 * Return trap.
 *
 * Reaching this symbol means the entry flow returned unexpectedly.
 */
.type mylinux_image_return, %function
mylinux_image_return:
    b mylinux_image_return
.size mylinux_image_return, . - mylinux_image_return

/*
 * Additional protocol labels for future implementation.
 * These are symbol markers, not independent executable handlers.
 */
.global mylinux_bp_start
.global mylinux_bp_cpu_state
.global mylinux_bp_exception_level
.global mylinux_bp_interrupt_state
.global mylinux_bp_mmu_state
.global mylinux_bp_device_tree
.global mylinux_bp_command_line
.global mylinux_bp_initramfs
.global mylinux_bp_memory_map
.global mylinux_bp_primary_cpu
.global mylinux_bp_secondary_cpu
.global mylinux_bp_handoff_complete

my_linux_bp_markers:
    b mylinux_image_return

my_linux_bp_start:
mylinux_bp_start:
    b mylinux_image_entry

mylinux_bp_cpu_state:
    b mylinux_image_return

mylinux_bp_exception_level:
    b mylinux_image_return

mylinux_bp_interrupt_state:
    b mylinux_image_return

mylinux_bp_mmu_state:
    b mylinux_image_return

mylinux_bp_device_tree:
    b mylinux_image_return

mylinux_bp_command_line:
    b mylinux_image_return

mylinux_bp_initramfs:
    b mylinux_image_return

mylinux_bp_memory_map:
    b mylinux_image_return

mylinux_bp_primary_cpu:
    b mylinux_primary_entry

mylinux_bp_secondary_cpu:
    b mylinux_secondary_entry

mylinux_bp_handoff_complete:
    b mylinux_image_return

/*
 * End marker.
 */
.global mylinux_boot_entry_end
mylinux_boot_entry_end:
    b mylinux_image_return

.section .note.GNU-stack, "", %progbits
