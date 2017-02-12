#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <omp.h>
#include "main.h"



void clear_numerics(planet objects[], settings *sim_set){
int k;

for(k=0; k<sim_set->n_bodies; k++){

	objects[k].cifi[0] = 0;
	objects[k].cifi[1] = 0;
	objects[k].cifi[2] = 0;

	objects[k].cdotifi[0] = 0;
	objects[k].cdotifi[1] = 0;
	objects[k].cdotifi[2] = 0;

	objects[k].d[0] = 0.;
	objects[k].d[1] = 0.;
	objects[k].d[2] = 0.;

}

}



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





void get_acc_vector(planet objects[], settings *sim_set, int skip_id, double acc[]){
// loc in AU, acc in m/s²
int i;
double a_x=0., a_y=0., a_z=0., dist, delta[3], GModist;
static const double kAU=1.E3*AU;
const double loc[3]={objects[skip_id].pos_new[0], objects[skip_id].pos_new[1], objects[skip_id].pos_new[2]};

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




void adaptive_rkn4_step(planet objects[], settings *sim_set){

int i, j, k, recalculate=0;
static const int n=5;

double acc[3];
double alphai_dtoau;

double fx[sim_set->n_bodies][n];
double fy[sim_set->n_bodies][n];
double fz[sim_set->n_bodies][n];

static const double c[4]={13./120., 0.3, 3./40., 1./60.};
static const double c_dot[4]={0.125, 0.375, 0.375, 0.125};
double dt_new=1.e100;

static const double alpha[5]={0., 1./3., 2./3., 1., 1.};
static const double gamma[5][4]={	{0., 0., 0., 0.},
					{1./18., 0., 0., 0.},
					{0., 2./9., 0., 0.},
					{1./3., 0., 1./6., 0.},
					{13./120., 0.3, 3./40., 1./60.} };

const double dt=sim_set->timestep * 86400.;
const double dtoau=dt/AU, dte_3=dt*1.e-3, dte_3dtoau=dte_3*dtoau;
const double c3dte_3dtoau = c[3]*dte_3dtoau;

// Set number of threads
omp_set_num_threads(sim_set->n_threads);

// Calculate f_i values
for(i=0; i<n; i++){

	// Assign positions for the i-th evaluation of the acceleration function
	#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set, i, fx, fy, fz, alphai_dtoau) private(j)
	for(k=0; k<sim_set->n_bodies; k++){

		objects[k].d[0]=0.;
		objects[k].d[1]=0.;
		objects[k].d[2]=0.;	
		
		for(j=0; j<i; j++){
			// m/s²
			objects[k].d[0] += gamma[i][j]*fx[k][j];
			objects[k].d[1] += gamma[i][j]*fy[k][j];
			objects[k].d[2] += gamma[i][j]*fz[k][j];
		}

		// AU
		alphai_dtoau = alpha[i]*dtoau;
		objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*alphai_dtoau + objects[k].d[0]*dte_3dtoau;
		objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*alphai_dtoau + objects[k].d[1]*dte_3dtoau;
		objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*alphai_dtoau + objects[k].d[2]*dte_3dtoau;

	}

	// Evaluate the acceleration function for all particles 
	#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set, fx, fy, fz, i) private(acc)
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
#pragma omp parallel for schedule(static) default (none) shared(sim_set, objects, fx, fy, fz, recalculate) reduction(min: dt_new)
for(k=0; k<sim_set->n_bodies; k++){

	// AU
	objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*dtoau + objects[k].cifi[0]*dte_3dtoau;
	objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*dtoau + objects[k].cifi[1]*dte_3dtoau;
	objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*dtoau + objects[k].cifi[2]*dte_3dtoau;

	// km/s
	objects[k].vel_new[0] = objects[k].vel[0] + objects[k].cdotifi[0]*dte_3;
	objects[k].vel_new[1] = objects[k].vel[1] + objects[k].cdotifi[1]*dte_3;
	objects[k].vel_new[2] = objects[k].vel[2] + objects[k].cdotifi[2]*dte_3;

	// Positional truncation error
	objects[k].pos_eps[0] = c3dte_3dtoau*(fx[k][3]-fx[k][4]);
	objects[k].pos_eps[1] = c3dte_3dtoau*(fy[k][3]-fy[k][4]);
	objects[k].pos_eps[2] = c3dte_3dtoau*(fz[k][3]-fz[k][4]);

	// Calculate total error
	objects[k].fe[0] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[0])/fabs(objects[k].pos_eps[0]);
	objects[k].fe[1] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[1])/fabs(objects[k].pos_eps[1]);
	objects[k].fe[2] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[2])/fabs(objects[k].pos_eps[2]);

	// Clear numerics
	objects[k].cifi[0] = 0;
	objects[k].cifi[1] = 0;
	objects[k].cifi[2] = 0;

	objects[k].cdotifi[0] = 0;
	objects[k].cdotifi[1] = 0;
	objects[k].cdotifi[2] = 0;

	// Find largest error
	objects[k].fe_min = objects[k].fe[0];

	if ( objects[k].fe[1] < objects[k].fe_min ){
		objects[k].fe_min = objects[k].fe[1];
		if ( objects[k].fe[2] < objects[k].fe_min ){
			objects[k].fe_min = objects[k].fe[2];
		}
	}

	if ( sim_set->timestep_smoothing > 1. ){
		objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*pow(objects[k].fe_min, 1./sim_set->timestep_smoothing)));
	}
	else{
		objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*objects[k].fe_min));
	}

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
	recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Update timestep size
	sim_set->timestep = dt_new;
	// That's it. Start over again with the new stepsize
	return;
}
else{

	// All particles passed the error check. Assign new values to final variables and clear numerics
	#pragma omp parallel for schedule(static) default (none) shared(sim_set, objects)
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
	// Update timestep size
	sim_set->timestep = dt_new;
	return;

}

}





void adaptive_rkn5_step(planet objects[], settings *sim_set){

int i, j, k, recalculate=0;
static const int n=7;

double acc[3];
double alphai_dtoau;

double fx[sim_set->n_bodies][n];
double fy[sim_set->n_bodies][n];
double fz[sim_set->n_bodies][n];

static const double c[6]={11./240., 0., 108./475., 8./45., 125./2736., 1./300.};
static const double c_dot[6]={1./24., 0., 27./95., 1./3., 125./456., 1./15.};
double dt_new=1.e100;

static const double alpha[7]={0., 1./12., 1./6., 0.5, 0.8, 1., 1.};
static const double gamma[7][6]={	{0., 0., 0., 0., 0., 0.},
				{1./288., 0., 0., 0., 0., 0.},
				{1./216., 1./108., 0., 0., 0., 0.},
				{0., 0., 0.125, 0., 0., 0.},
				{16./125., 0., 4./125., 4./25., 0., 0.},
				{-247./1152., 0., 12./19., 7./432., 4375./65664., 0.},
				{11./240., 0., 108./475., 8./45., 125./2736., 1./300. } };

const double dt=sim_set->timestep * 86400.;
const double dtoau=dt/AU, dte_3=dt*1.e-3, dte_3dtoau=dte_3*dtoau;
const double c5dte_3dtoau = c[5]*dte_3dtoau;

// Set number of threads
omp_set_num_threads(sim_set->n_threads);

// Calculate f_i values
for(i=0; i<n; i++){

	// Assign positions for the i-th evaluation of the acceleration function
	#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set, i, fx, fy, fz, alphai_dtoau) private(j)
	for(k=0; k<sim_set->n_bodies; k++){

		objects[k].d[0]=0.;
		objects[k].d[1]=0.;
		objects[k].d[2]=0.;	
		
		for(j=0; j<i; j++){
			// m/s²
			objects[k].d[0] += gamma[i][j]*fx[k][j];
			objects[k].d[1] += gamma[i][j]*fy[k][j];
			objects[k].d[2] += gamma[i][j]*fz[k][j];
		}

		// AU
		alphai_dtoau = alpha[i]*dtoau;
		objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*alphai_dtoau + objects[k].d[0]*dte_3dtoau;
		objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*alphai_dtoau + objects[k].d[1]*dte_3dtoau;
		objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*alphai_dtoau + objects[k].d[2]*dte_3dtoau;

	}

	// Evaluate the acceleration function for all particles 
	#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set, fx, fy, fz, i) private(acc)
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
#pragma omp parallel for schedule(static) default (none) shared(sim_set, objects, fx, fy, fz, recalculate) reduction(min: dt_new)
for(k=0; k<sim_set->n_bodies; k++){

	// AU
	objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*dtoau + objects[k].cifi[0]*dte_3dtoau;
	objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*dtoau + objects[k].cifi[1]*dte_3dtoau;
	objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*dtoau + objects[k].cifi[2]*dte_3dtoau;

	// km/s
	objects[k].vel_new[0] = objects[k].vel[0] + objects[k].cdotifi[0]*dte_3;
	objects[k].vel_new[1] = objects[k].vel[1] + objects[k].cdotifi[1]*dte_3;
	objects[k].vel_new[2] = objects[k].vel[2] + objects[k].cdotifi[2]*dte_3;

	// Positional truncation error
	objects[k].pos_eps[0] = c5dte_3dtoau*(fx[k][5]-fx[k][6]);
	objects[k].pos_eps[1] = c5dte_3dtoau*(fy[k][5]-fy[k][6]);
	objects[k].pos_eps[2] = c5dte_3dtoau*(fz[k][5]-fz[k][6]);

	// Calculate total error
	objects[k].fe[0] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[0])/fabs(objects[k].pos_eps[0]);
	objects[k].fe[1] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[1])/fabs(objects[k].pos_eps[1]);
	objects[k].fe[2] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[2])/fabs(objects[k].pos_eps[2]);

	// Clear numerics
	objects[k].cifi[0] = 0;
	objects[k].cifi[1] = 0;
	objects[k].cifi[2] = 0;

	objects[k].cdotifi[0] = 0;
	objects[k].cdotifi[1] = 0;
	objects[k].cdotifi[2] = 0;

	// Find largest error
	objects[k].fe_min = objects[k].fe[0];

	if ( objects[k].fe[1] < objects[k].fe_min ){
		objects[k].fe_min = objects[k].fe[1];
		if ( objects[k].fe[2] < objects[k].fe_min ){
			objects[k].fe_min = objects[k].fe[2];
		}
	}

	if ( sim_set->timestep_smoothing > 1. ){
		objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*pow(objects[k].fe_min, 1./sim_set->timestep_smoothing)));
	}
	else{
		objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*objects[k].fe_min));
	}

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
	recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Update timestep size
	sim_set->timestep = dt_new;
	// That's it. Start over again with the new stepsize
	return;
}
else{

	// All particles passed the error check. Assign new values to final variables and clear numerics
	#pragma omp parallel for schedule(static) default (none) shared(sim_set, objects)
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
	// Update timestep size
	sim_set->timestep = dt_new;
	return;

}

}




void adaptive_rkn6_step(planet objects[], settings *sim_set){

int i, j, k, recalculate=0;
static const int n=8;

double acc[3];
double alphai_dtoau;

double fx[sim_set->n_bodies][n];
double fy[sim_set->n_bodies][n];
double fz[sim_set->n_bodies][n];

static const double c[7]={61./1008., 0., 475./2016., 25./504., 125./1008., 25./1008., 11./2016.};
static const double c_dot[7]={19./288.,0.,25./96.,25./144.,25./144.,25./96.,19./288.};
double dt_new=1.e100;

static const double alpha[8]={0., 0.1, 0.2, 0.4, 0.6, 0.8, 1., 1.};
static const double gamma[8][7]={	{0., 0., 0., 0., 0., 0.},
					{1./200., 0., 0., 0., 0., 0.},
					{1./150., 1./75., 0., 0., 0., 0.},
					{2./75., 0., 4./75., 0., 0., 0.},
					{9./200., 0., 9./100., 9./200., 0., 0.},
					{199./3600., -19./150., 47./120., -119./1200., 89./900., 0.},
					{-179./1824., 17./38., 0., -37./152., 219./456., -157./1824. },
					{61./1008., 0., 475./2016., 25./504., 125./1008., 25./1008., 11./2016.}};

const double dt=sim_set->timestep * 86400.;
const double dtoau=dt/AU, dte_3=dt*1.e-3, dte_3dtoau=dte_3*dtoau;
const double c6dte_3dtoau = c[6]*dte_3dtoau;





// Set number of threads
omp_set_num_threads(sim_set->n_threads);

// Calculate f_i values
for(i=0; i<n; i++){

	// Assign positions for the i-th evaluation of the acceleration function
	#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set, i, fx, fy, fz, alphai_dtoau) private(j)
	for(k=0; k<sim_set->n_bodies; k++){

		objects[k].d[0]=0.;
		objects[k].d[1]=0.;
		objects[k].d[2]=0.;	
		
		for(j=0; j<i; j++){
			// m/s²
			objects[k].d[0] += gamma[i][j]*fx[k][j];
			objects[k].d[1] += gamma[i][j]*fy[k][j];
			objects[k].d[2] += gamma[i][j]*fz[k][j];
		}

		// AU
		alphai_dtoau = alpha[i]*dtoau;
		objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*alphai_dtoau + objects[k].d[0]*dte_3dtoau;
		objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*alphai_dtoau + objects[k].d[1]*dte_3dtoau;
		objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*alphai_dtoau + objects[k].d[2]*dte_3dtoau;

	}

	// Evaluate the acceleration function for all particles 
	#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set, fx, fy, fz, i) private(acc)
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
#pragma omp parallel for schedule(static) default (none) shared(sim_set, objects, fx, fy, fz, recalculate) reduction(min: dt_new)
for(k=0; k<sim_set->n_bodies; k++){

	// AU
	objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*dtoau + objects[k].cifi[0]*dte_3dtoau;
	objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*dtoau + objects[k].cifi[1]*dte_3dtoau;
	objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*dtoau + objects[k].cifi[2]*dte_3dtoau;

	// km/s
	objects[k].vel_new[0] = objects[k].vel[0] + objects[k].cdotifi[0]*dte_3;
	objects[k].vel_new[1] = objects[k].vel[1] + objects[k].cdotifi[1]*dte_3;
	objects[k].vel_new[2] = objects[k].vel[2] + objects[k].cdotifi[2]*dte_3;

	// Positional truncation error
	objects[k].pos_eps[0] = c6dte_3dtoau*(fx[k][6]-fx[k][7]);
	objects[k].pos_eps[1] = c6dte_3dtoau*(fy[k][6]-fy[k][7]);
	objects[k].pos_eps[2] = c6dte_3dtoau*(fz[k][6]-fz[k][7]);

	// Calculate total error
	objects[k].fe[0] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[0])/fabs(objects[k].pos_eps[0]);
	objects[k].fe[1] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[1])/fabs(objects[k].pos_eps[1]);
	objects[k].fe[2] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[2])/fabs(objects[k].pos_eps[2]);

	// Clear numerics
	objects[k].cifi[0] = 0;
	objects[k].cifi[1] = 0;
	objects[k].cifi[2] = 0;

	objects[k].cdotifi[0] = 0;
	objects[k].cdotifi[1] = 0;
	objects[k].cdotifi[2] = 0;

	// Find largest error
	objects[k].fe_min = objects[k].fe[0];

	if ( objects[k].fe[1] < objects[k].fe_min ){
		objects[k].fe_min = objects[k].fe[1];
		if ( objects[k].fe[2] < objects[k].fe_min ){
			objects[k].fe_min = objects[k].fe[2];
		}
	}

	if ( sim_set->timestep_smoothing > 1. ){
		objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*pow(objects[k].fe_min, 1./sim_set->timestep_smoothing)));
	}
	else{
		objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*objects[k].fe_min));
	}

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
	recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Update timestep size
	sim_set->timestep = dt_new;
	// That's it. Start over again with the new stepsize
	return;
}
else{

	// All particles passed the error check. Assign new values to final variables and clear numerics
	#pragma omp parallel for schedule(static) default (none) shared(sim_set, objects)
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
	// Update timestep size
	sim_set->timestep = dt_new;
	return;

}

}




/*
void adaptive_rkn7_step(planet objects[], settings *sim_set){

int i, j, k, recalculate=0;
static const int n=10;

double acc[3];
double alphai_dtoau;

double fx[sim_set->n_bodies][n];
double fy[sim_set->n_bodies][n];
double fz[sim_set->n_bodies][n];

static const double c[9]={67./2016.,0.,0.,440./3969.,25./252.,425./3024.,5./72.,625./14112.,11./4536.};
static const double c_dot[9]={23./2016.,0.,0.,880./3969.,-25./2016.,1075./3024.,65./1008.,4225./14112.,1087./18144.};
double dt_new=1.e100;

static const double alpha[10]={0.,19./375.,-0.7,0.1,0.2, 0.4,0.6,0.8,1., 1.};
static const double gamma[10][9]={	{0., 0., 0., 0., 0., 0., 0., 0., 0.},
					{361./281250., 0., 0., 0., 0., 0., 0., 0., 0.},
					{10437./7600., -343./304., 0., 0., 0., 0., 0., 0., 0.},
					{547./319200., 1125./342304., -1./4729200., 0., 0., 0., 0., 0., 0.},
					{74./9975., -1125./791578., -1./157640., 311./22200., 0., 0., 0., 0., 0.},
					{1028./29925., -6375./1583156., -55./319221., -13./1665., 467./8100., 0., 0., 0., 0.},
					{148349./19254600., 6375./1583156., 0., 1299964./14060925., 4783./253350., 173101./3040200., 0., 0., 0.},
					{116719112./18953746875., 1125./791578., 1680359./2992696875., 51962281./585871875., 104130509./855056250., 1995658./47503125., 15029./253125., 0., 0.},
					{604055892451./4935014784000.,0.,-206360699./115664409000.,0.,32963694031./528751584000.,9676095011./39166784000.,1641775937./176250528000.,2851784579./47000140800.,0.},
					{67./2016.,0.,0.,440./3969.,25./252.,425./3024.,5./72.,625./14112.,11./4536.} };

const double dt=sim_set->timestep * 86400.;
const double dtoau=dt/AU, dte_3=dt*1.e-3, dte_3dtoau=dte_3*dtoau;
const double c8dte_3dtoau = c[8]*dte_3dtoau;


// Set number of threads
omp_set_num_threads(sim_set->n_threads);

// Calculate f_i values
for(i=0; i<n; i++){

	// Assign positions for the i-th evaluation of the acceleration function
	#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set, i, fx, fy, fz, alphai_dtoau) private(j)
	for(k=0; k<sim_set->n_bodies; k++){

		objects[k].d[0]=0.;
		objects[k].d[1]=0.;
		objects[k].d[2]=0.;	
		
		for(j=0; j<i; j++){
			// m/s²
			objects[k].d[0] += gamma[i][j]*fx[k][j];
			objects[k].d[1] += gamma[i][j]*fy[k][j];
			objects[k].d[2] += gamma[i][j]*fz[k][j];
		}

		// AU
		alphai_dtoau = alpha[i]*dtoau;
		objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*alphai_dtoau + objects[k].d[0]*dte_3dtoau;
		objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*alphai_dtoau + objects[k].d[1]*dte_3dtoau;
		objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*alphai_dtoau + objects[k].d[2]*dte_3dtoau;

	}

	// Evaluate the acceleration function for all particles 
	#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set, fx, fy, fz, i) private(acc)
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
#pragma omp parallel for schedule(static) default (none) shared(sim_set, objects, fx, fy, fz, recalculate) reduction(min: dt_new)
for(k=0; k<sim_set->n_bodies; k++){

	// AU
	objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*dtoau + objects[k].cifi[0]*dte_3dtoau;
	objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*dtoau + objects[k].cifi[1]*dte_3dtoau;
	objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*dtoau + objects[k].cifi[2]*dte_3dtoau;

	// km/s
	objects[k].vel_new[0] = objects[k].vel[0] + objects[k].cdotifi[0]*dte_3;
	objects[k].vel_new[1] = objects[k].vel[1] + objects[k].cdotifi[1]*dte_3;
	objects[k].vel_new[2] = objects[k].vel[2] + objects[k].cdotifi[2]*dte_3;

	// Positional truncation error
	objects[k].pos_eps[0] = c8dte_3dtoau*(fx[k][8]-fx[k][9]);
	objects[k].pos_eps[1] = c8dte_3dtoau*(fy[k][8]-fy[k][9]);
	objects[k].pos_eps[2] = c8dte_3dtoau*(fz[k][8]-fz[k][9]);

	// Calculate total error
	objects[k].fe[0] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[0])/fabs(objects[k].pos_eps[0]);
	objects[k].fe[1] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[1])/fabs(objects[k].pos_eps[1]);
	objects[k].fe[2] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[2])/fabs(objects[k].pos_eps[2]);

	// Clear numerics
	objects[k].cifi[0] = 0;
	objects[k].cifi[1] = 0;
	objects[k].cifi[2] = 0;

	objects[k].cdotifi[0] = 0;
	objects[k].cdotifi[1] = 0;
	objects[k].cdotifi[2] = 0;

	// Find largest error
	objects[k].fe_min = objects[k].fe[0];

	if ( objects[k].fe[1] < objects[k].fe_min ){
		objects[k].fe_min = objects[k].fe[1];
		if ( objects[k].fe[2] < objects[k].fe_min ){
			objects[k].fe_min = objects[k].fe[2];
		}
	}

	if ( sim_set->timestep_smoothing > 1. ){
		objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*pow(objects[k].fe_min, 1./sim_set->timestep_smoothing)));
	}
	else{
		objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*objects[k].fe_min));
	}

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
	recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Update timestep size
	sim_set->timestep = dt_new;
	// That's it. Start over again with the new stepsize
	return;
}
else{

	// All particles passed the error check. Assign new values to final variables and clear numerics
	#pragma omp parallel for schedule(static) default (none) shared(sim_set, objects)
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
	// Update timestep size
	sim_set->timestep = dt_new;
	return;

}

}



void adaptive_rkn8_step(planet objects[], settings *sim_set){

int i, j, k, recalculate=0;
const int n=12;

double acc[3];
double alphai_dtoau;

double fx[sim_set->n_bodies][n];
double fy[sim_set->n_bodies][n];
double fz[sim_set->n_bodies][n];

const double c[11]={121./4200.,0.,0.,0,43./525., 33./350.,17./140., 3./56., 31./1050., 512./5775., 1./550.};
const double c_dot[11]={41./840.,0.,0.,0., 34./105.,9./35.,9./280., 9./280., 9./35., 0., 41./840.};
double dt_new=1.e100;

const double alpha[12]={0.,7./80.,7./40, 5./12., 0.5, 1./6., 1./3., 2./3., 5./6., 1./12., 1., 1.};
const double gamma[12][11]={	{0., 0., 0., 0., 0., 0., 0., 0., 0.},
				{49./12800., 0., 0., 0., 0., 0., 0., 0., 0.},
				{49./9600, 49./4800., 0., 0., 0., 0., 0.},
				{16825./381024., -625./11907., 18125./190512., 0., 0., 0.},
				{23./840., 0., 50./609., 9./580.,0., 0.},
				{533./68040., 0., 5050./641277.,-19./5220., 23./12636., 0., 0., 0.},
				{-4469./85050., 0., -2384000./641277., 3896./19575., -1451./15795., 502./135., 0., 0., 0.},
				{694./10125., 0., 0., -5504./10125., 424./2025., -104./2025., 364./675., 0., 0.},
				{30203./691200., 0., 0., 0., 9797./172800., 79391./518400., 20609./345600., 70609./2073600.},
				{1040381917./14863564800., 0., 548042275./109444608., 242737./5345280., 569927617./6900940800., -2559686731./530841600., -127250389./353894400., -53056229./2123366400., 23./5120.},
				{-33213637./179088000., 0., 604400./324597., 63826./445875., 0., -6399863./2558400., 110723./511680., 559511./35817600., 372449./7675200., 756604./839475. },
				{121./4200.,0.,0.,0.,43./525., 33./350.,17./140., 3./56., 31./1050., 512./5775., 1./550.} };

const double dt=sim_set->timestep * 86400.;
const double dtoau=dt/AU, dte_3=dt*1.e-3, dte_3dtoau=dte_3*dtoau;
const double c10dte_3dtoau = c[10]*dte_3dtoau;


// Set number of threads
omp_set_num_threads(sim_set->n_threads);

// Calculate f_i values
for(i=0; i<n; i++){

	// Assign positions for the i-th evaluation of the acceleration function
	#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set, i, fx, fy, fz, alphai_dtoau) private(j)
	for(k=0; k<sim_set->n_bodies; k++){

		objects[k].d[0]=0.;
		objects[k].d[1]=0.;
		objects[k].d[2]=0.;	
		
		for(j=0; j<i; j++){
			// m/s²
			objects[k].d[0] += gamma[i][j]*fx[k][j];
			objects[k].d[1] += gamma[i][j]*fy[k][j];
			objects[k].d[2] += gamma[i][j]*fz[k][j];
		}

		// AU
		alphai_dtoau = alpha[i]*dtoau;
		objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*alphai_dtoau + objects[k].d[0]*dte_3dtoau;
		objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*alphai_dtoau + objects[k].d[1]*dte_3dtoau;
		objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*alphai_dtoau + objects[k].d[2]*dte_3dtoau;

	}

	// Evaluate the acceleration function for all particles 
	#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set, fx, fy, fz, i) private(acc)
	for(k=0; k<sim_set->n_bodies; k++){

		get_acc_vector(objects, sim_set, k, acc);

		// m/s²
		fx[k][i] = acc[0];
		fy[k][i] = acc[1];
		fz[k][i] = acc[2];

		// Increment the interaction sums
		if(i<n){
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
#pragma omp parallel for schedule(static) default (none) shared(sim_set, objects, fx, fy, fz, recalculate) reduction(min: dt_new)
for(k=0; k<sim_set->n_bodies; k++){

	// AU
	objects[k].pos_new[0] = objects[k].pos[0] + objects[k].vel[0]*dtoau + objects[k].cifi[0]*dte_3dtoau;
	objects[k].pos_new[1] = objects[k].pos[1] + objects[k].vel[1]*dtoau + objects[k].cifi[1]*dte_3dtoau;
	objects[k].pos_new[2] = objects[k].pos[2] + objects[k].vel[2]*dtoau + objects[k].cifi[2]*dte_3dtoau;

	// km/s
	objects[k].vel_new[0] = objects[k].vel[0] + objects[k].cdotifi[0]*dte_3;
	objects[k].vel_new[1] = objects[k].vel[1] + objects[k].cdotifi[1]*dte_3;
	objects[k].vel_new[2] = objects[k].vel[2] + objects[k].cdotifi[2]*dte_3;

	// Positional truncation error
	objects[k].pos_eps[0] = c10dte_3dtoau*(fx[k][10]-fx[k][11]);
	objects[k].pos_eps[1] = c10dte_3dtoau*(fy[k][10]-fy[k][11]);
	objects[k].pos_eps[2] = c10dte_3dtoau*(fz[k][10]-fz[k][11]);

	// Calculate total error
	objects[k].fe[0] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[0])/fabs(objects[k].pos_eps[0]);
	objects[k].fe[1] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[1])/fabs(objects[k].pos_eps[1]);
	objects[k].fe[2] = sim_set->eps_pos_thresh*fabs(objects[k].pos_new[2])/fabs(objects[k].pos_eps[2]);

	// Clear numerics
	objects[k].cifi[0] = 0;
	objects[k].cifi[1] = 0;
	objects[k].cifi[2] = 0;

	objects[k].cdotifi[0] = 0;
	objects[k].cdotifi[1] = 0;
	objects[k].cdotifi[2] = 0;

	// Find largest error
	objects[k].fe_min = objects[k].fe[0];

	if ( objects[k].fe[1] < objects[k].fe_min ){
		objects[k].fe_min = objects[k].fe[1];
		if ( objects[k].fe[2] < objects[k].fe_min ){
			objects[k].fe_min = objects[k].fe[2];
		}
	}

	if ( sim_set->timestep_smoothing > 1. ){
		objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*pow(objects[k].fe_min, 1./sim_set->timestep_smoothing)));
	}
	else{
		objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*objects[k].fe_min));
	}

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
	recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Update timestep size
	sim_set->timestep = dt_new;
	// That's it. Start over again with the new stepsize
	return;
}
else{

	// All particles passed the error check. Assign new values to final variables and clear numerics
	#pragma omp parallel for schedule(static) default (none) shared(sim_set, objects)
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
	// Update timestep size
	sim_set->timestep = dt_new;
	return;

}

}

*/
