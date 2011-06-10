#!/bin/bash

ARM_ROOT=/home/paris/android-ndk-r5b

PLATFORM=$ARM_ROOT/platforms/android-8/arch-arm
ARM_INC=$PLATFORM/usr/include
ARM_LIB=$PLATFORM/usr/lib
ARM_TOOL=$ARM_ROOT/toolchains/arm-eabi-4.4.0/prebuilt/linux-x86
ARM_LIBO=$ARM_TOOL/lib/gcc/arm-eabi/4.4.0

INSTALL_DIR=$PWD/../x264install


./configure --prefix=$INSTALL_DIR \
	--disable-gpac \
	--extra-cflags=" -I$ARM_INC -fPIC -DANDROID -fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  -Wno-psabi -march=armv5te -mtune=xscale -msoft-float -mthumb -Os -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -DANDROID  -Wa,--noexecstack -MMD -MP " \
	--extra-ldflags="-L$ARM_LIBO  -nostdlib -Bdynamic  -Wl,--no-undefined -Wl,-z,noexecstack  -Wl,-z,nocopyreloc -Wl,-soname,/system/lib/libz.so -Wl,-rpath-link=$PLATFORM/usr/lib,-dynamic-linker=/system/bin/linker -L$ARM_LIB -nostdlib $ARM_LIB/crtbegin_dynamic.o $ARM_LIB/crtend_android.o -lc -lm -ldl -lgcc " \
	--cross-prefix=$ARM_TOOL/bin/arm-eabi- \
	--disable-asm \
	--disable-pthread \
	--host=arm-linux \

