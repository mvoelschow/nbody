#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "main.h"




// ***********************************************************
//   Initialize settings
// ***********************************************************
void init_settings(settings *sim_set){
// *******************************
// Benchmark mode: No output, no checks, maximum CPU stress
// *******************************
sim_set->benchmark_mode = 0;


// *******************************
// Total number of particles to simulate
// *******************************
sim_set->n_bodies = 200;


// *******************************
// Numerics
// *******************************

// Integrator order {5}
sim_set->integrator = 6;

// Maximum relative error per step for position and velocity
sim_set->eps_vel_thresh = 1.E-16;
sim_set->eps_pos_thresh = 1.E-16;

// Maximum relative deviation from the system's initial energy budget
sim_set->check_delta_E = 1;
sim_set->delta_E_thresh = 0.01;

// Timestep smoothing. Improves performance at the cost of some precision loss
sim_set->timestep_smoothing = 1.;

// Number of threads to use
sim_set->n_threads = 1;


// *******************************
// Time settings
// *******************************

// Largest simulation timestep [days]
sim_set->timestep_max = 100.;

// Time at which the simulation ends [days]
sim_set->time_end = 1.E2*YR;


// *******************************
// Camera and visual settings
// *******************************
// Resolution
sim_set->res_x = 800;
sim_set->res_y = 800;

// Fullscreen mode on/off
sim_set->fullscreen = 0;

// Scale setting on startup [AU]
sim_set->scale = 16.;

// Lowest scale setting [AU]
sim_set->scale_min = 0.1;

// Largest scale setting [AU]
sim_set->scale_max = 100.;

// Draw background
sim_set->draw_background = 1;

// VSYNC on/off (framerate cap at 60 FPS)
sim_set->vsync = 0;

// Show every calculated timestep
sim_set->interactive_mode = 1;

// Focus on the system's center of mass
sim_set->focus_on_cms = 1;


// *******************************
// Data output
// *******************************
// Time interval for automatic data output
sim_set->output_interval = 0.1*YR;

// Automatically output screenshots
sim_set->auto_screenshot = 0;

// Automatically output text file with kinetic parameters of all bodies
sim_set->auto_textfile = 0;

// Output evolution of the system's total energy
sim_set->output_delta_E = 0;


// *******************************
// Some technical parameters that (normally) don't need to be altered
// *******************************
sim_set->center_screen_x = 0.5*sim_set->res_x;
sim_set->center_screen_y = 0.5*sim_set->res_y;
sim_set->timestep = 0.1;
sim_set->select_box_size = 40;
sim_set->selected_object = -1;
sim_set->scale_step = 1.05;
sim_set->paused = 1;
sim_set->auto_timestep = 1;
sim_set->finished = 0;
sim_set->screenshot_counter = 0;
sim_set->screenshot_trigger = 1;
sim_set->auto_screenshot_counter = 0;
sim_set->auto_file_counter = 0;
sim_set->time_output = 0.;
sim_set->timestep_counter = 0;
sim_set->x_rot = 0.;
sim_set->y_rot = 0.;
      
}




// ***********************************************************
// Init bodies
// ***********************************************************
void init_bodies(planet objects[], settings *sim_set){

setup_asteroid_belt_and_planet(objects, sim_set);
//setup_planetesimals(objects, sim_set);
//setup_planetary_system(objects, sim_set);
//setup_stellar_filament(objects, sim_set);
//setup_stellar_sphere(objects, sim_set);

}
