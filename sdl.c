#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "main.h"
#include "SDL.h"
#include <SDL_ttf.h>



void create_screenshot(SDL_Renderer *renderer, settings *sim_set){
char path[18];

sprintf(path, "screenshot_%d.bmp", sim_set->screenshot_counter); // puts string into buffer

SDL_Surface *sshot = SDL_CreateRGBSurface(0, sim_set->res_x, sim_set->res_y, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
SDL_SaveBMP(sshot, path);
SDL_FreeSurface(sshot);

sim_set->screenshot_counter = sim_set->screenshot_counter + 1;
sim_set->screenshot_trigger = 0;

}



void create_auto_screenshot(SDL_Renderer *renderer, settings *sim_set){
char path[18];

sprintf(path, "auto_%d.bmp", sim_set->output_counter); // puts string into buffer

SDL_Surface *sshot = SDL_CreateRGBSurface(0, sim_set->res_x, sim_set->res_y, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
SDL_SaveBMP(sshot, path);
SDL_FreeSurface(sshot);

sim_set->output_counter++;

}




void switch_timestep_mode(settings *sim_set){
int i;

if ( sim_set->auto_timestep == 0 ){
sim_set->auto_timestep = 1;
}
else{
sim_set->auto_timestep = 0;

	while(sim_set->timestep > sim_set->timestep_max){
	sim_set->timestep = 0.5*sim_set->timestep;
	}
	
}

}




void switch_pause_mode(settings *sim_set){

if ( sim_set->paused == 0 ){
sim_set->paused = 1;
}
else{
sim_set->paused = 0;
}

}


void switch_icon_mode(settings *sim_set){

if ( sim_set->icon_mode == 0 ){
sim_set->icon_mode = 1;
}
else{
sim_set->icon_mode = 0;
}

}



int processEvents(SDL_Window *window, settings *sim_set, planet objects[]){
SDL_Event event;
int done=0;
double scale, timestep;
int delay, x, y;
int delta_x, delta_y;
int i;

	while(SDL_PollEvent(&event))
	{
		switch(event.type)
		{
			case SDL_WINDOWEVENT_CLOSE:
			{
				if(window){
				SDL_DestroyWindow(window);
				window = NULL;
				done = 1;
				}
			}
			break;

			case SDL_KEYDOWN:
			{
				switch(event.key.keysym.sym)
				{
					case SDLK_KP_PLUS:
					if( 2.*sim_set->timestep <= sim_set->timestep_max ){
						sim_set->timestep = sim_set->timestep * 2.;
					}
					break;

					case SDLK_KP_MINUS:
					sim_set->timestep = sim_set->timestep * 0.5;
					break;

					case SDLK_c:
					sim_set->center_screen_x = 0.5*sim_set->res_x;
					sim_set->center_screen_y = 0.5*sim_set->res_y;
					break;

					case SDLK_DOWN:
					sim_set->x_rot = sim_set->x_rot-1.;
					break;

					case SDLK_UP:
					sim_set->x_rot = sim_set->x_rot+1.;
					break;

					case SDLK_RIGHT:
					sim_set->y_rot = sim_set->y_rot+1.;
					break;

					case SDLK_LEFT:
					sim_set->y_rot = sim_set->y_rot-1.;
					break;

					case SDLK_n:
					sim_set->x_rot = 0.;
					sim_set->y_rot = 0.;
					break;

					case SDLK_ESCAPE:
					done = 1;
					break;

					case SDLK_SPACE:
					switch_pause_mode(sim_set);
					break;

					case SDLK_a:
					switch_timestep_mode(sim_set);
					break;

					case SDLK_s:
					sim_set->screenshot_trigger = 1;
					break;

					case SDLK_i:
					switch_icon_mode(sim_set);
					break;

					case SDLK_PAGEUP:
					zoom_in(sim_set);						
					break;

					case SDLK_PAGEDOWN:
					zoom_out(sim_set);	
					break;	
				}
			}
			break;

			case SDL_MOUSEWHEEL:
			{
			
			if ( event.wheel.y > 0 ){
			zoom_in_at_mouse_position(sim_set);		
			}
			else{
			zoom_out_at_mouse_position(sim_set);
			}
			}
			break;
			

			case SDL_MOUSEBUTTONDOWN:
			{

			if( event.button.state == SDL_PRESSED && event.button.button == SDL_BUTTON_LEFT){

			// Get mouse position
			SDL_GetMouseState( &x, &y );

			// Save mouse position at mouse click start
			sim_set->start_x = x;
			sim_set->start_y = y;

			}

			if( event.button.button == SDL_BUTTON_RIGHT){

			// Reset selection
			sim_set->selected_object = -1;
			
			// Get mouse position
			SDL_GetMouseState( &x, &y );

				// Check if a planet was clicked
				for (i=0;i<sim_set->n_bodies;i++){

				if( x >= objects[i].select_box_x[0] && x <= objects[i].select_box_x[1] && y >= objects[i].select_box_y[0] && y <= objects[i].select_box_y[1]){
				sim_set->selected_object = i;
				break;
				}

				}


			}


			}
			break;


			case SDL_MOUSEBUTTONUP:
			{

			if( event.button.state == SDL_RELEASED && event.button.button == SDL_BUTTON_LEFT ){

			// Get mouse position
			SDL_GetMouseState( &x, &y );

			// Assign position to settings type
			sim_set->end_x = x;
			sim_set->end_y = y;

			// calculate motion delta
			delta_x = sim_set->start_x-sim_set->end_x;
			delta_y = sim_set->start_y-sim_set->end_y;			

			sim_set->center_screen_x = sim_set->center_screen_x - delta_x;
			sim_set->center_screen_y = sim_set->center_screen_y - delta_y;		

			}

			}
			break;



			case SDL_QUIT:
			done = 1;
			break;
			}

	}

	const Uint8 *state = SDL_GetKeyboardState(NULL);


return done;

}





void Clear_to_black(SDL_Renderer *renderer, settings *sim_set){

// Set drawing color to black
SDL_SetRenderDrawColor(renderer, 0,0,0,255);				// (Red, Green, Blue, Alpha)

// Clear the screen to black
SDL_RenderClear(renderer);

}


void Draw_Background(SDL_Renderer *renderer, SDL_Texture *background, settings *sim_set){
SDL_Rect stretchRect;

if ( sim_set->draw_background == 1){
stretchRect.x = 0; 
stretchRect.y = 0; 
stretchRect.w = sim_set->res_x; 
stretchRect.h = sim_set->res_y; 

SDL_RenderCopy(renderer, background, NULL, &stretchRect);
}
else{
Clear_to_black(renderer, sim_set);
}

}




void Render_Screen(SDL_Renderer *renderer){
// Present now what we've drawn
SDL_RenderPresent(renderer);
}



void load_texture(SDL_Renderer *renderer, SDL_Texture **texture, char* path){
SDL_Surface* img_surf;

img_surf = SDL_LoadBMP(path);

if(img_surf==NULL){
printf("Error.");
SDL_Quit();
}

SDL_SetColorKey( img_surf, SDL_TRUE, SDL_MapRGB(img_surf->format,255, 0, 255) ); 
*texture = SDL_CreateTextureFromSurface(renderer, img_surf);

SDL_FreeSurface(img_surf);

}





void load_object_textures(SDL_Renderer *renderer, planet objects[], settings *sim_set){
int i;

for (i=0; i<sim_set->n_bodies; i++){
load_texture(renderer, &objects[i].icon, "sprites/sun_icon.bmp");
}

}








void render_cross(SDL_Renderer *renderer, SDL_Texture *cross, settings *sim_set, double screen_x, double screen_y){
SDL_Rect stretchRect;

stretchRect.x = screen_x - 0.5*sim_set->cross_size; 
stretchRect.y = screen_y - 0.5*sim_set->cross_size; 
stretchRect.w = sim_set->cross_size; 
stretchRect.h = sim_set->cross_size; 

SDL_RenderCopy(renderer, cross, NULL, &stretchRect);

}




void render_icon(SDL_Renderer *renderer, planet *object, settings *sim_set){
SDL_Rect stretchRect;

stretchRect.x = object->screen_pos[0] - 0.5*sim_set->icon_size; 
stretchRect.y = object->screen_pos[1] - 0.5*sim_set->icon_size; 
stretchRect.w = sim_set->icon_size; 
stretchRect.h = sim_set->icon_size; 

SDL_RenderCopy(renderer, object->icon, NULL, &stretchRect);

}





void render_icon_size_brightness(SDL_Renderer *renderer, planet *object, settings *sim_set, double size, double brightness){
SDL_Rect stretchRect;
SDL_Texture *icon_modified;
int scale;

stretchRect.x = object->screen_pos[0] - 0.5*size; 
stretchRect.y = object->screen_pos[1] - 0.5*size; 
stretchRect.w = size; 
stretchRect.h = size; 

scale = (int)(255*brightness);
icon_modified = object->icon;
SDL_SetTextureColorMod(icon_modified, scale, scale, scale);

SDL_RenderCopy(renderer, icon_modified, NULL, &stretchRect);

}





void render_all_bodies(SDL_Renderer *renderer, planet objects[], settings *sim_set, SDL_Texture *cross){
int i;

for(i=0; i<sim_set->n_bodies; i=i+1){

		if ( sim_set->icon_mode == 0){
		render_cross(renderer, cross, sim_set, objects[i].screen_pos[0], objects[i].screen_pos[1]);
		}
		else{
		render_icon(renderer, &objects[i], sim_set);
		}
	}

}






int compare_structs(const void *a, const void *b){
     
planet *struct_a = (planet *) a;
planet *struct_b = (planet *) b;
     
if (struct_a->z_projected < struct_b->z_projected) return 1;
else if (struct_a->z_projected == struct_b->z_projected) return 0;
else return -1;
     
}




void render_all_bodies_3D(SDL_Renderer *renderer, planet objects[], settings *sim_set, SDL_Texture *cross){
int i, i_plot;
double sin_y_rot, cos_y_rot, sin_x_rot, cos_x_rot;
double icon_size, delta;
double brightness;
double d_scale;

sin_y_rot = sin(sim_set->y_rot*deg_to_rad);
cos_y_rot = cos(sim_set->y_rot*deg_to_rad);

sin_x_rot = sin(sim_set->x_rot*deg_to_rad);
cos_x_rot = cos(sim_set->x_rot*deg_to_rad);


// Get projected distances 
for(i=0; i<sim_set->n_bodies; i=i+1){

objects[i].z_projected = (-objects[i].pos[0]*cos_x_rot*sin_y_rot+objects[i].pos[1]*sin_x_rot+objects[i].pos[2]*cos_x_rot*cos_y_rot);
objects[i].plot_order = i;

}

qsort(objects, sim_set->n_bodies, sizeof(objects[0]), compare_structs);
delta = 0.5*sim_set->scale;

for(i=sim_set->n_bodies; i>=0; i=i-1){

i_plot = objects[i].plot_order;
d_scale = (objects[i_plot].z_projected) / delta;

icon_size = sim_set->icon_size; // * (1.+d_scale);

if ( icon_size > sim_set->icon_size_max ) icon_size = sim_set->icon_size_max;
if ( icon_size < 1. ) icon_size = 1.;

brightness = 1.;//0.5 * (1. + d_scale);

if ( brightness < 0.1 ) brightness = 0.1;
if (brightness > 1. ) brightness = 1.;

		if ( sim_set->icon_mode == 0){
		render_cross(renderer, cross, sim_set, objects[i_plot].screen_pos[0], objects[i_plot].screen_pos[1]);
		}
		else{
		render_icon_size_brightness(renderer, &objects[i_plot], sim_set, icon_size, brightness);
		}
	}

}
