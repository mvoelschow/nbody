#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <string.h>

#include "data.h"
#include "cst.h"
#include "examples.h"

void init_settings(settings *sim_set);
void init_bodies(planet objects[], settings *sim_set);
void init_benchmark(settings *sim_set);
int get_file_lines(char *text);
void output_objects(planet objects[], settings *sim_set);
void load_settings_file(settings *sim_set);
void init_technical_parameters(settings *sim_set);
void read_input_file(planet objects[], settings *sim_set);