set(CMAKE_C_FLAGS_INIT "-O2 -g -ffreestanding -Wall -Wextra -Werror -nostdlib")
set(CMAKE_CXX_FLAGS_INIT "-O2 -g -ffreestanding -Wall -Wextra -Werror -nostdlib")
set(CMAKE_ASM-ATT_FLAGS_INIT "-g -Wall -Wextra -Werror -nostdlib -c")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)