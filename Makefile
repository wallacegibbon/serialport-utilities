C_SOURCE_FILES += $(wildcard ./lib/*.c)
C_SOURCE_FILES += ./src/main.c

C_INCLUDES += ./lib ./src

BUILD_DIR ?= build
TARGET ?= target

OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(C_SOURCE_FILES:.c=.c.o)))

C_FLAGS += $(addprefix -I, $(C_INCLUDES))

vpath %.c $(sort $(dir $(C_SOURCE_FILES)))

.PHONY: all clean

all: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	gcc -o $@ $^ -lserialport

$(BUILD_DIR)/%.c.o: %.c | $(BUILD_DIR)
	gcc $(C_FLAGS) -o $@ -c $< -MMD -MP -MF"$(@:%.o=%.d)"

$(BUILD_DIR):
	mkdir $@

clean:
	rm -rf build

