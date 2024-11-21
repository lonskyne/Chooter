#include "tilemap.h"

void generate_map(int map[WORLD_SIZE][WORLD_SIZE]) {
  int helper[WORLD_SIZE][WORLD_SIZE];
  // Must be an odd number to take effect. (4 is same as 3)
  int iterations = 5;

  // Generate random noise with 45% of walls
  for (int i = 0; i < WORLD_SIZE; i++) {
    for (int j = 0; j < WORLD_SIZE; j++) {
      float random = rand() % 100;
      if (random < 45)
        helper[i][j] = 1;
      else
        helper[i][j] = 0;
    }
  }

  int (*to)[WORLD_SIZE] = map;
  int (*from)[WORLD_SIZE] = helper;

  // Go trough iterations, every time changing from/to matrixes
  for(int it = 0; it < iterations; it++) {

    for(int i = 0; i < WORLD_SIZE; i++) {
      for(int j = 0; j < WORLD_SIZE; j++) {
        int neighbours = 0;
        // Check 3x3 neighbourhood of current cell
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
        else
          to[i][j]= 0;
      }
    }

    int (*temp)[WORLD_SIZE] = to;
    to = from;
    from = temp;
  }

}

void init_tilemap(int tile_world[WORLD_SIZE][WORLD_SIZE]) {
  int x, y;
  int cave_map[WORLD_SIZE][WORLD_SIZE];
  generate_map(cave_map);

  for (y = 0; y < WORLD_SIZE; y++) {
    for (x = 0; x < WORLD_SIZE; x++) {
      if(cave_map[y][x] == 1 || y == 0 || x == 0 || y == WORLD_SIZE - 1 || x == WORLD_SIZE - 1)
        tile_world[y][x] = 0; 
      else {
        // When generating walkable terrain, half is blank brown tile
        if(rand() % 2 < 1) {
          tile_world[y][x] = 1;
        }
        else {
          // And half are random other types
          tile_world[y][x] = 2 + rand() % (NUMBER_OF_TILES - 2);
        }
      }
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