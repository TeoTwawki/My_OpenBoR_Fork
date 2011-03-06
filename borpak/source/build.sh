#!/bin/sh
# Compiles borpak for the Linux platform. Also works with Cygwin on Windows, 
# although I don't know why you'd be using Cygwin instead of MinGW.

PLATFORM=$1

if [ `echo $PLATFORM | grep -o "win"` ]; then
  EXTENSION=".exe"
  SOURCE="scandir.c "
fi

if [ `echo $PLATFORM | grep -o "mac"` ]; then
  CFLAGS+="-arch x86_64 -arch i386 -arch ppc"
fi

TARGET="borpak"
CC="gcc"
SOURCE+="borpak.c stristr.c"

$CC $CFLAGS -o $TARGET$EXTENSION $SOURCE

