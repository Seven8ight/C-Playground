
# Compiler
CC = gcc

# Project files
SRC = ./Chapters/"Transformations - 5"/"Positioning - 3.c" /opt/Glad/src/glad.c
OUT = Positioning

# Include paths
INCLUDES = -I/opt/Glad/include/glad -I/opt/Glad/include/KHR -I/opt/cglm/include/cglm -I/opt/homebrew/include

# Library paths
LIBS = -L/opt/homebrew/lib -lglfw -framework OpenGL

# Flags
CFLAGS = $(INCLUDES)
LDFLAGS = $(LIBS)

# Build target
all:
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(OUT)

# Clean build
clean:
	rm -f $(OUT)