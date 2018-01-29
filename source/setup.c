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

// Number of threads to use
sim_set->n_threads = 2;

// Read object data from input file
sim_set->data_file = 0;


// *******************************
// Total number of particles to simulate
// *******************************
sim_set->n_bodies = 200;


// *******************************
// Numerics
// *******************************

// Integrator order {4,5,6}
sim_set->integrator = 6;

// Maximum relative error per step for position and velocity
sim_set->eps_vel_thresh = 1.E-16;
sim_set->eps_pos_thresh = 1.E-16;



// *******************************
// Time settings
// *******************************

// Largest simulation timestep [days]
sim_set->timestep_max = 100.;

// Time at which the simulation ends [days]
sim_set->time_end = 1.E3*YR;


// *******************************
// Camera and visual settings
// *******************************
// Resolution
sim_set->res_x = 800;
sim_set->res_y = 800;

// Fullscreen mode on/off
sim_set->fullscreen = 0;

sim_set->mode_3D = 1;

// Scale setting on startup [AU]
sim_set->scale = 16.*AU;

// Lowest scale setting [AU]
sim_set->scale_min = 0.1*AU;

// Largest scale setting [AU]
sim_set->scale_max = 100.*AU;

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
sim_set->auto_screenshot = 1;

// Automatically output text file with kinetic parameters of all bodies
sim_set->auto_textfile = 0;

}




void init_technical_parameters(settings *sim_set){
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
sim_set->resume = 0;
sim_set->screenshot_counter = 0;
sim_set->screenshot_trigger = 1;
sim_set->auto_screenshot_counter = 0;
sim_set->auto_file_counter = 0;
sim_set->time_output = 0.;
sim_set->timestep_counter = 0;
sim_set->x_rot = 0.;
sim_set->y_rot = 0.;
sim_set->icon_size_max = 30.;
sim_set->timestep_smoothing = 1.; 
sim_set->output_delta_E = 0;     
}





// ***********************************************************
// Init bodies
// ***********************************************************
void init_bodies(planet objects[], settings *sim_set){

//setup_asteroid_belt_and_planet(objects, sim_set);
//setup_planetesimals(objects, sim_set);
//setup_planetary_system(objects, sim_set);
//setup_stellar_filament(objects, sim_set);
setup_stellar_sphere(objects, sim_set);
//setup_benchmark(objects, sim_set);

}






// ***********************************************************
//   Initialize benchmark
// ***********************************************************
void init_benchmark(settings *sim_set){

sim_set->benchmark_mode = 1;
sim_set->n_bodies = 8000;
sim_set->eps_vel_thresh = 1.E-16;
sim_set->eps_pos_thresh = 1.E-16;
sim_set->timestep_smoothing = 1.;
sim_set->timestep_max = 100.;
sim_set->time_end = 2.E4*YR;
sim_set->res_x = 800;
sim_set->res_y = 800;
sim_set->fullscreen = 0;
sim_set->scale = 16.;
sim_set->scale_min = 0.1;
sim_set->scale_max = 100.;
sim_set->draw_background = 0;
sim_set->vsync = 0;
sim_set->interactive_mode = 0;
sim_set->focus_on_cms = 1;
sim_set->output_interval = 1.E2*YR;
sim_set->auto_screenshot = 0;
sim_set->auto_textfile = 0;
sim_set->output_delta_E = 0;
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



int get_file_lines(char *text){
FILE *fpr;
int lines=0;
int ch=0;

fpr = fopen(text,"r");    

if(!fpr){

	lines = 0;

}
else{

	while(!feof(fpr)){
  		ch = fgetc(fpr);
  		if(ch == '\n'){
    			lines++;
  		}
	}

	fclose(fpr);

}

return lines;

}





void output_objects(planet objects[], settings *sim_set){
FILE *fpr;
char* fname = "objects.dat";
int i;

fpr = fopen(fname,"w"); 

fprintf(fpr, "%s\n", "msun");
fprintf(fpr, "%s\n", "au");
fprintf(fpr, "%s\n", "kms");
fprintf(fpr, "%s\n", "");

for(i=0; i<sim_set->n_bodies; i++){

	fprintf(fpr,"%le %le %le %le %le %le %le %d %lf \n",objects[i].mass/M_SUN, objects[i].pos[0], objects[i].pos[1], objects[i].pos[2], objects[i].vel[0], objects[i].vel[1], objects[i].vel[2], objects[i].icon_num, objects[i].icon_size);

}

}




void load_settings_file(settings *sim_set){
char* fname = "settings.dat";
char unit[8];
char dummy[32];

FILE *fpr;

fpr = fopen(fname,"r");    

if(!fpr){
	printf("Error while loading %s. \n",fname);
	exit(1);
}

printf("\n Reading simulation settings from settings.dat file:");

// 1. line: benchmark mode
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->benchmark_mode) == 2 ){
	printf("\n %s = %d", dummy, sim_set->benchmark_mode);
}
else{
	printf("\n Error at line 1.");
	exit(1);
}

// 2. line: number of threads
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->n_threads) == 2 ){
	printf("\n %s = %d", dummy, sim_set->n_threads);
}
else{
	printf("\n Error at line 2.");
	exit(1);
}

// 3. line: read from data file
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->data_file) == 2 ){
	printf("\n %s = %d", dummy, sim_set->data_file);
}
else{
	printf("\n Error at line 3.");
	exit(1);
}

// 4. line: number of bodies
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->n_bodies) == 2 ){
	printf("\n %s = %d", dummy, sim_set->n_bodies);
}
else{
	printf("\n Error at line 4.");
	exit(1);
}

// 5. line: integrator order
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->integrator) == 2 ){
	printf("\n %s = %d", dummy, sim_set->integrator);
}
else{
	printf("\n Error at line 5.");
	exit(1);
}

// 6. line: velocity error threshold
if ( fscanf(fpr, "%s %lf \n", dummy, &sim_set->eps_vel_thresh) == 2 ){
	printf("\n %s = %le", dummy, sim_set->eps_vel_thresh);
}
else{
	printf("\n Error at line 6.");
	exit(1);
}

// 7. line: position error threshold
if ( fscanf(fpr, "%s %lf \n", dummy, &sim_set->eps_pos_thresh) == 2 ){
	printf("\n %s = %le", dummy, sim_set->eps_pos_thresh);
}
else{
	printf("\n Error at line 7.");
	exit(1);
}

// 8. line: timestep normalization
if ( fscanf(fpr, "%s %lf %s \n", dummy, &sim_set->timestep_max, unit) == 3 ){
	printf("\n %s = %le", dummy, sim_set->eps_pos_thresh);
	if ( strcmp(unit,"days") == 0 ){
		sim_set->timestep_max = sim_set->timestep_max;
	}
	if ( strcmp(unit,"yrs") == 0 ){
		sim_set->timestep_max = sim_set->timestep_max * YR;
	}
	if ( strcmp(unit,"Myrs") == 0 ){
		sim_set->timestep_max = sim_set->timestep_max * YR *1.e6;
	}
}
else{
	printf("\n Error at line 8.");
	exit(1);
}

// 9. line: Simulation time
if ( fscanf(fpr, "%s %lf %s \n", dummy, &sim_set->time_end, unit) == 3 ){
	printf("\n %s = %lf %s", dummy, sim_set->time_end, unit);

	if ( strcmp(unit,"days") == 0 ){
		sim_set->time_end = sim_set->time_end;
	}
	if ( strcmp(unit,"yrs") == 0 ){
		sim_set->time_end = sim_set->time_end * YR;
	}
	if ( strcmp(unit,"Myrs") == 0 ){
		sim_set->time_end = sim_set->time_end * YR * 1.e6;
	}
}
else{
	printf("\n Error at line 9.");
	exit(1);
}

// 10. line: x resolution
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->res_x) == 2 ){
	printf("\n %s = %d", dummy, sim_set->res_x);
}
else{
	printf("\n Error at line 10.");
	exit(1);
}

// 11. line: y resolution
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->res_y) == 2){
	printf("\n %s = %d", dummy, sim_set->res_y);
}
else{
	printf("\n Error at line 11.");
	exit(1);
}

// 12. line: fullscreen mode
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->fullscreen) == 2){
	printf("\n %s = %d", dummy, sim_set->fullscreen);
}
else{
	printf("\n Error at line 12.");
	exit(1);
}

// 13. line: mode 3D
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->mode_3D) == 2 ){
	printf("\n %s = %d", dummy, sim_set->mode_3D);
}
else{
	printf("\n Error at line 13.");
	exit(1);
}

// 14. line: scale setting on startup
if ( fscanf(fpr, "%s %lf %s \n", dummy, &sim_set->scale, unit) == 3){
	printf("\n %s = %lf %s", dummy, sim_set->scale, unit);
	if ( strcmp(unit,"pc") == 0 ){
		sim_set->scale = sim_set->scale * PC;
	}
	if ( strcmp(unit,"au") == 0 ){
		sim_set->scale = sim_set->scale;
	}
}
else{
	printf("\n Error at line 14.");
	exit(1);
}

// 15. line: min scale setting
if ( fscanf(fpr, "%s %lf %s \n", dummy, &sim_set->scale_min, unit) == 3){
	printf("\n %s = %lf %s", dummy, sim_set->scale_min, unit);

	if ( strcmp(unit,"pc") == 0 ){
		sim_set->scale_min = sim_set->scale_min * PC;
	}
	if ( strcmp(unit,"au") == 0 ){
		sim_set->scale_min = sim_set->scale_min;
	}
}
else{
	printf("\n Error at line 15.");
	exit(1);
}

// 16. line: max scale setting
if ( fscanf(fpr, "%s %lf %s \n", dummy, &sim_set->scale_max, unit) == 3){
	printf("\n %s = %lf %s", dummy, sim_set->scale_max, unit);

	if ( strcmp(unit,"pc") == 0 ){
		sim_set->scale_max = sim_set->scale_max * PC;
	}
	if ( strcmp(unit,"au") == 0 ){
		sim_set->scale_max = sim_set->scale_max;
	}
}
else{
	printf("\n Error at line 16.");
	exit(1);
}

// 17. line: draw background
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->draw_background) == 2){
	printf("\n %s = %d", dummy, sim_set->draw_background);
}
else{
	printf("\n Error at line 17.");
	exit(1);
}

// 18. line: vsync
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->vsync) == 2 ){
	printf("\n %s = %d", dummy, sim_set->vsync);
}
else{
	printf("\n Error at line 18.");
	exit(1);
}

// 19. line: interactive mode
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->interactive_mode) == 2 ){
	printf("\n %s = %d", dummy, sim_set->interactive_mode);
}
else{
	printf("\n Error at line 19.");
	exit(1);
}

// 20. line: center of mass-focus
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->focus_on_cms) == 2 ){
	printf("\n %s = %d", dummy, sim_set->focus_on_cms);
}
else{
	printf("\n Error at line 20.");
	exit(1);
}

// 21. line: output interval
if ( fscanf(fpr, "%s %lf %s \n", dummy, &sim_set->output_interval, unit) == 3 ){
	printf("\n %s = %lf %s", dummy, sim_set->output_interval, unit);

	if ( strcmp(unit,"days") == 0 ){
		sim_set->output_interval = sim_set->output_interval;
	}
	if ( strcmp(unit,"yrs") == 0 ){
		sim_set->output_interval = sim_set->output_interval * YR;
	}
	if ( strcmp(unit,"Myrs") == 0 ){
		sim_set->output_interval = sim_set->output_interval * YR * 1.e6;
	}
}
else{
	printf("\n Error at line 21.");
	exit(1);
}

// 22. line: automatic screenshot
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->auto_screenshot) == 2 ){
	printf("\n %s = %d", dummy, sim_set->auto_screenshot);
}
else{
	printf("\n Error at line 22.");
	exit(1);
}

// 23. line: automatic textfile
if ( fscanf(fpr, "%s %d \n", dummy, &sim_set->auto_textfile) == 2 ){
	printf("\n %s = %d", dummy, sim_set->auto_textfile);
}
else{
	printf("\n Error at line 23.");
	exit(1);
}

printf("\n");

}



void read_input_file(planet objects[], settings *sim_set){
char* fname = "input.dat";
char unit[8];
char header[1024];
int i, j;
FILE *fpr;
double mass_conv, dist_conv, vel_conv;

fpr = fopen(fname,"r");    

if(!fpr){
	printf("Error while loading %s. \n",fname);
	exit(1);
}

printf("\n Reading object data from input.dat file:");

// Read first line of input file
if ( fscanf(fpr,"%[^\n]\n", unit) == 1 ){

	if ( strcmp(unit, "msun") == 0 ){
		printf("\n Mass unit is msun.");
		mass_conv = M_SUN;
	}
	else if ( strcmp(unit, "mjup") == 0 ){
		printf("\n Mass unit is mjup.");
		mass_conv = M_JUP;
	}
	else{
		printf("\n Unknown unit at line 1.\n");
		exit(1);
	}

}
else{
	printf("\n Input error at line 1.\n");
	exit(1);
}

// Read second line of input file
if ( fscanf(fpr,"%[^\n]\n", unit) == 1 ){

	if ( strcmp(unit, "pc") == 0 ){
		printf("\n Coordinates given in pc.");
		dist_conv = PC;
	}
	else if ( strcmp(unit, "au") == 0 ){
		printf("\n Coordinates given in au.");
		dist_conv = 1.;
	}
	else{
		printf("Unknown unit at line 2.\n");
		exit(1);
	}
}
else{
	printf("\n Input error at line 2.\n");
	exit(1);
}

// Read third line of input file and skip the following blank line
if ( fscanf(fpr,"%[^\n]\n", unit) == 1 ){

	if ( strcmp(unit, "kms") == 0 ){
		printf("\n Velocities are km/s.");
		vel_conv = 1.;
	}
	else if ( strcmp(unit, "ms") == 0 ){
		printf("\n Velocities are m/s.");
		vel_conv = 1.e-3;
	}
	else{
		printf("Unknown unit at line 3.\n");
		exit(1);
	}
}
else{
	printf("\n Input error at line 3.\n");
	exit(1);
}

if ( fscanf(fpr,"%[^\n]\n", header) == 1 ){
	printf("\n Data header detected.");
	}
else{
	printf("\n Input error at line 4.\n");
	exit(1);
}

printf("\n Reading object data ...");

// Read object data
for(i=0; i<sim_set->n_bodies; i++){                        

	if ( fscanf(fpr,"%lf %lf %lf %lf %lf %lf %lf %d %lf",&objects[i].mass, &objects[i].pos[0], &objects[i].pos[1], &objects[i].pos[2], &objects[i].vel[0], &objects[i].vel[1], &objects[i].vel[2], &objects[i].icon_num, &objects[i].icon_size) == 9 ){

		// Unit conversion
		objects[i].mass = objects[i].mass * mass_conv;

		for(j=0; j<3; j++){
			objects[i].pos[j] = objects[i].pos[j] * dist_conv;
		}

		for(j=0; j<3; j++){
			objects[i].vel[j] = objects[i].vel[j] * vel_conv;
		}
	}
	else{
		printf("\n Input error at line %d \n", i+5);
		exit(1);
	}

}

printf(" done.\n\n");

fclose(fpr);

}
