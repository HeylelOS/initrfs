cmake_minimum_required(VERSION 3.14)

project(initrfs LANGUAGES C)

#########
# Build #
#########

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

include_directories(include)

file(GLOB_RECURSE MKINITRFS_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/mkinitrfs/*.c)
add_executable(mkinitrfs ${MKINITRFS_SOURCES})

file(GLOB_RECURSE INIT_SOURCES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/init/*.c)
add_executable(init ${INIT_SOURCES})

target_link_libraries(init -static)

########
# Test #
########

enable_testing()
add_subdirectory(test)

