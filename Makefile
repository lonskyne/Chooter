CC = gcc
IN = main.c src/main_state.c src/glad/glad.c src/tilemap.c
OUT = main.out
CFLAGS = -Wall -DGLFW_INCLUDE_NONE -Wno-incompatible-pointer-types
LFLAGS = -ldl -lm -lglfw
IFLAGS = -I. -I./include

.SILENT all: clean build run

clean:
	rm -f $(OUT)

build: $(IN) include/main_state.h include/stb_image.h src/tilemap.h
	$(CC) $(IN) -o $(OUT) $(CFLAGS) $(LFLAGS) $(IFLAGS)

run: $(OUT)
	./$(OUT)
