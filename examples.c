#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "main.h"


// ***********************************************************
//   Auxilary functions
// ***********************************************************
double randomDouble(){
      double r = (double)rand()/(double)RAND_MAX;
      return r;
}



// ***********************************************************
// N-2 asteroids and a massive 5 M_jup planet orbiting the sun
// ***********************************************************
void setup_asteroid_belt_and_planet(planet objects[], settings *sim_set){
int i;
double dist, r, phi;
double v_circ;
double p_orb, r_planet;


// Sun
objects[0].ident = 0;
objects[0].mass = M_SUN;
objects[0].icon_num = 0;
objects[0].icon_size = 24;

// Massive planet
objects[1].ident = 1;
objects[1].mass = 1.E-2*M_SUN;
objects[1].icon_num = 5;
objects[1].icon_size = 12;

r_planet = 6.; // [au]

// Place sun and planet on a proper orbit around their barycentre
objects[0].pos[0] = -r_planet * objects[1].mass / (objects[0].mass+objects[1].mass);
objects[0].pos[1] = 0.;
objects[0].pos[2] = 0.;

objects[1].pos[0] = r_planet + objects[0].pos[0];
objects[1].pos[1] = 0;
objects[1].pos[2] = 0.;

p_orb = sqrt( 4.*PI*PI * pow(r_planet*AU*1.E3,3) / (G_cst * (objects[0].mass+objects[1].mass) ) );

// Objects revolve clockwise. Sun starts in positve y-direction, planet in negative y-direction
objects[0].vel[0] = 0.;
objects[0].vel[1] = 2.*PI*objects[0].pos[0]*AU / p_orb;
objects[0].vel[2] = 0.;


objects[1].vel[0] = 0.;
objects[1].vel[1] = 2.*PI*objects[1].pos[0]*AU / p_orb;
objects[1].vel[2] = 0.;



// Planetesimals between 3 and 5 au
for(i=2; i<sim_set->n_bodies; i=i+1){

// Set body mass [kg]
objects[i].mass = 1.E19*(99.*randomDouble()+1.);
objects[i].icon_num = 1;
objects[i].icon_size = 4;

r = 3.+2.*randomDouble();
phi = randomDouble()*2.*PI;

// Set body position [AU]
// x component
objects[i].pos[0] = r*cos(phi);
// y component
objects[i].pos[1] = r*sin(phi);
// z component
objects[i].pos[2] = 0.1*(2.*randomDouble()-1.);

dist = sqrt(objects[i].pos[0]*objects[i].pos[0]+objects[i].pos[1]*objects[i].pos[1]+objects[i].pos[2]*objects[i].pos[2]);

v_circ = 1.E-3*sqrt( G_cst * objects[0].mass / (dist*AU*1.E3) );

// Set body velocity [km/s]
// x component
objects[i].vel[0] = -v_circ*sin(phi);
// y component
objects[i].vel[1] = v_circ*cos(phi);
// z component
objects[i].vel[2] = 0.;


// Set identifier [DO NOT CHANGE]
objects[i].ident = i;

}

}




// ***********************************************************
// N-1 asteroids orbiting the sun
// ***********************************************************
void setup_planetesimals(planet objects[], settings *sim_set){
int i;
double dist, r, phi;
double v_circ;

// Sun
objects[0].ident = 0;
objects[0].mass = M_SUN;
objects[0].icon_num = 0;
objects[0].icon_size = 24;

// Place the sun in the origin of the coordinate system
objects[0].pos[0] = 0.;
objects[0].pos[1] = 0.;
objects[0].pos[2] = 0.;

// Objects revolve clockwise. Sun starts in positve y-direction, planet in negative y-direction
objects[0].vel[0] = 0.;
objects[0].vel[1] = 0.;
objects[0].vel[2] = 0.;


// Planetesimals between 1 and 6 au
for(i=1; i<sim_set->n_bodies; i=i+1){

// Set body mass [kg]
objects[i].mass = 1.E19*(99.*randomDouble()+1.);
objects[i].icon_num = 1;
objects[i].icon_size = 4;

r = 1.+5.*randomDouble();
phi = randomDouble()*2.*PI;

// Set body position [AU]
// x component
objects[i].pos[0] = r*cos(phi);
// y component
objects[i].pos[1] = r*sin(phi);
// z component
objects[i].pos[2] = 0.1*(2.*randomDouble()-1.);

dist = sqrt(objects[i].pos[0]*objects[i].pos[0]+objects[i].pos[1]*objects[i].pos[1]+objects[i].pos[2]*objects[i].pos[2]);

v_circ = 1.E-3*sqrt( G_cst * objects[0].mass / (dist*AU*1.E3) );

// Set body velocity [km/s]
// x component
objects[i].vel[0] = -v_circ*sin(phi);
// y component
objects[i].vel[1] = v_circ*cos(phi);
// z component
objects[i].vel[2] = 0.;


// Set identifier [DO NOT CHANGE]
objects[i].ident = i;

}

}





// ***********************************************************
// A random planetary system
// ***********************************************************
void setup_planetary_system(planet objects[], settings *sim_set){
int i;
double dist, r, phi;
double v_circ;

// Sun
objects[0].ident = 0;
objects[0].mass = M_SUN;
objects[0].icon_num = 0;
objects[0].icon_size = 24;

// Place the sun in the origin of the coordinate system
objects[0].pos[0] = 0.;
objects[0].pos[1] = 0.;
objects[0].pos[2] = 0.;

// No CMS stuff ATM ...
objects[0].vel[0] = 0.;
objects[0].vel[1] = 0.;
objects[0].vel[2] = 0.;


// Planets between 1 and 16 au
for(i=1; i<sim_set->n_bodies; i=i+1){

// Set body mass [kg]
objects[i].mass = 1.E23*(9999.*pow(randomDouble(), 3)+1.);

if ( objects[i].mass <= 1.E24 && objects[i].mass > 1.E23 ){
// Very small terrestial planet
objects[i].icon_num = 1;
objects[i].icon_size = 4;
}
if ( objects[i].mass <= 1.E25 && objects[i].mass > 1.E24 ){
// Terrestial planet
objects[i].icon_num = 3;
objects[i].icon_size = 6;
}
if ( objects[i].mass <= 2.E26 && objects[i].mass > 1.E25 ){
// Super-Earth / Neptunian
objects[i].icon_num = 8;
objects[i].icon_size = 12;
}
if ( objects[i].mass <= 1.E27 && objects[i].mass > 2.E26 ){
// Jovian
objects[i].icon_num = 5;
objects[i].icon_size = 16;
}

r = 1.+29.*randomDouble();
phi = randomDouble()*2.*PI;

// Set body position [AU]
// x component
objects[i].pos[0] = r*cos(phi);
// y component
objects[i].pos[1] = r*sin(phi);
// z component
objects[i].pos[2] = 0.1*(2.*randomDouble()-1.);

dist = sqrt(objects[i].pos[0]*objects[i].pos[0]+objects[i].pos[1]*objects[i].pos[1]+objects[i].pos[2]*objects[i].pos[2]);

v_circ = 1.E-3*sqrt( G_cst * objects[0].mass / (dist*AU*1.E3) );

// Set body velocity [km/s]
// x component
objects[i].vel[0] = -v_circ*sin(phi);
// y component
objects[i].vel[1] = v_circ*cos(phi);
// z component
objects[i].vel[2] = 0.;


// Set identifier [DO NOT CHANGE]
objects[i].ident = i;

}

}




// ***********************************************************
// Template for a single filament of stars
// ***********************************************************
void setup_stellar_filament(planet objects[], settings *sim_set){
int i;

for(i=0; i<sim_set->n_bodies; i++){

// Set body mass [kg]
objects[i].mass = 0.1*M_SUN*(99.*randomDouble()+1.);
objects[i].icon_num = 0;
objects[i].icon_size = 8;

// Set body position [AU]
// x component
objects[i].pos[0] = 7.*PC*(2.*randomDouble()-1.);
// y component
objects[i].pos[1] = 0.7*PC*(2.*randomDouble()-1.);
// z component
objects[i].pos[2] = 0.7*PC*(2.*randomDouble()-1.);

// Set body velocity [km/s]
// x component
objects[i].vel[0] = 0.01*(2.*randomDouble()-1.);
// y component
objects[i].vel[1] = 0.01*(2.*randomDouble()-1.);
// z component
objects[i].vel[2] = 0.01*(2.*randomDouble()-1.);


// Set identifier [DO NOT CHANGE]
objects[i].ident = i;

}

}




// ***********************************************************
// Template for stars randomly distributed (in r and phi) within a sphere
// ***********************************************************
void setup_stellar_sphere(planet objects[], settings *sim_set){
int i;
double r, phi, theta;

for(i=0; i<sim_set->n_bodies; i++){

// Set body mass [kg]
objects[i].mass = 0.1*M_SUN*(99.*randomDouble()+1.);
objects[i].icon_num = 0;
objects[i].icon_size = 8;

r = 25.*PC*randomDouble();
phi = randomDouble()*2.*PI;
theta = randomDouble()*2.*PI;

// Set body position [AU]
// x component
objects[i].pos[0] = r*sin(theta)*cos(phi);
// y component
objects[i].pos[1] = r*sin(theta)*sin(phi);
// z component
objects[i].pos[2] = r*cos(theta);

// Set body velocity [km/s]
// x component
objects[i].vel[0] = 0.001*(2.*randomDouble()-1.);
// y component
objects[i].vel[1] = 0.001*(2.*randomDouble()-1.);
// z component
objects[i].vel[2] = 0.001*(2.*randomDouble()-1.);


// Set identifier [DO NOT CHANGE]
objects[i].ident = i;

}

}
