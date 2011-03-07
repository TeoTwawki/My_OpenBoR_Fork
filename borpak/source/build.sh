#!/bin/sh
# Compiles borpak for the following platforms...
# Windows (MinGW), Mac OS X and Linux

TARGET_PLATFORM=$1
HOST_PLATFORM=$(uname -s)

if [ $# -ne 1 ]; then
  echo "Usage: '`basename $0` mac' == Mac OS X"
  echo "\t\t win  == Windows"
  echo "\t\t lin  == Linux"
  exit 1
fi

# Target is Windows
if [ `echo $TARGET_PLATFORM | grep -o "win"` ]; then
  EXTENSION=".exe"
  SOURCE="scandir.c "
  if [ `echo $HOST_PLATFORM | grep -o "Darwin"` ]; then
    CC="i386-mingw32-"
    PATH="$PATH:/usr/local/i386-mingw32-4.3.0/bin"
  elif [ `echo $HOST_PLATFORM | grep -o "Linux"` ]; then
    CC="i586-mingw32msvc-"
  fi
fi

# Target is Linux
if [ `echo $TARGET_PLATFORM | grep -o "lin"` ]; then
  if [ `echo $HOST_PLATFORM | grep -o "Darwin"` ]; then
    CC="i386-linux-"
    PATH="$PATH:/usr/local/i386-linux-4.1.1/bin"
  fi
fi

# Target is Mac OS X
if [ `echo $TARGET_PLATFORM | grep -o "mac"` ]; then
  if [ `echo $HOST_PLATFORM | grep -o "Darwin"` ]; then
    CFLAGS+="-arch x86_64 -arch i386 -arch ppc"
  fi
fi

TARGET="borpak"
CC=${CC}"gcc"
RM="rm -rf"
SOURCE="$SOURCE borpak.c stristr.c"

$RM $TARGET$EXTENSION *.o
$CC $CFLAGS -o $TARGET$EXTENSION $SOURCE
