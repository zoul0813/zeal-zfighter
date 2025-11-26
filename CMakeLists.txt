cmake_minimum_required(VERSION 3.16)

# -------------------------------
# Toolchain
# -------------------------------
set(ZOS_TOOLCHAIN sdcc)
include($ENV{ZOS_PATH}/cmake/zos_init.cmake)

# -------------------------------
# Project
# -------------------------------
project(zfighter C ASM)

# -------------------------------
# Executable
# -------------------------------
add_executable(zfighter
    src/main.c
    src/assets.c
    src/attract.c
    src/belt.c
    src/bullet.c
    src/enemy.c
    src/hiscore.c
    src/patterns.c
    src/player.c
)

# # -------------------------------
# # Include and link SDKs
# # -------------------------------
zos_use(zgdk)
zos_use(zvb)

find_package(ZVB_SDK REQUIRED)
target_link_libraries(zfighter PUBLIC zvb_gfx zvb_sound)

find_package(ZGDK REQUIRED)
target_link_libraries(zfighter PUBLIC zgdk)

# -------------------------------
# Assets
# -------------------------------

gif2zeal(zfighter FILES
    "assets/tileset.gif"
    STRIP 160
    COMPRESSED)

add_asset(zfighter FILES
    "assets/shooter.zmt")


# -------------------------------
# Convert to binary
# -------------------------------
zos_add_outputs(zfighter)
