#!/bin/bash

ARM_ROOT=/home/paris/android-ndk-r5b

PLATFORM=$ARM_ROOT/platforms/android-8/arch-arm
ARM_INC=$PLATFORM/usr/include
ARM_LIB=$PLATFORM/usr/lib
ARM_TOOL=$ARM_ROOT/toolchains/arm-eabi-4.4.0/prebuilt/linux-x86
ARM_LIBO=$ARM_TOOL/lib/gcc/arm-eabi/4.4.0

X264_INSTALL_DIR=$PWD/x264install
X264_LIB_INC=$X264_INSTALL_DIR/include
X264_LIB_LIB=$X264_INSTALL_DIR/lib

AMR_INSTALL_DIR=$PWD/opencore-amr_install
AMR_LIB_INC=$AMR_INSTALL_DIR/include
AMR_LIB_LIB=$AMR_INSTALL_DIR/lib


./configure --target-os=linux \
	--arch=arm \
	--enable-cross-compile \
	--cc=$ARM_TOOL/bin/arm-eabi-gcc \
	--cross-prefix=$ARM_TOOL/bin/arm-eabi- \
	--nm=$ARM_TOOL/bin/arm-eabi-nm \
	--enable-static \
	--disable-shared \
	--enable-armv5te --enable-armv6 --enable-armv6t2 --enable-armvfp \
	--disable-asm --disable-yasm --enable-neon --enable-pic \
	--disable-amd3dnow --disable-amd3dnowext --disable-mmx --disable-mmx2 --disable-sse --disable-ssse3 \
	--enable-version3 \
	--enable-gpl \
	--disable-nonfree \
	--disable-stripping \
	--disable-ffplay \
	--disable-ffmpeg \
	--disable-ffprobe \
	--disable-ffserver \
	--disable-avdevice \
	--disable-avfilter \
	--disable-devices \
	--enable-libx264 \
	--enable-libopencore-amrnb \
	--extra-cflags="-fPIC -DANDROID " \
	--extra-cflags="-I$ARM_INC -I$X264_LIB_INC -I$AMR_LIB_INC " \
	--extra-cflags="-fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  -Wno-psabi -march=armv5te -mtune=xscale -msoft-float -mthumb -Os -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -DANDROID  -Wa,--noexecstack -MMD -MP " \
	--extra-ldflags=" -Wl,-T,$ARM_TOOL/arm-eabi/lib/ldscripts/armelf.x -Wl,-rpath-link=$ARM_LIB -L$ARM_LIB -nostdlib $ARM_TOOL/lib/gcc/arm-eabi/4.4.0/crtbegin.o $ARM_LIBO/crtend.o -lc -lm -ldl " \
	--extra-ldflags="-L$ARM_LIBO -L$X264_LIB_LIB -L$AMR_LIB_LIB -Wl,-rpath-link=$X264_LIB_LIB -nostdlib -Bdynamic  -Wl,--no-undefined -Wl,-z,noexecstack  -Wl,-z,nocopyreloc -Wl,-soname,/system/lib/libz.so -Wl,-rpath-link=$ARM_LIB,-dynamic-linker=/system/bin/linker -L$ARM_LIB -nostdlib $ARM_LIB/crtbegin_dynamic.o $ARM_LIB/crtend_android.o " \
	--extra-libs="-lx264 -lgcc -lopencore-amrnb "
	
