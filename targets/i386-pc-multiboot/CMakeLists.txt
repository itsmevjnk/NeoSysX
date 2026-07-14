cmake_minimum_required(VERSION 3.15)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/neosysx-grub2.iso"

    COMMAND ${CMAKE_COMMAND} -E make_directory "grub2-iso/boot/grub"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:i386-pc-multiboot_kernel> "grub2-iso/boot/kernel.elf"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_LIST_DIR}/grub.cfg" "grub2-iso/boot/grub/grub.cfg"
    COMMAND grub-mkrescue -o "${CMAKE_CURRENT_BINARY_DIR}/neosysx-grub2.iso" "grub2-iso"

    DEPENDS i386-pc-multiboot_kernel "${CMAKE_CURRENT_LIST_DIR}/grub.cfg"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building GRUB2 ISO image"
    VERBATIM
)
add_custom_target(
    i386-pc-multiboot_grub2_iso ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/neosysx-grub2.iso"
)
add_custom_target(
    i386-pc-multiboot_grub2_iso_qemu ALL
    COMMAND qemu-system-i386 -cdrom "${CMAKE_CURRENT_BINARY_DIR}/neosysx-grub2.iso"
    COMMENT "Running QEMU emulation"
    DEPENDS i386-pc-multiboot_grub2_iso
)

add_custom_command(
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/neosysx-grub.iso"

    COMMAND ${CMAKE_COMMAND} -E make_directory "grub-iso/boot/grub"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:i386-pc-multiboot_kernel> "grub-iso/kernel.elf"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_LIST_DIR}/menu.lst" "grub-iso/boot/grub/menu.lst"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_LIST_DIR}/stage2_eltorito" "grub-iso/boot/grub/stage2_eltorito"
    COMMAND genisoimage -R -b "boot/grub/stage2_eltorito" -no-emul-boot -boot-load-size 4 -boot-info-table -o "${CMAKE_CURRENT_BINARY_DIR}/neosysx-grub.iso" "grub-iso"

    DEPENDS i386-pc-multiboot_kernel "${CMAKE_CURRENT_LIST_DIR}/menu.lst" "${CMAKE_CURRENT_LIST_DIR}/stage2_eltorito"
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Building GRUB Legacy ISO image"
    VERBATIM
)
add_custom_target(
    i386-pc-multiboot_grub_iso ALL
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/neosysx-grub.iso"
)
add_custom_target(
    i386-pc-multiboot_grub_iso_qemu ALL
    COMMAND qemu-system-i386 -cdrom "${CMAKE_CURRENT_BINARY_DIR}/neosysx-grub.iso"
    COMMENT "Running QEMU emulation"
    DEPENDS i386-pc-multiboot_grub_iso
)