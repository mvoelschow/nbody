#include "setup.h"

// ***********************************************************
//   Initialize settings
// ***********************************************************
void init_settings(settings *app)
{
	// *******************************
	// Benchmark mode: No output, no checks, maximum CPU stress
	// *******************************
	app->benchmark_mode = 0;

	// Number of threads to use
	app->n_threads = 2;

	// Read object data from input file
	app->setup = 0;

	// *******************************
	// Total number of particles to simulate
	// *******************************
	app->n_bodies = 200;

	// *******************************
	// Numerics
	// *******************************

	// Integrator order {4,5,6}
	app->integrator = 6;

	// Maximum relative error per step for position and velocity
	app->eps_vel_thresh = 1.E-16;
	app->eps_pos_thresh = 1.E-16;

	// *******************************
	// Time settings
	// *******************************

	// Largest simulation timestep [days]
	app->timestep_max = 100.;

	// Time at which the simulation ends [days]
	app->time_end = 1.E3 * YR;

	// *******************************
	// Camera and visual settings
	// *******************************
	// Resolution
	app->res_x = 800;
	app->res_y = 800;

	// Fullscreen mode on/off
	app->fullscreen = 0;

	app->mode_3D = 1;

	// Scale setting on startup [AU]
	app->scale = 16. * AU;

	// Lowest scale setting [AU]
	app->scale_min = 0.1 * AU;

	// Largest scale setting [AU]
	app->scale_max = 100. * AU;

	// Draw background
	app->draw_background = 1;

	// VSYNC on/off (framerate cap at 60 FPS)
	app->vsync = 0;

	// Show every calculated timestep
	app->interactive_mode = 1;

	// Focus on the system's center of mass
	app->focus_on_cms = 1;

	// *******************************
	// Data output
	// *******************************
	// Time interval for automatic data output
	app->output_interval = 0.1 * YR;

	// Automatically output screenshots
	app->auto_screenshot = 1;

	// Automatically output text file with kinetic parameters of all bodies
	app->auto_textfile = 0;
}

// Some technical parameters that (normally) don't need to be altered
void init_technical_parameters(settings *app)
{

	app->center_screen_x = 0.5 * app->res_x;
	app->center_screen_y = 0.5 * app->res_y;
	app->timestep_max = app->timestep;
	app->timestep_min = app->timestep;
	app->scale_max = app->scale;
	app->scale_min = app->scale;
	app->select_box_size = 40;
	app->selected_object = -1;
	app->scale_step = 1.05;
	app->paused = 1;
	app->finished = 0;
	app->resume = 0;
	app->screenshot_counter = 0;
	app->screenshot_trigger = 1;
	app->auto_screenshot_counter = 0;
	app->auto_file_counter = 0;
	app->time_output = 0.;
	app->timestep_counter = 0;
	app->x_rot = 0.;
	app->y_rot = 0.;
	app->icon_size_max = 30.;
}

// ***********************************************************
// Init bodies
// ***********************************************************
void init_bodies(planet objects[], settings *app)
{

	switch (app->setup)
	{
	case 1:
		setup_asteroid_belt_and_planet(objects, app);
		break;
	case 2:
		setup_planetesimals(objects, app);
		break;
	case 3:
		setup_planetary_system(objects, app);
		break;
	case 4:
		setup_stellar_filament(objects, app);
		break;
	case 5:
		setup_stellar_sphere(objects, app);
		break;
	case 6:
		setup_solar_system(objects, app);
		break;
	default:
		setup_asteroid_belt_and_planet(objects, app);
		break;
	}
	//setup_benchmark(objects, sim_set);
}

// ***********************************************************
//   Initialize benchmark
// ***********************************************************
void init_benchmark(settings *app)
{

	app->benchmark_mode = 1;
	app->n_bodies = 8000;
	app->eps_vel_thresh = 1.E-16;
	app->eps_pos_thresh = 1.E-16;
	app->timestep_max = 100.;
	app->time_end = 2.E4 * YR;
	app->res_x = 800;
	app->res_y = 800;
	app->fullscreen = 0;
	app->scale = 16.;
	app->scale_min = 0.1;
	app->scale_max = 100.;
	app->draw_background = 0;
	app->vsync = 0;
	app->interactive_mode = 0;
	app->focus_on_cms = 1;
	app->output_interval = 1.E2 * YR;
	app->auto_screenshot = 0;
	app->auto_textfile = 0;
	app->center_screen_x = 0.5 * app->res_x;
	app->center_screen_y = 0.5 * app->res_y;
	app->timestep = 0.1;
	app->select_box_size = 40;
	app->selected_object = -1;
	app->scale_step = 1.05;
	app->paused = 1;
	app->finished = 0;
	app->screenshot_counter = 0;
	app->screenshot_trigger = 1;
	app->auto_screenshot_counter = 0;
	app->auto_file_counter = 0;
	app->time_output = 0.;
	app->timestep_counter = 0;
	app->x_rot = 0.;
	app->y_rot = 0.;
}

int get_file_lines(char *text)
{
	FILE *fpr;
	int lines = 0;
	int ch = 0;

	fpr = fopen(text, "r");

	if (!fpr)
	{

		lines = 0;
	}
	else
	{

		while (!feof(fpr))
		{
			ch = fgetc(fpr);
			if (ch == '\n')
			{
				lines++;
			}
		}

		fclose(fpr);
	}

	return lines;
}

void output_objects(planet objects[], settings *app)
{
	FILE *fpr;
	char *fname = "objects.dat";
	int i;

	fpr = fopen(fname, "w");

	fprintf(fpr, "%s\n", "msun");
	fprintf(fpr, "%s\n", "au");
	fprintf(fpr, "%s\n", "kms");
	fprintf(fpr, "%s\n", "");

	for (i = 0; i < app->n_bodies; i++)
	{

		fprintf(fpr, "%le %le %le %le %le %le %le %d %lf \n", objects[i].mass / M_SUN, objects[i].pos[0], objects[i].pos[1], objects[i].pos[2], objects[i].vel[0], objects[i].vel[1], objects[i].vel[2], objects[i].icon_num, objects[i].icon_size);
	}
}

void load_settings_file(settings *app)
{
	char *fname = "settings.dat";
	char unit[8];
	char dummy[32];

	FILE *fpr;

	fpr = fopen(fname, "r");

	if (!fpr)
	{
		printf("Error while loading %s. \n", fname);
		exit(1);
	}

	printf("\n Reading simulation settings from settings.dat file:");

	// 1. line: benchmark mode
	if (fscanf(fpr, "%s %d \n", dummy, &app->benchmark_mode) == 2)
	{
		printf("\n %s = %d", dummy, app->benchmark_mode);
	}
	else
	{
		printf("\n Error at line 1.");
		exit(1);
	}

	// 2. line: number of threads
	if (fscanf(fpr, "%s %d \n", dummy, &app->n_threads) == 2)
	{
		printf("\n %s = %d", dummy, app->n_threads);
	}
	else
	{
		printf("\n Error at line 2.");
		exit(1);
	}

	// 3. line: read from data file
	if (fscanf(fpr, "%s %d \n", dummy, &app->setup) == 2)
	{
		printf("\n %s = %d", dummy, app->setup);
	}
	else
	{
		printf("\n Error at line 3.");
		exit(1);
	}

	// 4. line: number of bodies
	if (fscanf(fpr, "%s %d \n", dummy, &app->n_bodies) == 2)
	{
		printf("\n %s = %d", dummy, app->n_bodies);
	}
	else
	{
		printf("\n Error at line 4.");
		exit(1);
	}

	// 5. line: integrator order
	if (fscanf(fpr, "%s %d \n", dummy, &app->integrator) == 2)
	{
		printf("\n %s = %d", dummy, app->integrator);
	}
	else
	{
		printf("\n Error at line 5.");
		exit(1);
	}

	// 6. line: error threshold
	if (fscanf(fpr, "%s %lf \n", dummy, &app->eps_vel_thresh) == 2)
	{
		app->eps_pos_thresh = app->eps_vel_thresh;
		printf("\n %s = %le", dummy, app->eps_vel_thresh);
	}
	else
	{
		printf("\n Error at line 6.");
		exit(1);
	}

	// 7. line: timestep
	if (fscanf(fpr, "%s %lf %s \n", dummy, &app->timestep, unit) == 3)
	{
		printf("\n %s = %lf %s", dummy, app->timestep, unit);
		if (strcmp(unit, "days") == 0)
		{
			app->timestep_max = app->timestep;
		}
		if (strcmp(unit, "yrs") == 0)
		{
			app->timestep_max = app->timestep * YR;
			app->timestep = app->timestep * YR;
		}
		if (strcmp(unit, "Myrs") == 0)
		{
			app->timestep_max = app->timestep * YR * 1.e6;
			app->timestep = app->timestep * YR * 1.e6;
		}
	}
	else
	{
		printf("\n Error at line 7.");
		exit(1);
	}

	// 8. line: Simulation time
	if (fscanf(fpr, "%s %lf %s \n", dummy, &app->time_end, unit) == 3)
	{
		printf("\n %s = %lf %s", dummy, app->time_end, unit);

		if (strcmp(unit, "days") == 0)
		{
			app->time_end = app->time_end;
		}
		if (strcmp(unit, "yrs") == 0)
		{
			app->time_end = app->time_end * YR;
		}
		if (strcmp(unit, "Myrs") == 0)
		{
			app->time_end = app->time_end * YR * 1.e6;
		}
	}
	else
	{
		printf("\n Error at line 8.");
		exit(1);
	}

	// 9. line: x resolution
	if (fscanf(fpr, "%s %d \n", dummy, &app->res_x) == 2)
	{
		printf("\n %s = %d", dummy, app->res_x);
	}
	else
	{
		printf("\n Error at line 9.");
		exit(1);
	}

	// 10. line: y resolution
	if (fscanf(fpr, "%s %d \n", dummy, &app->res_y) == 2)
	{
		printf("\n %s = %d", dummy, app->res_y);
	}
	else
	{
		printf("\n Error at line 10.");
		exit(1);
	}

	// 11. line: fullscreen mode
	if (fscanf(fpr, "%s %d \n", dummy, &app->fullscreen) == 2)
	{
		printf("\n %s = %d", dummy, app->fullscreen);
	}
	else
	{
		printf("\n Error at line 11.");
		exit(1);
	}

	// 12. line: mode 3D
	if (fscanf(fpr, "%s %d \n", dummy, &app->mode_3D) == 2)
	{
		printf("\n %s = %d", dummy, app->mode_3D);
	}
	else
	{
		printf("\n Error at line 12.");
		exit(1);
	}

	// 13. line: scale setting on startup
	if (fscanf(fpr, "%s %lf %s \n", dummy, &app->scale, unit) == 3)
	{
		printf("\n %s = %lf %s", dummy, app->scale, unit);
		if (strcmp(unit, "pc") == 0)
		{
			app->scale = app->scale * PC;
		}
		if (strcmp(unit, "au") == 0)
		{
			app->scale = app->scale;
		}
	}
	else
	{
		printf("\n Error at line 13.");
		exit(1);
	}

	// 14. line: draw background
	if (fscanf(fpr, "%s %d \n", dummy, &app->draw_background) == 2)
	{
		printf("\n %s = %d", dummy, app->draw_background);
	}
	else
	{
		printf("\n Error at line 14.");
		exit(1);
	}

	// 15. line: vsync
	if (fscanf(fpr, "%s %d \n", dummy, &app->vsync) == 2)
	{
		printf("\n %s = %d", dummy, app->vsync);
	}
	else
	{
		printf("\n Error at line 15.");
		exit(1);
	}

	// 16. line: interactive mode
	if (fscanf(fpr, "%s %d \n", dummy, &app->interactive_mode) == 2)
	{
		printf("\n %s = %d", dummy, app->interactive_mode);
	}
	else
	{
		printf("\n Error at line 16.");
		exit(1);
	}

	// 17. line: center of mass-focus
	if (fscanf(fpr, "%s %d \n", dummy, &app->focus_on_cms) == 2)
	{
		printf("\n %s = %d", dummy, app->focus_on_cms);
	}
	else
	{
		printf("\n Error at line 17.");
		exit(1);
	}

	// 18. line: output interval
	if (fscanf(fpr, "%s %lf %s \n", dummy, &app->output_interval, unit) == 3)
	{
		printf("\n %s = %lf %s", dummy, app->output_interval, unit);

		if (strcmp(unit, "days") == 0)
		{
			app->output_interval = app->output_interval;
		}
		if (strcmp(unit, "yrs") == 0)
		{
			app->output_interval = app->output_interval * YR;
		}
		if (strcmp(unit, "Myrs") == 0)
		{
			app->output_interval = app->output_interval * YR * 1.e6;
		}
	}
	else
	{
		printf("\n Error at line 18.");
		exit(1);
	}

	// 19. line: automatic screenshot
	if (fscanf(fpr, "%s %d \n", dummy, &app->auto_screenshot) == 2)
	{
		printf("\n %s = %d", dummy, app->auto_screenshot);
	}
	else
	{
		printf("\n Error at line 19.");
		exit(1);
	}

	// 20. line: automatic textfile
	if (fscanf(fpr, "%s %d \n", dummy, &app->auto_textfile) == 2)
	{
		printf("\n %s = %d", dummy, app->auto_textfile);
	}
	else
	{
		printf("\n Error at line 20.");
		exit(1);
	}

	// 21. line: gpu support
	if (fscanf(fpr, "%s %d \n", dummy, &app->use_gpu) == 2)
	{
		printf("\n %s = %d", dummy, app->use_gpu);
	}
	else
	{
		printf("\n Error at line 21.");
		exit(1);
	}

	// 22. line: autoscale on startup
	if (fscanf(fpr, "%s %d \n", dummy, &app->autoscale_on_startup) == 2)
	{
		printf("\n %s = %d", dummy, app->autoscale_on_startup);
	}
	else
	{
		printf("\n Error at line 22.");
		exit(1);
	}

	printf("\n \n");
}

void read_input_file(planet objects[], settings *app)
{
	char *fname = "input.dat";
	char unit[8];
	char header[1024];
	int i, j;
	FILE *fpr;
	double mass_conv, dist_conv, vel_conv;

	fpr = fopen(fname, "r");

	if (!fpr)
	{
		printf("Error while loading %s. \n", fname);
		exit(1);
	}

	printf("\n Reading object data from input.dat file:");

	// Read first line of input file
	if (fscanf(fpr, "%[^\n]\n", unit) == 1)
	{

		if (strcmp(unit, "msun") == 0)
		{
			printf("\n Mass unit is msun.");
			mass_conv = M_SUN;
		}
		else if (strcmp(unit, "mjup") == 0)
		{
			printf("\n Mass unit is mjup.");
			mass_conv = M_JUP;
		}
		else
		{
			printf("\n Unknown unit at line 1.\n");
			exit(1);
		}
	}
	else
	{
		printf("\n Input error at line 1.\n");
		exit(1);
	}

	// Read second line of input file
	if (fscanf(fpr, "%[^\n]\n", unit) == 1)
	{

		if (strcmp(unit, "pc") == 0)
		{
			printf("\n Coordinates given in pc.");
			dist_conv = PC;
		}
		else if (strcmp(unit, "au") == 0)
		{
			printf("\n Coordinates given in au.");
			dist_conv = 1.;
		}
		else
		{
			printf("Unknown unit at line 2.\n");
			exit(1);
		}
	}
	else
	{
		printf("\n Input error at line 2.\n");
		exit(1);
	}

	// Read third line of input file and skip the following blank line
	if (fscanf(fpr, "%[^\n]\n", unit) == 1)
	{

		if (strcmp(unit, "kms") == 0)
		{
			printf("\n Velocities are km/s.");
			vel_conv = 1.;
		}
		else if (strcmp(unit, "ms") == 0)
		{
			printf("\n Velocities are m/s.");
			vel_conv = 1.e-3;
		}
		else
		{
			printf("Unknown unit at line 3.\n");
			exit(1);
		}
	}
	else
	{
		printf("\n Input error at line 3.\n");
		exit(1);
	}

	if (fscanf(fpr, "%[^\n]\n", header) == 1)
	{
		printf("\n Data header detected.");
	}
	else
	{
		printf("\n Input error at line 4.\n");
		exit(1);
	}

	printf("\n Reading object data ...");

	// Read object data
	for (i = 0; i < app->n_bodies; i++)
	{

		objects[i].ident = i;
		if (fscanf(fpr, "%lf %lf %lf %lf %lf %lf %lf %d %lf", &objects[i].mass, &objects[i].pos[0], &objects[i].pos[1], &objects[i].pos[2], &objects[i].vel[0], &objects[i].vel[1], &objects[i].vel[2], &objects[i].icon_num, &objects[i].icon_size) == 9)
		{

			// Unit conversion
			objects[i].mass = objects[i].mass * mass_conv;

			for (j = 0; j < 3; j++)
			{
				objects[i].pos[j] = objects[i].pos[j] * dist_conv;
			}

			for (j = 0; j < 3; j++)
			{
				objects[i].vel[j] = objects[i].vel[j] * vel_conv;
			}
		}
		else
		{
			printf("\n Input error at line %d \n", i + 5);
			exit(1);
		}
	}

	printf(" done.\n\n");

	fclose(fpr);
}
