set(CMAKE_C_FLAGS_INIT "-O2 -g -ffreestanding -Wall -Wextra -Werror -nostdlib -lgcc")
set(CMAKE_CXX_FLAGS_INIT "-O2 -g -ffreestanding -Wall -Wextra -Werror -nostdlib -lgcc")
set(CMAKE_ASM-ATT_FLAGS_INIT "-g -Wall -Wextra -Werror -nostdlib -lgcc -c")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)