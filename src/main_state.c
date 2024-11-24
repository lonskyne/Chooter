#include "particles.h"
#include "roach.h"
#include "tilemap.h"
#include <game_constants.h>
#include <glad/glad.h>
#include <main_state.h>
#include <math.h>
#include <rafgl.h>
#include <stdio.h>
#include <time.h>

#define VIEW_DISTANCE 300

rafgl_raster_t tiles[NUMBER_OF_TILES];
int tile_world[WORLD_SIZE][WORLD_SIZE];
particle_t particles[MAX_PARTICLES];
roach_t roaches[MAX_ROACHES];

static rafgl_raster_t raster;
static rafgl_texture_t texture;
static rafgl_spritesheet_t hero;

static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;

static char save_file[256];
int save_file_no = 0;

int hero_tile_x, hero_tile_y;

int hero_pos_x = WORLD_SIZE * 64 / 2;
int hero_pos_y = WORLD_SIZE * 64 / 2;

int camx = RASTER_WIDTH / 2;
int camy = RASTER_HEIGHT / 2;

float location = 0;
float selector = 0;

int hero_speed = 200;
int hero_rotation = 0;

int roach_anim_frame = 0;
int roach_hover_frames = 10;

int debug_mode = 1;

// TODO make vision impair more gradual not just circle
void impair_vision(rafgl_raster_t *raster) {
  rafgl_pixel_rgb_t resulting;

  for (int y = 0; y < raster_height; y++) {
    for (int x = 0; x < raster_width; x++) {
      int cx = raster_width / 2, cy = raster_height / 2;
      resulting.rgba = rafgl_RGB(0, 0, 0);

      if (rafgl_distance2D(cx, cy, x, y) > VIEW_DISTANCE)
        pixel_at_m((*raster), x, y) = resulting;
    }
  }
}

void main_state_init(GLFWwindow *window, void *args, int width, int height) {
  srand(time(NULL));

  raster_width = width;
  raster_height = height;

  rafgl_raster_init(&raster, raster_width, raster_height);

  rafgl_spritesheet_init(&hero, "res/images/character.png", 1, 29);

  for (int i = 0; i < MAX_ROACHES; i++)
    rafgl_spritesheet_init(&(roaches[i].sprite), "res/images/roach.png", 4, 29);

  char tile_path[256];

  // Loading tile sprites from disk to tile array
  for (int i = 0; i < NUMBER_OF_TILES; i++) {
    sprintf(tile_path, "res/tiles/tile%d.png", i);
    rafgl_raster_load_from_image(&tiles[i], tile_path);
  }

  init_tilemap(tile_world);
  roaches_init(hero_pos_x, hero_pos_y);
  rafgl_texture_init(&texture);
}

void main_state_update(GLFWwindow *window, float delta_time,
                       rafgl_game_data_t *game_data, void *args) {
  int i, gen = 5, radius = 10;
  float angle, speed;

  camx = hero_pos_x - 480;
  camy = hero_pos_y - 480;

  roach_hover_frames--;
  if (roach_hover_frames == 0) {
    roach_anim_frame++;
    roach_anim_frame %= 4;
    roach_hover_frames = 10;
  }

  // The tile the player is currently on
  hero_tile_x = rafgl_clampi((hero_pos_x + 25) / TILE_SIZE, 0, WORLD_SIZE - 1);
  hero_tile_y = rafgl_clampi((hero_pos_y + 27) / TILE_SIZE, 0, WORLD_SIZE - 1);

  if (game_data->keys_down[RAFGL_KEY_F1])
    debug_mode = 1;
  if (game_data->keys_down[RAFGL_KEY_F2])
    debug_mode = 0;

  // Shooting, if shooting then stop moving and rotating
  if (game_data->keys_down[RAFGL_KEY_SPACE]) {
    for (i = 0; (i < MAX_PARTICLES) && gen; i++) {
      if (particles[i].life <= 0) {
        particles[i].life = 100 * randf() + 100;
        particles[i].x = hero_pos_x - camx + 32;
        particles[i].y = hero_pos_y - camy + 32;

        angle = hero_rotation * 12 * M_PI / 180;

        hurt_roaches(delta_time, angle, hero_pos_x, hero_pos_y);

        speed = (0.3f + 0.1 * randf()) * radius * 10;
        particles[i].dx = cosf(angle) * speed;
        particles[i].dy = sinf(angle) * speed;
        gen--;
      }
    }
  } else {
    // Rotation, we have 29 rotation sprites, every by 12 degrees
    // TODO: Add rotation speed and do * delta_time
    if (game_data->keys_down[RAFGL_KEY_RIGHT]) {
      hero_rotation++;
      hero_rotation %= 29;
    } else if (game_data->keys_down[RAFGL_KEY_LEFT]) {
      hero_rotation--;
      if (hero_rotation < 0)
        hero_rotation = 28;
    }

    int future_hero_pos_x = hero_pos_x;
    int future_hero_pos_y = hero_pos_y;

    // Movement
    if (game_data->keys_down[RAFGL_KEY_W]) {
      future_hero_pos_y = hero_pos_y - hero_speed * delta_time;
    } else if (game_data->keys_down[RAFGL_KEY_S]) {
      future_hero_pos_y = hero_pos_y + hero_speed * delta_time;
    } else if (game_data->keys_down[RAFGL_KEY_A]) {
      future_hero_pos_x = hero_pos_x - hero_speed * delta_time;
    } else if (game_data->keys_down[RAFGL_KEY_D]) {
      future_hero_pos_x = hero_pos_x + hero_speed * delta_time;
    }

    // Calculate the tile we would end up after current frame movement
    int future_hero_tile_x =
        rafgl_clampi((future_hero_pos_x + 25) / TILE_SIZE, 0, WORLD_SIZE - 1);
    int future_hero_tile_y =
        rafgl_clampi((future_hero_pos_y + 27) / TILE_SIZE, 0, WORLD_SIZE - 1);

    // If the future position does not put us in a wall, update current position
    if (tile_world[future_hero_tile_y][future_hero_tile_x] != 0) {
      hero_pos_x = future_hero_pos_x;
      hero_pos_y = future_hero_pos_y;
    }
  }

  rafgl_pixel_rgb_t resulting;

  for (int y = 0; y < raster_height; y++) {
    for (int x = 0; x < raster_width; x++) {

      resulting.rgba = rafgl_RGB(20, 20, 20);

      pixel_at_m(raster, x, y) = resulting;
    }
  }

  update_particles(delta_time);
  update_roaches(delta_time, hero_pos_x, hero_pos_y);

  render_tilemap(&raster, camx, camy, raster_width, raster_height);
  draw_particles(&raster);
  rafgl_raster_draw_spritesheet(&raster, &hero, 0, hero_rotation,
                                hero_pos_x - camx, hero_pos_y - camy);

  draw_roaches(&raster, camx, camy);

  if (debug_mode) {
    for (int i = 0; i < MAX_ROACHES; i++) {
      if (roaches[i].life > 0) {
        if ((int)(roaches[i].x - camx) > ROACH_HITBOX_SIZE &&
            (int)(roaches[i].y - camy) > ROACH_HITBOX_SIZE)
          rafgl_raster_draw_circle(&raster, (int)(roaches[i].x - camx + 32),
                                   (int)(roaches[i].y - camy + 32),
                                   ROACH_HITBOX_SIZE, rafgl_RGB(255, 255, 0));
      }
    }
    rafgl_raster_draw_rectangle(&raster, hero_tile_x * TILE_SIZE - camx,
                                hero_tile_y * TILE_SIZE - camy, TILE_SIZE,
                                TILE_SIZE, rafgl_RGB(255, 255, 0));
  } else {
    impair_vision(&raster);
  }

  // Screenshot
  if (game_data->keys_pressed[RAFGL_KEY_PRINT_SCREEN]) {
    sprintf(save_file, "save%d.png", save_file_no++);
    rafgl_raster_save_to_png(&raster, save_file);
  }
}

void main_state_render(GLFWwindow *window, void *args) {
  rafgl_texture_load_from_raster(&texture, &raster);
  rafgl_texture_show(&texture, 0);
}

void main_state_cleanup(GLFWwindow *window, void *args) {
  rafgl_raster_cleanup(&raster);
  rafgl_texture_cleanup(&texture);
}