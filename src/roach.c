#include "roach.h"
#include <math.h>
#include <stdio.h>

void generate_roach(int hero_pos_x, int hero_pos_y,
                    roach_t roaches[MAX_ROACHES]) {
  hero_pos_x = 480;
  hero_pos_y = 480;
  int distance = 200 + rand() % 200;
  int rotation = rand() % 29;

  int angle = 180 - rotation * 12;

  if (angle < 0)
    angle += 360;

  for (int i = 0; i < MAX_ROACHES; i++) {
    if (roaches[i].life <= 0) {
      roaches[i].x = (float)hero_pos_x +
                     (distance * cos((float)(360 - angle) * M_PI / 180.0f));
      roaches[i].y = (float)hero_pos_y +
                     (distance * sin((float)(360 - angle) * M_PI / 180.0f));
      roaches[i].life = ROACH_HEALTH;
      roaches[i].rotation = rotation;
      roaches[i].hover_frame = 10;
      roaches[i].anim_frame = 0;
      break;
    }
  }
}

void roaches_init(int hero_pos_x, int hero_pos_y,
                  roach_t roaches[MAX_ROACHES]) {
  for (int i = 0; i < MAX_ROACHES; i++) {
    roaches[i].life = 0;
  }

  for (int i = 0; i < 1; i++)
    generate_roach(hero_pos_x, hero_pos_y, roaches);
}

void draw_roaches(rafgl_raster_t *raster, int camx, int camy) {
  for (int i = 0; i < MAX_ROACHES; i++) {
    if (roaches[i].life <= 0)
      continue;

    roaches[i].hover_frame--;
    if (roaches[i].hover_frame == 0) {
      roaches[i].anim_frame++;
      roaches[i].anim_frame %= 4;
      roaches[i].hover_frame = 10;
    }

    rafgl_raster_draw_spritesheet(raster, &(roaches[i].sprite),
                                  roaches[i].anim_frame, roaches[i].rotation,
                                  roaches[i].x - camx, roaches[i].y - camy);
  }
}

void update_roaches(float delta_time, int hero_pos_x, int hero_pos_y) {
  for (int i = 0; i < MAX_ROACHES; i++) {
    if (roaches[i].life <= 0)
      continue;

    // Calculate the angle from the player to the roach
    double angle = atan2((double)(-roaches[i].y + hero_pos_y),
                         (double)(roaches[i].x - hero_pos_x));

    double tmp_angle = angle * 180.0f / M_PI;

    // Get the rotation from the angle by reversing the formula.
    if (tmp_angle > 180)
      tmp_angle -= 360;

    roaches[i].rotation = (int)((180.0f - tmp_angle) / 12.0f) % 29;

    double move_len =
        sqrt((hero_pos_x - roaches[i].x) * (hero_pos_x - roaches[i].x) +
             (hero_pos_y - roaches[i].y) * (hero_pos_y - roaches[i].y));
    double move_x = (hero_pos_x - roaches[i].x) / move_len;
    double move_y = (hero_pos_y - roaches[i].y) / move_len;

    roaches[i].x += move_x * ROACH_SPEED * delta_time;
    roaches[i].y += move_y * ROACH_SPEED * delta_time;
  }
}

void hurt_roaches(float delta_time, float angle, int hero_pos_x,
                  int hero_pos_y) {
  for (int i = 0; i < MAX_ROACHES; i++) {
    if (roaches[i].life <= 0)
      continue;

    double hero_roach_dist = sqrt((hero_pos_x - roaches[i].x) * (hero_pos_x - roaches[i].x) + (hero_pos_y - roaches[i].y) * (hero_pos_y - roaches[i].y));

    // Calculate where our shot lands
    double shot_land_x = hero_pos_x + (hero_roach_dist * cos(angle * M_PI / 180.0f));
    double shot_land_y = hero_pos_y + (hero_roach_dist * sin(angle * M_PI / 180.0f));

    double shot_roach_dist = sqrt((shot_land_x - roaches[i].x) * (shot_land_x - roaches[i].x) + (shot_land_y - roaches[i].y) * (shot_land_y - roaches[i].y));

    if(shot_roach_dist < ROACH_HITBOX_SIZE) {
        roaches[i].life -= delta_time * 100;
        printf("HIT! %f\n", roaches[i].life);
    }
  }
}