#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "main.h"
#include "SDL.h"
#include <SDL_ttf.h>




int main(){

// SDL specific
SDL_Window *window;		// Declare a window
SDL_Renderer *renderer;		// Declare a renderer

SDL_Texture *background;
SDL_Surface *background_surf;

int done = 0;

// Misc
settings sim_set;
planet *objects;


// Initialize random number generator
srand(time(NULL));

// ***********************************************************
//   Initialize simulation settings
// ***********************************************************
init_settings(&sim_set);


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
			case 5: adaptive_rkn5_step(objects, &sim_set); break;
			case 6: adaptive_rkn6_step(objects, &sim_set); break;
			default: adaptive_rkn5_step(objects, &sim_set); break;
		}

		// Check for simulation end
		if ( sim_set.time >= sim_set.time_end && sim_set.finished == 0 ){
		done = 1;
		}

	}

exit(0);

}



// ***********************************************************
//   Allocate the body array
// ***********************************************************
objects = (planet *)malloc(sizeof(planet)*sim_set.n_bodies);


// ***********************************************************
//   Initialize bodies
// ***********************************************************
init_bodies(objects, &sim_set);
// Calculate total mass
sim_set.m_tot = 0.;
int i;
for(i=0;i<sim_set.n_bodies;i++){
	sim_set.m_tot+=objects[i].mass;
}


// ***********************************************************
//   Calculate initial energy
// ***********************************************************
sim_set.E_tot_0 = get_total_energy(objects, &sim_set);
sim_set.E_tot = sim_set.E_tot_0;


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
	printf ("ERROR: SDL graphics library could not be initialized.");
	SDL_Quit();
}

// Initialize SDL2 ttf library
if (TTF_Init() != 0){
	printf ("ERROR: SDL TTF library could not be initialized.");
	SDL_Quit();
}

// Open font file
TTF_Font *fntCourier = TTF_OpenFont( "fonts/HighlandGothicFLF.ttf", 36 );


// Create an application window
if ( sim_set.fullscreen == 0 ){

	window = SDL_CreateWindow("nbody 0.2.3 ALPHA",		// Window title
				SDL_WINDOWPOS_UNDEFINED,	// Initial x position
				SDL_WINDOWPOS_UNDEFINED,	// Initial y position
				sim_set.res_x,			// width [pixels]
				sim_set.res_y,			// height [pixels]
				0				// flags
				);
}
else{

	window = SDL_CreateWindow("nbody 0.2.3 ALPHA",		// Window title
				SDL_WINDOWPOS_UNDEFINED,	// Initial x position
				SDL_WINDOWPOS_UNDEFINED,	// Initial y position
				sim_set.res_x,			// width [pixels]
				sim_set.res_y,			// height [pixels]
				SDL_WINDOW_FULLSCREEN		// flags
				);
}

// Initialize renderer ( VSYNC ON/OFF)
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
background_surf = SDL_LoadBMP("sprites/background.bmp");

if(background_surf==NULL){
printf("Error. Background bitmap not found.");
SDL_Quit();
}

background = SDL_CreateTextureFromSurface(renderer, background_surf);
SDL_FreeSurface(background_surf);




// ***********************************************************
// enter main loop
// ***********************************************************


// Event loop
while(!done){

	// Check for events
	done=processEvents(&sim_set, objects);

	// Update bodies
	if ( sim_set.paused != 1 ){

		switch(sim_set.integrator) {
			case 5: adaptive_rkn5_step(objects, &sim_set); break;
			case 6: adaptive_rkn6_step(objects, &sim_set); break;
//			case 7: adaptive_rkn7_step(objects, &sim_set); break;
//			case 8: adaptive_rkn8_step(objects, &sim_set); break;
			default: adaptive_rkn5_step(objects, &sim_set); break;
		}
	}

	// Update screen
	if ( sim_set.time >= sim_set.time_output || sim_set.interactive_mode == 1 || sim_set.paused == 1) {

		// Draw background
		Draw_Background(renderer, background, &sim_set);

		if ( sim_set.focus_on_cms == 1 && sim_set.paused == 0) center_at_cms(&sim_set, objects);

		// Render objects
		render_all_bodies_3D(renderer, objects, &sim_set);

		// Some HUD
		render_hud(renderer, fntCourier, &sim_set, objects);

		if (sim_set.paused == 1) render_paused(renderer, fntCourier, &sim_set);

		// Finally, show it all
		Render_Screen(renderer);

	}

	// Check if automatic output is scheduled
	if ( sim_set.paused != 1 && sim_set.time >= sim_set.time_output ) {
	
		if ( sim_set.check_delta_E == 1 ){
			sim_set.E_tot = get_total_energy(objects, &sim_set);
			if ( sim_set.output_delta_E == 1 ) Write_Numerical_Stats(&sim_set);
		}

		generate_auto_output(renderer, objects, &sim_set);
		sim_set.time_output += sim_set.output_interval;
	}


	// Check if screenshot output is scheduled
	if( sim_set.screenshot_trigger == 1 ){
		create_screenshot(renderer, &sim_set);
	}

	// Check for simulation end
	if ( sim_set.paused == 0 ) {

		if ( sim_set.time >= sim_set.time_end && sim_set.finished == 0 ){
			sim_set.finished = 1;
			sim_set.paused = 1;
		}

		if ( fabs((sim_set.E_tot-sim_set.E_tot_0)/sim_set.E_tot_0) > sim_set.delta_E_thresh ){
			sim_set.finished = 1;
			sim_set.paused = 1;
		}

	}

	// Don't burn the CPU in pause mode
	if ( sim_set.paused == 1 ) SDL_Delay(25);

}

// Unload font
TTF_CloseFont( fntCourier );

// Close window renderer
SDL_DestroyWindow(window);
SDL_DestroyRenderer(renderer);

// Clean exit
SDL_Quit();

return 0;

}
