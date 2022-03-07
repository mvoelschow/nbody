#pragma once
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "cst.h"
#include "data.h"
#include "hud.h"

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