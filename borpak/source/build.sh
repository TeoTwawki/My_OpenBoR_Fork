#!/bin/sh
# Compiles borpak for the Linux platform. Also works with Cygwin on Windows, 
# although I don't know why you'd be using Cygwin instead of MinGW.

PLATFORM=$1

if [ `echo $PLATFORM | grep -o "win"` ]; then
  EXTENSION=".exe"
  SOURCE="scandir.c "
fi

if [ `echo $PLATFORM | grep -o "macwin"` ]; then
  CC="i386-mingw32-"
  PATH="$PATH:/usr/local/i386-mingw32-4.3.0/bin"
elif [ `echo $PLATFORM | grep -o "mac"` ]; then
  CFLAGS+="-arch x86_64 -arch i386 -arch ppc"
fi

if [ `echo $PLATFORM | grep -o "linwin"` ]; then
  CC="i586-mingw32msvc-"
fi

TARGET="borpak"
CC=${CC}"gcc"
SOURCE="$SOURCE borpak.c stristr.c"

$CC $CFLAGS -o $TARGET$EXTENSION $SOURCE

