CC = gcc
CFLAGS = -I/opt/homebrew/include
LDFLAGS = -L/opt/homebrew/lib
LDLIBS = -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
TARGET = game
SRC = main.c music.c interface.c character.c

all:
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS) $(LDLIBS) && ./$(TARGET)
