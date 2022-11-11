TARGET_EXEC := basic-tutorial-3

BUILD_DIR := bin
SRC_DIRS := ./

CC := gcc
CXX := g++

SRCS := $(shell find $(SRC_DIRS) -name '*.cpp' -or -name '*.c' -or -name '*.s')
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

CPPFLAGS := $(INC_FLAGS) -MMD -MP `pkg-config --cflags --libs gstreamer-1.0`
CFLAGS := $(INC_FLAGS) 
LDFLAGS := `pkg-config --cflags --libs gstreamer-1.0`

# The final build step.
all: 
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(SRCS) -o $(BUILD_DIR)/$(TARGET_EXEC) $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(TARGET_EXEC) *.o *.d

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)