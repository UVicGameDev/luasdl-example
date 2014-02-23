int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_Window *window = SDL_CreateWindow("Template",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            640, 480,
                            0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    return 0;
}
