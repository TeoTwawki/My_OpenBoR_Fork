#------------------------------------------------------------------------------
#
#               	BOR Pak Makefile for Windows & Linux 
#
#------------------------------------------------------------------------------

MAIN            = borpak

#------------------------------------------------------------------------------
# Defines
#------------------------------------------------------------------------------

ifdef BUILD_WIN
TARGET          = $(MAIN).exe
RES             = $(MAIN).res
CC 	        = $(SDKPATH)/bin/gcc.exe
STRIP 	        = $(SDKPATH)/bin/strip.exe
INCLUDES        = $(SDKPATH)/include
LIBRARIES       = $(SDKPATH)/lib
PLATFORM        = "Windows"
endif


ifdef BUILD_LINUX
TARGET 	        = $(MAIN)
CC 	        = gcc
STRIP 	        = strip
SDKPATH         = /usr
INCLUDES        = $(SDKPATH)/include
LIBRARIES       = $(SDKPATH)/local/lib
PLATFORM        = "Linux"
endif

#------------------------------------------------------------------------------
# Objects
#------------------------------------------------------------------------------

OBJS            = ../../source/stristr.o $(MAIN).o

#------------------------------------------------------------------------------
# Compiler Flags
#------------------------------------------------------------------------------

CFLAGS 	        = -O2                                                         \
                  -g                                                          \
                  -Wall                                                       \
                  -Werror                                                     \
                  -fsigned-char                                               \
                  -fomit-frame-pointer                                        \
                  -fno-ident                                                  \
                  -freorder-blocks

ifdef BUILD_WIN                  
CFLAGS         += -DWIN32
endif

#------------------------------------------------------------------------------
# Rules to manage Files and Libraries for All Other Platforms
#------------------------------------------------------------------------------

all : $(TARGET)
%.o : %.c
	@echo Compiling $(PLATFORM) Port: $<...
	@$(CC) $(CFLAGS) -c $< -o $@
$(TARGET) : $(OBJS)
	@echo
	@echo Creating $(PLATFORM) Port: $(TARGET)...
	@$(CC) -o $(TARGET) $(OBJS)
	@echo Stripping $(PLATFORM) Port: $(TARGET)...
	@$(STRIP) $(TARGET)
	@echo
	@echo Completed $(PLATFORM) Port!
	@echo $(TARGET) is now ready!

#------------------------------------------------------------------------------
# Rules to CleanUp Files for All Platforms
#------------------------------------------------------------------------------

clean:
	@echo
	@echo "Removing All Files..."
	@rm -f $(OBJS) $(TARGET)
	@echo Done!
	@echo
