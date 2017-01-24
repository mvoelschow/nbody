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
sprintf(path, "output_%d.dat", sim_set->auto_file_counter);

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
fprintf(ofp, "%i %le %le %le %le %le %le %le \n", objects[i].ident, objects[i].mass/M_SUN, objects[i].pos[0], objects[i].pos[1], objects[i].pos[2], objects[i].vel[0], objects[i].vel[1], objects[i].vel[2] );
}

// Close file
fclose (ofp);      

sim_set->auto_file_counter++;

}




void generate_auto_output( SDL_Renderer *renderer, planet objects[], settings *sim_set){

if ( sim_set->time >= sim_set->time_output ){
	if( sim_set->auto_textfile == 1 ) Generate_Output_File( objects, sim_set );
	if( sim_set->auto_screenshot == 1 ) create_auto_screenshot( renderer, sim_set );
}

}



void Write_Numerical_Stats(settings *sim_set){
FILE *ofp; 

// Get pointer to file
ofp = fopen("stat_num.dat", "a");
if (ofp == NULL) {
  fprintf(stderr, "Can't open output file stat_num.dat!");
  exit(1);
}

fprintf(ofp, "%i %le %le %le \n", sim_set->timestep_counter, sim_set->time/365.25, sim_set->E_tot, fabs((sim_set->E_tot-sim_set->E_tot_0)/sim_set->E_tot_0));

// Close file
fclose (ofp);      

}
