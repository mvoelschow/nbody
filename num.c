#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
// #include <omp.h>

#include "main.h"




double get_total_energy(planet objects[], settings *sim_set){
int i, j;
double e_kin, e_pot, e_sum, v_abs, dist, dx, dy, dz, kAU;

kAU = 1.E3*AU;
e_sum = 0;

for(i=0; i<sim_set->n_bodies; i++){

	// Kinetic energy
	v_abs = 1.e3*sqrt(objects[i].vel[0]*objects[i].vel[0]+objects[i].vel[1]*objects[i].vel[1]+objects[i].vel[2]*objects[i].vel[2]); // m/s
	e_kin = 0.5*objects[i].mass*v_abs*v_abs; // Joules
	e_pot = 0.;

	for(j=0; j<sim_set->n_bodies;j++){

		if( i==j ) continue;

		// AU->m
		dx = kAU*(objects[i].pos[0] - objects[j].pos[0]);
		dy = kAU*(objects[i].pos[1] - objects[j].pos[1]);
		dz = kAU*(objects[i].pos[2] - objects[j].pos[2]);

		// m
		dist = sqrt( dx*dx + dy*dy + dz*dz);

		// Joules
		e_pot += -G_cst * objects[i].mass * objects[j].mass / dist;

	}

e_sum += e_kin + e_pot;

}

return e_sum;

}





// t_off in sec
void get_acc_vector(planet objects[], settings *sim_set, int skip_id, double acc[]){
// loc in AU, acc in m/s²
int i;
double a_x, a_y, a_z, dist, delta[3], GModist, kAU;
double loc[3];

loc[0] = objects[skip_id].pos_new[0];
loc[1] = objects[skip_id].pos_new[1];
loc[2] = objects[skip_id].pos_new[2];

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
delta[0] = kAU*(objects[i].pos_new[0] - loc[0]);
delta[1] = kAU*(objects[i].pos_new[1] - loc[1]);
delta[2] = kAU*(objects[i].pos_new[2] - loc[2]);

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



int rkn56_step(planet objects[], settings *sim_set){

planet object;

int i, j, k, l;
const int n=7;

double vx0, vy0, vz0, dx, dy, dz;
double acc[3], loc[3], dt, dtsq;
double vel[3], pos[3], vel_hat[3];
double vel_eps[3], pos_eps[3];

double fx[sim_set->n_bodies][n];
double fy[sim_set->n_bodies][n];
double fz[sim_set->n_bodies][n];

double c[n];
double c_dot[n];
double c_hat[n];
double fe, dt_new, dt_new_guess;

double alpha[n];
double beta[n][n];
double gamma[n][n];

double dtoau, dte_3, dte_3dtoau;

dt_new = 1.e100;

alpha[0] = 0.;
alpha[1] = 1./12.;
alpha[2] = 1./6.;
alpha[3] = 0.5;
alpha[4] = 0.8;
alpha[5] = 1.;
alpha[6] = 1.;

gamma[0][0] = 0.;

gamma[1][0] = 1./288.;

gamma[2][0] = 1./216.;
gamma[2][1] = 1./108.;

gamma[3][0] = 0.;
gamma[3][1] = 0.;
gamma[3][2] = 0.125;

gamma[4][0] = 16./125.;
gamma[4][1] = 0.;
gamma[4][2] = 4./125.;
gamma[4][3] = 4./25.;

gamma[5][0] = -247./1152.;
gamma[5][1] = 0.;
gamma[5][2] = 12./19.;
gamma[5][3] = 7./432.;
gamma[5][4] = 4375./65664.;

gamma[6][0] = 11./240.;
gamma[6][1] = 0.;
gamma[6][2] = 108./475.;
gamma[6][3] = 8./45.;
gamma[6][4] = 125./2736.;
gamma[6][5] = 1./300.;

c[0] = gamma[6][0];
c[1] = 0.;
c[2] = gamma[6][2];
c[3] = gamma[6][3];
c[4] = gamma[6][4];
c[5] = gamma[6][5];

c_dot[0] = 1./24.;
c_dot[1] = 0.;
c_dot[2] = 27./95.;
c_dot[3] = 1./3.;
c_dot[4] = 125./456.;
c_dot[5] = 1./15.;

dt = sim_set->timestep * 86400.; // days -> sec
dtsq = dt*dt;
dte_3 = dt*1.e-3;
dtoau = dt/AU;
dte_3dtoau = dte_3*dtoau;

// Step 1: Initial values
for(k=0; k<sim_set->n_bodies; k++){

	// AU
	objects[k].pos_new[0] = objects[k].pos[0];
	objects[k].pos_new[1] = objects[k].pos[1];
	objects[k].pos_new[2] = objects[k].pos[2];

	// m/s² for all three blocks
	objects[k].cifi[0] = 0;
	objects[k].cifi[1] = 0;
	objects[k].cifi[2] = 0;

	objects[k].cdotifi[0] = 0;
	objects[k].cdotifi[1] = 0;
	objects[k].cdotifi[2] = 0;

	objects[k].d[0]=0.;
	objects[k].d[1]=0.;
	objects[k].d[2]=0.;

}

// Calculate f_i values
for(i=0; i<n; i++){

	// Assign positions for the i-th evaluation of the acceleration function
	for(k=0; k<sim_set->n_bodies; k++){

		for(j=0; j<i; j++){
			// m/s²
			objects[k].d[0] += gamma[i][j]*fx[k][j];
			objects[k].d[1] += gamma[i][j]*fy[k][j];
			objects[k].d[2] += gamma[i][j]*fz[k][j];
		}

		// AU
		objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*alpha[i]*dtoau + dte_3 * objects[k].d[0] * dtoau;
		objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*alpha[i]*dtoau + dte_3 * objects[k].d[1] * dtoau;
		objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*alpha[i]*dtoau + dte_3 * objects[k].d[2] * dtoau;

	}

	// Evaluate the acceleration function for all particles 
	for(k=0; k<sim_set->n_bodies; k++){

		get_acc_vector(objects, sim_set, k, acc);

		// m/s²
		fx[k][i] = acc[0];
		fy[k][i] = acc[1];
		fz[k][i] = acc[2];

		// Increment the interaction sums
		if(i<n-1){
			// m/s² for all three blocks
			objects[k].cifi[0] += fx[k][i]*c[i];
			objects[k].cifi[1] += fy[k][i]*c[i];
			objects[k].cifi[2] += fz[k][i]*c[i];
	
			objects[k].cdotifi[0] += fx[k][i]*c_dot[i];
			objects[k].cdotifi[1] += fy[k][i]*c_dot[i];
			objects[k].cdotifi[2] += fz[k][i]*c_dot[i];
		}
	
	}

}

// Store new positions in temporary variables and calculate the truncation error estimate
for(k=0; k<sim_set->n_bodies; k++){

	// AU
	pos[0] = objects[k].pos[0] + objects[k].vel[0]*dtoau + objects[k].cifi[0]*dte_3dtoau;
	pos[1] = objects[k].pos[1] + objects[k].vel[1]*dtoau + objects[k].cifi[1]*dte_3dtoau;
	pos[2] = objects[k].pos[2] + objects[k].vel[2]*dtoau + objects[k].cifi[2]*dte_3dtoau;

	// km/s
	vel[0] = objects[k].vel[0] + objects[k].cdotifi[0]*dte_3;
	vel[1] = objects[k].vel[1] + objects[k].cdotifi[1]*dte_3;
	vel[2] = objects[k].vel[2] + objects[k].cdotifi[2]*dte_3;

	// Positional truncation error
	pos_eps[0] = c[5]*dte_3dtoau*(fx[k][5]-fx[k][6]);
	pos_eps[1] = c[5]*dte_3dtoau*(fy[k][5]-fy[k][6]);
	pos_eps[2] = c[5]*dte_3dtoau*(fz[k][5]-fz[k][6]);

	// Calculate mean error
	objects[k].eps_pos = (fabs(pos_eps[0])+fabs(pos_eps[1])+fabs(pos_eps[2]))/3.; 
	fe = sim_set->eps_pos_thresh*(fabs(pos[0])+fabs(pos[1])+fabs(pos[2]))/objects[k].eps_pos;
	dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*sqrt(fe)));

	// Chose the smallest timestep estimate for the next step
	if ( dt_new_guess < dt_new ) dt_new = dt_new_guess;

	// Check error thresholds
	if( fe < 1. ){
		// Chose new timestep
		sim_set->timestep = dt_new;
		// Indicate that no new position and velocity values have been output
		return 1;
	}
	else{
		// Assign values to temporary variable
		objects[k].pos_new[0] = pos[0];
		objects[k].pos_new[1] = pos[1];
		objects[k].pos_new[2] = pos[2];

		objects[k].vel_new[0] = vel[0];
		objects[k].vel_new[1] = vel[1];
		objects[k].vel_new[2] = vel[2];
	}

}

// All particles passed the error check. Assign new values to final variable
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



void adaptive_rkn56_step(planet objects[], settings *sim_set){
int check;
check=1;

check = rkn56_step(objects, sim_set);

}





// To be updated
/*

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

double dtoau, dte_3, dte_3dtoau;

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
dte_3dtoau = dte_3*dtoau;

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
	loc[0] = object.pos[0] + vx0*alpha[i]*dtoau + dx * dte_3dtoau;
	loc[1] = object.pos[1] + vy0*alpha[i]*dtoau + dy * dte_3dtoau;
	loc[2] = object.pos[2] + vz0*alpha[i]*dtoau + dz * dte_3dtoau;

	// Evaluate x_dot_dot
	get_acc_vector_toff(objects, sim_set, k, alpha[i]*dt, loc, acc);

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

pos[0] = object.pos[0] + vx0*dtoau + cifi[0]* dte_3dtoau;
pos[1] = object.pos[1] + vy0*dtoau + cifi[1]* dte_3dtoau;
pos[2] = object.pos[2] + vz0*dtoau + cifi[2]* dte_3dtoau;

vel[0] = object.vel[0] + cdotifi[0]*dte_3;
vel[1] = object.vel[1] + cdotifi[1]*dte_3;
vel[2] = object.vel[2] + cdotifi[2]*dte_3;

vel_hat[0] = object.vel[0] + chatifi[0]*dte_3;
vel_hat[1] = object.vel[1] + chatifi[1]*dte_3;
vel_hat[2] = object.vel[2] + chatifi[2]*dte_3;

vel_eps[0] = dte_3*(-fx[0]/360.+128.*fx[2]/4275.+2197.*fx[3]/75240.-0.02*fx[4]-2.*fx[5]/55.);
vel_eps[1] = dte_3*(-fy[0]/360.+128.*fy[2]/4275.+2197.*fy[3]/75240.-0.02*fy[4]-2.*fy[5]/55.);
vel_eps[2] = dte_3*(-fz[0]/360.+128.*fz[2]/4275.+2197.*fz[3]/75240.-0.02*fz[4]-2.*fz[5]/55.);


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



int rkn67_step(planet objects[], settings *sim_set){

planet object;

int i, j, k, l;
const int n=8;

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

double cifi[3], cdotifi[3];

double dtoau, dte_3, dte_3dtoau;

alpha[0] = 0.;
alpha[1] = 0.1;
alpha[2] = 0.2;
alpha[3] = 0.4;
alpha[4] = 0.6;
alpha[5] = 0.8;
alpha[6] = 1.;
alpha[7] = 1.;

gamma[0][0] = 0.;

gamma[1][0] = 1./200.;

gamma[2][0] = 1./150.;
gamma[2][1] = 1./75.;

gamma[3][0] = 2./75.;
gamma[3][1] = 0.;
gamma[3][2] = 4./75.;

gamma[4][0] = 9./200.;
gamma[4][1] = 0.;
gamma[4][2] = 9./100.;
gamma[4][3] = 9./200.;

gamma[5][0] = 199./3600.;
gamma[5][1] = -19./150.;
gamma[5][2] = 47./120.;
gamma[5][3] = -119./1200.;
gamma[5][4] = 89./900.;

gamma[6][0] = -179./1824.;
gamma[6][1] = 17./38.;
gamma[6][2] = 0.;
gamma[6][3] = -37./152.;
gamma[6][4] = 219./456.;
gamma[6][5] = -157./1824.;

gamma[7][0] = 61./1008.;
gamma[7][1] = 0.;
gamma[7][2] = 475./2016.;
gamma[7][3] = 25./504.;
gamma[7][4] = 125./1008.;
gamma[7][5] = 25./1008.;
gamma[7][6] = 11./2016.;

c[0] = gamma[7][0];
c[1] = 0.;
c[2] = gamma[7][2];
c[3] = gamma[7][3];
c[4] = gamma[7][4];
c[5] = gamma[7][5];
c[6] = gamma[7][6];

c_dot[0] = 19./288.;
c_dot[1] = 0.;
c_dot[2] = 25./96.;
c_dot[3] = 25./144.;
c_dot[4] = 25./144.;
c_dot[5] = 25./96.;
c_dot[6] = 19./288.;

dt = sim_set->timestep * 86400.; // days -> sec
dtsq = dt*dt;
dte_3 = dt*1.e-3;
dtoau = dt/AU;
dte_3dtoau = dte_3*dtoau;

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
	loc[0] = object.pos[0] + vx0*alpha[i]*dtoau + dte_3dtoau * dx;
	loc[1] = object.pos[1] + vy0*alpha[i]*dtoau + dte_3dtoau * dy;
	loc[2] = object.pos[2] + vz0*alpha[i]*dtoau + dte_3dtoau * dz;

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
	
}

pos[0] = object.pos[0] + vx0*dtoau + cifi[0] * dte_3dtoau;
pos[1] = object.pos[1] + vy0*dtoau + cifi[1] * dte_3dtoau;
pos[2] = object.pos[2] + vz0*dtoau + cifi[2] * dte_3dtoau;

vel[0] = object.vel[0] + cdotifi[0]*dte_3;
vel[1] = object.vel[1] + cdotifi[1]*dte_3;
vel[2] = object.vel[2] + cdotifi[2]*dte_3;

pos_eps[0] = c[6]*dte_3dtoau * (fx[6]-fx[7]);
pos_eps[1] = c[6]*dte_3dtoau * (fy[6]-fy[7]);
pos_eps[2] = c[6]*dte_3dtoau * (fz[6]-fz[7]);

// Calculate mean error
objects[k].eps_pos = (fabs(pos_eps[0])+fabs(pos_eps[1])+fabs(pos_eps[2]))/3.; 

fe = sim_set->eps_pos_thresh*(fabs(pos[0])+fabs(pos[1])+fabs(pos[2]))/objects[k].eps_pos;

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



void adaptive_rkn67_step(planet objects[], settings *sim_set){
int check;
check=1;

check = rkn67_step(objects, sim_set);

}




int rkn78_step(planet objects[], settings *sim_set){

planet object;

int i, j, k, l;
const int n=10;

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

double cifi[3], cdotifi[3];

double dtoau, dte_3, dte_3dtoau;

alpha[0] = 0.;
alpha[1] = 19./375.;
alpha[2] = -0.7;
alpha[3] = 0.1;
alpha[4] = 0.2;
alpha[5] = 0.4;
alpha[6] = 0.6;
alpha[7] = 0.8;
alpha[8] = 1.;
alpha[9] = 1.;

gamma[0][0] = 0.;

gamma[1][0] = 361./281250.;

gamma[2][0] = 10437./7600.;
gamma[2][1] = -343./304.;

gamma[3][0] = 547./319200.;
gamma[3][1] = 1125./342304.;
gamma[3][2] = -1./4729200.;

gamma[4][0] = 74./9975.;
gamma[4][1] = -1125./791578.;
gamma[4][2] = -1./157640.;
gamma[4][3] = 311./22200.;

gamma[5][0] = 1028./29925.;
gamma[5][1] = -6375./1583156.;
gamma[5][2] = -55./319221.;
gamma[5][3] = -13./1665.;
gamma[5][4] = 467./8100.;

gamma[6][0] = 148349./19254600.;
gamma[6][1] = 6375./1583156.;
gamma[6][2] = 0.;
gamma[6][3] = 1299964./14060925.;
gamma[6][4] = 4783./253350.;
gamma[6][5] = 173101./3040200.;

gamma[7][0] = 116719112./18953746875.;
gamma[7][1] = 1125./791578.;
gamma[7][2] = 1680359./2992696875.;
gamma[7][3] = 51962281./585871875.;
gamma[7][4] = 104130509./855056250.;
gamma[7][5] = 1995658./47503125.;
gamma[7][6] = 15029./253125.;

gamma[8][0] = 604055892451./4935014784000.;
gamma[8][1] = 0.;
gamma[8][2] = -206360699./115664409000.;
gamma[8][3] = 0.;
gamma[8][4] = 32963694031./528751584000.;
gamma[8][5] = 9676095011./39166784000.;
gamma[8][6] = 1641775937./176250528000.;
gamma[8][7] = 2851784579./47000140800.;

gamma[9][0] = 67./2016.;
gamma[9][1] = 0.;
gamma[9][2] = 0.;
gamma[9][3] = 440./3969.;
gamma[9][4] = 25./252.;
gamma[9][5] = 425./3024.;
gamma[9][6] = 5./72.;
gamma[9][7] = 625./14112.;
gamma[9][8] = 11./4536.;

c[0] = gamma[9][0];
c[1] = 0.;
c[2] = 0.;
c[3] = gamma[9][3];
c[4] = gamma[9][4];
c[5] = gamma[9][5];
c[6] = gamma[9][6];
c[7] = gamma[9][7];
c[8] = gamma[9][8];

c_dot[0] = 23./2016.;
c_dot[1] = 0.;
c_dot[2] = 0.;
c_dot[3] = 880./3969.;
c_dot[4] = -25./2016.;
c_dot[5] = 1075./3024.;
c_dot[6] = 65./1008.;
c_dot[7] = 4225./14112.;
c_dot[8] = 1087./18144.;

dt = sim_set->timestep * 86400.; // days -> sec
dtsq = dt*dt;
dte_3 = dt*1.e-3;
dtoau = dt/AU;
dte_3dtoau = dte_3*dtoau;

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
	loc[0] = object.pos[0] + vx0*alpha[i]*dtoau + dte_3dtoau * dx;
	loc[1] = object.pos[1] + vy0*alpha[i]*dtoau + dte_3dtoau * dy;
	loc[2] = object.pos[2] + vz0*alpha[i]*dtoau + dte_3dtoau * dz;

	// Evaluate x_dot_dot
	get_acc_vector_toff(objects, sim_set, k, alpha[i]*dt, loc, acc);

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
	
}

pos[0] = object.pos[0] + vx0*dtoau + cifi[0] * dte_3dtoau;
pos[1] = object.pos[1] + vy0*dtoau + cifi[1] * dte_3dtoau;
pos[2] = object.pos[2] + vz0*dtoau + cifi[2] * dte_3dtoau;

vel[0] = object.vel[0] + cdotifi[0]*dte_3;
vel[1] = object.vel[1] + cdotifi[1]*dte_3;
vel[2] = object.vel[2] + cdotifi[2]*dte_3;

pos_eps[0] = c[8]*dte_3dtoau * (fx[8]-fx[9]);
pos_eps[1] = c[8]*dte_3dtoau * (fy[8]-fy[9]);
pos_eps[2] = c[8]*dte_3dtoau * (fz[8]-fz[9]);

// Calculate mean error
objects[k].eps_pos = (fabs(pos_eps[0])+fabs(pos_eps[1])+fabs(pos_eps[2]))/3.; 

fe = sim_set->eps_pos_thresh*(fabs(pos[0])+fabs(pos[1])+fabs(pos[2]))/objects[k].eps_pos;

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



void adaptive_rkn78_step(planet objects[], settings *sim_set){
int check;
check=1;

check = rkn78_step(objects, sim_set);

}


*/

