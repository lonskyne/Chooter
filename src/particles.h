#include <rafgl.h>

#define MAX_PARTICLES 50

typedef struct _particle_t {
  float x, y, dx, dy;
  int life;

} particle_t;

extern particle_t particles[MAX_PARTICLES];

void draw_particles(rafgl_raster_t *raster);

void update_particles(float delta_time);