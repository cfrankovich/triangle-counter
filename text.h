#ifndef MY_TEXT_HEADER
#define MY_TEXT_HEADER

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define LIBERATION_MONO_REGULAR "/usr/share/fonts/liberation-fonts/LiberationMono-Regular.ttf"

typedef struct
{
	char *string;
	SDL_Surface *surface;
	SDL_Texture *texture;
	SDL_Rect rect;
	SDL_Color color;
	TTF_Font *font;
} Text_t;

Text_t init_text(char *textstring, TTF_Font *font, int x, int y, SDL_Color olor, SDL_Renderer *rendrr);

#endif
