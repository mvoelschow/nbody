#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <omp.h>
#include "main.h"



void init_euler_gpu(cl_kernel *kernel, cl_device_id *device_id, cl_mem *body_mem_obj, cl_context *context, cl_program *program, settings *sim_set ){
cl_int flag=0;

if ( flag != 0 ){
	printf("\n OpenCL ERROR: buffer creation on computing device failed.");
	SDL_Quit();
}

// Load the kernel source code into the array source_str
FILE *fp;
char *source_str;
size_t source_size;

fp = fopen("kernels/euler.cl", "r");
if (!fp) {
	fprintf(stderr, "Failed to load kernel.\n");
	exit(1);
}

// Create a program from the kernel source
source_str = (char*)malloc(MAX_SOURCE_SIZE);
source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
fclose( fp );
*program = clCreateProgramWithSource(*context, 1, (const char **)&source_str, (const size_t *)&source_size, &flag);

flag = clBuildProgram(*program, 1, device_id, NULL, NULL, NULL);
size_t len=0;
flag = clGetProgramBuildInfo(*program, *device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
char *buildlog = calloc(len, sizeof(char));
flag = clGetProgramBuildInfo(*program, *device_id, CL_PROGRAM_BUILD_LOG, len, buildlog, NULL);

//printf("\n Buildlog:   %s\n",buildlog);

*kernel = clCreateKernel(*program, "evaluate_acc", &flag);

// Set the arguments of the kernel
flag = clSetKernelArg(*kernel, 0, sizeof(cl_mem), (void *)body_mem_obj);

if ( flag == CL_INVALID_KERNEL ) printf("\n CL_INVALID_KERNEL");
if ( flag == CL_INVALID_ARG_INDEX ) printf("\n CL_INVALID_ARG_INDEX");
if ( flag == CL_INVALID_ARG_VALUE ) printf("\n CL_INVALID_ARG_VALUE");
if ( flag == CL_INVALID_MEM_OBJECT ) printf("\n CL_INVALID_MEM_OBJECT");
if ( flag == CL_INVALID_SAMPLER ) printf("\n CL_INVALID_SAMPLER");
if ( flag == CL_INVALID_ARG_SIZE ) printf("\n CL_INVALID_ARG_SIZE");

if (flag != CL_SUCCESS){
	printf("Error: Failed to set kernel argument 0! %d\n", flag);
	exit(1);
}

int n = sim_set->n_bodies;
flag = clSetKernelArg(*kernel, 1, sizeof(int), (void *)&n);

if (flag != CL_SUCCESS){
	printf("Error: Failed to set kernel argument 1! %d\n", flag);
	exit(1);
}

float dt = 86400.*(float)sim_set->timestep;
flag = clSetKernelArg(*kernel, 2, sizeof(float), (void *)&dt);

if (flag != CL_SUCCESS){
	printf("Error: Failed to set kernel argument 2! %d\n", flag);
	exit(1);
}

}





void init_velocity_verlet_gpu(cl_kernel *kernel_1, cl_kernel *kernel_2, cl_device_id *device_id, cl_mem *body_mem_obj, cl_context *context, cl_program *program, settings *sim_set ){
cl_int flag=0;

if ( flag != 0 ){
	printf("\n OpenCL ERROR: buffer creation on computing device failed.");
	SDL_Quit();
}

// Load the kernel source code into the array source_str
FILE *fp;
char *source_str;
size_t source_size;

fp = fopen("kernels/verlet.cl", "r");
if (!fp) {
	fprintf(stderr, "Failed to load kernel.\n");
	exit(1);
}

// Create a program from the kernel source
source_str = (char*)malloc(MAX_SOURCE_SIZE);
source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
fclose( fp );
*program = clCreateProgramWithSource(*context, 1, (const char **)&source_str, (const size_t *)&source_size, &flag);

flag = clBuildProgram(*program, 1, device_id, NULL, NULL, NULL);
size_t len=0;
flag = clGetProgramBuildInfo(*program, *device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
char *buildlog = calloc(len, sizeof(char));
flag = clGetProgramBuildInfo(*program, *device_id, CL_PROGRAM_BUILD_LOG, len, buildlog, NULL);
//printf("\n Buildlog:   %s\n",buildlog);

*kernel_1 = clCreateKernel(*program, "verlet_tic", &flag);
*kernel_2 = clCreateKernel(*program, "verlet_toc", &flag);

// Set the arguments of the kernel for kernel 1
flag = clSetKernelArg(*kernel_1, 0, sizeof(cl_mem), (void *)body_mem_obj);

if ( flag == CL_INVALID_KERNEL ) printf("\n CL_INVALID_KERNEL");
if ( flag == CL_INVALID_ARG_INDEX ) printf("\n CL_INVALID_ARG_INDEX");
if ( flag == CL_INVALID_ARG_VALUE ) printf("\n CL_INVALID_ARG_VALUE");
if ( flag == CL_INVALID_MEM_OBJECT ) printf("\n CL_INVALID_MEM_OBJECT");
if ( flag == CL_INVALID_SAMPLER ) printf("\n CL_INVALID_SAMPLER");
if ( flag == CL_INVALID_ARG_SIZE ) printf("\n CL_INVALID_ARG_SIZE");

if (flag != CL_SUCCESS){
	printf("Error: Failed to set kernel 1 argument 0! %d\n", flag);
	exit(1);
}

int n = sim_set->n_bodies;
flag = clSetKernelArg(*kernel_1, 1, sizeof(int), (void *)&n);

if (flag != CL_SUCCESS){
	printf("Error: Failed to set kernel 1 argument 1! %d\n", flag);
	exit(1);
}

float dt = 86400.*(float)sim_set->timestep;
flag = clSetKernelArg(*kernel_1, 2, sizeof(float), (void *)&dt);

if (flag != CL_SUCCESS){
	printf("Error: Failed to set kernel 1 argument 2! %d\n", flag);
	exit(1);
}

// Set the arguments of the kernel for kernel 2
flag = clSetKernelArg(*kernel_2, 0, sizeof(cl_mem), (void *)body_mem_obj);

if ( flag == CL_INVALID_KERNEL ) printf("\n CL_INVALID_KERNEL");
if ( flag == CL_INVALID_ARG_INDEX ) printf("\n CL_INVALID_ARG_INDEX");
if ( flag == CL_INVALID_ARG_VALUE ) printf("\n CL_INVALID_ARG_VALUE");
if ( flag == CL_INVALID_MEM_OBJECT ) printf("\n CL_INVALID_MEM_OBJECT");
if ( flag == CL_INVALID_SAMPLER ) printf("\n CL_INVALID_SAMPLER");
if ( flag == CL_INVALID_ARG_SIZE ) printf("\n CL_INVALID_ARG_SIZE");

if (flag != CL_SUCCESS){
	printf("Error: Failed to set kernel 2 argument 0! %d\n", flag);
	exit(1);
}

flag = clSetKernelArg(*kernel_2, 1, sizeof(int), (void *)&n);

if (flag != CL_SUCCESS){
	printf("Error: Failed to set kernel 2 argument 1! %d\n", flag);
	exit(1);
}

flag = clSetKernelArg(*kernel_2, 2, sizeof(float), (void *)&dt);

if (flag != CL_SUCCESS){
	printf("Error: Failed to set kernel 2 argument 2! %d\n", flag);
	exit(1);
}

}




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



void get_acc_vector(planet objects[], settings *sim_set, int skip_id, double acc[]){
// loc in AU, acc in m/s²
int i, j;
double a[3]={0.,0.,0.,}, dist, delta[3], GModist;
static const double kAU=1.E3*AU;
const double loc[3]={objects[skip_id].pos_new[0], objects[skip_id].pos_new[1], objects[skip_id].pos_new[2]};

for(i=0; i<sim_set->n_bodies; i++){

	if ( i == skip_id ) continue;

	// AU->m
	for (j=0; j<3; j++) delta[j] = kAU*(objects[i].pos_new[j] - loc[j]);

	// m
	dist = sqrt( delta[0]*delta[0] + delta[1]*delta[1] + delta[2]*delta[2]);

	// SI
	GModist = G_cst * objects[i].mass/(dist*dist*dist);

	for (j=0; j<3; j++) a[j] += GModist * delta[j];
}

for (j=0; j<3; j++) acc[j] = a[j];

}



void get_acc_vector_old(planet objects[], settings *sim_set, int skip_id, double acc[]){
// loc in AU, acc in m/s²
int i, j;
double a[3]={0.,0.,0.,}, dist, delta[3], GModist;
static const double kAU=1.E3*AU;
const double loc[3]={objects[skip_id].pos[0], objects[skip_id].pos[1], objects[skip_id].pos[2]};

for(i=0; i<sim_set->n_bodies; i++){

	if ( i == skip_id ) continue;

	// AU->m
	for (j=0; j<3; j++) delta[j] = kAU*(objects[i].pos[j] - loc[j]);

	// m
	dist = sqrt( delta[0]*delta[0] + delta[1]*delta[1] + delta[2]*delta[2]);

	// SI
	GModist = G_cst * objects[i].mass/(dist*dist*dist);

	for (j=0; j<3; j++) a[j] += GModist * delta[j];
}

for (j=0; j<3; j++) acc[j] = a[j];

}




/*
Only implemented for debugging reasons. I STRONGLY DISCOURAGE ANYONE FROM USING EULER INTEGRATORS ...
*/
void euler_step_gpu(planet objects[], body_gpu bodies[], settings *sim_set, cl_command_queue *command_queue, cl_mem *body_mem_obj, cl_kernel *kernel){

const size_t global_item_size = sim_set->n_bodies; 	// Process the entire list
const size_t local_item_size = 100;
int i, j;
cl_int flag;

// Fill struct bodies with data
for(i=0; i<sim_set->n_bodies; i++){
	for(j=0; j<3; j++) bodies[i].pos[j] = (float)(objects[i].pos[j]);	// position in AU
	for(j=0; j<3; j++) bodies[i].vel[j] = (float)(objects[i].vel[j]);	// vel in km/s
	bodies[i].mass = (float)(objects[i].mass/M_SUN);
}

// Copy struct body to respective memory buffer
flag = clEnqueueWriteBuffer(*command_queue, *body_mem_obj, CL_TRUE, 0, sim_set->n_bodies * sizeof(body_gpu), bodies, 0, NULL, NULL);
if ( flag == CL_INVALID_COMMAND_QUEUE ) printf("command_queue is not a valid command-queue.");
if ( flag == CL_INVALID_CONTEXT) printf("the context associated with command_queue and buffer are not the same or if the context associated with command_queue and events in	event_wait_list are not the same.");
if ( flag == CL_INVALID_MEM_OBJECT) printf("buffer is not a valid buffer object.");
if ( flag == CL_INVALID_VALUE) printf("the region being written specified by (offset, cb) is out of bounds or if ptr is a NULL value.");
if ( flag == CL_INVALID_EVENT_WAIT_LIST) printf("event_wait_list is NULL and num_events_in_wait_list greater than 0, or event_wait_list is not NULL and num_events_in_wait_list is 0, or if event objects in event_wait_list are not valid events.");
if ( flag == CL_MEM_OBJECT_ALLOCATION_FAILURE) printf("there is a failure to allocate memory for data store associated with buffer.");
if ( flag == CL_OUT_OF_HOST_MEMORY) printf("there is a failure to allocate resources required by the OpenCL implementation on the host.");

// Perform calculations
flag = clEnqueueNDRangeKernel(*command_queue, *kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);
if ( flag == CL_INVALID_PROGRAM_EXECUTABLE ) printf("there is no successfully built program executable available for device associated with command_queue.");
if ( flag == CL_INVALID_COMMAND_QUEUE ) printf("command_queue is not a valid command-queue.");
if ( flag == CL_INVALID_KERNEL ) printf("kernel is not a valid kernel object.");
if ( flag == CL_INVALID_CONTEXT ) printf("context associated with command_queue and kernel is not the same or if the context associated with command_queue and events in event_wait_list are not the same.");
if ( flag == CL_INVALID_KERNEL_ARGS ) printf("the kernel argument values have not been specified.");
if ( flag == CL_INVALID_WORK_DIMENSION ) printf("work_dim is not a valid value (i.e. a value between 1 and 3).");
if ( flag == CL_INVALID_WORK_GROUP_SIZE ) printf("local_work_size is specified and number of work-items specified by global_work_size is not evenly divisable by size of work-group given by local_work_size or does not match the work-group size specified for kernel using the __attribute__((reqd_work_group_size(X, Y, Z))) qualifier in program source.");
if ( flag == CL_INVALID_WORK_GROUP_SIZE ) printf("local_work_size is specified and the total number of work-items in the work-group computed as local_work_size[0] *... local_work_size[work_dim - 1] is greater than the value specified by CL_DEVICE_MAX_WORK_GROUP_SIZE in the table of OpenCL Device Queries for clGetDeviceInfo.");
if ( flag == CL_INVALID_WORK_GROUP_SIZE ) printf("local_work_size is NULL and the __attribute__((reqd_work_group_size(X, Y, Z))) qualifier is used to declare the work-group size for kernel in the program source.");
if ( flag == CL_INVALID_WORK_ITEM_SIZE ) printf("the number of work-items specified in any of local_work_size[0], ... local_work_size[work_dim - 1] is greater than the corresponding values specified by CL_DEVICE_MAX_WORK_ITEM_SIZES[0], .... CL_DEVICE_MAX_WORK_ITEM_SIZES[work_dim - 1].");
if ( flag == CL_INVALID_GLOBAL_OFFSET ) printf("global_work_offset is not NULL.");
if ( flag == CL_OUT_OF_RESOURCES ) printf("there is a failure to queue the execution instance of kernel on the command-queue because of insufficient resources needed to execute the kernel. For example, the explicitly specified local_work_size causes a failure to execute the kernel because of insufficient resources such as registers or local memory. Another example would be the number of read-only image args used in kernel exceed the CL_DEVICE_MAX_READ_IMAGE_ARGS value for device or the number of write-only image args used in kernel exceed the CL_DEVICE_MAX_WRITE_IMAGE_ARGS value for device or the number of samplers used in kernel exceed CL_DEVICE_MAX_SAMPLERS for device.");
if ( flag == CL_MEM_OBJECT_ALLOCATION_FAILURE ) printf("there is a failure to allocate memory for data store associated with image or buffer objects specified as arguments to kernel.");
if ( flag == CL_INVALID_EVENT_WAIT_LIST ) printf("event_wait_list is NULL and num_events_in_wait_list > 0, or event_wait_list is not NULL and num_events_in_wait_list is 0, or if event objects in event_wait_list are not valid events.");
if ( flag == CL_OUT_OF_HOST_MEMORY ) printf("there is a failure to allocate resources required by the OpenCL implementation on the host.");

// Read the memory buffer on the device to the local variable
flag = clEnqueueReadBuffer(*command_queue, *body_mem_obj, CL_TRUE, 0, sim_set->n_bodies * sizeof(body_gpu), bodies, 0, NULL, NULL);
if ( flag == CL_INVALID_COMMAND_QUEUE ) printf("command_queue is not a valid command-queue.");
if ( flag == CL_INVALID_CONTEXT) printf("the context associated with command_queue and buffer are not the same or if the context associated with command_queue and events in	event_wait_list are not the same.");
if ( flag == CL_INVALID_MEM_OBJECT) printf("buffer is not a valid buffer object.");
if ( flag == CL_INVALID_VALUE) printf("the region being written specified by (offset, cb) is out of bounds or if ptr is a NULL value.");
if ( flag == CL_INVALID_EVENT_WAIT_LIST) printf("event_wait_list is NULL and num_events_in_wait_list greater than 0, or event_wait_list is not NULL and num_events_in_wait_list is 0, or if event objects in event_wait_list are not valid events.");
if ( flag == CL_MEM_OBJECT_ALLOCATION_FAILURE) printf("there is a failure to allocate memory for data store associated with buffer.");
if ( flag == CL_OUT_OF_HOST_MEMORY) printf("there is a failure to allocate resources required by the OpenCL implementation on the host.");

// Update body data
for(i=0; i<sim_set->n_bodies; i++){
	for(j=0; j<3; j++){
		objects[i].pos[j]= (double)(bodies[i].pos_new[j]);
		objects[i].vel[j]= (double)(bodies[i].vel_new[j]);	
	}		
}

// Increment timestep counter
sim_set->timestep_counter++;

// Increment time according to old timestep
sim_set->time += sim_set->timestep;

return;

}



void velocity_verlet_step_gpu(planet objects[], body_gpu bodies[], settings *sim_set, cl_command_queue *command_queue, cl_mem *body_mem_obj, cl_kernel *kernel_1, cl_kernel *kernel_2){
int i, j;
const size_t global_item_size = sim_set->n_bodies;
const int n_bodies = sim_set->n_bodies;
cl_int flag;



// Fill struct bodies with data
// Some leaky versions of OpenCL need this as a workaround ...
for(i=0; i<n_bodies; i++){
	for(j=0; j<3; j++) bodies[i].pos[j] = (float)(objects[i].pos[j]); 	// position in AU
	for(j=0; j<3; j++) bodies[i].vel[j] = (float)(objects[i].vel[j]);	// vel in km/s
	bodies[i].mass = (float)(objects[i].mass/M_SUN);	
}


// Copy struct body to respective memory buffer
flag = clEnqueueWriteBuffer(*command_queue, *body_mem_obj, CL_TRUE, 0, n_bodies * sizeof(body_gpu), bodies, 0, NULL, NULL);

// Perform calculations
flag = clEnqueueNDRangeKernel(*command_queue, *kernel_1, 1, NULL, &global_item_size, NULL, 0, NULL, NULL);
flag = clEnqueueNDRangeKernel(*command_queue, *kernel_2, 1, NULL, &global_item_size, NULL, 0, NULL, NULL);

// Read the memory buffer on the device to the local variable
flag = clEnqueueReadBuffer(*command_queue, *body_mem_obj, CL_TRUE, 0, n_bodies * sizeof(body_gpu), bodies, 0, NULL, NULL);

// Update body data
for(i=0; i<n_bodies; i++){
	for(j=0; j<3; j++) objects[i].pos[j]= (double)(bodies[i].pos[j]);
	for(j=0; j<3; j++) objects[i].vel[j]= (double)(bodies[i].vel[j]);	
}

// Increment timestep counter
sim_set->timestep_counter++;

// Increment time according to old timestep
sim_set->time += sim_set->timestep;

return;

}




void velocity_verlet_step(planet objects[], settings *sim_set){

int k, l;
double acc[3];

const int n=sim_set->n_bodies;
const double dt=sim_set->timestep * 86400.;
const double dtoau=dt/AU, dte_3=dt*1.e-3, dte_3_half = 0.5*dte_3;


#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(acc, l)
for(k=0; k<n; k++){

	get_acc_vector_old(objects, sim_set, k, acc);

	for(l=0; l<3; l++) objects[k].vel_new[l] = objects[k].vel[l] + acc[l]*dte_3_half;
	for(l=0; l<3; l++) objects[k].pos_new[l] = objects[k].pos[l] + objects[k].vel_new[l]*dtoau;
}

// Evaluate the acceleration function
#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(acc, l)
for(k=0; k<n; k++){

	get_acc_vector(objects, sim_set, k, acc);

	// Full step velocity estimate
	for(l=0; l<3; l++){
		objects[k].vel[l] = objects[k].vel_new[l] + acc[l]*dte_3_half;
		objects[k].pos[l] = objects[k].pos_new[l];
	}
}

// Increment timestep counter
sim_set->timestep_counter++;

// Increment time according to old timestep
sim_set->time += sim_set->timestep;

return;

}




void forest_ruth_step(planet objects[], settings *sim_set){

int k,l;

double acc[3];

const double dt=sim_set->timestep * 86400.;
const double dtoau=dt/AU, dte_3=dt*1.e-3;
const double theta = 1./(2.-pow(2.,1./3.));

// First cycle
#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(l)
for(k=0; k<sim_set->n_bodies; k++){

	// Assign positions for first acceleration function evaluation
	for(l=0; l<3; l++) objects[k].pos_new[l] = objects[k].pos[l] + theta*objects[k].vel[l]*dtoau*0.5;

}

// Evaluate the acceleration function for all particles and calculate velocity
#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(acc, l)
for(k=0; k<sim_set->n_bodies; k++){

	get_acc_vector(objects, sim_set, k, acc);

	for(l=0; l<3; l++) objects[k].vel_new[l] = objects[k].vel[l] + theta*acc[l]*dte_3;
	for(l=0; l<3; l++) objects[k].pos[l] = objects[k].pos_new[l] + (1.-theta)*objects[k].vel_new[l]*dtoau*0.5;
}

#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(acc, l)
// Evaluate the acceleration function for all particles and calculate velocity
for(k=0; k<sim_set->n_bodies; k++){

	get_acc_vector_old(objects, sim_set, k, acc);

	for(l=0; l<3; l++) objects[k].vel_new[l] = objects[k].vel_new[l] + (1.-2.*theta)*acc[l]*dte_3;	
	for(l=0; l<3; l++) objects[k].pos_new[l] = objects[k].pos[l] + (1.-theta)*objects[k].vel_new[l]*dtoau*0.5;

}

// Final velocity
#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(acc, l)
for(k=0; k<sim_set->n_bodies; k++){

	get_acc_vector(objects, sim_set, k, acc);

	for(l=0; l<3; l++) objects[k].vel[l] = objects[k].vel_new[l] + theta*acc[l]*dte_3;
	for(l=0; l<3; l++) objects[k].pos[l] = objects[k].pos_new[l] + theta*objects[k].vel[l]*dtoau*0.5;

}

// Increment timestep counter
sim_set->timestep_counter++;
// Increment time according to old timestep
sim_set->time = sim_set->time + sim_set->timestep;

return;

}



void position_extended_forest_ruth_step(planet objects[], settings *sim_set){

int k,l;

double acc[3];

const double dt=sim_set->timestep * 86400.;
const double dtoau=dt/AU, dte_3=dt*1.e-3;
const double xi = 0.1786178958448091;
const double lambda = -0.2123418310626054;
const double chi = -0.6626458266981849e-01;

// First cycle
#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(l)
for(k=0; k<sim_set->n_bodies; k++){

	// Assign positions for first acceleration function evaluation
	for(l=0; l<3; l++) objects[k].pos[l] = objects[k].pos[l] + xi*objects[k].vel[l]*dtoau;

}

// Evaluate the acceleration function for all particles and calculate velocity
#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(acc, l)
for(k=0; k<sim_set->n_bodies; k++){

	get_acc_vector_old(objects, sim_set, k, acc);

	for(l=0; l<3; l++) objects[k].vel_new[l] = objects[k].vel[l] + (1.-2.*lambda)*acc[l]*dte_3*0.5;	
	for(l=0; l<3; l++) objects[k].pos_new[l] = objects[k].pos[l] + chi*objects[k].vel_new[l]*dtoau;
}


// Evaluate the acceleration function for all particles and calculate velocity
#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(acc, l)
for(k=0; k<sim_set->n_bodies; k++){

	get_acc_vector(objects, sim_set, k, acc);

	for(l=0; l<3; l++) objects[k].vel_new[l] = objects[k].vel_new[l] + lambda*acc[l]*dte_3;
	for(l=0; l<3; l++) objects[k].pos[l] = objects[k].pos_new[l] + (1.-2.*(chi+xi))*objects[k].vel_new[l]*dtoau;
}



// Evaluate the acceleration function for all particles and calculate velocity
#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(acc, l)
for(k=0; k<sim_set->n_bodies; k++){

	get_acc_vector_old(objects, sim_set, k, acc);

	for(l=0; l<3; l++) objects[k].vel_new[l] = objects[k].vel_new[l] + lambda*acc[l]*dte_3;
	for(l=0; l<3; l++) objects[k].pos_new[l] = objects[k].pos[l] + chi*objects[k].vel_new[l]*dtoau;
}


// Evaluate the acceleration function for all particles and calculate velocity
#pragma omp parallel for schedule(static) default (none) shared(objects, sim_set) private(acc, l)
for(k=0; k<sim_set->n_bodies; k++){

	get_acc_vector(objects, sim_set, k, acc);

	// Assign final positions and velocities
	for(l=0; l<3; l++) objects[k].vel[l] = objects[k].vel_new[l] + (1.-2.*lambda)*acc[l]*dte_3*0.5;
	for(l=0; l<3; l++) objects[k].pos[l] = objects[k].pos_new[l] + xi*objects[k].vel[l]*dtoau;
	

}

// Increment timestep counter
sim_set->timestep_counter++;
// Increment time according to old timestep
sim_set->time = sim_set->time + sim_set->timestep;

return;

}





void adaptive_rkn4_step(planet objects[], settings *sim_set){

int i, j, k, recalculate=0;
static const int n=5;

double acc[3];
double alphai_dtoau;

double fx[sim_set->n_bodies][n];
double fy[sim_set->n_bodies][n];
double fz[sim_set->n_bodies][n];

static const double c[5]={13./120., 0.3, 3./40., 1./60., 0.};
static const double c_dot[5]={0.125, 0.375, 0.375, 0.125, 0.};
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
double t_go;

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
		// m/s² for all three blocks
		objects[k].cifi[0] += fx[k][i]*c[i];
		objects[k].cifi[1] += fy[k][i]*c[i];
		objects[k].cifi[2] += fz[k][i]*c[i];
	
		objects[k].cdotifi[0] += fx[k][i]*c_dot[i];
		objects[k].cdotifi[1] += fy[k][i]*c_dot[i];
		objects[k].cdotifi[2] += fz[k][i]*c_dot[i];
	
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

	objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*objects[k].fe_min));

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
		recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Make sure we do not go beyond time_end	
	t_go = sim_set->time_end - sim_set->time;
	if ( dt_new>t_go && sim_set->resume == 0 ){
		sim_set->timestep = t_go;
	}
	else{
		// Update timestep size
		sim_set->timestep = dt_new;
		// That's it. Start over again with the new stepsize
	}
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
	sim_set->timestep_counter++;
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

static const double c[7]={11./240., 0., 108./475., 8./45., 125./2736., 1./300., 0.};
static const double c_dot[7]={1./24., 0., 27./95., 1./3., 125./456., 1./15., 0.};
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
const double inv_or_p_one = 1./6.;
double t_go;

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
		// m/s² for all three blocks
		objects[k].cifi[0] += fx[k][i]*c[i];
		objects[k].cifi[1] += fy[k][i]*c[i];
		objects[k].cifi[2] += fz[k][i]*c[i];
	
		objects[k].cdotifi[0] += fx[k][i]*c_dot[i];
		objects[k].cdotifi[1] += fy[k][i]*c_dot[i];
		objects[k].cdotifi[2] += fz[k][i]*c_dot[i];
		
	
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

	objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*pow(objects[k].fe_min, inv_or_p_one)));

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
	recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Make sure we do not go beyond time_end	
	t_go = sim_set->time_end - sim_set->time;
	if ( dt_new>t_go && sim_set->resume == 0 ){
		sim_set->timestep = t_go;
	}
	else{
		// Update timestep size
		sim_set->timestep = dt_new;
		// That's it. Start over again with the new stepsize
	}
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
	sim_set->timestep_counter++;
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

static const double c[8]={61./1008., 0., 475./2016., 25./504., 125./1008., 25./1008., 11./2016., 0.};
static const double c_dot[8]={19./288.,0.,25./96.,25./144.,25./144.,25./96.,19./288., 0.};
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
const double inv_or_p_one = 1./7.;
double t_go;


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
		// m/s² for all three blocks
		objects[k].cifi[0] += fx[k][i]*c[i];
		objects[k].cifi[1] += fy[k][i]*c[i];
		objects[k].cifi[2] += fz[k][i]*c[i];
	
		objects[k].cdotifi[0] += fx[k][i]*c_dot[i];
		objects[k].cdotifi[1] += fy[k][i]*c_dot[i];
		objects[k].cdotifi[2] += fz[k][i]*c_dot[i];
	
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

	objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*pow(objects[k].fe_min, inv_or_p_one)));

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
	recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Make sure we do not go beyond time_end	
	t_go = sim_set->time_end - sim_set->time;
	if ( dt_new>t_go && sim_set->resume == 0 ){
		sim_set->timestep = t_go;
	}
	else{
		// Update timestep size
		sim_set->timestep = dt_new;
		// That's it. Start over again with the new stepsize
	}
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
	sim_set->timestep_counter++;
	// Increment time according to old timestep
	sim_set->time = sim_set->time + sim_set->timestep;
	// Update timestep size
	sim_set->timestep = dt_new;
	return;

}

}





void adaptive_rkn7_step(planet objects[], settings *sim_set){

int i, j, k, recalculate=0;
static const int n=10;

double acc[3];
double alphai_dtoau;

double fx[sim_set->n_bodies][n];
double fy[sim_set->n_bodies][n];
double fz[sim_set->n_bodies][n];

static const double c[10]={67./2016.,0.,0.,440./3969.,25./252.,425./3024.,5./72.,625./14112.,11./4536.,0.};
static const double c_dot[10]={23./2016.,0.,0.,880./3969.,-25./2016.,1075./3024.,65./1008.,4225./14112.,1087./18144.,0.};
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
const double inv_or_p_one = 0.125;
double t_go;

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
		// m/s² for all three blocks
		objects[k].cifi[0] += fx[k][i]*c[i];
		objects[k].cifi[1] += fy[k][i]*c[i];
		objects[k].cifi[2] += fz[k][i]*c[i];
	
		objects[k].cdotifi[0] += fx[k][i]*c_dot[i];
		objects[k].cdotifi[1] += fy[k][i]*c_dot[i];
		objects[k].cdotifi[2] += fz[k][i]*c_dot[i];
	
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

	objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*pow(objects[k].fe_min, inv_or_p_one)));

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
	recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Make sure we do not go beyond time_end	
	t_go = sim_set->time_end - sim_set->time;
	if ( dt_new>t_go && sim_set->resume == 0 ){
		sim_set->timestep = t_go;
	}
	else{
		// Update timestep size
		sim_set->timestep = dt_new;
		// That's it. Start over again with the new stepsize
	}
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
	sim_set->timestep_counter++;
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

const double c[12]={121./4200.,0.,0.,0,43./525., 33./350.,17./140., 3./56., 31./1050., 512./5775., 1./550., 0.};
const double c_dot[12]={41./840.,0.,0.,0., 34./105.,9./35.,9./280., 9./280., 9./35., 0., 41./840., 0.};
double dt_new=1.e100;

const double alpha[12]={0.,7./80.,7./40, 5./12., 0.5, 1./6., 1./3., 2./3., 5./6., 1./12., 1., 1.};
const double gamma[12][11]={	{0., 0., 0., 0., 0., 0., 0., 0., 0.,0.,0.},
				{49./12800., 0., 0., 0., 0., 0., 0., 0., 0.,0.,0.},
				{49./9600, 49./4800., 0., 0., 0., 0., 0.,0.,0.,0.,0.,},
				{16825./381024., -625./11907., 18125./190512., 0., 0., 0.,0.,0.,0.,0.,0.,},
				{23./840., 0., 50./609., 9./580.,0., 0.,0.,0.,0.,0.,0.},
				{533./68040., 0., 5050./641277.,-19./5220., 23./12636., 0., 0., 0.,0.,0.,0.},
				{-4469./85050., 0., -2384000./641277., 3896./19575., -1451./15795., 502./135., 0., 0., 0.,0.,0.},
				{694./10125., 0., 0., -5504./10125., 424./2025., -104./2025., 364./675., 0., 0.,0.,0.},
				{30203./691200., 0., 0., 0., 9797./172800., 79391./518400., 20609./345600., 70609./2073600.,0.,0.,0.},
				{1040381917./14863564800., 0., 548042275./109444608., 242737./5345280., 569927617./6900940800., -2559686731./530841600., -127250389./353894400., -53056229./2123366400., 23./5120.,0.,0.},
				{-33213637./179088000., 0., 604400./324597., 63826./445875., 0., -6399863./2558400., 110723./511680., 559511./35817600., 372449./7675200., 756604./839475.,0.},
				{121./4200.,0.,0.,0.,43./525., 33./350.,17./140., 3./56., 31./1050., 512./5775., 1./550.} };

const double dt=sim_set->timestep * 86400.;
const double dtoau=dt/AU, dte_3=dt*1.e-3, dte_3dtoau=dte_3*dtoau;
const double c10dte_3dtoau = c[10]*dte_3dtoau;
const double inv_or_p_one = 1./9.;
double t_go;

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
		// m/s² for all three blocks
		objects[k].cifi[0] += fx[k][i]*c[i];
		objects[k].cifi[1] += fy[k][i]*c[i];
		objects[k].cifi[2] += fz[k][i]*c[i];
	
		objects[k].cdotifi[0] += fx[k][i]*c_dot[i];
		objects[k].cdotifi[1] += fy[k][i]*c_dot[i];
		objects[k].cdotifi[2] += fz[k][i]*c_dot[i];
	
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

	objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*pow(objects[k].fe_min, inv_or_p_one)));

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
	recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Make sure we do not go beyond time_end	
	t_go = sim_set->time_end - sim_set->time;
	if ( dt_new>t_go && sim_set->resume == 0 ){
		sim_set->timestep = t_go;
	}
	else{
		// Update timestep size
		sim_set->timestep = dt_new;
		// That's it. Start over again with the new stepsize
	}
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
	sim_set->timestep_counter++;
	// Increment time according to old timestep
	sim_set->time = sim_set->time + sim_set->timestep;
	// Update timestep size
	sim_set->timestep = dt_new;
	return;

}

}



void adaptive_rkn9_step(planet objects[], settings *sim_set){

int i, j, k, recalculate=0;
const int n=15;

double acc[3];
double alphai_dtoau;

double fx[sim_set->n_bodies][n];
double fy[sim_set->n_bodies][n];
double fz[sim_set->n_bodies][n];

const double c[15]={8501./268800.,0.,0.,0.,0.,0.,7101./44800.,1857./11200.,27./8960.,1377./22400.,561./11200.,27./2240.,1539./89600.,1./3840., 0.};
const double c_dot[15]={2827./89600.,0.,0.,0.,0.,0.,16011/89600.,2733./11200.,999./44800.,4779./44800.,2103./11200.,27./1120.,15471./89600.,2887./89600., 0.};
double dt_new=1.e100;

const double alpha[15]={0.,1./8.,1./4.,5./6.,5./9.,2./9.,1./9.,1./3.,4./9.,5./9.,2./3.,7./9.,8./9.,1.,1.};
const double gamma[15][14]={	{0., 0., 0., 0., 0., 0., 0., 0., 0.,0.,0.,0.,0.,0.},
				{1./128., 0., 0., 0., 0., 0., 0., 0., 0.,0.,0.,0.,0.,0.},
				{1./96.,1./48., 0., 0., 0., 0., 0.,0.,0.,0.,0.,0.,0.,0.},
				{925/1944.,-250/243.,875/972., 0., 0., 0.,0.,0.,0.,0.,0.,0.,0.,0.},
				{575./13122., 0., 5000./45927., 25./15309.,0., 0.,0.,0.,0.,0.,0.,0.,0.,0.},
				{11918./820125., 0., 33536./2525985.,1664./1913625., -148./37125., 0., 0., 0.,0.,0.,0.,0.,0.,0.},
				{26669/6561000., 0., -30656/2525985., -976/21049875., 1511/4009500., 4463/320760., 0., 0., 0.,0.,0.,0.,0.,0.},
				{331./27000., 0., 0., -28./482625., 1./2250., 43./3960., 5./156., 0., 0.,0.,0.,0.,0.,0.},
				{56./3645., 0., 0., 0.,0., 16./1215., 64./1215., 64./3645., 0.,0.,0.,0.,0.,0.},
				{4493159./5495493600., 0., 59150752./528941259., 43880008./57301969725., -4467641./13433428800., -14421911./67167144., 101171201./635034816., 8422393./73273248., -447031./24424416.,0.,0.,0.,0.,0.},
				{4540709./175896000., 0., -130288./3627855., -221552./1179052875., 0., 84323./1209285., 2743075./36586368., 82937./1407168., 12547./1407168., 77193881./3869712000.,0.,0.,0.,0. },
				{20435635759./311411304000.,0.,-260376736./881568765., -181105004./95503282875.,-78067026887./7612276320000., 288366050303./532859342400., -81897956141./1049571432000., -516053297./24220879200., 53450249./1794139200.,1068019598143./17761978080000.,8745859./694008000.,0.,0., 0.},
				{1502334950729./62126555148000.,0.,12568960./528941259.,36094208./286509848625.,32136481./51901884000.,-2005294163./39964450680.,808953327787./5127906139200.,2873425163851./18223789510080.,-3808059547./276118022880.,3351482704201./50621637528000.,22215607./1977922800.,12047./689472.,0.,0.},
				{10111757./307176800.,0.,0.,0.,0.,0.,95712041./614353600.,18442353./122870720.,5297501./61435360.,-56555829./614353600.,29101081./153588400.,-1279537./24574144.,171./5774.,0.},
				{8501./268800.,0.,0.,0.,0.,0.,7101./44800.,1857./11200.,27./8960.,1377./22400.,561./11200.,27./2240.,1539./89600.,1./3840.} };

const double dt=sim_set->timestep * 86400.;
const double dtoau=dt/AU, dte_3=dt*1.e-3, dte_3dtoau=dte_3*dtoau;
const double c13dte_3dtoau = c[13]*dte_3dtoau;
const double inv_or_p_one = 0.1;
double t_go;

/*
for(i=1; i<n-1; i++){
	sum=0.;
	for(j=0; j<i; j++) sum+=gamma[i][j];
	printf("\n %.17g", sum/(alpha[i]*alpha[i]/2.));
}
*/
/*
sum=0.;
for(i=0; i<14; i++) sum+=c_dot[i];
printf("\n %.17g", sum);

exit(0);
*/
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
		// m/s² for all three blocks
		objects[k].cifi[0] += fx[k][i]*c[i];
		objects[k].cifi[1] += fy[k][i]*c[i];
		objects[k].cifi[2] += fz[k][i]*c[i];
	
		objects[k].cdotifi[0] += fx[k][i]*c_dot[i];
		objects[k].cdotifi[1] += fy[k][i]*c_dot[i];
		objects[k].cdotifi[2] += fz[k][i]*c_dot[i];
	
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
	objects[k].pos_eps[0] = c13dte_3dtoau*(fx[k][13]-fx[k][14]);
	objects[k].pos_eps[1] = c13dte_3dtoau*(fy[k][13]-fy[k][14]);
	objects[k].pos_eps[2] = c13dte_3dtoau*(fz[k][13]-fz[k][14]);

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

	objects[k].dt_new_guess = sim_set->timestep * fmin(2., fmax(0.2,0.9*pow(objects[k].fe_min, inv_or_p_one)));

	// Chose the smallest timestep estimate for the next step
	if ( objects[k].dt_new_guess < dt_new ) dt_new = objects[k].dt_new_guess;

	// Check error thresholds
	if( objects[k].fe_min < 1. ){
		recalculate=1;
	}

}

if ( recalculate == 1 ) {
	// Make sure we do not go beyond time_end	
	t_go = sim_set->time_end - sim_set->time;
	if ( dt_new>t_go && sim_set->resume == 0 ){
		sim_set->timestep = t_go;
	}
	else{
		// Update timestep size
		sim_set->timestep = dt_new;
		// That's it. Start over again with the new stepsize
	}
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
	sim_set->timestep_counter++;
	// Increment time according to old timestep
	sim_set->time += sim_set->timestep;
	// Update timestep size
	sim_set->timestep = dt_new;
	return;

}

}
