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

./configure --prefix=$MY_X264_INSTALL \
	--disable-gpac \
	--extra-cflags="$MY_CFLAGS" \
	--extra-ldflags="$MY_LDFLAGS -lgcc" \
	--disable-asm \
	--cross-prefix=$ARM_TOOL/bin/$abi- \
	--disable-pthread \
	--host=arm-linux \

make install
