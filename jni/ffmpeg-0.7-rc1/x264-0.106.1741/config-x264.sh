#!/bin/bash

# ensure ANDROID_NDK_HOME is set
if [ "" == "$ANDROID_NDK_HOME" ]; then
  echo "Please set ANDROID_NDK_HOME to your Android Native Development Kit path.";
  exit 1;
fi

# For Android NDK r5 and 6 (at least), 4.4.3 is used
export abi=arm-linux-androideabi;
export gccvers=4.4.3
echo "NDK=$(cat $ANDROID_NDK_HOME/RELEASE.TXT); $abi-$gccvers ABI";

PLATFORM=$ANDROID_NDK_HOME/platforms/android-8/arch-arm
ARM_INC=$PLATFORM/usr/include
ARM_LIB=$PLATFORM/usr/lib
ARM_TOOL=$ANDROID_NDK_HOME/toolchains/$abi-$gccvers/prebuilt/linux-x86
ARM_LIBO=$ARM_TOOL/lib/gcc/$abi/$gccvers

INSTALL_DIR=$PWD/../x264_${TARGET_ARCH_ABI}_install


./configure --prefix=$INSTALL_DIR \
	--disable-gpac \
	--extra-cflags=" -I$ARM_INC -fPIC -DANDROID -fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  -Wno-psabi -march=${MARCH} -mtune=xscale -msoft-float -mthumb -Os -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -DANDROID  -Wa,--noexecstack -MMD -MP " \
	--extra-ldflags="-L$ARM_LIBO  -nostdlib -Bdynamic  -Wl,--no-undefined -Wl,-z,noexecstack  -Wl,-z,nocopyreloc -Wl,-soname,/system/lib/libz.so -Wl,-rpath-link=$PLATFORM/usr/lib,-dynamic-linker=/system/bin/linker -L$ARM_LIB -nostdlib $ARM_LIB/crtbegin_dynamic.o $ARM_LIB/crtend_android.o -lc -lm -ldl -lgcc " \
	--disable-asm \
	--cross-prefix=$ARM_TOOL/bin/$abi- \
	--disable-pthread \
	--host=arm-linux || exit -1

make install

exit 0
