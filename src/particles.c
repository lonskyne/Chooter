#include "particles.h"

void draw_particles(rafgl_raster_t *raster) {
  int i;
  particle_t p;
  for (i = 0; i < MAX_PARTICLES; i++) {
    p = particles[i];
    if (p.life <= 0)
      continue;
    rafgl_raster_draw_line(raster, p.x - p.dx, p.y - p.dy, p.x, p.y,
                           rafgl_RGB(255, 128, 0));
  }
}

void update_particles(float delta_time) {
  for (int i = 0; i < MAX_PARTICLES; i++) {
    if (particles[i].life <= 0)
      continue;

    particles[i].life--;

    particles[i].x += particles[i].dx;
    particles[i].y += particles[i].dy;
    particles[i].dx *= 0.995f;
    particles[i].dy *= 0.995f;
  }
}