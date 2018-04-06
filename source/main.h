#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include <SDL.h>
#include "SDL_ttf.h"
#include <CL/cl.h>

#define G_cst 6.67408E-11 // SI units

#define PI 3.141592653589793

#define MAX_SOURCE_SIZE (0x100000)

#define deg_to_rad 0.0174532925199
#define rad_to_deg 57.29577951308

#define AU 149597871. // [km/AU]
#define YR 365.25 // [Julian year]
#define M_SUN 1.98855E30 // [M_Sun->kg]
#define M_JUP 1.89813E27 // [M_Jup->kg]
#define PC 206264.81 // [AU/pc]


typedef struct{
	int n_threads;
	double cms[3];
	int focus_on_cms;
	int draw_background;
	int res_x, res_y;
	int fullscreen;
	int n_bodies;
	double timestep;
	int autoscale_on_startup;
	int timestep_counter;
	double x_rot, y_rot;
	double scale;
	double scale_min, scale_max, scale_step;
	double time;
	double time_end;
	double timestep_min;
	double timestep_max;
	double time_output;
	double output_interval;
	int paused;
	int finished;
	int resume;
	double eps_vel_thresh;
	double eps_pos_thresh;
	double m_tot;
	int mode_3D;
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
	int setup;

	// OpenCL
	int use_gpu;

	// SDL stuff
	SDL_Texture *icon_sun;
	SDL_Texture *icon_mercury;
	SDL_Texture *icon_venus;
	SDL_Texture *icon_earth;
	SDL_Texture *icon_mars;
	SDL_Texture *icon_jupiter;
	SDL_Texture *icon_saturn;
	SDL_Texture *icon_uranus;
	SDL_Texture *icon_neptune;
	SDL_Texture *icon_pluto;
	SDL_Texture *icon_moon;
} settings;


typedef struct{
	float vel[3];
	float pos[3];
	float vel_new[3];
	float pos_new[3];
	float mass;
} body_gpu;


typedef struct{
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
	double screen_pos[2];
	double screen_size, size;
	double z_projected;
	int plot_order;
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
void setup_solar_system(planet objects[], settings *sim_set);

// From num.c
void init_euler_gpu(cl_kernel *kernel, cl_device_id *device_id, cl_mem *body_mem_obj, cl_context *context, cl_program *program, settings *sim_set );
void init_velocity_verlet_gpu(cl_kernel *kernel_1, cl_kernel *kernel_2, cl_device_id *device_id, cl_mem *body_mem_obj, cl_context *context, cl_program *program, settings *sim_set );
void clear_numerics(planet objects[], settings *sim_set);
void euler_step_gpu(planet objects[], body_gpu bodies[], settings *sim_set, cl_command_queue *command_queue, cl_mem *body_mem_obj, cl_kernel *kernel);
void velocity_verlet_step_gpu(planet objects[], body_gpu bodies[], settings *sim_set, cl_command_queue *command_queue, cl_mem *body_mem_obj, cl_kernel *kernel_1, cl_kernel *kernel_2);
void velocity_verlet_step(planet objects[], settings *sim_set);
void forest_ruth_step(planet objects[], settings *sim_set);
void position_extended_forest_ruth_step(planet objects[], settings *sim_set);
void adaptive_rkn4_step(planet objects[], settings *sim_set);
void adaptive_rkn5_step(planet objects[], settings *sim_set);
void adaptive_rkn6_step(planet objects[], settings *sim_set);
void adaptive_rkn7_step(planet objects[], settings *sim_set);
void adaptive_rkn8_step(planet objects[], settings *sim_set);
void adaptive_rkn9_step(planet objects[], settings *sim_set);

// From output.c
void Generate_Output_File( planet objects[], settings *sim_set );
void generate_auto_output( SDL_Renderer *renderer, planet objects[], settings *sim_set);

// From sdl.c
void apply_autoscale(settings *sim_set, planet objects[]);
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

// From vsop87.h
double Mercury_X0 (double t);
double Mercury_X1 (double t);
double Mercury_X2 (double t);
double Mercury_X3 (double t);
double Mercury_X4 (double t);
double Mercury_X5 (double t);

double Mercury_Y0 (double t);
double Mercury_Y1 (double t);
double Mercury_Y2 (double t);
double Mercury_Y3 (double t);
double Mercury_Y4 (double t);
double Mercury_Y5 (double t);

double Mercury_Z0 (double t);
double Mercury_Z1 (double t);
double Mercury_Z2 (double t);
double Mercury_Z3 (double t);
double Mercury_Z4 (double t);
double Mercury_Z5 (double t);

double Venus_X0 (double t);
double Venus_X1 (double t);
double Venus_X2 (double t);
double Venus_X3 (double t);
double Venus_X4 (double t);
double Venus_X5 (double t);

double Venus_Y0 (double t);
double Venus_Y1 (double t);
double Venus_Y2 (double t);
double Venus_Y3 (double t);
double Venus_Y4 (double t);
double Venus_Y5 (double t);

double Venus_Z0 (double t);
double Venus_Z1 (double t);
double Venus_Z2 (double t);
double Venus_Z3 (double t);
double Venus_Z4 (double t);
double Venus_Z5 (double t);

double Earth_X0 (double t);
double Earth_X1 (double t);
double Earth_X2 (double t);
double Earth_X3 (double t);
double Earth_X4 (double t);
double Earth_X5 (double t);

double Earth_Y0 (double t);
double Earth_Y1 (double t);
double Earth_Y2 (double t);
double Earth_Y3 (double t);
double Earth_Y4 (double t);
double Earth_Y5 (double t);

double Earth_Z0 (double t);
double Earth_Z1 (double t);
double Earth_Z2 (double t);
double Earth_Z3 (double t);
double Earth_Z4 (double t);
double Earth_Z5 (double t);

double Mars_X0 (double t);
double Mars_X1 (double t);
double Mars_X2 (double t);
double Mars_X3 (double t);
double Mars_X4 (double t);
double Mars_X5 (double t);

double Mars_Y0 (double t);
double Mars_Y1 (double t);
double Mars_Y2 (double t);
double Mars_Y3 (double t);
double Mars_Y4 (double t);
double Mars_Y5 (double t);

double Mars_Z0 (double t);
double Mars_Z1 (double t);
double Mars_Z2 (double t);
double Mars_Z3 (double t);
double Mars_Z4 (double t);
double Mars_Z5 (double t);

double Jupiter_X0 (double t);
double Jupiter_X1 (double t);
double Jupiter_X2 (double t);
double Jupiter_X3 (double t);
double Jupiter_X4 (double t);
double Jupiter_X5 (double t);

double Jupiter_Y0 (double t);
double Jupiter_Y1 (double t);
double Jupiter_Y2 (double t);
double Jupiter_Y3 (double t);
double Jupiter_Y4 (double t);
double Jupiter_Y5 (double t);

double Jupiter_Z0 (double t);
double Jupiter_Z1 (double t);
double Jupiter_Z2 (double t);
double Jupiter_Z3 (double t);
double Jupiter_Z4 (double t);
double Jupiter_Z5 (double t);

double Saturn_X0 (double t);
double Saturn_X1 (double t);
double Saturn_X2 (double t);
double Saturn_X3 (double t);
double Saturn_X4 (double t);
double Saturn_X5 (double t);

double Saturn_Y0 (double t);
double Saturn_Y1 (double t);
double Saturn_Y2 (double t);
double Saturn_Y3 (double t);
double Saturn_Y4 (double t);
double Saturn_Y5 (double t);

double Saturn_Z0 (double t);
double Saturn_Z1 (double t);
double Saturn_Z2 (double t);
double Saturn_Z3 (double t);
double Saturn_Z4 (double t);
double Saturn_Z5 (double t);

double Uranus_X0 (double t);
double Uranus_X1 (double t);
double Uranus_X2 (double t);
double Uranus_X3 (double t);
double Uranus_X4 (double t);
double Uranus_X5 (double t);

double Uranus_Y0 (double t);
double Uranus_Y1 (double t);
double Uranus_Y2 (double t);
double Uranus_Y3 (double t);
double Uranus_Y4 (double t);
double Uranus_Y5 (double t);

double Uranus_Z0 (double t);
double Uranus_Z1 (double t);
double Uranus_Z2 (double t);
double Uranus_Z3 (double t);
double Uranus_Z4 (double t);
double Uranus_Z5 (double t);

double Neptune_X0 (double t);
double Neptune_X1 (double t);
double Neptune_X2 (double t);
double Neptune_X3 (double t);
double Neptune_X4 (double t);
double Neptune_X5 (double t);

double Neptune_Y0 (double t);
double Neptune_Y1 (double t);
double Neptune_Y2 (double t);
double Neptune_Y3 (double t);
double Neptune_Y4 (double t);
double Neptune_Y5 (double t);

double Neptune_Z0 (double t);
double Neptune_Z1 (double t);
double Neptune_Z2 (double t);
double Neptune_Z3 (double t);
double Neptune_Z4 (double t);
double Neptune_Z5 (double t);

double Sun_X0 (double t);
double Sun_X1 (double t);
double Sun_X2 (double t);
double Sun_X3 (double t);
double Sun_X4 (double t);
double Sun_X5 (double t);

double Sun_Y0 (double t);
double Sun_Y1 (double t);
double Sun_Y2 (double t);
double Sun_Y3 (double t);
double Sun_Y4 (double t);
double Sun_Y5 (double t);

double Sun_Z0 (double t);
double Sun_Z1 (double t);
double Sun_Z2 (double t);
double Sun_Z3 (double t);
double Sun_Z4 (double t);
double Sun_Z5 (double t);

double Mercury_X (double t);
double Mercury_Y (double t);
double Mercury_Z (double t);
double Venus_X (double t);
double Venus_Y (double t);
double Venus_Z (double t);
double Earth_X (double t);
double Earth_Y (double t);
double Earth_Z (double t);
double Mars_X (double t);
double Mars_Y (double t);
double Mars_Z (double t);
double Jupiter_X (double t);
double Jupiter_Y (double t);
double Jupiter_Z (double t);
double Saturn_X (double t);
double Saturn_Y (double t);
double Saturn_Z (double t);
double Uranus_X (double t);
double Uranus_Y (double t);
double Uranus_Z (double t);
double Neptune_X (double t);
double Neptune_Y (double t);
double Neptune_Z (double t);
double Sun_X (double t);
double Sun_Y (double t);
double Sun_Z (double t);
