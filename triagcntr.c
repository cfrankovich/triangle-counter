#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "text.h"

#define FPS 60 
#define WIDTH 500
#define HEIGHT 500
#define MAXPOINTS 26 

typedef struct {
	char letter;
	int x, y;
} Point;

/* https://en.wikipedia.org/wiki/Midpoint_circle_algorithm */
void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
	const int32_t diameter = (radius * 2);
	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y)
	{
		//  Each of the following renders an octant of the circle
		SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
		SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
		SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

		if (error <= 0)
		{
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0)
		{
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}

}

int main(int argc, char **argv)
{
	bool running, simulate; 
	running = simulate = true;
	SDL_Event event;

	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;

	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Triangle Counter", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, 0);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	TTF_Init();
	TTF_Font *font = TTF_OpenFont(LIBERATION_MONO_REGULAR, 12);

	SDL_Color white = { 255, 255, 255 };

	int totalpoints, totalsolpoints;
	totalpoints = 0;
	totalsolpoints = 0;

	Point *points[MAXPOINTS];
	Point *solpoints[MAXPOINTS];

	char pointcount[32] = "Points: 0\0";
	Text_t pointstext = init_text(pointcount, font, 0, 0, white, renderer);

	int32_t elapsed;
	int frametime;

	while (running)
	{
		elapsed = SDL_GetTicks();
		SDL_PollEvent(&event);
		int mposx, mposy;
		SDL_GetMouseState(&mposx, &mposy);

		switch (event.type)
		{
			case SDL_QUIT:
				printf("Exiting...\n");
				running = false;
				break;

			case SDL_MOUSEBUTTONDOWN:
				Point *newpoint;
				newpoint = malloc(sizeof(Point));
				newpoint->letter = 65 + totalpoints + '0'; 

				bool in;
				int iter;
				in = false;
				for (iter = 0; iter < totalpoints; ++iter)
				{
					if (in) break;
					else if ( (pow(mposx - points[iter]->x, 2) + pow(mposy - points[iter]->y, 2)) <= 49 ) 
					{
						newpoint->x = points[iter]->x;	
						newpoint->y = points[iter]->y;	
						in = true;
					}
				}
				for (iter = 0; iter < totalsolpoints; ++iter)
				{
					if (in) break;
					else if ( (pow(mposx - solpoints[iter]->x, 2) + pow(mposy - solpoints[iter]->y, 2)) <= 49 ) 
					{
						newpoint->x = solpoints[iter]->x;	
						newpoint->y = solpoints[iter]->y;	
						in = true;
					}
				}
				if (!in)
				{
					newpoint->x = mposx;
					newpoint->y = mposy;
				}

				points[totalpoints] = newpoint;
				totalpoints++;
				sprintf(pointcount, "Points: %d", totalpoints);
				pointstext = init_text(pointcount, font, 0, 0, white, renderer);

				if (totalpoints >= 2)
				{
					int i;
					int x1, x2, y1, y2;
					float a1, yi1;

					x1 = points[totalpoints-2]->x;
					y1 = -points[totalpoints-2]->y;
					x2 = points[totalpoints-1]->x;
					y2 = -points[totalpoints-1]->y;

					a1 = (float)(y2 - y1)/(x2 - x1);
					yi1 = -((a1 * x1) - y1);

					for (i = 0; i < totalpoints-2; ++i)
					{
						int x3, x4, y3, y4;
						float a2, yi2;
						x3 = points[i]->x;
						y3 = -points[i]->y;
						x4 = points[i+1]->x;
						y4 = -points[i+1]->y;
						
						if (x4 == x1 && y4 == y1) continue;

						a2 = (float)(y4 - y3)/(x4 - x3);
						yi2 = -((a2 * x3) - y3);

						int x, y;
						x = ((-yi2) - (-yi1)) / ((-a1) - (-a2)); 
						y = -((yi1 * a2) - (yi2 * a1)) / ((-a1) - (-a2));

						/* Checks to see if the solution is on the line */
						if (x < ((x1 < x2) ? x1 : x2) || x > ((x1 > x2) ? x1 : x2)) continue;
						else if (-y > ((y1 > y2) ? y1 : y2) || -y < ((y1 < y2) ? y1 : y2)) continue;

						Point *solpoint;
						solpoint = malloc(sizeof(Point));
						solpoint->letter = '!'; 
						solpoint->x = x;
						solpoint->y = y;
						solpoints[totalsolpoints] = solpoint;
						totalsolpoints++;

					}

				}

				break;

			case SDL_KEYDOWN:
				switch(event.key.keysym.sym)
				{
					case SDLK_c:
						int i;
						for (i = 0; i < totalpoints; ++i) free(points[i]);
						for (i = 0; i < totalsolpoints; ++i) free(solpoints[i]); 
						totalpoints = 0;
						totalsolpoints = 0;
						sprintf(pointcount, "Points: %d", totalpoints);
						pointstext = init_text(pointcount, font, 0, 0, white, renderer);
						break;

					case SDLK_q:
						printf("Exiting...\n");
						running = false;
						break;
				}
				break;

		}


		/* Background */
		SDL_SetRenderDrawColor(renderer, 0x17, 0x17, 0x17, 0xFF);
		SDL_RenderClear(renderer);

		/* Points */
		int iter;
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		for (iter = 0; iter < totalpoints; ++iter)
		{
			SDL_RenderDrawPoint(renderer, points[iter]->x, points[iter]->y);
			if ( (pow(mposx - points[iter]->x, 2) + pow(mposy - points[iter]->y, 2)) <= 400 ) 
				DrawCircle(renderer, points[iter]->x, points[iter]->y, 7);

			/* (x-h)^2 + (y-k)^2 = r^2 */
			if ( (pow(mposx - points[iter]->x, 2) + pow(mposy - points[iter]->y, 2)) <= 49 ) 
			{
				SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
				for (int i = 0; i < 7; ++i)
					DrawCircle(renderer, points[iter]->x, points[iter]->y, i);
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}

		}
		/* This is stupid but it works */
		for (iter = 0; iter < totalsolpoints; ++iter)
		{
			SDL_RenderDrawPoint(renderer, solpoints[iter]->x, solpoints[iter]->y);
			if ( (pow(mposx - solpoints[iter]->x, 2) + pow(mposy - solpoints[iter]->y, 2)) <= 400 ) 
				DrawCircle(renderer, solpoints[iter]->x, solpoints[iter]->y, 7);

			if ( (pow(mposx - solpoints[iter]->x, 2) + pow(mposy - solpoints[iter]->y, 2)) <= 49 ) 
			{
				SDL_SetRenderDrawColor(renderer, 0xAA, 0xFF, 0xAA, 0xFF);
				for (int i = 0; i < 7; ++i)
					DrawCircle(renderer, solpoints[iter]->x, solpoints[iter]->y, i);
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
			}

		}

		/* Ghost Line */
		if (totalpoints > 0)
		{
			SDL_SetRenderDrawColor(renderer, 0x88, 0x88, 0x88, 0xFF);
			SDL_RenderDrawLine(renderer, points[totalpoints-1]->x, points[totalpoints-1]->y, mposx, mposy);
		}

		/* Lines */
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		for (iter = 1; iter < totalpoints; ++iter)
		{
			SDL_RenderDrawLine(renderer, points[iter-1]->x, points[iter-1]->y, points[iter]->x, points[iter]->y);
		}

		SDL_RenderCopy(renderer, pointstext.texture, NULL, &pointstext.rect);
		

		SDL_RenderPresent(renderer);

		frametime = SDL_GetTicks() - elapsed;
		if (1000 / FPS > frametime) SDL_Delay((1000 / FPS) - frametime);

	}

	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

