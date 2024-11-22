#include <rafgl.h>

#define MAX_ROACHES 50
#define ROACH_HEALTH 100

typedef struct _roach_t {
  float x, y, dx, dy;
  int hover_frame;
  int anim_frame;
  int rotation;
  int life;
  
  rafgl_spritesheet_t sprite;
} roach_t;

extern roach_t roaches[MAX_ROACHES];

void roaches_init(int hero_pos_x, int hero_pos_y, roach_t roaches[MAX_ROACHES]);

void draw_roaches(rafgl_raster_t *raster, roach_t roaches[MAX_ROACHES]);

void update_roaches(float delta_time, roach_t roaches[MAX_ROACHES]);