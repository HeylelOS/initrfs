cmake_minimum_required(VERSION 3.14)

project(initrfs LANGUAGES C ASM)

#########
# Build #
#########

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

execute_process(
	COMMAND uname -m
	OUTPUT_STRIP_TRAILING_WHITESPACE
	OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR
)

file(GLOB_RECURSE MKINITRFS_SOURCES CONFIGURE_DEPENDS src/mkinitrfs/*.c)
add_executable(mkinitrfs ${MKINITRFS_SOURCES})

file(GLOB_RECURSE LIBINITCRT_SOURCES CONFIGURE_DEPENDS src/libinitcrt/*.c src/libinitcrt/*.${CMAKE_SYSTEM_PROCESSOR}.s)
add_library(initcrt ${LIBINITCRT_SOURCES})

file(GLOB_RECURSE INIT_SOURCES CONFIGURE_DEPENDS src/init/*.c)
add_executable(init ${INIT_SOURCES})

target_compile_options(initcrt PRIVATE -nostdinc -ffreestanding)
target_include_directories(initcrt PUBLIC include)
target_link_libraries(initcrt -nostdlib -static)

target_compile_options(init PRIVATE -nostdinc -ffreestanding)
target_link_libraries(init PRIVATE initcrt -nostdlib -static)

########
# Test #
########

enable_testing()
add_subdirectory(test)

