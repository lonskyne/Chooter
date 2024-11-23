#include <rafgl.h>

#define MAX_ROACHES 50
#define ROACH_HEALTH 100
#define ROACH_SPEED 100
#define ROACH_HITBOX_SIZE 200

typedef struct _roach_t {
  float x, y, dx, dy;
  int hover_frame;
  int anim_frame;
  int rotation;
  float life;
  
  rafgl_spritesheet_t sprite;
} roach_t;

extern roach_t roaches[MAX_ROACHES];

void roaches_init(int hero_pos_x, int hero_pos_y, roach_t roaches[MAX_ROACHES]);

void draw_roaches(rafgl_raster_t *raster, int camx, int camy);

void update_roaches(float delta_time, int hero_pos_x, int hero_pos_y);

void hurt_roaches(float delta_time, float angle, int hero_pos_x, int hero_pos_y);