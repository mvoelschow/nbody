#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "main.h"

void Generate_Output_File( planet objects[], settings *sim_set ){
char path[18];
FILE *ofp; 
int i;

// Generate filename
sprintf(path, "output_%d.dat", sim_set->output_counter); // puts string into buffer

// Get pointer to file
ofp = fopen(path, "w");
if (ofp == NULL) {
  fprintf(stderr, "Can't open output file %s!\n", path);
  exit(1);
}

// Write file header
fprintf(ofp, "# Timestep number: ");
fprintf(ofp, "%i \n", sim_set->timestep_counter);

fprintf(ofp, "# Simulated time: ");
fprintf(ofp, "%e", sim_set->time/YR);
fprintf(ofp, " yr \n");

for (i=0; i<sim_set->n_bodies; i++){
fprintf(ofp, "%i %e %e %e %e %e %e %e \n", objects[i].ident, objects[i].mass/M_SUN, objects[i].pos[0], objects[i].pos[1], objects[i].pos[2], objects[i].vel[0], objects[i].vel[1], objects[i].vel[2] );
}

/*Schliesse Datei*/
fclose (ofp);      

sim_set->output_counter = sim_set->output_counter + 1;
sim_set->time_output = sim_set->time_output + sim_set->output_interval;

}
