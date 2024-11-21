#include "particles.h"
#include "tilemap.h"
#include <game_constants.h>
#include <glad/glad.h>
#include <main_state.h>
#include <math.h>
#include <rafgl.h>
#include <stdio.h>
#include <time.h>

rafgl_raster_t tiles[NUMBER_OF_TILES];
int tile_world[WORLD_SIZE][WORLD_SIZE];
particle_t particles[MAX_PARTICLES];

static rafgl_raster_t raster;
static rafgl_texture_t texture;
static rafgl_spritesheet_t hero;

static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;

static char save_file[256];
int save_file_no = 0;

int selected_x, selected_y;
int hero_tile_x, hero_tile_y;

int hero_pos_x = RASTER_WIDTH / 2;
int hero_pos_y = RASTER_HEIGHT / 2;

int camx = RASTER_WIDTH / 2;
int camy = RASTER_HEIGHT / 2;

float location = 0;
float selector = 0;

int hero_speed = 200;
int hero_rotation = 0;

void main_state_init(GLFWwindow *window, void *args, int width, int height) {
  srand(time(NULL));

  raster_width = width;
  raster_height = height;

  rafgl_raster_init(&raster, raster_width, raster_height);

  rafgl_spritesheet_init(&hero, "res/images/character.png", 1, 29);

  char tile_path[256];

  // Loading tile sprites from disk to tile array
  for (int i = 0; i < NUMBER_OF_TILES; i++) {
    sprintf(tile_path, "res/tiles/tile%d.png", i);
    rafgl_raster_load_from_image(&tiles[i], tile_path);
  }

  init_tilemap(tile_world);

  rafgl_texture_init(&texture);
}

void main_state_update(GLFWwindow *window, float delta_time,
                       rafgl_game_data_t *game_data, void *args) {
  int i, gen = 5, radius = 10;
  float angle, speed;

  camx = hero_pos_x - 450;
  camy = hero_pos_y - 450;

  // Calculate wich tile is currently selected
  selected_x = rafgl_clampi((game_data->mouse_pos_x + camx) / TILE_SIZE, 0,
                            WORLD_SIZE - 1);
  selected_y = rafgl_clampi((game_data->mouse_pos_y + camy) / TILE_SIZE, 0,
                            WORLD_SIZE - 1);

  // The tile the player is currently on
  hero_tile_x = rafgl_clampi((hero_pos_x + 25) / TILE_SIZE, 0,
                            WORLD_SIZE - 1);
  hero_tile_y = rafgl_clampi((hero_pos_y + 27) / TILE_SIZE, 0,
                            WORLD_SIZE - 1);

  // Shooting, if shooting then stop moving and rotating
  if (game_data->keys_down[RAFGL_KEY_SPACE]) {
    for (i = 0; (i < MAX_PARTICLES) && gen; i++) {
      if (particles[i].life <= 0) {
        particles[i].life = 100 * randf() + 100;
        particles[i].x = hero_pos_x - camx + 32;
        particles[i].y = hero_pos_y - camy + 32;

        angle = hero_rotation * 12 * M_PI / 180;
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
    int future_hero_tile_x = rafgl_clampi((future_hero_pos_x + 25) / TILE_SIZE, 0,
                            WORLD_SIZE - 1);
    int future_hero_tile_y = rafgl_clampi((future_hero_pos_y + 27) / TILE_SIZE, 0,
                            WORLD_SIZE - 1);

    // If the future position does not put us in a wall, update current position
    if(tile_world[future_hero_tile_y][future_hero_tile_x] != 0) {
      hero_pos_x = future_hero_pos_x;
      hero_pos_y = future_hero_pos_y;
    }
  }

  int x, y;
  float xn, yn;

  rafgl_pixel_rgb_t sampled, resulting;

  for (y = 0; y < raster_height; y++) {
    yn = 1.0f * y / raster_height;
    for (x = 0; x < raster_width; x++) {
      xn = 1.0f * x / raster_width;

      resulting = sampled;

      resulting.rgba = rafgl_RGB(0, 0, 0);

      pixel_at_m(raster, x, y) = resulting;
    }
  }

  update_particles(delta_time);

  render_tilemap(&raster, camx, camy, raster_width, raster_height);
  draw_particles(&raster);
  rafgl_raster_draw_spritesheet(&raster, &hero, 0, hero_rotation,
                                hero_pos_x - camx, hero_pos_y - camy);
                                
  rafgl_raster_draw_rectangle(&raster, hero_tile_x * TILE_SIZE - camx, hero_tile_y * TILE_SIZE - camy, TILE_SIZE, TILE_SIZE, rafgl_RGB(255, 255, 0));


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