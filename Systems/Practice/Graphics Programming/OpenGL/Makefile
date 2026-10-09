
# Compiler
CC = /usr/bin/clang

# Project files
SRC = "./Exercises/Chapter 2/Lighting Maps - 011/Maps.c"  /opt/Glad/src/glad.c -Wall -Wextra -Wpedantic
OUT = MapsExercise

# Include paths
INCLUDES = -I/opt/Glad/include/glad -I/opt/Glad/include/KHR -I/opt/cglm/include/cglm -I/opt/homebrew/include

# Library paths
LIBS = -L/opt/homebrew/lib -lglfw -framework OpenGL

# Flags
SDK := /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk

CFLAGS = $(INCLUDES) -isysroot $(SDK)
LDFLAGS = $(LIBS) -isysroot $(SDK)

# Build target
all:
	$(CC) -g $(SRC) $(CFLAGS) $(LDFLAGS) -o $(OUT)

# Clean build
clean:
	rm -f $(OUT)