#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "data.h"
#include "cst.h"

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