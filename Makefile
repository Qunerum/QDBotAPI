CC_LINUX = gcc
CC_WIN = x86_64-w64-mingw32-gcc

CFLAGS = -Wall -Wextra -O2 -Iinclude
LDFLAGS_WIN = -static -static-libgcc -static-libstdc++

SRC_DIR = src
OBJ = obj
BUILD_DIR = build

SOURCES = $(wildcard $(SRC_DIR)/*.c)

ifeq ($(OS),Windows_NT)
    PLATFORM = win
    EXT = .exe
    TARGET_CC = $(CC_WIN)
    MKDIR = mkdir -p
else
    PLATFORM = linux
    EXT =
    TARGET_CC = $(CC_LINUX)
    MKDIR = mkdir -p
endif

OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ)/$(PLATFORM)/%.o, $(SOURCES))
TARGET = $(BUILD_DIR)/$(PLATFORM)/qcode$(EXT)

all: linux

linux:
	@$(MAKE) PLATFORM=linux EXT= TARGET_CC=$(CC_LINUX) build_platform

win:
	@$(MAKE) PLATFORM=win EXT=.exe TARGET_CC=$(CC_WIN) LDFLAGS="$(LDFLAGS_WIN)" build_platform

build_platform: $(TARGET)

$(OBJ)/$(PLATFORM)/%.o: $(SRC_DIR)/%.c
	@$(MKDIR) $(OBJ)/$(PLATFORM)
	@$(TARGET_CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	@$(MKDIR) $(BUILD_DIR)/$(PLATFORM)
	@$(TARGET_CC) $(OBJS) -o $(TARGET) -lcurl -lwebsockets -lssl -lcrypto $(LDFLAGS)

run: all
	@clear
	@echo "--- Running on $(PLATFORM) ---"
	./$(TARGET)

clean:
	@rm -rf $(OBJ) $(BUILD_DIR)

.PHONY: all linux win run clean build_platform
