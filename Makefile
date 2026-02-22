OS_NAME = $(shell uname -s)

TARGET ?= DESKTOP

RELEASE ?= DEBUG

# Folder containing the source code
SRC_DIR = ./src

# Build output folder
OBJDIR := objects

# Game output name
OUTPUT_NAME = main

CC := g++

C_SRCS :=
CPP_SRCS :=
OBJS :=

OUTPUT :=

LDFLAGS := 
CFLAGS := -Wall -Wextra #-Werror -fsanitize=address
INCLUDE := -I $(SRC_DIR) -I ./libs/glad/include -I./libs/cglm/include

# Get the source files and setup other variables depending on the OS
ifeq ($(OS_NAME), Linux)
	C_SRCS  += $(shell find $(SRC_DIR) -type f -name '*.c') ./libs/glad/src/glad.c
	CPP_SRCS += $(shell find $(SRC_DIR) -type f -name '*.cpp')
else ifeq ($(OS_NAME), Darwin)
	C_SRCS  += $(shell find $(SRC_DIR) -type f -name '*.c') ./libs/glad/src/glad.c
	CPP_SRCS += $(shell find $(SRC_DIR) -type f -name '*.cpp')
else
	C_SRCS += $(wildcard $(SRC_DIR)/*.c) $(wildcard $(SRC_DIR)/*/*.c) ./libs/glad/src/glad.c
	CPP_SRCS += $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)
endif

# Setup the object files
OBJS += $(patsubst $(SRC_DIR)/%.cpp, $(OBJDIR)/%.o, $(CPP_SRCS)) $(patsubst $(SRC_DIR)/%.c, $(OBJDIR)/%.o, $(C_SRCS))

# Setup web and desktop configurations
ifeq ($(TARGET), WEB)
	CC = em++
	LDFLAGS += -sMAX_WEBGL_VERSION=2 -s USE_SDL=3 -s USE_SDL_IMAGE=3 -s USE_SDL_TTF=3 -s SDL2_IMAGE_FORMATS='["png"]' -s WASM=1 --preload-file shaders -Wno-unused-command-line-argument -Wno-deprecated

# OUTPUT = index.js
	OUTPUT = index.html
else ifeq ($(OS_NAME), Linux)
	LDFLAGS += `pkg-config --libs --cflags sdl3 sdl3-image sdl3-ttf` -lm

	OUTPUT = $(OUTPUT_NAME).out
else ifeq ($(OS_NAME), Darwin)
	LDFLAGS += `pkg-config --libs --cflags sdl3 sdl3-image sdl3-ttf` -lm

	OUTPUT = $(OUTPUT_NAME).out
else
	LDFLAGS += `pkg-config --libs --cflags sdl3 sdl3-image sdl3-ttf | sed 's/-mwindows//'` -lm

	OUTPUT = $(OUTPUT_NAME).exe
endif

# Setup CFLAGS for either release or debug
ifeq ($(RELEASE), DEBUG)
	CFLAGS += -g -O0 # -Wno-unused-variable
ifeq ($(TARGET), WEB)
	CFLAGS += --emrun
endif
else
	CFLAGS += -O2
endif

.PHONY: all clean run

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(OBJS) -o $@ $(CFLAGS) $(LDFLAGS) $(INCLUDE)

$(OBJDIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS) $(INCLUDE)

$(OBJDIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(CFLAGS) $(LDFLAGS) $(INCLUDE)

clean:
	rm -rf *.o *.exe *.out $(OBJDIR) *.js *.wasm *.data

run:
	./$(OUTPUT_NAME).out
