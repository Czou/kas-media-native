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

INSTALL_DIR=$PWD/../opencore-amr_install

export LD="$ARM_TOOL/bin/$abi-ld"
export CC="$ARM_TOOL/bin/$abi-gcc"
export CXX="$ARM_TOOL/bin/$abi-g++"
export   CFLAGS="-I$ARM_INC -DANDROID -fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -D__ARM_ARCH_7A__   -Wno-psabi -march=armv7-a -msoft-float -mthumb -Os -O -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -Wa,--noexecstack -MMD -MP"
export CXXFLAGS="-I$ARM_INC -DANDROID -fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -D__ARM_ARCH_7A__   -Wno-psabi -march=armv7-a -msoft-float -mthumb -Os -O -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -Wa,--noexecstack -MMD -MP" 
export LDFLAGS="-I$ARM_INC -L$ARM_LIBO -nostdlib -Bdynamic  -Wl,--no-undefined -Wl,-z,noexecstack  -Wl,-z,nocopyreloc -Wl,-soname,/system/lib/libz.so -Wl,-rpath-link=$PLATFORM/usr/lib,-dynamic-linker=/system/bin/linker -L$ARM_LIB  -lc -lm -ldl -lsupc++ -Wl,--library-path=$PLATFORM/usr/lib/ -Xlinker $PLATFORM/usr/lib/crtbegin_dynamic.o -Xlinker $PLATFORM/usr/lib/crtend_android.o "

./configure --prefix=$INSTALL_DIR --host=arm

