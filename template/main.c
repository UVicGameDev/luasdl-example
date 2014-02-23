#include <SDL.h>

#include <stdio.h>

int main(int argc, char *argv[])
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // create the window
    SDL_Window *window = SDL_CreateWindow(
                            "Template",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            640, 480,
                            0);
    if (!window) {
        printf("SDL_CreateWindow: %s\n", SDL_GetError());
        return -1;
    }

    // create a renderer for the window
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        printf("SDL_CreateRenderer: %s\n", SDL_GetError());
        return -1;
    }

    // begin main loop
    int running = 1;
    while (running)
    {
        // handle all the events
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT) {
                running = 0;
            }
        }

        // render everything
        SDL_RenderClear(renderer);

        SDL_RenderPresent(renderer);

        SDL_Delay(1000/60);
    }

    SDL_Quit();

    return 0;
}
