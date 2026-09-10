
# Compiler
CC = gcc

# Project files
SRC = "./Exercises/Chapter 2/Colors - 09/Colors.c"  /opt/Glad/src/glad.c
OUT = Colors

# Include paths
INCLUDES = -I/opt/Glad/include/glad -I/opt/Glad/include/KHR -I/opt/cglm/include/cglm -I/opt/homebrew/include

# Library paths
LIBS = -L/opt/homebrew/lib -lglfw -framework OpenGL

# Flags
CFLAGS = $(INCLUDES)
LDFLAGS = $(LIBS)

# Build target
all:
	$(CC) -g $(SRC) $(CFLAGS) $(LDFLAGS) -o $(OUT)

# Clean build
clean:
	rm -f $(OUT)