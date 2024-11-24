#include "roach.h"
#include <math.h>
#include <stdio.h>

float spawn_timer;
float roach_spawn_gap;

void generate_roach(int hero_pos_x, int hero_pos_y) {
  int distance = 500 + rand() % 500;
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

float hurt_player(float delta_time, int hero_pos_x, int hero_pos_y,
                  int roach_index) {
  if (rafgl_distance2D(hero_pos_x, hero_pos_y, roaches[roach_index].x,
                       roaches[roach_index].y) < 20) {
    return delta_time * (float)ROACH_DAMAGE;
  }

  return 0.0f;
}

void roaches_init() {
  for (int i = 0; i < MAX_ROACHES; i++) {
    roaches[i].life = 0;
  }

  roach_spawn_gap = INITIAL_ROACH_SPAWN_GAP;
  spawn_timer = roach_spawn_gap;
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

float update_roaches(float delta_time, int hero_pos_x, int hero_pos_y) {
  spawn_timer -= delta_time;

  if (spawn_timer <= 0) {
    generate_roach(hero_pos_x, hero_pos_y);
    roach_spawn_gap -= delta_time;
    if (roach_spawn_gap < 0.3)
      roach_spawn_gap = 0.3;
    spawn_timer = roach_spawn_gap;
  }

  float total_damage = 0;

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

    total_damage += hurt_player(delta_time, hero_pos_x, hero_pos_y, i);
  }

  return total_damage;
}

void hurt_roaches(float delta_time, float angle, int hero_pos_x,
                  int hero_pos_y) {
  for (int i = 0; i < MAX_ROACHES; i++) {
    if (roaches[i].life <= 0)
      continue;

    double hero_roach_dist =
        sqrt((hero_pos_x - roaches[i].x) * (hero_pos_x - roaches[i].x) +
             (hero_pos_y - roaches[i].y) * (hero_pos_y - roaches[i].y));

    // Calculate where our shot lands
    double shot_land_x = hero_pos_x + (hero_roach_dist * cos(angle));
    double shot_land_y = hero_pos_y + (hero_roach_dist * sin(angle));

    // If roach is in hibox distance of the shot, it hurts it
    double shot_roach_dist =
        sqrt((shot_land_x - roaches[i].x) * (shot_land_x - roaches[i].x) +
             (shot_land_y - roaches[i].y) * (shot_land_y - roaches[i].y));

    if (shot_roach_dist < ROACH_HITBOX_SIZE)
      roaches[i].life -= delta_time * 70;
  }
}