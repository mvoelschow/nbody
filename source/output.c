#include "output.h"

void Generate_Output_File(planet objects[], settings *app)
{
	char path[32];
	FILE *ofp;
	int i;

	// Generate filename
	sprintf(path, "output_%d.dat", app->auto_file_counter);

	// Get pointer to file
	ofp = fopen(path, "w");
	if (ofp == NULL)
	{
		fprintf(stderr, "Can't open output file %s!\n", path);
		exit(1);
	}

	// Write file header
	fprintf(ofp, "# Timestep number: ");
	fprintf(ofp, "%i \n", app->timestep_counter);

	fprintf(ofp, "# Simulated time: ");
	fprintf(ofp, "%e", app->time / YR);
	fprintf(ofp, " yr \n");

	for (i = 0; i < app->n_bodies; i++)
	{
		fprintf(ofp, "%i %.17g %.17g %.17g %.17g %.17g %.17g %.17g \n", objects[i].ident, objects[i].mass / M_SUN, objects[i].pos[0], objects[i].pos[1], objects[i].pos[2], objects[i].vel[0], objects[i].vel[1], objects[i].vel[2]);
	}

	// Close file
	fclose(ofp);

	app->auto_file_counter++;
}

void create_auto_screenshot(SDL_Renderer *renderer, settings *app)
{
	char path[18];

	sprintf(path, "auto_%d.bmp", app->auto_screenshot_counter); // puts string into buffer

	SDL_Surface *sshot = SDL_CreateRGBSurface(0, app->res_x, app->res_y, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
	SDL_SaveBMP(sshot, path);
	SDL_FreeSurface(sshot);

	app->auto_screenshot_counter++;
}

void generate_auto_output(SDL_Renderer *renderer, planet objects[], settings *app)
{

	if (app->time >= app->time_output)
	{
		if (app->auto_textfile == 1)
			Generate_Output_File(objects, app);
		if (app->auto_screenshot == 1)
			create_auto_screenshot(renderer, app);
	}
}
