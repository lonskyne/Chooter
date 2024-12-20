CC = gcc
IN = main.c src/main_state.c src/glad/glad.c src/tilemap.c src/particles.c src/roach.c
OUT = main.out
CFLAGS = -Wall -DGLFW_INCLUDE_NONE -Wno-incompatible-pointer-types -O2 -g -fno-omit-frame-pointer
LFLAGS = -ldl -lm -lglfw
IFLAGS = -I. -I./include

.SILENT all: clean build run

clean:
	rm -f $(OUT)

build: $(IN) include/main_state.h include/stb_image.h src/tilemap.h src/particles.h src/roach.h
	$(CC) $(IN) -o $(OUT) $(CFLAGS) $(LFLAGS) $(IFLAGS)

run: $(OUT)
	./$(OUT)
