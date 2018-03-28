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




double JulDate(int D, int M, int Y, double UT){
int y, m, B;

if ( M <= 2 ){
	y = Y-1;
	m = M+12;
}
else{
	y = Y;
	m = M;
}	

if ( Y<1582 ){
	B = -2;
}
else if ( Y == 1582 ){
	if( M<10 ){
		B = -2;
	}
	else if( M == 10 ){
		if( D<=4 ){
			B=-2;
		}
		else{
			B = floor( y/400. ) - floor( y/100. );
		}
	}
	else{
		B = floor( y/400. ) - floor( y/100. );
	}
}			
else{
	B = floor( y/400. ) - floor( y/100. );
}

return floor(365.25*y)+floor(30.6001*(m+1))+B+1720996.5+D+UT/24.;

}


double diff_stencil(double (*f)(double), double x, double h){
	return (-f(x+2*h)+8*f(x+h)-8*f(x-h)+f(x-2*h))/(12.*h);
}


// ***********************************************************
// A random planetary system
// ***********************************************************
void setup_solar_system(planet objects[], settings *sim_set){
int i;
double dist, r, phi;
double v_circ;

// Get system time
time_t now=time(NULL);
struct tm* tm = localtime(&now);

int year = 1900+tm->tm_year;
int month = 1+tm->tm_mon;
int day = tm->tm_mday;
int hour = tm->tm_hour;
int min = tm->tm_min;
double sec = tm->tm_sec;
double UT = hour+min/60.+sec/3600.;

// Transform to Julian date
double jd_now = JulDate(day, month, year, UT);

// Calculate t
double t = (jd_now - 2451545.) / 365250.;

// Sun
objects[0].ident = 0;
objects[0].mass = M_SUN;
objects[0].icon_num = 0;
objects[0].icon_size = 12;

objects[0].pos[0] = Sun_X (t);
objects[0].pos[1] = Sun_Y (t);
objects[0].pos[2] = Sun_Z (t);

// dt estimate: days needed for 0.1" segment of orbit, transformed to Julian millenia
double dt = ((12.*365.)/(360.*3600*10))/365250.;

objects[0].vel[0] = diff_stencil( &Sun_X, t, dt )*AU/(365250.*86400.);
objects[0].vel[1] = diff_stencil( &Sun_Y, t, dt )*AU/(365250.*86400.);
objects[0].vel[2] = diff_stencil( &Sun_Z, t, dt )*AU/(365250.*86400.);

// Mercury
objects[1].ident = 1;
objects[1].mass = 3.3011E23;
objects[1].icon_num = 1;
objects[1].icon_size = 4;

objects[1].pos[0] = Mercury_X (t);
objects[1].pos[1] = Mercury_Y (t);
objects[1].pos[2] = Mercury_Z (t);

// dt estimate: days needed for 0.1" segment of orbit, transformed to Julian millenia
dt = ((88.)/(360.*3600*10))/365250.;

objects[1].vel[0] = diff_stencil( Mercury_X, t, dt )*AU/(365250.*86400.);
objects[1].vel[1] = diff_stencil( Mercury_Y, t, dt )*AU/(365250.*86400.);
objects[1].vel[2] = diff_stencil( Mercury_Z, t, dt )*AU/(365250.*86400.);

// Venus
objects[2].ident = 2;
objects[2].mass = 4.8675E24;
objects[2].icon_num = 2;
objects[2].icon_size = 6;

objects[2].pos[0] = Venus_X (t);
objects[2].pos[1] = Venus_Y (t);
objects[2].pos[2] = Venus_Z (t);

// dt estimate: days needed for 0.1" segment of orbit, transformed to Julian millenia
dt = ((365.)/(360.*3600*10))/365250.;

objects[2].vel[0] = diff_stencil( Venus_X, t, dt )*AU/(365250.*86400.);
objects[2].vel[1] = diff_stencil( Venus_Y, t, dt )*AU/(365250.*86400.);
objects[2].vel[2] = diff_stencil( Venus_Z, t, dt )*AU/(365250.*86400.);

// Earth
objects[3].ident = 3;
objects[3].mass = 5.97237E24;
objects[3].icon_num = 3;
objects[3].icon_size = 6;

objects[3].pos[0] = Earth_X (t);
objects[3].pos[1] = Earth_Y (t);
objects[3].pos[2] = Earth_Z (t);

// dt estimate: days needed for 0.1" segment of orbit, transformed to Julian millenia
dt = ((250.)/(360.*3600*10))/365250.;

objects[3].vel[0] = diff_stencil( Earth_X, t, dt )*AU/(365250.*86400.);
objects[3].vel[1] = diff_stencil( Earth_Y, t, dt )*AU/(365250.*86400.);
objects[3].vel[2] = diff_stencil( Earth_Z, t, dt )*AU/(365250.*86400.);

// Mars
objects[4].ident = 4;
objects[4].mass = 6.4171E23;
objects[4].icon_num = 4;
objects[4].icon_size = 5;

objects[4].pos[0] = Mars_X (t);
objects[4].pos[1] = Mars_Y (t);
objects[4].pos[2] = Mars_Z (t);

// dt estimate: days needed for 0.1" segment of orbit, transformed to Julian millenia
dt = ((687.)/(360.*3600*10))/365250.;

objects[4].vel[0] = diff_stencil( Mars_X, t, dt )*AU/(365250.*86400.);
objects[4].vel[1] = diff_stencil( Mars_Y, t, dt )*AU/(365250.*86400.);
objects[4].vel[2] = diff_stencil( Mars_Z, t, dt )*AU/(365250.*86400.);

// Jupiter
objects[5].ident = 5;
objects[5].mass = 1.8986E27;
objects[5].icon_num = 5;
objects[5].icon_size = 12;

objects[5].pos[0] = Jupiter_X (t);
objects[5].pos[1] = Jupiter_Y (t);
objects[5].pos[2] = Jupiter_Z (t);

// dt estimate: days needed for 0.1" segment of orbit, transformed to Julian millenia
dt = ((12.*365.)/(360.*3600*10))/365250.;

objects[5].vel[0] = diff_stencil( Jupiter_X, t, dt )*AU/(365250.*86400.);
objects[5].vel[1] = diff_stencil( Jupiter_Y, t, dt )*AU/(365250.*86400.);
objects[5].vel[2] = diff_stencil( Jupiter_Z, t, dt )*AU/(365250.*86400.);

// Saturn
objects[6].ident = 6;
objects[6].mass = 5.6836E26;
objects[6].icon_num = 6;
objects[6].icon_size = 15;

objects[6].pos[0] = Saturn_X (t);
objects[6].pos[1] = Saturn_Y (t);
objects[6].pos[2] = Saturn_Z (t);

// dt estimate: days needed for 0.1" segment of orbit, transformed to Julian millenia
dt = ((29.*365.)/(360.*3600*10))/365250.;

objects[6].vel[0] = diff_stencil( Saturn_X, t, dt )*AU/(365250.*86400.);
objects[6].vel[1] = diff_stencil( Saturn_Y, t, dt )*AU/(365250.*86400.);
objects[6].vel[2] = diff_stencil( Saturn_Z, t, dt )*AU/(365250.*86400.);

// Uranus
objects[7].ident = 7;
objects[7].mass = 8.681E25;
objects[7].icon_num = 7;
objects[7].icon_size = 10;

objects[7].pos[0] = Uranus_X (t);
objects[7].pos[1] = Uranus_Y (t);
objects[7].pos[2] = Uranus_Z (t);

// dt estimate: days needed for 0.1" segment of orbit, transformed to Julian millenia
dt = ((87.*365.)/(360.*3600*10))/365250.;

objects[7].vel[0] = diff_stencil( Uranus_X, t, dt )*AU/(365250.*86400.);
objects[7].vel[1] = diff_stencil( Uranus_Y, t, dt )*AU/(365250.*86400.);
objects[7].vel[2] = diff_stencil( Uranus_Z, t, dt )*AU/(365250.*86400.);

// Neptune
objects[8].ident = 8;
objects[8].mass = 1.0243E26;
objects[8].icon_num = 8;
objects[8].icon_size = 10;

objects[8].pos[0] = Neptune_X (t);
objects[8].pos[1] = Neptune_Y (t);
objects[8].pos[2] = Neptune_Z (t);

// dt estimate: days needed for 0.1" segment of orbit, transformed to Julian millenia
dt = ((167.*365.)/(360.*3600*10))/365250.;

objects[8].vel[0] = diff_stencil( Neptune_X, t, dt )*AU/(365250.*86400.);
objects[8].vel[1] = diff_stencil( Neptune_Y, t, dt )*AU/(365250.*86400.);
objects[8].vel[2] = diff_stencil( Neptune_Z, t, dt )*AU/(365250.*86400.);

// Planetesimals between 3 and 5 au
for(i=9; i<sim_set->n_bodies; i++){

	// Set body mass [kg]
	objects[i].mass = 1.E19*(99.*randomDouble()+1.);
	objects[i].icon_num = 1;
	objects[i].icon_size = 2;

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
	objects[i].icon_size = 6;

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



// ***********************************************************
// Benchmark template for a cubic filament of stars aligned on a grid
// ***********************************************************
void setup_benchmark(planet objects[], settings *sim_set){
int i, j, k, n;
double x_start, y_start, z_start, delta;

x_start = -10.*PC;
y_start = -10.*PC;
z_start = -10.*PC;

delta = 1.*PC;

j=-1;
k=0;
n=0;

for(i=0; i<sim_set->n_bodies; i++){

	// Set body mass [kg]
	objects[i].mass = 0.1*(double)(j+2)*M_SUN;
	objects[i].icon_num = 0;
	objects[i].icon_size = 8;

	// Set identifier [DO NOT CHANGE]
	objects[i].ident = i;

	j++;

	if ( j>19 ) {
		j=0;
		k++;
	}

	if ( k>19 ) {
		k=0;
		n++;
	}

	// Set body position [AU]
	// x component
	objects[i].pos[0] = x_start + (double)j*delta;
	// y component
	objects[i].pos[1] = y_start + (double)k*delta;
	// z component
	objects[i].pos[2] = z_start + (double)n*delta;

	// Set body velocity [km/s]
	// x component
	objects[i].vel[0] = 0.;
	// y component
	objects[i].vel[1] = 0.;
	// z component
	objects[i].vel[2] = 0.;

}

}
