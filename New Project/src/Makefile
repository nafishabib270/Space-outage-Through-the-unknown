# Makefile for GeoWorld (refactored)
# Adjust RAYLIB_PATH and platform flags as needed.

CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11
LIBS    = -lraylib -lm -lpthread -ldl

SRCS    = main.c \
          game_state.c \
          score.c \
          bullet.c \
          enemy.c \
          alien.c \
          booster.c \
          collision.c \
          spawner.c \
          spaceship.c \
          astronaut.c \
          ui.c

OBJS    = $(SRCS:.c=.o)
TARGET  = geoworld

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
