#pragma once

#include <math.h>
#include <limits.h>
#include <time.h>

#include "data.h"
#include "cst.h"
#include "vsop87.h"

void setup_asteroid_belt_and_planet(planet objects[], settings *sim_set);
void setup_planetesimals(planet objects[], settings *sim_set);
void setup_planetary_system(planet objects[], settings *sim_set);
void setup_stellar_filament(planet objects[], settings *sim_set);
void setup_stellar_sphere(planet objects[], settings *sim_set);
void setup_benchmark(planet objects[], settings *sim_set);
void setup_solar_system(planet objects[], settings *sim_set);