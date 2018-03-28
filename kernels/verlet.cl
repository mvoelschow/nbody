struct body_gpu{
	float vel[3];
	float pos[3];
	float vel_new[3];
	float pos_new[3];
	float mass;
};


__kernel void verlet_tic(__global struct body_gpu *bodies, int n, float dt) {
    
// Get the index of the current element
const int i = get_global_id(0);
float a[3]={0.,0.,0.,};
float rinv, mfac;
float d[3];
const float loc[3]={bodies[i].pos[0], bodies[i].pos[1], bodies[i].pos[2]};

// Calculate interaction_sums
for(int k=0; k<i; k++){
	for(int j=0;j<3;j++) d[j] = (bodies[k].pos[j]-loc[j]);	// AU

	rinv = rsqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);			// AU

	mfac = 3.964164165e-14f*bodies[k].mass*rinv*rinv*rinv;			// 1/s^2

	for(int j=0;j<3;j++) a[j] += mfac*d[j];				// AU/s^2
}

for(int k=i+1; k<n; k++){
	for(int j=0;j<3;j++) d[j] = (bodies[k].pos[j]-loc[j]);

	rinv = rsqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);

	mfac = 3.964164165e-14f*bodies[k].mass*rinv*rinv*rinv;

	for(int j=0;j<3;j++) a[j] += mfac*d[j];
}

// Assign new position and velocity
for(int j=0;j<3;j++) bodies[i].vel_new[j] = bodies[i].vel[j]+a[j]*dt*7.47989355e7f;
for(int j=0;j<3;j++) bodies[i].pos_new[j] = loc[j]+bodies[i].vel_new[j]*dt*6.684587e-9f;

}


__kernel void verlet_toc(__global struct body_gpu *bodies, __private int n, __private float dt) {
    
// Get the index of the current element
const int i = get_global_id(0);
float a[3]={0.,0.,0.,};
float rinv, mfac;
float d[3];
const float loc[3]={bodies[i].pos_new[0], bodies[i].pos_new[1], bodies[i].pos_new[2]};

// Calculate interaction_sums
for(int k=0; k<i; k++){
	for(int j=0;j<3;j++) d[j] = (bodies[k].pos_new[j]-loc[j]);	// AU

	rinv = rsqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);			// AU

	mfac = 3.964164165e-14f*bodies[k].mass*rinv*rinv*rinv;			// 1/s^2

	for(int j=0;j<3;j++) a[j] += mfac*d[j];				// AU/s^2
}

for(int k=i+1; k<n; k++){
	for(int j=0;j<3;j++) d[j] = (bodies[k].pos_new[j]-loc[j]);

	rinv = rsqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);

	mfac = 3.964164165e-14f*bodies[k].mass*rinv*rinv*rinv;

	for(int j=0;j<3;j++) a[j] += mfac*d[j];
}

// Assign new position and velocity
for(int j=0;j<3;j++) bodies[i].vel[j] = bodies[i].vel_new[j]+a[j]*dt*7.47989355e7f;
for(int j=0;j<3;j++) bodies[i].pos[j] = bodies[i].pos_new[j];

}
