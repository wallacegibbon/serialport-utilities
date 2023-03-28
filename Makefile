C_SOURCE_FILES += $(wildcard ./lib/*.c)
C_SOURCE_FILES += ./src/main.c

C_INCLUDES += ./lib ./src

#TARGET = serialport-json-reader
TARGET = serialport-lines-reader

LD_FLAGS += -lserialport

include ./miscellaneous-makefiles/simple-gcc-single.mk

