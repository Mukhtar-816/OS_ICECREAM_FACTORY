CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LIBS = -lraylib -lpthread -lGL -lm -ldl -lrt -lX11

SRC = src/main.c src/factory.c src/gui.c
OBJ = $(SRC:.c=.o)
TARGET = ice_factory

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
