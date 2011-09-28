#!/bin/bash

# ensure ANDROID_NDK_HOME is set
if [ "" == "$ANDROID_NDK_HOME" ]; then
  echo "Please set ANDROID_NDK_HOME to your Android Native Development Kit path.";
  exit 1;
fi

# For Android NDK r5 and 6 (at least), 4.4.3 is used
export armelf=armelf_linux_eabi.x;
export abi=arm-linux-androideabi;
export gccvers=4.4.3
echo "NDK=$(cat $ANDROID_NDK_HOME/RELEASE.TXT); $abi-$gccvers ABI";

PLATFORM=$ANDROID_NDK_HOME/platforms/android-8/arch-arm
ARM_INC=$PLATFORM/usr/include
ARM_LIB=$PLATFORM/usr/lib
ARM_TOOL=$ANDROID_NDK_HOME/toolchains/$abi-$gccvers/prebuilt/linux-x86
ARM_LIBO=$ARM_TOOL/lib/gcc/$abi/$gccvers

#export USE_X264_TREE=x264-0.106.1741
if [ "" == "$USE_X264_TREE" ]; then
  echo "sin x264=$USE_X264_TREE"
  export X264_LIB_INC=;
  export X264_LIB_LIB=;
  export X264_C_EXTRA=;
  export X264_LD_EXTRA=;
  export X264_L=;
  export X264_CONFIGURE_OPTS='--disable-gpl --disable-libx264';
else
  echo "con x264=$USE_X264_TREE"
  export X264_SRC=$USE_X264_TREE;
  export X264_INSTALL_DIR=$PWD/x264install;
  export X264_LIB_INC=$X264_INSTALL_DIR/include;
  export X264_LIB_LIB=$X264_INSTALL_DIR/lib;
  export X264_C_EXTRA="-I$X264_LIB_INC";
  export X264_LD_EXTRA="-L$X264_LIB_LIB -rpath-link=$X264_LIB_LIB";
  export X264_L=-lx264;
  cd $X264_SRC;
  echo "configure x264";
  ./config-x264.sh || exit -1;
  cd ..;
  export X264_CONFIGURE_OPTS='--enable-gpl --enable-libx264';
fi

AMR_SRC=opencore-amr-0.1.2
AMR_INSTALL_DIR=$PWD/opencore-amr_install
AMR_LIB_INC=$AMR_INSTALL_DIR/include
AMR_LIB_LIB=$AMR_INSTALL_DIR/lib
cd $AMR_SRC
echo "configure opencore-amr"
./config-amr.sh || exit -1
cd ..

./configure --target-os=linux \
	--arch=arm \
	--enable-cross-compile \
	--cc=$ARM_TOOL/bin/$abi-gcc \
	--cross-prefix=$ARM_TOOL/bin/$abi- \
	--nm=$ARM_TOOL/bin/$abi-nm \
	--enable-static \
	--disable-shared \
	--enable-armv5te --enable-armv6 --enable-armv6t2 --enable-armvfp \
	--disable-asm --disable-yasm --enable-neon --enable-pic \
	--disable-amd3dnow --disable-amd3dnowext --disable-mmx --disable-mmx2 --disable-sse --disable-ssse3 \
	--enable-version3 \
	--disable-nonfree \
	--disable-stripping \
	--disable-ffplay \
	--disable-ffmpeg \
	--disable-ffprobe \
	--disable-ffserver \
	--disable-avdevice \
	--disable-avfilter \
	--disable-devices \
	--extra-cflags="-fPIC -DANDROID " \
	--extra-cflags="-I$ARM_INC -I$AMR_LIB_INC $X264_C_EXTRA " \
	--extra-cflags="-fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  -Wno-psabi -march=armv5te -mtune=xscale -msoft-float -mthumb -Os -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -DANDROID  -Wa,--noexecstack -MMD -MP " \
	--extra-ldflags=" -Wl,-T,$ARM_TOOL/$abi/lib/ldscripts/$armelf -Wl,-rpath-link=$ARM_LIB -L$ARM_LIB -nostdlib $ARM_TOOL/lib/gcc/$abi/$gccvers/crtbegin.o $ARM_LIBO/crtend.o -lc -lm -ldl " \
	--extra-ldflags="-L$ARM_LIBO $X264_LD_EXTRA -L$AMR_LIB_LIB -Wl,-nostdlib -Bdynamic  -Wl,--no-undefined -Wl,-z,noexecstack  -Wl,-z,nocopyreloc -Wl,-soname,/system/lib/libz.so -Wl,-rpath-link=$ARM_LIB,-dynamic-linker=/system/bin/linker -L$ARM_LIB -nostdlib $ARM_LIB/crtbegin_dynamic.o $ARM_LIB/crtend_android.o " \
	--extra-libs="$X264_L -lgcc -lopencore-amrnb " $X264_CONFIGURE_OPTS --enable-libopencore-amrnb \


