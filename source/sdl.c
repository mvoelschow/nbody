#include "sdl.h"

typedef struct z_struct
{
	double z_projected;
	int plot_order;
} z_struct;

void apply_autoscale(settings *app, planet objects[])
{
	int i;
	double dx, dy, dz;
	double r, r_max = 0.;

	// Reset rotation
	app->x_rot = 0.;
	app->y_rot = 0.;

	center_at_cms(app, objects);

	// Find maximum cms distance
	for (i = 0; i < app->n_bodies; i++)
	{

		dx = objects[i].pos[0] - app->cms[0];
		dy = objects[i].pos[1] - app->cms[1];
		dz = objects[i].pos[2] - app->cms[2];

		// calculate x-y distance
		r = sqrt(dx * dx + dy * dy + dz * dz);

		if (r > r_max)
			r_max = r;
	}

	app->scale = 2.05 * r_max;

	center_at_cms(app, objects);

	// Reset scale min/max
	app->scale_max = app->scale;
	app->scale_min = app->scale;
}

void center_at_cms(settings *app, planet objects[])
{
	int i, j;
	double scale_factor;
	double x, y, z;
	double sin_y_rot, cos_y_rot, sin_x_rot, cos_x_rot;
	double sx, sy;

	sin_y_rot = sin(app->y_rot * deg_to_rad);
	cos_y_rot = cos(app->y_rot * deg_to_rad);

	sin_x_rot = sin(app->x_rot * deg_to_rad);
	cos_x_rot = cos(app->x_rot * deg_to_rad);

	// Clear CMS
	for (j = 0; j < 3; j++)
		app->cms[j] = 0.;

	// Calculate CMS coordinates
	for (i = 0; i < app->n_bodies; i++)
	{

		for (j = 0; j < 3; j++)
			app->cms[j] += objects[i].pos[j] * objects[i].mass;
	}

	for (j = 0; j < 3; j++)
		app->cms[j] = app->cms[j] / app->m_tot;

	x = app->cms[0];
	y = app->cms[1];
	z = app->cms[2];

	scale_factor = app->res_x / app->scale;

	sx = scale_factor * x * cos_y_rot + scale_factor * z * sin_y_rot;
	sy = scale_factor * x * sin_x_rot * sin_y_rot + scale_factor * y * cos_x_rot - scale_factor * z * sin_x_rot * cos_y_rot;

	app->center_screen_x = 0.5 * app->res_x - sx;
	app->center_screen_y = 0.5 * app->res_y - sy;
}

void create_screenshot(SDL_Renderer *renderer, settings *app)
{
	char path[18];

	sprintf(path, "screenshot_%d.bmp", app->screenshot_counter); // puts string into buffer

	SDL_Surface *sshot = SDL_CreateRGBSurface(0, app->res_x, app->res_y, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
	SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888, sshot->pixels, sshot->pitch);
	SDL_SaveBMP(sshot, path);
	SDL_FreeSurface(sshot);

	app->screenshot_counter = app->screenshot_counter + 1;
	app->screenshot_trigger = 0;
}

void switch_interactive_mode(settings *app)
{

	if (app->interactive_mode == 0)
	{
		app->interactive_mode = 1;
	}
	else
	{
		app->interactive_mode = 0;
	}
}

void switch_pause_mode(settings *app)
{

	if (app->paused == 0)
	{
		app->paused = 1;
	}
	else
	{
		app->paused = 0;
	}
}

int processEvents(settings *app, planet objects[])
{
	SDL_Event event;
	int done = 0;
	int x, y;
	int delta_x, delta_y;

	while (SDL_PollEvent(&event))
	{

		switch (event.type)
		{

		case SDL_WINDOWEVENT_CLOSE:
		{
			done = 1;
		}
		break;

		case SDL_KEYDOWN:
		{
			switch (event.key.keysym.sym)
			{

			case SDLK_a:
				apply_autoscale(app, objects);
				break;

			case SDLK_o:
				app->center_screen_x = 0.5 * app->res_x;
				app->center_screen_y = 0.5 * app->res_y;
				break;

			case SDLK_c:
				center_at_cms(app, objects);
				break;

			case SDLK_r:
				app->timestep_max = app->timestep;
				app->timestep_min = app->timestep;
				app->scale_min = app->scale;
				app->scale_max = app->scale;
				break;

			case SDLK_DOWN:
				app->x_rot = app->x_rot - 1.;
				break;

			case SDLK_UP:
				app->x_rot = app->x_rot + 1.;
				break;

			case SDLK_RIGHT:
				app->y_rot = app->y_rot + 1.;
				break;

			case SDLK_LEFT:
				app->y_rot = app->y_rot - 1.;
				break;

			case SDLK_n:
				app->x_rot = 0.;
				app->y_rot = 0.;
				break;

			case SDLK_ESCAPE:
				done = 1;
				break;

			case SDLK_SPACE:
				switch_pause_mode(app);
				break;

			case SDLK_i:
				switch_interactive_mode(app);
				break;

			case SDLK_s:
				app->screenshot_trigger = 1;
				break;

			case SDLK_PAGEUP:
				zoom_in(app);
				break;

			case SDLK_PAGEDOWN:
				zoom_out(app);
				break;
			}
		}
		break;

		case SDL_MOUSEWHEEL:
		{

			if (event.wheel.y > 0)
			{
				zoom_in_at_mouse_position(app);
			}
			else
			{
				zoom_out_at_mouse_position(app);
			}
		}
		break;

		case SDL_MOUSEBUTTONDOWN:
		{

			if (event.button.state == SDL_PRESSED && event.button.button == SDL_BUTTON_LEFT)
			{

				// Get mouse position
				SDL_GetMouseState(&x, &y);

				// Save mouse position at mouse click start
				app->start_x = x;
				app->start_y = y;
			}
			if (event.button.state == SDL_PRESSED && event.button.button == SDL_BUTTON_RIGHT && app->selected_object == -1)
			{

				// Get mouse position
				SDL_GetMouseState(&x, &y);
				/*
				// Check if a planet was clicked
				for (i=0;i<app->n_bodies;i++){

					if( x >= objects[i].select_box_x[0] && x <= objects[i].select_box_x[1] && y >= objects[i].select_box_y[0] && y <= objects[i].select_box_y[1]){
						app->selected_object = i;
						break;
					}
					else{
						// Reset selection
						app->selected_object = -1;
					}

				}
*/
			}
		}
		break;

		case SDL_MOUSEBUTTONUP:
		{

			if (event.button.state == SDL_RELEASED && event.button.button == SDL_BUTTON_LEFT)
			{

				// Get mouse position
				SDL_GetMouseState(&x, &y);

				// Assign position to settings type
				app->end_x = x;
				app->end_y = y;

				// calculate motion delta
				delta_x = app->start_x - app->end_x;
				delta_y = app->start_y - app->end_y;

				app->center_screen_x = app->center_screen_x - delta_x;
				app->center_screen_y = app->center_screen_y - delta_y;
			}
		}
		break;

		case SDL_QUIT:
			done = 1;
			break;
		}
	}

	return done;
}

void Clear_to_black(SDL_Renderer *renderer, settings *app)
{

	// Set drawing color to black
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // (Red, Green, Blue, Alpha)

	// Clear the screen to black
	SDL_RenderClear(renderer);
}

void Draw_Background(SDL_Renderer *renderer, SDL_Texture *background, settings *app)
{
	SDL_Rect stretchRect;

	if (app->draw_background == 1)
	{
		stretchRect.x = 0;
		stretchRect.y = 0;
		stretchRect.w = app->res_x;
		stretchRect.h = app->res_y;

		SDL_RenderCopy(renderer, background, NULL, &stretchRect);
	}
	else
	{
		Clear_to_black(renderer, app);
	}
}

void Render_Screen(SDL_Renderer *renderer)
{
	// Present now what we've drawn
	SDL_RenderPresent(renderer);
}

void load_texture(SDL_Renderer *renderer, SDL_Texture **texture, char *path)
{
	SDL_Surface *img_surf;

	img_surf = SDL_LoadBMP(path);

	if (img_surf == NULL)
	{
		printf("Error.");
		SDL_Quit();
	}

	SDL_SetColorKey(img_surf, SDL_TRUE, SDL_MapRGB(img_surf->format, 255, 0, 255));
	*texture = SDL_CreateTextureFromSurface(renderer, img_surf);

	SDL_FreeSurface(img_surf);
}

void load_object_textures(SDL_Renderer *renderer, settings *app)
{

	load_texture(renderer, &app->icon_sun, "sprites/sun_icon.bmp");
	load_texture(renderer, &app->icon_mercury, "sprites/mercury_icon.bmp");
	load_texture(renderer, &app->icon_venus, "sprites/venus_icon.bmp");
	load_texture(renderer, &app->icon_earth, "sprites/earth_icon.bmp");
	load_texture(renderer, &app->icon_mars, "sprites/mars_icon.bmp");
	load_texture(renderer, &app->icon_moon, "sprites/moon_icon.bmp");
	load_texture(renderer, &app->icon_mars, "sprites/mars_icon.bmp");
	load_texture(renderer, &app->icon_jupiter, "sprites/jupiter_icon.bmp");
	load_texture(renderer, &app->icon_saturn, "sprites/saturn_icon.bmp");
	load_texture(renderer, &app->icon_uranus, "sprites/uranus_icon.bmp");
	load_texture(renderer, &app->icon_neptune, "sprites/neptune_icon.bmp");
	load_texture(renderer, &app->icon_pluto, "sprites/pluto_icon.bmp");
}

void render_icon(SDL_Renderer *renderer, planet *object, settings *app)
{
	SDL_Rect stretchRect;

	stretchRect.x = round(object->screen_pos[0] - 0.5 * object->icon_size);
	stretchRect.y = round(object->screen_pos[1] - 0.5 * object->icon_size);
	stretchRect.w = round(object->icon_size);
	stretchRect.h = round(object->icon_size);

	switch (object->icon_num)
	{
	case 0:
		SDL_RenderCopy(renderer, app->icon_sun, NULL, &stretchRect);
		break;
	case 1:
		SDL_RenderCopy(renderer, app->icon_mercury, NULL, &stretchRect);
		break;
	case 2:
		SDL_RenderCopy(renderer, app->icon_venus, NULL, &stretchRect);
		break;
	case 3:
		SDL_RenderCopy(renderer, app->icon_earth, NULL, &stretchRect);
		break;
	case 4:
		SDL_RenderCopy(renderer, app->icon_mars, NULL, &stretchRect);
		break;
	case 5:
		SDL_RenderCopy(renderer, app->icon_jupiter, NULL, &stretchRect);
		break;
	case 6:
		SDL_RenderCopy(renderer, app->icon_saturn, NULL, &stretchRect);
		break;
	case 7:
		SDL_RenderCopy(renderer, app->icon_uranus, NULL, &stretchRect);
		break;
	case 8:
		SDL_RenderCopy(renderer, app->icon_neptune, NULL, &stretchRect);
		break;
	case 9:
		SDL_RenderCopy(renderer, app->icon_pluto, NULL, &stretchRect);
		break;
	case 10:
		SDL_RenderCopy(renderer, app->icon_moon, NULL, &stretchRect);
		break;
	default:
		SDL_RenderCopy(renderer, app->icon_sun, NULL, &stretchRect);
		break;
	}
}

void render_icon_size_brightness(SDL_Renderer *renderer, planet *object, settings *app, double size, double brightness)
{
	SDL_Rect stretchRect;
	SDL_Texture *icon_modified;
	int scale;

	stretchRect.x = round(object->screen_pos[0] - 0.5 * size);
	stretchRect.y = round(object->screen_pos[1] - 0.5 * size);
	stretchRect.w = round(size);
	stretchRect.h = round(size);

	scale = (int)(255 * brightness);

	switch (object->icon_num)
	{
	case 0:
		icon_modified = app->icon_sun;
		break;
	case 1:
		icon_modified = app->icon_mercury;
		break;
	case 2:
		icon_modified = app->icon_venus;
		break;
	case 3:
		icon_modified = app->icon_earth;
		break;
	case 4:
		icon_modified = app->icon_mars;
		break;
	case 5:
		icon_modified = app->icon_jupiter;
		break;
	case 6:
		icon_modified = app->icon_saturn;
		break;
	case 7:
		icon_modified = app->icon_uranus;
		break;
	case 8:
		icon_modified = app->icon_neptune;
		break;
	case 9:
		icon_modified = app->icon_pluto;
		break;
	case 10:
		icon_modified = app->icon_moon;
		break;
	default:
		icon_modified = app->icon_sun;
		break;
	}

	SDL_SetTextureColorMod(icon_modified, scale, scale, scale);

	SDL_RenderCopy(renderer, icon_modified, NULL, &stretchRect);
}

void render_all_bodies(SDL_Renderer *renderer, planet objects[], settings *app)
{
	int i;

	for (i = 0; i < app->n_bodies; i = i + 1)
	{
		render_icon_size_brightness(renderer, &objects[i], app, 10, 1.);
		//render_icon(renderer, &objects[i], app);
	}
}

int compare_structs(const void *a, const void *b)
{
	z_struct *struct_a = (z_struct *)a;
	z_struct *struct_b = (z_struct *)b;
	if (struct_a->z_projected < struct_b->z_projected)
		return 1;
	else if (struct_a->z_projected == struct_b->z_projected)
		return 0;
	else
		return -1;
}

void render_all_bodies_2D(SDL_Renderer *renderer, planet objects[], settings *app)
{
	int i, i_plot;
	double sin_y_rot, cos_y_rot, sin_x_rot, cos_x_rot, delta;
	z_struct z[app->n_bodies];

	// Calculate screen coordinates
	get_planar_screen_coordinates(objects, app);

	sin_y_rot = sin(app->y_rot * deg_to_rad);
	cos_y_rot = cos(app->y_rot * deg_to_rad);

	sin_x_rot = sin(app->x_rot * deg_to_rad);
	cos_x_rot = cos(app->x_rot * deg_to_rad);

	// Get projected distances
	for (i = 0; i < app->n_bodies; i++)
	{
		z[i].z_projected = (-objects[i].pos[0] * cos_x_rot * sin_y_rot + objects[i].pos[1] * sin_x_rot + objects[i].pos[2] * cos_x_rot * cos_y_rot);
		z[i].plot_order = i;
	}

	qsort(z, app->n_bodies, sizeof(z[0]), compare_structs);
	delta = 0.5 * app->scale;

	for (i = app->n_bodies - 1; i >= 0; i--)
	{
		if (z[i].z_projected > delta)
			continue;

		i_plot = z[i].plot_order;

		render_icon(renderer, &objects[i_plot], app);
	}
}

void render_all_bodies_3D(SDL_Renderer *renderer, planet objects[], settings *app)
{
	int i, i_plot;
	double sin_y_rot, cos_y_rot, sin_x_rot, cos_x_rot;
	double icon_size, min_size, delta;
	double brightness;
	double d_scale;
	z_struct z[app->n_bodies];

	// Calculate screen coordinates
	get_planar_screen_coordinates(objects, app);

	sin_y_rot = sin(app->y_rot * deg_to_rad);
	cos_y_rot = cos(app->y_rot * deg_to_rad);

	sin_x_rot = sin(app->x_rot * deg_to_rad);
	cos_x_rot = cos(app->x_rot * deg_to_rad);

	// Get projected distances
	for (i = 0; i < app->n_bodies; i++)
	{
		z[i].z_projected = (-objects[i].pos[0] * cos_x_rot * sin_y_rot + objects[i].pos[1] * sin_x_rot + objects[i].pos[2] * cos_x_rot * cos_y_rot);
		z[i].plot_order = i;
	}

	qsort(z, app->n_bodies, sizeof(z[0]), compare_structs);
	delta = 0.5 * app->scale;

	for (i = app->n_bodies - 1; i >= 0; i--)
	{
		if (z[i].z_projected > delta)
			continue;

		i_plot = z[i].plot_order;

		d_scale = z[i].z_projected / delta;

		icon_size = objects[i_plot].icon_size * (1. + d_scale);
		min_size = 0.5 * objects[i_plot].icon_size;

		if (icon_size > app->icon_size_max)
			icon_size = app->icon_size_max;
		if (icon_size < min_size)
			icon_size = min_size;

		if (d_scale < 0.)
		{
			brightness = 1. + d_scale;
			if (brightness < 0.5)
				brightness = 0.5;
			if (brightness > 1.)
				brightness = 1.;
		}
		else
		{
			brightness = 1.;
		}

		render_icon_size_brightness(renderer, &objects[i_plot], app, icon_size, brightness);
	}
}
