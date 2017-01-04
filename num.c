#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#include "main.h"







void get_acc_vector(planet objects[], settings *sim_set, int skip_id, double loc[3], double acc[]){
// loc in AU, acc in m/s²
int i;
double a_x, a_y, a_z, dist, delta[3], GModist, kAU;

//omp_set_num_threads(2);

//#pragma omp parallel default(none) private(kAU, i, delta, GModist) shared(a_x, a_y, a_z, objects, sim_set, skip_id, loc, acc)

a_x = 0.;
a_y = 0.;
a_z = 0.;

kAU = 1.E3*AU;



//#pragma omp parallel for schedule(static) reduction(+:a_x) reduction(+:a_y) reduction(+:a_z)
for(i=0; i<sim_set->n_bodies; i=i+1){

if ( i == skip_id ){
continue;
}

// AU->m
delta[0] = kAU*(objects[i].pos[0] - loc[0]);
delta[1] = kAU*(objects[i].pos[1] - loc[1]);
delta[2] = kAU*(objects[i].pos[2] - loc[2]);

// 2D distance !!!!
// m
dist = sqrt( delta[0]*delta[0] + delta[1]*delta[1] + delta[2]*delta[2]) ;

// SI
GModist = G_cst * objects[i].mass/(dist*dist*dist);

a_x += GModist * delta[0];
a_y += GModist * delta[1];
a_z += GModist * delta[2];

}

acc[0] = a_x;
acc[1] = a_y;
acc[2] = a_z;

}







int runge_kutta_fehlberg_step(planet objects[], settings *sim_set){

planet object;

int i, j;

double acc[3], loc[3], dt;
double vel[3], pos[3];
double vel_prime[3], pos_prime[3];
double vel_eps[3], pos_eps[3];

double a1x, a2x, a3x, a4x, a5x, a6x;
double a1y, a2y, a3y, a4y, a5y, a6y;
double a1z, a2z, a3z, a4z, a5z, a6z;

double v1x, v2x, v3x, v4x, v5x, v6x;
double v1y, v2y, v3y, v4y, v5y, v6y;
double v1z, v2z, v3z, v4z, v5z, v6z;

double b1, b2, b3, b4, b5, b6;
double b1s, b2s, b3s, b4s, b5s, b6s;

double a21;
double a31, a32;
double a41, a42, a43;
double a51, a52, a53, a54;
double a61, a62, a63, a64, a65;

double c1, c2, c3, c4, c5, c6;
double dtoau, dte_3;


a21 = 0.25;

a31 = 3./32.;
a32 = 9./32.;

a41 = 1932./2197.;
a42 = -7200./2197.;
a43 = 7296./2197.;

a51 = 439./216.;
a52 = -8.;
a53 = 3680./513.;
a54 = -845./4104.;

a61 = -8./27.;
a62 = 2.;
a63 = -3544./2565.;
a64 = 1859./4104.;
a65 = -11./40.;

dt = sim_set->timestep * 86400.; // days -> sec

b1 = 16./135.;
b2 = 0.; 
b3 = 6656./12825.;
b4 = 28561./56430.;
b5 = -0.18;
b6 = 2./55.;

b1s = 25./216.;
b2s = 0.; 
b3s = 1408./2565.;
b4s = 2197./4104.;
b5s = -0.2;
b6s = 0.;

c2 = 0.25;
c3 = 0.375;
c4 = 12./13.;
c5 = 1.;
c6 = 0.5;

dtoau = dt/AU;
dte_3 = dt*1.e-3;

// #pragma omp parallel for
for(j=0; j<sim_set->n_bodies; j++){

object = objects[j];

// Step 1
// AU
loc[0] = object.pos[0];
loc[1] = object.pos[1];
loc[2] = object.pos[2];

// m/s²
get_acc_vector(objects, sim_set, j, loc, acc);
a1x = acc[0];
a1y = acc[1];
a1z = acc[2];

// km/s
v1x = object.vel[0];
v1y = object.vel[1];
v1z = object.vel[2];


// Step 2
// AU
loc[0] = object.pos[0] + v1x*a21*dtoau;
loc[1] = object.pos[1] + v1y*a21*dtoau;
loc[2] = object.pos[2] + v1z*a21*dtoau;

get_acc_vector(objects, sim_set, j, loc, acc);
a2x = acc[0];
a2y = acc[1];
a2z = acc[2];

// km/s
v2x = object.vel[0] + a1x*a21*dte_3;
v2y = object.vel[1] + a1y*a21*dte_3;
v2z = object.vel[2] + a1z*a21*dte_3;



// Step 3
// AU
loc[0] = object.pos[0] + (v1x*a31+v2x*a32)*dtoau;
loc[1] = object.pos[1] + (v1y*a31+v2y*a32)*dtoau;
loc[2] = object.pos[2] + (v1z*a31+v2z*a32)*dtoau;

get_acc_vector(objects, sim_set, j, loc, acc);
a3x = acc[0];
a3y = acc[1];
a3z = acc[2];

// km/s
v3x = object.vel[0] + (a1x*a31+a2x*a32)*dte_3;
v3y = object.vel[1] + (a1y*a31+a2y*a32)*dte_3;
v3z = object.vel[2] + (a1z*a31+a2z*a32)*dte_3;



// Step 4
// AU
loc[0] = object.pos[0] + (v1x*a41+v2x*a42+v3x*a43)*dtoau;
loc[1] = object.pos[1] + (v1y*a41+v2y*a42+v3y*a43)*dtoau;
loc[2] = object.pos[2] + (v1z*a41+v2z*a42+v3z*a43)*dtoau;

get_acc_vector(objects, sim_set, j, loc, acc);
a4x = acc[0];
a4y = acc[1];
a4z = acc[2];

// km/s
v4x = object.vel[0] + (a1x*a41+a2x*a42+a3x*a43)*dte_3;
v4y = object.vel[1] + (a1y*a41+a2y*a42+a3y*a43)*dte_3;
v4z = object.vel[2] + (a1z*a41+a2z*a42+a3z*a43)*dte_3;



// Step 5
// AU
loc[0] = object.pos[0] + (v1x*a51+v2x*a52+v3x*a53+v4x*a54)*dtoau;
loc[1] = object.pos[1] + (v1y*a51+v2y*a52+v3y*a53+v4y*a54)*dtoau;
loc[2] = object.pos[2] + (v1z*a51+v2z*a52+v3z*a53+v4z*a54)*dtoau;

get_acc_vector(objects, sim_set, j, loc, acc);
a5x = acc[0];
a5y = acc[1];
a5z = acc[2];

// km/s
v5x = object.vel[0] + (a1x*a51+a2x*a52+a3x*a53+a4x*a54)*dte_3;
v5y = object.vel[1] + (a1y*a51+a2y*a52+a3y*a53+a4y*a54)*dte_3;
v5z = object.vel[2] + (a1z*a51+a2z*a52+a3z*a53+a4z*a54)*dte_3;


// Step 6
// AU
loc[0] = object.pos[0] + (v1x*a61+v2x*a62+v3x*a63+v4x*a64+v5x*a65)*dtoau;
loc[1] = object.pos[1] + (v1y*a61+v2y*a62+v3y*a63+v4y*a64+v5y*a65)*dtoau;
loc[2] = object.pos[2] + (v1z*a61+v2z*a62+v3z*a63+v4z*a64+v5z*a65)*dtoau;

get_acc_vector(objects, sim_set, j, loc, acc);
a6x = acc[0];
a6y = acc[1];
a6z = acc[2];

// km/s
v6x = object.vel[0] + (a1x*a61+a2x*a62+a3x*a63+a4x*a64+a5x*a65)*dte_3;
v6y = object.vel[1] + (a1y*a61+a2y*a62+a3y*a63+a4y*a64+a5y*a65)*dte_3;
v6z = object.vel[2] + (a1z*a61+a2z*a62+a3z*a63+a4z*a64+a5z*a65)*dte_3;


// Fifth-order position and velocity vector for stepsize control
vel_prime[0] = object.vel[0] + (b1*a1x + b2*a2x + b3*a3x + b4*a4x + b5*a5x + b6*a6x)*dte_3;
vel_prime[1] = object.vel[1] + (b1*a1y + b2*a2y + b3*a3y + b4*a4y + b5*a5y + b6*a6y)*dte_3;
vel_prime[2] = object.vel[2] + (b1*a1z + b2*a2z + b3*a3z + b4*a4z + b5*a5z + b6*a6z)*dte_3;

pos_prime[0] = object.pos[0] + (b1*v1x + b2*v2x + b3*v3x + b4*v4x + b5*v5x + b6*v6x)*dtoau;
pos_prime[1] = object.pos[1] + (b1*v1y + b2*v2y + b3*v3y + b4*v4y + b5*v5y + b6*v6y)*dtoau;
pos_prime[2] = object.pos[2] + (b1*v1z + b2*v2z + b3*v3z + b4*v4z + b5*v5z + b6*v6z)*dtoau;

// Fourth-order position and velocity vector
vel[0] = object.vel[0] + (b1s*a1x + b2s*a2x + b3s*a3x + b4s*a4x + b5s*a5x + b6s*a6x)*dte_3;
vel[1] = object.vel[1] + (b1s*a1y + b2s*a2y + b3s*a3y + b4s*a4y + b5s*a5y + b6s*a6y)*dte_3;
vel[2] = object.vel[2] + (b1s*a1z + b2s*a2z + b3s*a3z + b4s*a4z + b5s*a5z + b6s*a6z)*dte_3;

pos[0] = object.pos[0] + (b1s*v1x + b2s*v2x + b3s*v3x + b4s*v4x + b5s*v5x + b6s*v6x)*dtoau;
pos[1] = object.pos[1] + (b1s*v1y + b2s*v2y + b3s*v3y + b4s*v4y + b5s*v5y + b6s*v6y)*dtoau;
pos[2] = object.pos[2] + (b1s*v1z + b2s*v2z + b3s*v3z + b4s*v4z + b5s*v5z + b6s*v6z)*dtoau;

	// Calculate errors
	for(i=0; i<3; i=i+1){ 
	if(vel_prime[i]!=0.){
	vel_eps[i] = (vel[i]-vel_prime[i])/vel_prime[i];
	}
	else{
	vel_eps[i] = 0.;
	}
	if(pos_prime[i]!=0.){
	pos_eps[i] = (pos[i]-pos_prime[i])/pos_prime[i];
	}
	else{
	pos_eps[i] = 0.;
	}
	}

	objects[j].eps_vel = (fabs(vel_eps[0])+fabs(vel_eps[1])+fabs(vel_eps[2]))/3.; 
	objects[j].eps_pos = (fabs(pos_eps[0])+fabs(pos_eps[1])+fabs(pos_eps[2]))/3.; 

// Check error thresholds
if( objects[j].eps_vel > sim_set->eps_vel_thresh || objects[j].eps_pos > sim_set->eps_pos_thresh){
// Indicate that no new position and velocity values have been output
return 1;
}
else{
// Assign values
objects[j].pos_new[0] = pos[0];
objects[j].pos_new[1] = pos[1];
objects[j].pos_new[2] = pos[2];

objects[j].vel_new[0] = vel[0];
objects[j].vel_new[1] = vel[1];
objects[j].vel_new[2] = vel[2];

}

}

for(j=0;j<sim_set->n_bodies;j++){
objects[j].pos[0] = objects[j].pos_new[0];
objects[j].pos[1] = objects[j].pos_new[1];
objects[j].pos[2] = objects[j].pos_new[2];

objects[j].vel[0] = objects[j].vel_new[0];
objects[j].vel[1] = objects[j].vel_new[1];
objects[j].vel[2] = objects[j].vel_new[2];
}

sim_set->timestep_counter = sim_set->timestep_counter + 1;
return 0;

}




void adaptive_rkf_step(planet objects[], settings *sim_set){
int check;
check=1;

//while(check==1){
check = runge_kutta_fehlberg_step(objects, sim_set);

if( check == 1){
// Reduce timestep
sim_set-> timestep = sim_set-> timestep / sim_set->timestep_factor;
sim_set->timestep_lock = 1;
}
else{

//break;

//}

//}

sim_set->time = sim_set->time + sim_set->timestep;
sim_set->timestep_lock = 0;

if( sim_set->auto_timestep == 1 && sim_set->timestep_factor * sim_set-> timestep <= sim_set->timestep_max && sim_set->timestep_lock == 0)
sim_set-> timestep = sim_set->timestep_factor * sim_set-> timestep;

}

}
