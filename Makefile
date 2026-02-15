CC = gcc
CFLAGS = -g -Wall -Wextra -Wpedantic -std=c99 -DPLATFORM_DESKTOP -DPLATFORM_DESKTOP_GLF
INCLUDES = -I. -I/home/per/work/raylib/src -I/home/per/work/raylib/src/external -I/usr/local/include
LIBS = -L. -L/home/per/work/raylib/src -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -latomic
SRCS = snaker.c
BUILD_DIR = build
OBJS = $(BUILD_DIR)/snaker.o
MAIN = $(BUILD_DIR)/snaker

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm $(BUILD_DIR)/*
