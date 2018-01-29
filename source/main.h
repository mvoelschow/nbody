#include <SDL.h>
#include "SDL_ttf.h"

#define G_cst 6.67408E-11 // SI units

#define PI 3.141592653589793

#define deg_to_rad 0.0174532925199
#define rad_to_deg 57.29577951308

#define AU 149597871. // [km/AU]
#define YR 365.25 // [Julian year]
#define M_SUN 1.98855E30 // [M_Sun->kg]
#define M_JUP 1.89813E27 // [M_Jup->kg]
#define PC 206264.81 // [AU/pc]


typedef struct
{
int n_threads;
double cms[3];
int focus_on_cms;
int draw_background;
int res_x, res_y;
int fullscreen;
int n_bodies;
double timestep;
int auto_timestep;
int timestep_counter;
double x_rot, y_rot;
double scale;
double scale_min, scale_max, scale_step;
double time;
double time_end;
double timestep_max;
double timestep_smoothing;
double time_output;
double output_interval;
int paused;
int finished;
int resume;
double eps_vel_thresh;
double eps_pos_thresh;
double m_tot;
double E_tot_0;
double E_tot;
double delta_E_thresh;
int mode_3D;
int output_delta_E;
double center_screen_x, center_screen_y;
double start_x, start_y;
double end_x, end_y;
double size_factor;
double select_box_size;
int selected_object;
int auto_screenshot;
int auto_textfile;
int screenshot_counter;
int screenshot_trigger;
int auto_screenshot_counter;
int auto_file_counter;
double zoom_factor;
int vsync;
int integrator;
int interactive_mode;
int benchmark_mode;
double icon_size_max;
int data_file;

// Barns-Hut stuff
int treesize;

// SDL stuff
SDL_Texture *icon_sun;
SDL_Texture *icon_mercury;
SDL_Texture *icon_earth;
SDL_Texture *icon_mars;
SDL_Texture *icon_jupiter;
SDL_Texture *icon_neptune;
SDL_Texture *icon_pluto;
SDL_Texture *icon_moon;
} settings;




typedef struct
{
double pos[3];
double vel[3];
double mass;
// Numerical stuff
double pos_new[3];
double vel_new[3];
double cifi[3];
double cdotifi[3];
double chatifi[3];
double d[3];
double pos_eps[3];
double fe[3];
double fe_min;
double dt_new_guess;

double acc[3];
double screen_pos[2];
double screen_size, size;
double z_projected;
int plot_order;
double select_box_x[2];
double select_box_y[2];
int ident;
int icon_num;
double icon_size;
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
void render_hud(SDL_Renderer *renderer, TTF_Font *fntCourier, settings *sim_set, planet objects[]);

// From examples.c
void setup_asteroid_belt_and_planet(planet objects[], settings *sim_set);
void setup_planetesimals(planet objects[], settings *sim_set);
void setup_planetary_system(planet objects[], settings *sim_set);
void setup_stellar_filament(planet objects[], settings *sim_set);
void setup_stellar_sphere(planet objects[], settings *sim_set);
void setup_benchmark(planet objects[], settings *sim_set);

// From num.c
void clear_numerics(planet objects[], settings *sim_set);
double get_total_energy(planet objects[], settings *sim_set);
void adaptive_rkn4_step(planet objects[], settings *sim_set);
void adaptive_rkn5_step(planet objects[], settings *sim_set);
void adaptive_rkn6_step(planet objects[], settings *sim_set);
void adaptive_rkn7_step(planet objects[], settings *sim_set);
void adaptive_rkn8_step(planet objects[], settings *sim_set);

// From output.c
void Generate_Output_File( planet objects[], settings *sim_set );
void generate_auto_output( SDL_Renderer *renderer, planet objects[], settings *sim_set);
void Write_Numerical_Stats(settings *sim_set);

// From sdl.c
void center_at_cms(settings *sim_set, planet objects[]);
void create_screenshot(SDL_Renderer *renderer, settings *sim_set);
void create_auto_screenshot(SDL_Renderer *renderer, settings *sim_set);
void Draw_Background(SDL_Renderer *renderer, SDL_Texture *background, settings *sim_set);
void load_object_textures(SDL_Renderer *renderer, settings *sim_set);
int processEvents(settings *sim_set, planet objects[]);
void render_all_bodies(SDL_Renderer *renderer, planet objects[], settings *sim_set);
void render_all_bodies_2D(SDL_Renderer *renderer, planet objects[], settings *sim_set);
void render_all_bodies_3D(SDL_Renderer *renderer, planet objects[], settings *sim_set);
void Render_Screen(SDL_Renderer *renderer);

// From setup.c
void init_settings(settings *sim_set);
void init_bodies(planet objects[], settings *sim_set);
void init_benchmark(settings *sim_set);
int get_file_lines(char *text);
void output_objects(planet objects[], settings *sim_set);
void load_settings_file(settings *sim_set);
void init_technical_parameters(settings *sim_set);
void read_input_file(planet objects[], settings *sim_set);
