CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lm
NAME = triagcntr 
SDL_CFLAGS := $(shell pkg-config --cflags sdl2 SDL2_ttf)
SDL_LIBS := $(shell pkg-config --libs sdl2 SDL2_ttf)
override CFLAGS += $(SDL_CFLAGS)
override LIBS += $(SDL_LIBS)

all: create 
	@echo "Done!"

create: triagcntr.o text.o 
	$(CC) $(LDFLAGS) -o $(NAME) $^ $(CFLAGS) $(LIBS)

triagcntr.o: triagcntr.c 
	$(CC) $(LDFLAGS) -o $@ -c $^ $(CFLAGS) $(LIBS)

text.o: text.c
	$(CC) $(LDFLAGS) -o $@ -c $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(NAME) *.o
	@echo "Cleaned!"

