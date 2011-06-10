prefix=/home/paris/eclipse_workspaces/workspace-2.0/tk-android-media/jni/ffmpeg-0.7-rc1/x264-0.106.1741/../x264install
exec_prefix=${prefix}
bindir=${exec_prefix}/bin
libdir=${exec_prefix}/lib
includedir=${prefix}/include
ARCH=ARM
SYS=LINUX
CC=/home/paris/android-ndk-r5b/toolchains/arm-eabi-4.4.0/prebuilt/linux-x86/bin/arm-eabi-gcc
CFLAGS=-Wshadow -O3 -fno-fast-math  -Wall -I.  -I/home/paris/android-ndk-r5b/platforms/android-8/arch-arm/usr/include -fPIC -DANDROID -fpic -mthumb-interwork -ffunction-sections -funwind-tables -fstack-protector -fno-short-enums -D__ARM_ARCH_5__ -D__ARM_ARCH_5T__ -D__ARM_ARCH_5E__ -D__ARM_ARCH_5TE__  -Wno-psabi -march=armv5te -mtune=xscale -msoft-float -mthumb -Os -fomit-frame-pointer -fno-strict-aliasing -finline-limit=64 -DANDROID  -Wa,--noexecstack -MMD -MP  -std=gnu99 -s -fomit-frame-pointer -fno-tree-vectorize
LDFLAGS= -L/home/paris/android-ndk-r5b/toolchains/arm-eabi-4.4.0/prebuilt/linux-x86/lib/gcc/arm-eabi/4.4.0  -nostdlib -Bdynamic  -Wl,--no-undefined -Wl,-z,noexecstack  -Wl,-z,nocopyreloc -Wl,-soname,/system/lib/libz.so -Wl,-rpath-link=/home/paris/android-ndk-r5b/platforms/android-8/arch-arm/usr/lib,-dynamic-linker=/system/bin/linker -L/home/paris/android-ndk-r5b/platforms/android-8/arch-arm/usr/lib -nostdlib /home/paris/android-ndk-r5b/platforms/android-8/arch-arm/usr/lib/crtbegin_dynamic.o /home/paris/android-ndk-r5b/platforms/android-8/arch-arm/usr/lib/crtend_android.o -lc -lm -ldl -lgcc  -lm -s
LDFLAGSCLI=
AR=/home/paris/android-ndk-r5b/toolchains/arm-eabi-4.4.0/prebuilt/linux-x86/bin/arm-eabi-ar
RANLIB=/home/paris/android-ndk-r5b/toolchains/arm-eabi-4.4.0/prebuilt/linux-x86/bin/arm-eabi-ranlib
STRIP=/home/paris/android-ndk-r5b/toolchains/arm-eabi-4.4.0/prebuilt/linux-x86/bin/arm-eabi-strip
AS=
ASFLAGS=
EXE=
VIS=no
HAVE_GETOPT_LONG=1
DEVNULL=/dev/null
GPL=yes
