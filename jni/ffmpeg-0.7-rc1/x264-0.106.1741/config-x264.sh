#!/bin/bash

# ensure ANDROID_NDK_HOME is set
if [ "" == "$ANDROID_NDK_HOME" ]; then
  echo "Please set ANDROID_NDK_HOME to your Android Native Development Kit path.";
  exit 1;
fi

if [ "" == "$MY_X264_INSTALL" ]; then
  echo "Please set MY_X264_INSTALL to the location where x264 should be installed.";
  exit -1;
fi

# For Android NDK r5 and 6 (at least), 4.4.3 is used
export abi=arm-linux-androideabi;
export gccvers=4.4.3
echo "NDK=$(cat $ANDROID_NDK_HOME/RELEASE.TXT); $abi-$gccvers ABI";

./configure --prefix=$MY_X264_INSTALL \
	--disable-gpac \
	--extra-cflags="$MY_CFLAGS" \
	--extra-ldflags="-L$ARM_LIBO  -nostdlib -Bdynamic  -Wl,--no-undefined -Wl,-z,noexecstack  -Wl,-z,nocopyreloc -Wl,-soname,/system/lib/libz.so -Wl,-rpath-link=$PLATFORM/usr/lib,-dynamic-linker=/system/bin/linker -L$ARM_LIB -nostdlib $ARM_LIB/crtbegin_dynamic.o $ARM_LIB/crtend_android.o -lc -lm -ldl -lgcc " \
	--disable-asm \
	--cross-prefix=$ARM_TOOL/bin/$abi- \
	--disable-pthread \
	--host=arm-linux \

make install
