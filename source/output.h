#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "cst.h"
#include "data.h"

void Generate_Output_File( planet objects[], settings *sim_set );
void generate_auto_output( SDL_Renderer *renderer, planet objects[], settings *sim_set);
void create_auto_screenshot(SDL_Renderer *renderer, settings *sim_set);