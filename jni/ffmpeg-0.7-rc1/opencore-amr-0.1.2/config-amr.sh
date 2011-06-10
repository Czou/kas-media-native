#!/bin/bash

ARM_ROOT=/home/paris/android-ndk-r5b

PLATFORM=$ARM_ROOT/platforms/android-8/arch-arm
ARM_INC=$PLATFORM/usr/include
ARM_LIB=$PLATFORM/usr/lib
ARM_TOOL=$ARM_ROOT/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86
ARM_LIBO=$ARM_TOOL/lib/gcc/arm-linux-androideabi/4.4.3

INSTALL_DIR=$PWD/../opencore-amr_install

export LD="$ARM_TOOL/bin/arm-linux-androideabi-ld"
export CC="$ARM_TOOL/bin/arm-linux-androideabi-gcc"
export CXX="$ARM_TOOL/bin/arm-linux-androideabi-g++"
export   CFLAGS="-I$ARM_INC -DANDROID -fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -D__ARM_ARCH_7A__   -Wno-psabi -march=armv7-a -msoft-float -mthumb -Os -O -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -Wa,--noexecstack -MMD -MP"
export CXXFLAGS="-I$ARM_INC -DANDROID -fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -D__ARM_ARCH_7A__   -Wno-psabi -march=armv7-a -msoft-float -mthumb -Os -O -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -Wa,--noexecstack -MMD -MP" 
export LDFLAGS="-I$ARM_INC -L$ARM_LIBO -nostdlib -Bdynamic  -Wl,--no-undefined -Wl,-z,noexecstack  -Wl,-z,nocopyreloc -Wl,-soname,/system/lib/libz.so -Wl,-rpath-link=$PLATFORM/usr/lib,-dynamic-linker=/system/bin/linker -L$ARM_LIB  -lc -lm -ldl -lsupc++ -Wl,--library-path=$PLATFORM/usr/lib/ -Xlinker $PLATFORM/usr/lib/crtbegin_dynamic.o -Xlinker $PLATFORM/usr/lib/crtend_android.o "

./configure --prefix=$INSTALL_DIR --host=arm

