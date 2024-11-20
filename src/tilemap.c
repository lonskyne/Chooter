#include "tilemap.h"

void generate_map(int map[WORLD_SIZE][WORLD_SIZE]) {
  int helper[WORLD_SIZE][WORLD_SIZE];
  int iterations = 5;

  // Generate random noise with 45% of walls
  for (int i = 0; i < WORLD_SIZE; i++) {
    for (int j = 0; j < WORLD_SIZE; j++) {
      if (randf() < 0.45f)
        helper[i][j] = 1;
      else
        helper[i][j] = 0;
    }
  }

  int **to = map, **from = helper;

  for(int it = 0; it < iterations; it++) {

    for(int i = 0; i < WORLD_SIZE; i++) {
      for(int j = 0; j < WORLD_SIZE; j++) {
        int neighbours = 0;

        for(int k = -1; k < 2; k++) {
          for(int l = -1; l < 2; l++) {
            if(i + k >= 0 && i + k < WORLD_SIZE && j + l >= 0 && j + l < WORLD_SIZE) {
              if(from[i + k][j + l] == 1)
                neighbours++;
            }
          }
        }

        if(neighbours >= 5)
          to[i][j] = 1;
      }
    }

    int **temp = to;
    to = from;
    from = temp;
  }
}

void init_tilemap(void) {
  int x, y;
  int cave_map[WORLD_SIZE][WORLD_SIZE];

  for (y = 0; y < WORLD_SIZE; y++) {
    for (x = 0; x < WORLD_SIZE; x++) {
      generate_map(cave_map);
      tile_world[y][x] = 3 + rand() % 2;
    }
  }
}

void render_tilemap(rafgl_raster_t *raster, int camx, int camy,
                    int raster_width, int raster_height) {
  int x, y;
  int x0 = camx / TILE_SIZE;

  int x1 = x0 + (raster_width / TILE_SIZE) + 1;
  int y0 = camy / TILE_SIZE;
  int y1 = y0 + (raster_height / TILE_SIZE) + 1;

  if (x0 < 0)
    x0 = 0;
  if (y0 < 0)
    y0 = 0;
  if (x1 < 0)
    x1 = 0;
  if (y1 < 0)
    y1 = 0;

  if (x0 >= WORLD_SIZE)
    x0 = WORLD_SIZE - 1;
  if (y0 >= WORLD_SIZE)
    y0 = WORLD_SIZE - 1;
  if (x1 >= WORLD_SIZE)
    x1 = WORLD_SIZE - 1;
  if (y1 >= WORLD_SIZE)
    y1 = WORLD_SIZE - 1;

  rafgl_raster_t *draw_tile;

  for (y = y0; y <= y1; y++) {
    for (x = x0; x <= x1; x++) {
      draw_tile = tiles + (tile_world[y][x] % NUMBER_OF_TILES);
      rafgl_raster_draw_raster(raster, draw_tile, x * TILE_SIZE - camx,
                               y * TILE_SIZE - camy);
    }
  }
}