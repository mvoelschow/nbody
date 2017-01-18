// Compile via
// gcc -O3 -o nbody nbody.c setup.c output.c sdl.c hud.c num.c -lm `sdl2-config --cflags --libs` -lSDL2_ttf

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "main.h"
#include "SDL.h"
#include <SDL_ttf.h>



void generate_auto_output( SDL_Renderer *renderer, planet objects[], settings *sim_set){

if ( sim_set->time >= sim_set->time_output ){
	if( sim_set->auto_textfile == 1 ) Generate_Output_File( objects, sim_set );
	if( sim_set->auto_screenshot == 1 ) create_auto_screenshot( renderer, sim_set );
}

}



// ***********************************************************
// Main program
// ***********************************************************


int main(){

// SDL specific
SDL_Window *window;		// Declare a window
SDL_Renderer *renderer;		// Declare a renderer

SDL_Texture *background;
SDL_Surface *background_surf;

SDL_Texture *cross;
SDL_Surface *cross_surf;

int done = 0;
int i;
double delta_t;

// Misc
settings sim_set;

planet *objects;

double scale;
double time_step;

// Initialize random number generator
srand(time(NULL));


// ***********************************************************
//   Initialize simulation settings
// ***********************************************************
init_settings(&sim_set);


// ***********************************************************
//   Allocate the body array
// ***********************************************************
objects = (planet *)malloc(sizeof(planet)*sim_set.n_bodies);



// ***********************************************************
//   Initialize bodies
// ***********************************************************
init_bodies(objects, &sim_set);




// ***********************************************************
//   Initialize time argument
// ***********************************************************
sim_set.time = 0.;



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

// Define colors
SDL_Color clrWhite = {255,255,255,255}; 
SDL_Color clrRed = {255,0,0,255}; 


// Create an application window
if ( sim_set.fullscreen == 0 ){

window = SDL_CreateWindow(	"nbody 0.1.118 ALPHA",	// Window title
				SDL_WINDOWPOS_UNDEFINED,	// Initial x position
				SDL_WINDOWPOS_UNDEFINED,	// Initial y position
				sim_set.res_x,			// width [pixels]
				sim_set.res_y,			// height [pixels]
				0				// flags
				);
}
else{

window = SDL_CreateWindow(	"nbody 0.1.118 ALPHA",	// Window title
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


// Load cross bitmap
cross_surf = SDL_LoadBMP("sprites/cross.bmp");

if(cross_surf==NULL){
printf("Error. Cross hair bitmap not found.");
SDL_Quit();
}

SDL_SetColorKey( cross_surf, SDL_TRUE, SDL_MapRGB(cross_surf->format,255, 0, 255) ); 
cross = SDL_CreateTextureFromSurface(renderer, cross_surf);

SDL_FreeSurface(cross_surf);




// ***********************************************************
// enter main loop
// ***********************************************************


// Event loop
while(!done)
{

// Check for events
done=processEvents(window, &sim_set, objects);

// Draw background
Draw_Background(renderer, background, &sim_set);

// Update bodies
if ( sim_set.paused != 1 ){

	switch(sim_set.integrator) {
		case 4: adaptive_rkn45_step(objects, &sim_set); break;
		case 5: adaptive_rkn56_step(objects, &sim_set); break;
		case 6: adaptive_rkn67_step(objects, &sim_set); break;
		case 7: adaptive_rkn78_step(objects, &sim_set); break;
		default: adaptive_rkn45_step(objects, &sim_set); break;
	}

}
else{
render_paused(renderer, fntCourier, &sim_set);
}

// Calculate screen coordinates
get_planar_screen_coordinates(objects,&sim_set);

// Render objects
render_all_bodies_3D(renderer, objects, &sim_set, cross);

// Some HUD
// Scale setting
render_scale_setting(renderer, fntCourier, &sim_set);

// Timestep setting
render_timestep_setting(renderer, fntCourier, &sim_set);

// Time information
render_time_information(renderer, fntCourier, &sim_set);

// Object info box
if ( sim_set.selected_object != -1) render_object_info_box(renderer, objects, fntCourier, &sim_set);

// Finally, show it all
Render_Screen(renderer);

// Check if automatic output is scheduled
if ( sim_set.paused != 1 && sim_set.time >= sim_set.time_output ) {
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

}

}

// Unload fond
TTF_CloseFont( fntCourier );

// Close window renderer
SDL_DestroyWindow(window);
SDL_DestroyRenderer(renderer);

// Clean exit
SDL_Quit();

return 0;

}
