#include "roach.h"
#include <math.h>

void generate_roach(int hero_pos_x, int hero_pos_y, roach_t roaches[MAX_ROACHES]) {
    hero_pos_x = 480;
    hero_pos_y = 480;
    int distance = 200 + rand() % 200;
    int rotation = rand() % 29;

    int angle = 180 - rotation * 12;

    if(angle < 0)
        angle += 360; 


    for(int i = 0; i < MAX_ROACHES; i++) {
        if(roaches[i].life <= 0) {
            roaches[i].x = (float)hero_pos_x + (distance * cos((float)(360 - angle) * M_PI / 180.0f));
            roaches[i].y = (float)hero_pos_y + (distance * sin((float)(360 - angle) * M_PI / 180.0f));
            roaches[i].life = ROACH_HEALTH;
            roaches[i].rotation = rotation;
            roaches[i].hover_frame = 10;
            roaches[i].anim_frame = 0;
            break;
        }
    }
}

void roaches_init(int hero_pos_x, int hero_pos_y, roach_t roaches[MAX_ROACHES]) {
    for(int i = 0; i < MAX_ROACHES; i++) {
        roaches[i].life = 0;
    }

    for(int i = 0; i < 10; i++)
        generate_roach(hero_pos_x, hero_pos_y, roaches);
}

void draw_roaches(rafgl_raster_t *raster , roach_t roaches[MAX_ROACHES]) {
  (void)0;
}

void update_roaches(float delta_time, roach_t roaches[MAX_ROACHES]) {
  for (int i = 0; i < MAX_ROACHES; i++) {
    if (roaches[i].life <= 0)
      continue;

    roaches[i].x += roaches[i].dx;
    roaches[i].y += roaches[i].dy;
  }
}