
# Compiler
CC = gcc

# Project files
SRC = Index.c /opt/Glad/src/glad.c
OUT = Index

# Include paths
INCLUDES = -I/opt/Glad/include/glad -I/opt/Glad/include/KHR -I/opt/homebrew/include

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