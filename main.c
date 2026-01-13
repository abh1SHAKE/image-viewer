#include <stdio.h>
#include <stdlib.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

int main(int argc, char *argv[]) {
    const int width = 900;
    const int height = 600;

    printf("Call on me");

    SDL_Window *pwindow = SDL_CreateWindow(
        "Image Viewer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        0
    );

    SDL_Surface *psurface = SDL_GetWindowSurface(pwindow);

    Uint8 r, g, b;
    r = 0x00;
    g = 0x42;
    b = 0x25;
    Uint32 color = SDL_MapRGB(psurface -> format, r, g, b);

    SDL_Rect pixel = (SDL_Rect){0, 0, 1, 1};
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            pixel.x = x;
            pixel.y = y;
            SDL_FillRect(psurface, &pixel, color);
        }
    }

    SDL_UpdateWindowSurface(pwindow);

    SDL_Delay(3000);
}