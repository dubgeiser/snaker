CC = gcc
CCFLAGS = -Wall -Wextra -std=c99 -DPLATFORM_DESKTOP -DPLATFORM_DESKTOP_GLF
INCLUDES = -I. -I/home/per/work/raylib/src -I/home/per/work/raylib/src/external -I/usr/local/include
LIBS = -L. -L/home/per/work/raylib/src -L/usr/local/lib -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -latomic
SRCS = snaker.c
OBJS = $(SRCS:.c=.o)
MAIN = build/snaker

$(MAIN): $(OBJS)
	$(CC) $(CCFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LIBS)

.c .o:
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm *.o $(MAIN)
