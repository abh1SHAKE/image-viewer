#include <stdio.h>
#include <stdlib.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#define MAX_WIN_W 1280
#define MAX_WIN_H 720

int main(int argc, char *argv[]) {
    #ifdef _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    #endif

    FILE *in = stdin;
    char line[1024];

    // Read magic number (P3 - ASCII, P6 - Binary)
    fgets(line, sizeof(line), in);

    int is_p3 = 0, is_p6 = 0;
    if (line[0] == 'P' && line[1] == '3') {
        is_p3 = 1;
    } else if (line[0] == 'P' && line[1] == '6') {
        is_p6 = 1;
    } else {
        fprintf(stderr, "Unsupported PPM format\n");
        return 1;
    }

    // Read until we get dimensions (skip comments)
    do {
        fgets(line, sizeof(line), in);
    } while (line[0] == '#');

    int width, height;
    sscanf(line, "%d %d", &width, &height);

    // Read maxval
    do {
        fgets(line, sizeof(line), in);
    } while (line[0] == '#');

    int maxval;
    sscanf(line, "%d", &maxval);
    if (maxval != 255) {
        fprintf(stderr, "Only maxval 255 supported\n");
        return 1;
    }

    // Calculate scale to preserve aspect ratio
    float scale_x = (float)MAX_WIN_W / width;
    float scale_y = (float)MAX_WIN_H / height;
    float scale = (scale_x < scale_y) ? scale_x : scale_y;

    // Clamp to 1.0 if image is smaller than max bounds
    if (scale > 1.0f) {
        scale = 1.0f;
    }

    int win_w = (int)(width * scale);
    int win_h = (int)(height * scale);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *pwindow = SDL_CreateWindow(
        "Image Viewer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        win_w, win_h,
        0
    );

    if (!pwindow) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Surface *psurface = SDL_GetWindowSurface(pwindow);
    if (!psurface) {
        fprintf(stderr, "SDL_GetWindowSurface failed: %s\n", SDL_GetError());
        return 1;
    }

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Uint8 r, g, b;

            if (is_p3) {
                int ri, gi, bi;
                fscanf(in, "%d %d %d", &ri, &gi, &bi);
                r = (Uint8) ri;
                g = (Uint8) gi;
                b = (Uint8) bi;
            } else {
                fread(&r, 1, 1, in);
                fread(&g, 1, 1, in);
                fread(&b, 1, 1, in);
            }

            Uint32 color = SDL_MapRGB(psurface -> format, r, g, b);
            
            SDL_Rect pixel = {
                (int)(x * scale),
                (int)(y * scale),
                (int)(scale + 1),
                (int)(scale + 1),
            };

            SDL_FillRect(psurface, &pixel, color);
        }
    }

    SDL_UpdateWindowSurface(pwindow);

    // App is running
    int app_running = 1;
    while (app_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                app_running = 0;
            } 
        }

        SDL_UpdateWindowSurface(pwindow);
        SDL_Delay(100);
    }

    SDL_DestroyWindow(pwindow);
    SDL_Quit();
    return 0;
}