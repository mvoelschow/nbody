#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "main.h"
#include <omp.h>

int main(void){

// SDL specific
SDL_Window *window;		// Declare a window
SDL_Renderer *renderer;		// Declare a renderer

// OpenCL
cl_platform_id platform_id = NULL;
cl_device_id device_id = NULL;   
cl_uint num_devices;
cl_uint num_platforms;
cl_mem body_mem_obj;
cl_program program;
cl_kernel kernel_1, kernel_2;

// Init OpenCL
cl_int flag = clGetPlatformIDs(1, &platform_id, &num_platforms);
flag = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);

// Create OpenCL context
cl_context context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &flag);

// Create OpenCL command queue
cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &flag);

int i, j, done = 0;

// Misc
settings sim_set;
planet *objects;
body_gpu *bodies;

// Initialize random number generator
srand(time(NULL));


// ***********************************************************
//   Initialize simulation settings
// ***********************************************************
load_settings_file(&sim_set);
init_technical_parameters(&sim_set);

// Set number of threads
omp_set_num_threads(sim_set.n_threads);


// ***********************************************************
//   Shortcut for benchmark mode
// ***********************************************************
if ( sim_set.benchmark_mode == 1 ) {


	init_benchmark(&sim_set);
	objects = (planet *)malloc(sizeof(planet)*sim_set.n_bodies);
	setup_benchmark(objects,&sim_set);
	sim_set.time = 0.;
	clear_numerics(objects, &sim_set);

	// Event loop
	while(!done){

		// Update bodies
		switch(sim_set.integrator) {
			case 2: velocity_verlet_step(objects, &sim_set); break;
			case 4: adaptive_rkn4_step(objects, &sim_set); break;
			case 5: adaptive_rkn5_step(objects, &sim_set); break;
			case 6: adaptive_rkn6_step(objects, &sim_set); break;
			case 7: adaptive_rkn7_step(objects, &sim_set); break;
			case 8: adaptive_rkn8_step(objects, &sim_set); break;
			case 9: adaptive_rkn9_step(objects, &sim_set); break;
			default: adaptive_rkn6_step(objects, &sim_set); break;
		}

		// Check for simulation end
		if ( sim_set.time >= sim_set.time_end && sim_set.finished == 0 ){
		done = 1;
		}

	}

	exit(0);

}


// ***********************************************************
//   Allocate and initialize body array
// ***********************************************************
if ( sim_set.setup == 0 ){
	sim_set.n_bodies = get_file_lines("input.dat") - 4;
	printf(" \n Input file contains %d objects.", sim_set.n_bodies);
	objects = (planet *)malloc(sizeof(planet)*sim_set.n_bodies);
	read_input_file(objects, &sim_set);
}
else{
	objects = (planet *)malloc(sizeof(planet)*sim_set.n_bodies);
	init_bodies(objects, &sim_set);
}

// Calculate total mass
sim_set.m_tot = 0.;

for(i=0;i<sim_set.n_bodies;i++){
	sim_set.m_tot+=objects[i].mass;
}


// ***********************************************************
//   Initialize OpenCL stuff
// ***********************************************************
if ( sim_set.use_gpu == 1 ){

	// Create memory buffer on the device for each vector 
	body_mem_obj = clCreateBuffer(context, CL_MEM_READ_WRITE, sim_set.n_bodies * sizeof(body_gpu), NULL, &flag);

	// Allocate memory for gpu body array 
	bodies = (body_gpu *)malloc(sizeof(body_gpu)*sim_set.n_bodies);

	// Fill struct bodies with data
	for(i=0; i<sim_set.n_bodies; i++){
		for(j=0; j<3; j++) bodies[i].pos[j] = (float)(objects[i].pos[j]); 	// position in AU
		for(j=0; j<3; j++) bodies[i].vel[j] = (float)(objects[i].vel[j]);	// vel in km/s
		bodies[i].mass = (float)(objects[i].mass/M_SUN);			// M in solar mass units
	}

	// Copy struct body to respective memory buffer
	flag = clEnqueueWriteBuffer(command_queue, body_mem_obj, CL_TRUE, 0, sim_set.n_bodies * sizeof(body_gpu), bodies, 0, NULL, NULL);

	// Initialize kernel
	switch(sim_set.integrator) {
		case 1: init_euler_gpu(&kernel_1, &device_id, &body_mem_obj, &context, &program, &sim_set ); break;
		case 2: init_velocity_verlet_gpu(&kernel_1, &kernel_2, &device_id, &body_mem_obj, &context, &program, &sim_set ); break;
		default: init_euler_gpu(&kernel_1, &device_id, &body_mem_obj, &context, &program, &sim_set ); break;
	}

}


// ***********************************************************
//   Initialize time argument and numerics
// ***********************************************************
sim_set.time = 0.;
clear_numerics(objects, &sim_set);


// ***********************************************************
// initialize SDL2 and set up window
// ***********************************************************

// Initialize SDL2
if (SDL_Init(SDL_INIT_VIDEO) != 0){	
	printf ("\n ERROR: SDL graphics library initialization failed.");
	SDL_Quit();
}

// Initialize SDL2 ttf library
if (TTF_Init() != 0){
	printf ("\n ERROR: SDL TTF library initialization failed");
	SDL_Quit();
}

// Open font file
TTF_Font *fntCourier = TTF_OpenFont( "fonts/HighlandGothicFLF.ttf", 36 );


// Create an application window
if ( sim_set.fullscreen == 0 ){

	window = SDL_CreateWindow("nbody 0.5 ALPHA",		// Window title
				SDL_WINDOWPOS_UNDEFINED,	// Initial x position
				SDL_WINDOWPOS_UNDEFINED,	// Initial y position
				sim_set.res_x,			// width [pixels]
				sim_set.res_y,			// height [pixels]
				0				// flags
				);
}
else{

	window = SDL_CreateWindow("nbody 0.5 ALPHA",		// Window title
				SDL_WINDOWPOS_UNDEFINED,	// Initial x position
				SDL_WINDOWPOS_UNDEFINED,	// Initial y position
				sim_set.res_x,			// width [pixels]
				sim_set.res_y,			// height [pixels]
				SDL_WINDOW_FULLSCREEN		// flags
				);
}

// Initialize renderer
if ( sim_set.vsync == 1 ){
	renderer = SDL_CreateRenderer(window,-1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // VSYNC ON
}
else{
	renderer = SDL_CreateRenderer(window,-1, SDL_RENDERER_ACCELERATED); // VSYNC OFF
}



// ***********************************************************
// load textures
// ***********************************************************

// Load body icons
load_object_textures(renderer, &sim_set);


// Load background bitmap
SDL_Texture *background;
SDL_Surface *background_surf;
background_surf = SDL_LoadBMP("sprites/background.bmp");

if(background_surf==NULL){
	printf("Error. Background bitmap not found.");
	SDL_Quit();
}

background = SDL_CreateTextureFromSurface(renderer, background_surf);
SDL_FreeSurface(background_surf);


// ***********************************************************
// Apply autoscale
// ***********************************************************
if ( sim_set.autoscale_on_startup == 1 ) apply_autoscale(&sim_set, objects);


// ***********************************************************
// enter main loop
// ***********************************************************
while(!done){

	// Check for events
	done=processEvents(&sim_set, objects);

	// Update bodies
	if ( sim_set.paused != 1 ){

		// Choose integrator
		if ( sim_set.use_gpu == 0 ){
			switch(sim_set.integrator) {
				case 2: velocity_verlet_step(objects, &sim_set); break;
				case 3: forest_ruth_step(objects, &sim_set); break;
				case 4: position_extended_forest_ruth_step(objects, &sim_set); break;
				case 5: adaptive_rkn5_step(objects, &sim_set); break;
				case 6: adaptive_rkn6_step(objects, &sim_set); break;
				case 7: adaptive_rkn7_step(objects, &sim_set); break;
				case 8: adaptive_rkn8_step(objects, &sim_set); break;
				case 9: adaptive_rkn9_step(objects, &sim_set); break;
				default: adaptive_rkn6_step(objects, &sim_set); break;
			}
		}
		else{
			switch(sim_set.integrator) {
				case 1: euler_step_gpu(objects, bodies, &sim_set, &command_queue, &body_mem_obj, &kernel_1); break;
				case 2: velocity_verlet_step_gpu(objects, bodies, &sim_set, &command_queue, &body_mem_obj, &kernel_1, &kernel_2); break;
				default: velocity_verlet_step_gpu(objects, bodies, &sim_set, &command_queue, &body_mem_obj, &kernel_1, &kernel_2); break;
			}
		}
	}

	// Update screen
	if ( sim_set.time >= sim_set.time_output || sim_set.interactive_mode == 1 || sim_set.paused == 1) {

		// Draw background
		Draw_Background(renderer, background, &sim_set);

		if ( sim_set.focus_on_cms == 1 && sim_set.paused == 0) center_at_cms(&sim_set, objects);

		// Render objects
		if ( sim_set.mode_3D == 1 ){
			render_all_bodies_3D(renderer, objects, &sim_set);
		}
		else{
			render_all_bodies_2D(renderer, objects, &sim_set);
		}

		// Some HUD
		render_hud(renderer, fntCourier, &sim_set, objects);

		if (sim_set.paused == 1) render_paused(renderer, fntCourier, &sim_set);

		// Finally, show it all
		Render_Screen(renderer);

	}

	// Check if automatic output is scheduled
	if ( sim_set.paused != 1 && sim_set.time >= sim_set.time_output ) {

		generate_auto_output(renderer, objects, &sim_set);
		sim_set.time_output += sim_set.output_interval;
	}


	// Check if screenshot output is scheduled
	if( sim_set.screenshot_trigger == 1 ) create_screenshot(renderer, &sim_set);

	// Check for simulation end
	if ( sim_set.paused == 0 ) {

		if ( sim_set.time >= sim_set.time_end && sim_set.finished == 0 && sim_set.resume == 0){
			sim_set.finished = 1;
			sim_set.paused = 1;
		}

	}

	// Wanna continue? Why not ...
	if ( sim_set.paused == 1  && sim_set.finished == 1 && sim_set.resume == 0) sim_set.resume = 1;
	

	// Don't burn the CPU in pause mode
	if ( sim_set.paused == 1 ) SDL_Delay(25);

}

// Clean up OpenCL
if ( sim_set.use_gpu == 1 ){
	flag = clFlush(command_queue);
	flag = clFinish(command_queue);
	flag = clReleaseProgram(program);
	flag = clReleaseMemObject(body_mem_obj);
	flag = clReleaseCommandQueue(command_queue);
	flag = clReleaseContext(context);

	switch(sim_set.integrator) {
		case 1: flag = clReleaseKernel(kernel_1); break;
		case 2: flag = clReleaseKernel(kernel_1); flag = clReleaseKernel(kernel_2); break;
		default: flag = clReleaseKernel(kernel_1); flag = clReleaseKernel(kernel_2); break;
	
	}
	

	free(bodies);
}

free(objects);

// Unload font
TTF_CloseFont( fntCourier );

// Close window renderer
SDL_DestroyWindow(window);
SDL_DestroyRenderer(renderer);

// Clean exit
SDL_Quit();

return 0;

}
