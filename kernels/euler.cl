struct __attribute__ ((packed)) body_gpu{
	float vel[3];
	float pos[3];
	float vel_new[3];
	float pos_new[3];
	float mass;
};


__kernel void evaluate_acc(__global struct body_gpu *bodies, __private int n, __private float dt) {
    
// Get the index of the current element
int i = get_global_id(0);
int j, k;
float a[3]={0.,0.,0.,};
float r, mfac;
float d[3];

// Calculate interaction_sums
for(k=0; k<i; k++){
	for(j=0;j<3;j++) d[j] = (bodies[k].pos[j]-bodies[i].pos[j]);	// AU

	r = sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);			// AU

	mfac = 3.964164165e-14*bodies[k].mass/(r*r*r);			// 1/s^2

	for(j=0;j<3;j++) a[j] += mfac*d[j];				// AU/s^2
}

for(k=i+1; k<n; k++){
	for(j=0;j<3;j++) d[j] = (bodies[k].pos[j]-bodies[i].pos[j]);

	r = sqrt(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);

	mfac = 3.964164165e-14*bodies[k].mass/(r*r*r);

	for(j=0;j<3;j++) a[j] += mfac*d[j];
}

// Assign new position and velocity
for(j=0;j<3;j++){
	bodies[i].vel_new[j] = bodies[i].vel[j]+a[j]*dt*1.49597871e8;
	bodies[i].pos_new[j] = bodies[i].pos[j]+bodies[i].vel_new[j]*dt*6.684587e-9;
}

}
