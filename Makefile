C_SOURCE_FILES += $(wildcard ./lib/*.c)
C_SOURCE_FILES += ./src/main.c

C_INCLUDES += ./lib ./src

BUILD_DIR ?= build
SERIALPORT_JSON_READER ?= serialport-json-reader

CC = gcc

SERIALPORT_JSON_READER_OBJECTS = \
$(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCE_FILES:.c=.c.o)))

C_FLAGS += $(addprefix -I, $(C_INCLUDES)) -MMD -MP -MF"$(@:%.o=%.d)"

vpath %.c $(sort $(dir $(C_SOURCE_FILES)))

.PHONY: all clean

all: $(BUILD_DIR)/$(SERIALPORT_JSON_READER)

$(BUILD_DIR)/$(SERIALPORT_JSON_READER): $(SERIALPORT_JSON_READER_OBJECTS)
	$(CC) -o $@ $^ -lserialport

$(BUILD_DIR)/%.c.o: %.c | $(BUILD_DIR)
	$(CC) $(C_FLAGS) -o $@ -c $<

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf build

