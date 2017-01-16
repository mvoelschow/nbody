#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
// #include <omp.h>

#include "main.h"





// t_off in sec
void get_acc_vector_toff(planet objects[], settings *sim_set, int skip_id, double t_off, double loc[3], double acc[]){
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
delta[0] = kAU*(objects[i].pos[0] + t_off*objects[i].vel[0]/AU - loc[0]);
delta[1] = kAU*(objects[i].pos[1] + t_off*objects[i].vel[1]/AU - loc[1]);
delta[2] = kAU*(objects[i].pos[2] + t_off*objects[i].vel[2]/AU - loc[2]);

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




int rkn45_step(planet objects[], settings *sim_set){

planet object;

int i, j, k, l;
const int n=6;

double vx0, vy0, vz0, dx, dy, dz;
double acc[3], loc[3], dt, dtsq;
double vel[3], pos[3], vel_hat[3];
double vel_eps[3], pos_eps[3];

double fx[n];
double fy[n];
double fz[n];

double c[n];
double c_dot[n];
double c_hat[n];
double fe, dt_new;

double alpha[n];
double beta[n][n];
double gamma[n][n];

double cifi[3], cdotifi[3], chatifi[3];

double dtoau, dte_3;

alpha[0] = 0.;
alpha[1] = 0.25;
alpha[2] = 0.375;
alpha[3] = 12./13.;
alpha[4] = 1.;
alpha[5] = 0.5;

gamma[0][0] = 0.;

gamma[1][0] = 1./32.;

gamma[2][0] = 9./256.;
gamma[2][1] = 9./256.;

gamma[3][0] = 27342./142805.;
gamma[3][1] = -49266./142805.;
gamma[3][2] = 82764./142805.;

gamma[4][0] = 5./18.;
gamma[4][1] = -2./3.;
gamma[4][2] = 8./9.;
gamma[4][3] = 0.;

gamma[5][0] = -3./16.;
gamma[5][1] = 1.;
gamma[5][2] = -11./15.;
gamma[5][3] = 0.;
gamma[5][4] = 11./240.;

c[0] = 253./2160.;
c[1] = 0.;
c[2] = 4352./12825.;
c[3] = 2197./41040.;
c[4] = -0.01;

c_dot[0] = 25./216.;
c_dot[1] = 0.;
c_dot[2] = 1408./2565.;
c_dot[3] = 2197./4104.;
c_dot[4] = -0.2;

c_hat[0] = 16./135.;
c_hat[1] = 0.;
c_hat[2] = 6656./12825.;
c_hat[3] = 28561./56430.;
c_hat[4] = -9./50.;
c_hat[5] = 2./55.;

dt = sim_set->timestep * 86400.; // days -> sec
dtsq = dt*dt;
dte_3 = dt*1.e-3;
dtoau = dt/AU;

for(k=0; k<sim_set->n_bodies; k++){

object = objects[k];

// Step 1: Initial values
// AU
loc[0] = object.pos[0];
loc[1] = object.pos[1];
loc[2] = object.pos[2];

// km/s
vx0 = object.vel[0];
vy0 = object.vel[1];
vz0 = object.vel[2];

// m/s²
get_acc_vector_toff(objects, sim_set, k, 0., loc, acc);
fx[0] = acc[0];
fy[0] = acc[1];
fz[0] = acc[2];


// m/s² for all three blocks
cifi[0] = c[0]*fx[0];
cifi[1] = c[0]*fy[0];
cifi[2] = c[0]*fz[0];

cdotifi[0] = c_dot[0]*fx[0];
cdotifi[1] = c_dot[0]*fy[0];
cdotifi[2] = c_dot[0]*fz[0];

chatifi[0] = c_hat[0]*fx[0];
chatifi[1] = c_hat[0]*fy[0];
chatifi[2] = c_hat[0]*fz[0];

dx=0.;
dy=0.;
dz=0.;

// Calculate f_i values
for(i=1; i<n; i++){

	for(j=0; j<i; j++){
		// m/s²
		dx = dx + gamma[i][j]*fx[j];
		dy = dy + gamma[i][j]*fy[j];
		dz = dz + gamma[i][j]*fz[j];
	}

	// AU
	loc[0] = object.pos[0] + vx0*alpha[i]*dtoau + dte_3 * dx * dtoau;
	loc[1] = object.pos[1] + vy0*alpha[i]*dtoau + dte_3 * dy * dtoau;
	loc[2] = object.pos[2] + vz0*alpha[i]*dtoau + dte_3 * dz * dtoau;

	// Evaluate x_dot_dot
	get_acc_vector_toff(objects, sim_set, k, alpha[i]*dt, loc, acc);
	//get_acc_vector(objects, sim_set, k, loc, acc);

	// m/s²
	fx[i] = acc[0];
	fy[i] = acc[1];
	fz[i] = acc[2];

	// m/s² for all three blocks
	if(i<n-1){
		cifi[0] = cifi[0] + fx[i]*c[i];
		cifi[1] = cifi[1] + fy[i]*c[i];
		cifi[2] = cifi[2] + fz[i]*c[i];
	
		cdotifi[0] = cdotifi[0] + fx[i]*c_dot[i];
		cdotifi[1] = cdotifi[1] + fy[i]*c_dot[i];
		cdotifi[2] = cdotifi[2] + fz[i]*c_dot[i];
	}

	chatifi[0] = chatifi[0] + fx[i]*c_hat[i];
	chatifi[1] = chatifi[1] + fy[i]*c_hat[i];
	chatifi[2] = chatifi[2] + fz[i]*c_hat[i];
	
}

pos[0] = object.pos[0] + vx0*dtoau + dte_3 * cifi[0]* dtoau;
pos[1] = object.pos[1] + vy0*dtoau + dte_3 * cifi[1]* dtoau;
pos[2] = object.pos[2] + vz0*dtoau + dte_3 * cifi[2]* dtoau;

vel[0] = object.vel[0] + cdotifi[0]*dte_3;
vel[1] = object.vel[1] + cdotifi[1]*dte_3;
vel[2] = object.vel[2] + cdotifi[2]*dte_3;

vel_hat[0] = object.vel[0] + chatifi[0]*dte_3;
vel_hat[1] = object.vel[1] + chatifi[1]*dte_3;
vel_hat[2] = object.vel[2] + chatifi[2]*dte_3;

vel_eps[0] = 1.e-3*dt*(-fx[0]/360.+128.*fx[2]/4275.+2197.*fx[3]/75240.-0.02*fx[4]-2.*fx[5]/55.);
vel_eps[1] = 1.e-3*dt*(-fy[0]/360.+128.*fy[2]/4275.+2197.*fy[3]/75240.-0.02*fy[4]-2.*fy[5]/55.);
vel_eps[2] = 1.e-3*dt*(-fz[0]/360.+128.*fz[2]/4275.+2197.*fz[3]/75240.-0.02*fz[4]-2.*fz[5]/55.);


// Calculate mean error
objects[k].eps_vel = (fabs(vel_eps[0])+fabs(vel_eps[1])+fabs(vel_eps[2]))/3.; 

fe = sim_set->eps_vel_thresh*(fabs(vel[0])+fabs(vel[1])+fabs(vel[2]))/objects[k].eps_vel;

dt_new = sim_set->timestep * fmin(2., fmax(0.2,0.9*sqrt(fe)));

// Check error thresholds
if( fe < 1. ){
// Chose new timestep
sim_set->timestep = dt_new;
// Indicate that no new position and velocity values have been output
return 1;
}
else{
// Assign values
//printf("Assigning ...");
objects[k].pos_new[0] = pos[0];
objects[k].pos_new[1] = pos[1];
objects[k].pos_new[2] = pos[2];

objects[k].vel_new[0] = vel[0];
objects[k].vel_new[1] = vel[1];
objects[k].vel_new[2] = vel[2];

}

}

for(k=0;k<sim_set->n_bodies;k++){
objects[k].pos[0] = objects[k].pos_new[0];
objects[k].pos[1] = objects[k].pos_new[1];
objects[k].pos[2] = objects[k].pos_new[2];

objects[k].vel[0] = objects[k].vel_new[0];
objects[k].vel[1] = objects[k].vel_new[1];
objects[k].vel[2] = objects[k].vel_new[2];
}

// Increment timestep counter
sim_set->timestep_counter = sim_set->timestep_counter + 1;
// Increment time according to old timestep
sim_set->time = sim_set->time + sim_set->timestep;
// Update timestep
sim_set->timestep = dt_new;
return 0;

}




void adaptive_rkn45_step(planet objects[], settings *sim_set){
int check;
check=1;

check = rkn45_step(objects, sim_set);

}
