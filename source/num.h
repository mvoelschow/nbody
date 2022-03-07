#pragma once

#define likely(x)      __builtin_expect(!!(x), 1)
#define unlikely(x)    __builtin_expect(!!(x), 0)

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <omp.h>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

#include "data.h"
#include "cst.h"

void init_euler_gpu(cl_kernel *kernel, cl_device_id *device_id, cl_mem *body_mem_obj, cl_context *context, cl_program *program, settings *sim_set );
void init_velocity_verlet_gpu(cl_kernel *kernel_1, cl_kernel *kernel_2, cl_device_id *device_id, cl_mem *body_mem_obj, cl_context *context, cl_program *program, settings *sim_set );
void clear_numerics(planet objects[], settings *sim_set);
void euler_step_gpu(planet objects[], body_gpu bodies[], settings *sim_set, cl_command_queue *command_queue, cl_mem *body_mem_obj, cl_kernel *kernel);
void velocity_verlet_step_gpu(planet objects[], body_gpu bodies[], settings *sim_set, cl_command_queue *command_queue, cl_mem *body_mem_obj, cl_kernel *kernel_1, cl_kernel *kernel_2);
void velocity_verlet_step(planet objects[], settings *sim_set);
void forest_ruth_step(planet objects[], settings *sim_set);
void position_extended_forest_ruth_step(planet objects[], settings *sim_set);
void adaptive_rkn4_step(planet objects[], settings *sim_set);
void adaptive_rkn5_step(planet objects[], settings *sim_set);
void adaptive_rkn6_step(planet objects[], settings *sim_set);
void adaptive_rkn7_step(planet objects[], settings *sim_set);
void adaptive_rkn8_step(planet objects[], settings *sim_set);
void adaptive_rkn9_step(planet objects[], settings *sim_set);