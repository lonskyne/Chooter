#include <rafgl.h>

#define WORLD_SIZE 128
#define NUMBER_OF_TILES 9
#define TILE_SIZE 64

extern rafgl_raster_t tiles[NUMBER_OF_TILES];

extern int tile_world[WORLD_SIZE][WORLD_SIZE];

void init_tilemap(int tile_world[WORLD_SIZE][WORLD_SIZE]);

void render_tilemap(rafgl_raster_t *raster, int camx, int camy, int raster_width, int raster_height);