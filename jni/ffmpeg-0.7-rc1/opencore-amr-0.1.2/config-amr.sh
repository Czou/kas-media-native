#!/bin/bash

# ensure ANDROID_NDK_HOME is set
if [ "" == "$ANDROID_NDK_HOME" ]; then
  echo "Please set ANDROID_NDK_HOME to your Android Native Development Kit path.";
  exit 1;
fi

if [ "" == "$MY_AMR_INSTALL" ]; then
  echo "Please set MY_AMR_INSTALL to the location where amr should be installed.";
  exit -1;
fi

# For Android NDK r5 and 6 (at least), 4.4.3 is used
export abi=arm-linux-androideabi;
export gccvers=4.4.3
echo "NDK=$(cat $ANDROID_NDK_HOME/RELEASE.TXT); $abi-$gccvers ABI";

export LD="$ARM_TOOL/bin/$abi-ld"
export CC="$ARM_TOOL/bin/$abi-gcc"
export CXX="$ARM_TOOL/bin/$abi-g++"
export   CFLAGS="$MY_CFLAGS"
export CXXFLAGS="$MY_CFLAGS" 
export LDFLAGS="-I$ARM_INC -L$ARM_LIBO -nostdlib -Bdynamic  -Wl,--no-undefined -Wl,-z,noexecstack  -Wl,-z,nocopyreloc -Wl,-soname,/system/lib/libz.so -Wl,-rpath-link=$PLATFORM/usr/lib,-dynamic-linker=/system/bin/linker -L$ARM_LIB  -lc -lm -ldl -lsupc++ -Wl,--library-path=$PLATFORM/usr/lib/ -Xlinker $PLATFORM/usr/lib/crtbegin_dynamic.o -Xlinker $PLATFORM/usr/lib/crtend_android.o "

./configure --prefix=$MY_AMR_INSTALL --host=arm

make install
