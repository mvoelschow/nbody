#include "hud.h"

void get_planar_screen_coordinates(planet objects[], settings *app)
{
	int i;
	double scale_factor;
	double center_x, center_y;
	double x, y, z;
	double sin_y_rot, cos_y_rot, sin_x_rot, cos_x_rot;

	sin_y_rot = sin(app->y_rot * deg_to_rad);
	cos_y_rot = cos(app->y_rot * deg_to_rad);

	sin_x_rot = sin(app->x_rot * deg_to_rad);
	cos_x_rot = cos(app->x_rot * deg_to_rad);

	center_x = app->center_screen_x;
	center_y = app->center_screen_y;

	for (i = 0; i < app->n_bodies; i = i + 1)
	{

		x = objects[i].pos[0];
		y = objects[i].pos[1];
		z = objects[i].pos[2];

		scale_factor = app->res_x / app->scale;

		objects[i].screen_pos[0] = center_x + scale_factor * x * cos_y_rot + scale_factor * z * sin_y_rot;
		objects[i].screen_pos[1] = center_y + scale_factor * x * sin_x_rot * sin_y_rot + scale_factor * y * cos_x_rot - scale_factor * z * sin_x_rot * cos_y_rot;
	}
}

void zoom_in_at_mouse_position(settings *app)
{
	double scale_new;
	double x, y, scale_factor;
	double x_new, y_new, scale_factor_new;
	int mx, my;

	SDL_GetMouseState(&mx, &my);

	scale_factor = app->res_x / app->scale;

	// x and y at monitor center for current scale setting
	x = (mx - app->center_screen_x) / scale_factor;
	y = (my - app->center_screen_y) / scale_factor;

	scale_new = app->scale / app->scale_step;

	app->scale = scale_new;

	scale_factor_new = app->res_x / scale_new;

	// x and y at monitor center for new scale setting
	x_new = (mx - app->center_screen_x) / scale_factor_new;
	y_new = (my - app->center_screen_y) / scale_factor_new;

	app->center_screen_x -= (x - x_new) * scale_factor_new;
	app->center_screen_y -= (y - y_new) * scale_factor_new;
}

void zoom_out_at_mouse_position(settings *app)
{
	double scale_new;
	double x, y, scale_factor;
	double x_new, y_new, scale_factor_new;
	int mx, my;

	SDL_GetMouseState(&mx, &my);

	scale_factor = app->res_x / app->scale;

	// x and y at monitor center for current scale setting
	x = (mx - app->center_screen_x) / scale_factor;
	y = (my - app->center_screen_y) / scale_factor;

	scale_new = app->scale * app->scale_step;

	app->scale = scale_new;

	scale_factor_new = app->res_x / scale_new;

	// x and y at monitor center for new scale setting
	x_new = (mx - app->center_screen_x) / scale_factor_new;
	y_new = (my - app->center_screen_y) / scale_factor_new;

	app->center_screen_x -= (x - x_new) * scale_factor_new;
	app->center_screen_y -= (y - y_new) * scale_factor_new;
}

void zoom_out(settings *app)
{
	double scale_new;
	double x, y, scale_factor;
	double x_new, y_new, scale_factor_new;

	scale_factor = app->res_x / app->scale;

	// x and y at monitor center for current scale setting
	x = (0.5 * app->res_x - app->center_screen_x) / scale_factor;
	y = (0.5 * app->res_y - app->center_screen_y) / scale_factor;

	scale_new = app->scale * app->scale_step;

	app->scale = scale_new;

	scale_factor_new = app->res_x / scale_new;

	// x and y at monitor center for new scale setting
	x_new = (0.5 * app->res_x - app->center_screen_x) / scale_factor_new;
	y_new = (0.5 * app->res_y - app->center_screen_y) / scale_factor_new;

	app->center_screen_x -= (x - x_new) * scale_factor_new;
	app->center_screen_y -= (y - y_new) * scale_factor_new;
}

void zoom_in(settings *app)
{
	double scale_new;
	double x, y, scale_factor;
	double x_new, y_new, scale_factor_new;

	scale_factor = app->res_x / app->scale;

	// x and y at monitor center for current scale setting
	x = (0.5 * app->res_x - app->center_screen_x) / scale_factor;
	y = (0.5 * app->res_y - app->center_screen_y) / scale_factor;

	scale_new = app->scale / app->scale_step;

	app->scale = scale_new;

	scale_factor_new = app->res_x / scale_new;

	// x and y at monitor center for new scale setting
	x_new = (0.5 * app->res_x - app->center_screen_x) / scale_factor_new;
	y_new = (0.5 * app->res_y - app->center_screen_y) / scale_factor_new;

	app->center_screen_x -= (x - x_new) * scale_factor_new;
	app->center_screen_y -= (y - y_new) * scale_factor_new;
}

void render_text(SDL_Renderer *renderer, char *text, int x, int y, int w, int h, SDL_Color color, TTF_Font *fnt)
{
	SDL_Rect stretchRect;
	SDL_Surface *sText;
	SDL_Texture *text_texture;

	stretchRect.x = x;
	stretchRect.y = y;
	stretchRect.w = w;
	stretchRect.h = h;

	// sprintf(s, "%f", jd_tdb);

	sText = TTF_RenderText_Blended(fnt, text, color);
	text_texture = SDL_CreateTextureFromSurface(renderer, sText);

	SDL_FreeSurface(sText);
	SDL_RenderCopy(renderer, text_texture, NULL, &stretchRect);

	SDL_DestroyTexture(text_texture);
}

void render_dynamic(SDL_Renderer *renderer, char buffer[20], double value, int x, int y, int w, int h, SDL_Color color, TTF_Font *fnt)
{
	SDL_Rect stretchRect;
	SDL_Surface *sText;
	SDL_Texture *text_texture;
	char s[20];

	stretchRect.x = x;
	stretchRect.y = y;
	stretchRect.w = w;
	stretchRect.h = h;

	sprintf(s, buffer, value);

	sText = TTF_RenderText_Blended(fnt, s, color);
	text_texture = SDL_CreateTextureFromSurface(renderer, sText);

	SDL_FreeSurface(sText);
	SDL_RenderCopy(renderer, text_texture, NULL, &stretchRect);

	SDL_DestroyTexture(text_texture);
}

void render_float(SDL_Renderer *renderer, double value, int x, int y, int w, int h, SDL_Color color, TTF_Font *fnt)
{
	SDL_Rect stretchRect;
	SDL_Surface *sText;
	SDL_Texture *text_texture;
	char s[20];

	stretchRect.x = x;
	stretchRect.y = y;
	stretchRect.w = w;
	stretchRect.h = h;

	sprintf(s, "%-14.1f", value);

	sText = TTF_RenderText_Blended(fnt, s, color);
	text_texture = SDL_CreateTextureFromSurface(renderer, sText);

	SDL_FreeSurface(sText);
	SDL_RenderCopy(renderer, text_texture, NULL, &stretchRect);

	SDL_DestroyTexture(text_texture);
}

void render_exponential(SDL_Renderer *renderer, double value, int x, int y, int w, int h, SDL_Color color, TTF_Font *fnt)
{
	SDL_Rect stretchRect;
	SDL_Surface *sText;
	SDL_Texture *text_texture;
	char s[20];

	stretchRect.x = x;
	stretchRect.y = y;
	stretchRect.w = w;
	stretchRect.h = h;

	sprintf(s, "%-14.1e", value);

	sText = TTF_RenderText_Blended(fnt, s, color);
	text_texture = SDL_CreateTextureFromSurface(renderer, sText);

	SDL_FreeSurface(sText);
	SDL_RenderCopy(renderer, text_texture, NULL, &stretchRect);

	SDL_DestroyTexture(text_texture);
}

void render_integer(SDL_Renderer *renderer, int value, int x, int y, int w, int h, SDL_Color color, TTF_Font *fnt)
{
	SDL_Rect stretchRect;
	SDL_Surface *sText;
	SDL_Texture *text_texture;
	char s[20];

	stretchRect.x = x;
	stretchRect.y = y;
	stretchRect.w = w;
	stretchRect.h = h;

	sprintf(s, "%i", value);

	sText = TTF_RenderText_Blended(fnt, s, color);
	text_texture = SDL_CreateTextureFromSurface(renderer, sText);

	SDL_FreeSurface(sText);
	SDL_RenderCopy(renderer, text_texture, NULL, &stretchRect);

	SDL_DestroyTexture(text_texture);
}

void render_integer_02(SDL_Renderer *renderer, int value, int x, int y, int w, int h, SDL_Color color, TTF_Font *fnt)
{
	SDL_Rect stretchRect;
	SDL_Surface *sText;
	SDL_Texture *text_texture;
	char s[20];

	stretchRect.x = x;
	stretchRect.y = y;
	stretchRect.w = w;
	stretchRect.h = h;

	sprintf(s, "%02i", value);

	sText = TTF_RenderText_Blended(fnt, s, color);
	text_texture = SDL_CreateTextureFromSurface(renderer, sText);

	SDL_FreeSurface(sText);
	SDL_RenderCopy(renderer, text_texture, NULL, &stretchRect);

	SDL_DestroyTexture(text_texture);
}

void render_object_info_box(SDL_Renderer *renderer, planet objects[], TTF_Font *fntCourier, settings *app)
{
	int select;
	double v;
	static const SDL_Color clrWhite = {255, 255, 255, 255};

	select = app->selected_object;

	// Calculate some interesting stuff

	// Absolute value of velocity [km/s]
	v = sqrt(objects[select].vel[0] * objects[select].vel[0] + objects[select].vel[1] * objects[select].vel[1] + objects[select].vel[2] * objects[select].vel[2]);

	// Render box

	// Object number
	render_text(renderer, "#", app->res_x - 110, app->res_y - 300, 30, 30, clrWhite, fntCourier);
	render_integer(renderer, objects[select].ident, app->res_x - 60, app->res_y - 300, 50, 30, clrWhite, fntCourier);

	// Object mass
	render_text(renderer, "M", app->res_x - 140, app->res_y - 270, 20, 30, clrWhite, fntCourier);
	render_float(renderer, objects[select].mass / M_SUN, app->res_x - 110, app->res_y - 270, 100, 30, clrWhite, fntCourier);
	render_text(renderer, "M_sun", app->res_x - 40, app->res_y - 270, 30, 30, clrWhite, fntCourier);

	// Object velocity
	render_text(renderer, "v", app->res_x - 140, app->res_y - 240, 20, 30, clrWhite, fntCourier);
	render_float(renderer, v, app->res_x - 110, app->res_y - 240, 100, 30, clrWhite, fntCourier);
	render_text(renderer, "km/s", app->res_x - 40, app->res_y - 240, 30, 30, clrWhite, fntCourier);

	// Position
	//render_float(renderer, objects[select].pos[2], app->res_x-110, app->res_y-210, 100, 30, clrWhite, fntCourier);
}

void render_paused(SDL_Renderer *renderer, TTF_Font *fntCourier, settings *app)
{

	SDL_Color clrWhite = {255, 255, 255, 255};

	render_text(renderer, "Paused.", app->res_x - 100, app->res_y - 40, 90, 40, clrWhite, fntCourier);
}

void render_scale_setting(SDL_Renderer *renderer, TTF_Font *fntCourier, settings *app)
{
	SDL_Rect stretchRect;
	static const SDL_Color clrWhite = {255, 255, 255, 255};

	render_text(renderer, "Box width", app->res_x - 100, 0, 90, 40, clrWhite, fntCourier);

	if (app->scale > app->scale_max)
		app->scale_max = app->scale;
	if (app->scale < app->scale_min)
		app->scale_min = app->scale;

	stretchRect.x = app->res_x - 100.;
	stretchRect.y = 40.;
	stretchRect.w = 100. * log(1. + 9999. * app->scale / app->scale_max) / log(9999.);
	stretchRect.h = 20;

	// Set drawing color to red
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

	SDL_RenderFillRect(renderer, &stretchRect);

	render_exponential(renderer, app->scale, app->res_x - 100, 60, 100, 40, clrWhite, fntCourier);
	render_text(renderer, "au", app->res_x - 25, 60, 20, 40, clrWhite, fntCourier);
}

void render_timestep_setting(SDL_Renderer *renderer, TTF_Font *fntCourier, settings *app)
{
	SDL_Rect stretchRect;
	static const SDL_Color clrWhite = {255, 255, 255, 255};

	render_text(renderer, "Timestep", 10, app->res_y - 100, 100, 40, clrWhite, fntCourier);

	stretchRect.x = 10.;
	stretchRect.y = app->res_y - 60.;

	if (app->timestep > app->timestep_max)
		app->timestep_max = app->timestep;
	if (app->timestep < app->timestep_min)
		app->timestep_min = app->timestep;

	//if( app->timestep < app->timestep_max){
	stretchRect.w = 100. * log(1. + 9999. * app->timestep / app->timestep_max) / log(9999.);
	//}
	//else{
	//stretchRect.w = 100.;
	//}
	stretchRect.h = 20;

	// Set drawing color to blue
	SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

	SDL_RenderFillRect(renderer, &stretchRect);

	render_exponential(renderer, app->timestep, 10, app->res_y - 40, 100, 40, clrWhite, fntCourier);
	render_text(renderer, "days", 85, app->res_y - 40, 30, 40, clrWhite, fntCourier);
	//if ( app->auto_timestep == 1 ) render_text(renderer, "AUTO", 10, app->res_y-40, 60, 40, clrWhite, fntCourier);
}

void render_time_information(SDL_Renderer *renderer, TTF_Font *fntCourier, settings *app)
{
	static const SDL_Color clrWhite = {255, 255, 255, 255};

	render_text(renderer, "Time", 10, 0, 60, 40, clrWhite, fntCourier);
	render_exponential(renderer, app->time / YR, 10, 30, 100, 40, clrWhite, fntCourier);
	render_text(renderer, "yrs", 90, 30, 30, 40, clrWhite, fntCourier);
}

void render_hud(SDL_Renderer *renderer, TTF_Font *fntCourier, settings *app, planet objects[])
{

	// Scale setting
	render_scale_setting(renderer, fntCourier, app);

	// Timestep setting
	render_timestep_setting(renderer, fntCourier, app);

	// Time information
	render_time_information(renderer, fntCourier, app);

	// Object info box
	//if ( app->selected_object != -1) render_object_info_box(renderer, objects, fntCourier, app);
}
