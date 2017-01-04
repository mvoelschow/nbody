#include "SDL.h"
#include "SDL_ttf.h"

#define G_cst 6.67408E-11 // SI units

#define PI 3.1415962

#define deg_to_rad 0.0174532925199
#define rad_to_deg 57.29577951308

#define AU 149597871. // [AU->km]
#define YR 365.25 // [Julian year]
#define M_SUN 1.98855E30 // [M_Sun->kg]
#define PC 206264.81 // [AU->pc]


typedef struct
{
int draw_background;
int res_x, res_y;
int fullscreen;
int n_bodies;
double timestep;
int auto_timestep;
int timestep_counter;
int timestep_lock;
double timestep_factor;
double x_rot, y_rot;
double scale;
double scale_min, scale_max, scale_step;
double time;
double time_end;
double timestep_max;
double time_output;
double output_interval;
int paused;
int finished;
double eps_vel_thresh;
double eps_pos_thresh;
double center_screen_x, center_screen_y;
double start_x, start_y;
double end_x, end_y;
double size_factor;
double cross_size;
double cross_thresh;
double select_box_size;
int selected_object;
int auto_screenshot;
int auto_textfile;
int screenshot_counter;
int screenshot_trigger;
int output_counter;
double zoom_factor;
int icon_mode;
double icon_size;
double icon_size_max;
} settings;




typedef struct
{
char label[8];
double pos[3];
double vel[3];
double pos_new[3];
double vel_new[3];
double acc[3];
double eps_pos;
double eps_vel;
double screen_pos[2];
double alpha, delta;
double screen_size, size;
double z_projected;
int plot_order;
double mass;
double select_box_x[2];
double select_box_y[2];
int ident;
SDL_Texture *icon;
} planet;



// Prototypes

// From hud.c
void get_planar_screen_coordinates(planet objects[], settings *sim_set);
void render_object_info_box(SDL_Renderer *renderer, planet objects[], TTF_Font *fntCourier, settings *sim_set);
void render_paused(SDL_Renderer *renderer, TTF_Font *fntCourier, settings *sim_set);
void render_scale_setting(SDL_Renderer *renderer, TTF_Font *fntCourier, settings *sim_set);
void render_time_information(SDL_Renderer *renderer, TTF_Font *fntCourier, settings *sim_set);
void render_timestep_setting(SDL_Renderer *renderer, TTF_Font *fntCourier, settings *sim_set);
void zoom_out(settings *sim_set);
void zoom_in(settings *sim_set);
void zoom_out_at_mouse_position(settings *sim_set);
void zoom_in_at_mouse_position(settings *sim_set);

// From num.c
void adaptive_rkf_step(planet objects[], settings *sim_set);

// From output.c
void Generate_Output_File( planet objects[], settings *sim_set );

// From sdl.c
void create_screenshot(SDL_Renderer *renderer, settings *sim_set);
void create_auto_screenshot(SDL_Renderer *renderer, settings *sim_set);
void Draw_Background(SDL_Renderer *renderer, SDL_Texture *background, settings *sim_set);
void load_object_textures(SDL_Renderer *renderer, planet objects[], settings *sim_set);
int processEvents(SDL_Window *window, settings *sim_set, planet objects[]);
void render_all_bodies(SDL_Renderer *renderer, planet objects[], settings *sim_set, SDL_Texture *cross);
void render_all_bodies_3D(SDL_Renderer *renderer, planet objects[], settings *sim_set, SDL_Texture *cross);
void Render_Screen(SDL_Renderer *renderer);

// From setup.c
void init_settings(settings *sim_set);
void init_bodies(planet objects[], settings *sim_set);