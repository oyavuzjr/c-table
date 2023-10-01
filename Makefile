# Variables
CC = gcc
CFLAGS = `pkg-config --cflags glib-2.0`
LIBS = `pkg-config --libs glib-2.0`

# Target and dependencies
TARGET = main
SOURCE = main.c

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(SOURCE)
	$(CC) $(SOURCE) -o $(TARGET) $(CFLAGS) $(LIBS)

# Clean up
clean:
	rm -f $(TARGET)
