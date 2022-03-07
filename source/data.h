#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

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
	int plot_order;
	int ident;
	int icon_num;
	double icon_size;
} planet;