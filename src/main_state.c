#include <main_state.h>
#include <glad/glad.h>
#include <math.h>
#include <rafgl.h>
#include <game_constants.h>

#define MAX_PARTICLES 50

static rafgl_raster_t raster;
static rafgl_texture_t texture;
static rafgl_spritesheet_t hero;


static int raster_width = RASTER_WIDTH, raster_height = RASTER_HEIGHT;

static char save_file[256];
int save_file_no = 0;

typedef struct _particle_t
{
    float x, y, dx, dy;
    int life;

} particle_t;

particle_t particles[MAX_PARTICLES];


void draw_particles(rafgl_raster_t *raster)
{
    int i;
    particle_t p;
    for(i = 0; i < MAX_PARTICLES; i++)
    {
        p = particles[i];
        if(p.life <= 0) continue;
        rafgl_raster_draw_line(raster, p.x - p.dx, p.y - p.dy, p.x, p.y, rafgl_RGB(255, 128,  0));
    }
}


void update_particles(float delta_time)
{
    int i;
    for(i = 0; i < MAX_PARTICLES; i++)
    {
        if(particles[i].life <= 0) continue;

        particles[i].life--;

        particles[i].x += particles[i].dx;
        particles[i].y += particles[i].dy;
        particles[i].dx *= 0.995f;
        particles[i].dy *= 0.995f;
    }
}


void main_state_init(GLFWwindow *window, void *args, int width, int height)
{
    raster_width = width;
    raster_height = height;

    rafgl_raster_init(&raster, raster_width, raster_height);

    rafgl_spritesheet_init(&hero, "res/images/character.png", 1, 29);

    rafgl_texture_init(&texture);
}


float location = 0;
float selector = 0;

int hero_pos_x = RASTER_WIDTH / 2;
int hero_pos_y = RASTER_HEIGHT / 2;

int hero_speed = 150;
int hero_rotation = 0;

void main_state_update(GLFWwindow *window, float delta_time, rafgl_game_data_t *game_data, void *args)
{
    int i, gen = 5, radius = 10;
    float angle, speed;

    if(game_data->keys_down[RAFGL_KEY_SPACE])
    {
        for(i = 0; (i < MAX_PARTICLES) && gen; i++)
        {
            if(particles[i].life <= 0)
            {
                particles[i].life = 100 * randf() + 100;
                particles[i].x = hero_pos_x + 32;
                particles[i].y = hero_pos_y + 32;

                angle = hero_rotation * 12 * M_PI / 180;
                speed = ( 0.3f + 0.1 * randf()) * radius*10;
                particles[i].dx = cosf(angle) * speed;
                particles[i].dy = sinf(angle) * speed;
                gen--;

            }
        }
    }

    update_particles(delta_time);

    if(game_data->keys_down[RAFGL_KEY_RIGHT]) {
        hero_rotation++;
        hero_rotation %= 29;
    }
    else if(game_data->keys_down[RAFGL_KEY_LEFT]) {
        hero_rotation--;
        if(hero_rotation < 0)
            hero_rotation = 28;
    }

    if(game_data->keys_down[RAFGL_KEY_W])
    {
        hero_pos_y = hero_pos_y - hero_speed * delta_time;
    }
    else if(game_data->keys_down[RAFGL_KEY_S])
    {
        hero_pos_y = hero_pos_y + hero_speed * delta_time;
    }
    else if(game_data->keys_down[RAFGL_KEY_A])
    {
        hero_pos_x = hero_pos_x - hero_speed * delta_time;
    }
    else if(game_data->keys_down[RAFGL_KEY_D])
    {
        hero_pos_x = hero_pos_x + hero_speed * delta_time;
    }

    int x, y;

    float xn, yn;

    rafgl_pixel_rgb_t sampled, sampled2, resulting, resulting2;


    for(y = 0; y < raster_height; y++)
    {
        yn = 1.0f * y / raster_height;
        for(x = 0; x < raster_width; x++)
        {
            xn = 1.0f * x / raster_width;

            resulting = sampled;
            resulting2 = sampled2;

            resulting.rgba = rafgl_RGB(255, 255, 255);

            pixel_at_m(raster, x, y) = resulting;
        }
    }

    draw_particles(&raster);
    rafgl_raster_draw_spritesheet(&raster, &hero, 0, hero_rotation, hero_pos_x, hero_pos_y);


    if(game_data->keys_pressed[RAFGL_KEY_PRINT_SCREEN])
    {
        sprintf(save_file, "save%d.png", save_file_no++);
        rafgl_raster_save_to_png(&raster, save_file);
    }


}


void main_state_render(GLFWwindow *window, void *args)
{
    rafgl_texture_load_from_raster(&texture, &raster);
    rafgl_texture_show(&texture, 0);
}


void main_state_cleanup(GLFWwindow *window, void *args)
{
    rafgl_raster_cleanup(&raster);
    rafgl_texture_cleanup(&texture);

}
