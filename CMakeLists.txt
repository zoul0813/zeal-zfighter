cmake_minimum_required(VERSION 3.16)

# -------------------------------
# CMake Setup
# -------------------------------
# Application Version
set(APP_VERSION_MAJOR 0)
set(APP_VERSION_MINOR 0)
set(APP_VERSION_PATCH 0)
set(APP_NAME "zfighter")
set(target_name "zfighter")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)
# -------------------------------
# Toolchain
# -------------------------------
set(ZOS_TOOLCHAIN sdcc)
include($ENV{ZOS_PATH}/cmake/zos_init.cmake)

# -------------------------------
# Project
# -------------------------------
project(${target_name} C ASM)

# -------------------------------
# Executable
# -------------------------------
add_executable(${target_name}
    src/main.c
    src/assets.c
    src/attract.c
    src/belt.c
    src/bullet.c
    src/enemy.c
    src/patterns.c
    src/player.c
)

# # -------------------------------
# # Include and link SDKs
# # -------------------------------
zos_use(coreutils)
find_package(COREUTILS REQUIRED)

zos_use(zgdk)
find_package(ZGDK REQUIRED)

zos_use(zvb)
find_package(ZVB_SDK REQUIRED)

zgdk_set_hiscore_path(${target_name})

target_link_libraries(${target_name} PUBLIC zvb_gfx zvb_sound)
target_link_libraries(${target_name} PUBLIC core)
target_link_libraries(${target_name} PUBLIC zgdk)

# -------------------------------
# Assets
# -------------------------------

gif2zeal(${target_name} FILES
    "assets/tileset.gif"
    STRIP 160
    COMPRESSED)

add_asset(${target_name} FILES
    "assets/shooter.zmt")


# -------------------------------
# Convert to binary
# -------------------------------
zgdk_add_outputs(${target_name})
