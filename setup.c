#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "main.h"


// ***********************************************************
//   Auxilary functions
// ***********************************************************
double randomDouble(){
      double r = (double)rand()/(double)RAND_MAX;
      return r;
}




// ***********************************************************
//   Initialize settings
// ***********************************************************
void init_settings(settings *sim_set){

// *******************************
// Total number of particles to simulate
// *******************************
sim_set->n_bodies = 500;


// *******************************
// Time settings
// *******************************
// Maximum relative error per step for position and velocity
sim_set->eps_vel_thresh = 1.E-11;
sim_set->eps_pos_thresh = 1.E-12;

// Largest simulation timestep [days]
// Should be a power of 2
sim_set->timestep_max = 100.;

// Time at which the simulation ends [days]
sim_set->time_end = 1.E2*YR;


// *******************************
// Camera and visual settings
// *******************************
// Resolution
sim_set->res_x = 1200;
sim_set->res_y = 900;

// Fullscreen mode on/off
sim_set->fullscreen = 0;

// Scale setting on startup [AU]
sim_set->scale = 20.;

// Lowest scale setting [AU]
sim_set->scale_min = 0.1;

// Largest scale setting [AU]
sim_set->scale_max = 100.;

// Size of body icons [pixels]
sim_set->icon_size = 4;

// Maximum icon size in pseudo 3D rendering mode
sim_set->icon_size_max = 20;

// Draw background
sim_set->draw_background = 1;


// *******************************
// Data output
// *******************************
// Time interval for automatic data output
sim_set->output_interval = 0.1*YR;

// Automatically output screenshots
sim_set->auto_screenshot = 1;

// Automatically output text file with kinetic parameters of all bodies
sim_set->auto_textfile = 0;


// *******************************
// Some technical parameters that (normally) don't need to be altered
// *******************************
sim_set->center_screen_x = 0.5*sim_set->res_x;
sim_set->center_screen_y = 0.5*sim_set->res_y;
sim_set->timestep = 0.1;
sim_set->select_box_size = 40;
sim_set->selected_object = -1;
sim_set->icon_mode = 1;
sim_set->scale_step = 1.05;
sim_set->paused = 1;
sim_set->auto_timestep = 1;
sim_set->finished = 0;
sim_set->screenshot_counter = 0;
sim_set->screenshot_trigger = 1;
sim_set->output_counter = 0;
sim_set->time_output = 0.;
sim_set->timestep_counter = 0;
sim_set->cross_size = 2*sim_set->icon_size;
sim_set->x_rot = 0.;
sim_set->y_rot = 0.;
sim_set->timestep_lock = 0;
sim_set->timestep_factor = 1.1;

}




// ***********************************************************
// Init bodies
// ***********************************************************
// N-1 asteroids and a massive planet orbiting the sun
void init_bodies(planet objects[], settings *sim_set){
int i;
double dist, r, phi;
double v_circ;
double p_orb, r_planet;


// Sun
objects[0].ident = 0;
objects[0].mass = M_SUN;

// Massive planet
objects[1].ident = 1;
objects[1].mass = 1.E-2*M_SUN;

r_planet = 6.; // [au]

// Place sun and planet on a proper orbit around their barycentre
objects[0].pos[0] = -r_planet * objects[1].mass / objects[0].mass;
objects[0].pos[1] = 0.;
objects[0].pos[2] = 0.;

objects[1].pos[0] = r_planet - r_planet * objects[1].mass/ objects[0].mass;
objects[1].pos[1] = 0;
objects[1].pos[2] = 0.;

p_orb = sqrt( 4.*PI*PI * pow(r_planet*AU*1.E3,3) / (G_cst * (objects[0].mass+objects[1].mass) ) );

// Objects revolve clockwise. Sun starts in positve y-direction, planet in negative y-direction
objects[0].vel[0] = 0.;
objects[0].vel[1] = 2.*PI*objects[0].pos[0]*AU / p_orb;
objects[0].vel[2] = 0.;


objects[1].vel[0] = 0.;
objects[1].vel[1] = 2.*PI*objects[1].pos[0]*AU / p_orb;
objects[1].vel[2] = 0.;



// Planetesimals between 1 and 10 au
for(i=2; i<sim_set->n_bodies; i=i+1){

// Set body mass [kg]
objects[i].mass = 1.E19*(99.*randomDouble()+1.);

r = 2+3.*randomDouble();
phi = randomDouble()*2.*PI;

// Set body position [AU]
// x component
objects[i].pos[0] = r*cos(phi);
// y component
objects[i].pos[1] = r*sin(phi);
// z component
objects[i].pos[2] = 0.1*(2.*randomDouble()-1.);

dist = sqrt(objects[i].pos[0]*objects[i].pos[0]+objects[i].pos[1]*objects[i].pos[1]+objects[i].pos[2]*objects[i].pos[2]);

v_circ = 1.E-3*sqrt( G_cst * objects[0].mass / (dist*AU*1.E3) );

// Set body velocity [km/s]
// x component
objects[i].vel[0] = -v_circ*sin(phi);
// y component
objects[i].vel[1] = v_circ*cos(phi);
// z component
objects[i].vel[2] = 0.;


// Set identifier [DO NOT CHANGE]
objects[i].ident = i;

}

}

/*
void init_bodies(planet objects[], settings *sim_set){
int i;
double r, r_max, phi;

r_max = 3.*PC;

for(i=0; i<sim_set->n_bodies; i=i+1){

// Set body mass [kg]
objects[i].mass = 0.1*M_SUN*(99.*randomDouble()+1.);

r = randomDouble()*r_max;
phi = randomDouble()*2.*PI;

// Set body position [AU]
// x component
objects[i].pos[0] = r*cos(phi);
// y component
objects[i].pos[1] = r*sin(phi);
// z component
objects[i].pos[2] = 1.*PC*(2.*randomDouble()-1.);

// Set body velocity [km/s]
// x component
objects[i].vel[0] = 0.01*(2.*randomDouble()-1.);
// y component
objects[i].vel[1] = 0.01*(2.*randomDouble()-1.);
// z component
objects[i].vel[2] = 0.01*(2.*randomDouble()-1.);


// Set identifier [DO NOT CHANGE]
objects[i].ident = i;

}

}

*/



// ***********************************************************
// Template for two filaments of stars
// ***********************************************************
/*
void init_bodies(planet objects[], settings *sim_set){
int i;

for(i=0; i<sim_set->n_bodies/2; i=i+1){

// Set body mass [kg]
objects[i].mass = 0.1*M_SUN*(99.*randomDouble()+1.);

// Set body position [AU]
// x component
objects[i].pos[0] = 7.*PC*(2.*randomDouble()-1.);
// y component
objects[i].pos[1] = 1.5*PC+0.7*PC*(2.*randomDouble()-1.);
// z component
objects[i].pos[2] = 0.7*PC*(2.*randomDouble()-1.);

// Set body velocity [km/s]
// x component
objects[i].vel[0] = 0.01*(2.*randomDouble()-1.);
// y component
objects[i].vel[1] = 0.01*(2.*randomDouble()-1.);
// z component
objects[i].vel[2] = 0.01*(2.*randomDouble()-1.);


// Set identifier [DO NOT CHANGE]
objects[i].ident = i;

}

for(i=sim_set->n_bodies/2; i<sim_set->n_bodies; i=i+1){

// Set body mass [kg]
objects[i].mass = 0.1*M_SUN*(99.*randomDouble()+1.);

// Set body position [AU]
// x component
objects[i].pos[0] = 7.*PC*(2.*randomDouble()-1.);
// y component
objects[i].pos[1] = 0.7*PC*(2.*randomDouble()-1.) - 1.5*PC;
// z component
objects[i].pos[2] = 0.7*PC*(2.*randomDouble()-1.);

// Set body velocity [km/s]
// x component
objects[i].vel[0] = 0.01*(2.*randomDouble()-1.);
// y component
objects[i].vel[1] = 0.01*(2.*randomDouble()-1.);
// z component
objects[i].vel[2] = 0.01*(2.*randomDouble()-1.);


// Set identifier [DO NOT CHANGE]
objects[i].ident = i;

}

}
*/





// ***********************************************************
// Template for a single filament of stars
// ***********************************************************
/*
void init_bodies(planet objects[], settings *sim_set){
int i;

for(i=0; i<sim_set->n_bodies; i=i+1){

// Set body mass [kg]
objects[i].mass = 0.1*M_SUN*(99.*randomDouble()+1.);

// Set body position [AU]
// x component
objects[i].pos[0] = 7.*PC*(2.*randomDouble()-1.);
// y component
objects[i].pos[1] = 0.7*PC*(2.*randomDouble()-1.);
// z component
objects[i].pos[2] = 0.7*PC*(2.*randomDouble()-1.);

// Set body velocity [km/s]
// x component
objects[i].vel[0] = 0.01*(2.*randomDouble()-1.);
// y component
objects[i].vel[1] = 0.01*(2.*randomDouble()-1.);
// z component
objects[i].vel[2] = 0.01*(2.*randomDouble()-1.);


// Set identifier [DO NOT CHANGE]
objects[i].ident = i;

}

}
*/
