#include <SDL.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <string.h>
#include <stdio.h>

SDL_Window *gWindow;
SDL_Renderer *gRenderer;

SDL_Texture * LoadTexture(const char *spriteSheetPath);

int main(int argc, char *argv[])
{
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    // Create window and renderer
    if (SDL_CreateWindowAndRenderer(640, 480, 0, &gWindow, &gRenderer)) {
        printf("SDL_CreateWindowAndRenderer: %s\n", SDL_GetError());
        return -1;
    }

    // Initialize lua and read the script
    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    if (luaL_dofile(L, "game.lua")) {
        printf("Failed to load game.lua: %s\n", lua_tostring(L, -1));
        return -1;
    }

    // load texture for the player
    SDL_Texture *playerTexture = LoadTexture("player.bmp");
    if (!playerTexture) {
        printf("LoadTexture: %s\n", SDL_GetError());
        return -1;
    }

    // initialize player position/texture coordinate information
    SDL_Rect playerSrcRect = { 0, 0, 16, 16 };
    SDL_Rect playerDstRect = { 0, 0, 64, 64 };
    double playerX = 0.0f;
    double playerY = 0.0f;

    typedef enum PlayerDirection
    {
        PLAYER_DIRECTION_UP,
        PLAYER_DIRECTION_LEFT,
        PLAYER_DIRECTION_RIGHT,
        PLAYER_DIRECTION_DOWN
    } PlayerDirection;

    typedef enum PlayerState
    {
        PLAYER_STATE_IDLE,
        PLAYER_STATE_WALKING
    } PlayerState;

    PlayerDirection playerDirection = PLAYER_DIRECTION_DOWN;
    PlayerState playerState = PLAYER_STATE_IDLE;
    Uint32 timeOfLastStateSwitch = SDL_GetTicks();

    Uint32 timeOfLastFrame = SDL_GetTicks();

    // begin main loop
    int isGameRunning = 1;
    while (isGameRunning)
    {
        // count the number of milliseconds since the last frame
        Uint32 timeOfThisFrame = SDL_GetTicks();
        Uint32 frameDeltaTime = timeOfThisFrame - timeOfLastFrame;
        timeOfLastFrame = timeOfThisFrame;

        // handle all the events
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            // if the window is being requested to close, then stop the game.
            if (e.type == SDL_QUIT) {
                isGameRunning = 0;
            } else if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
                // get the event handling function to call
                if (e.type == SDL_KEYDOWN) {
                    lua_getglobal(L, "OnKeyPressed");
                } else {
                    lua_getglobal(L, "OnKeyReleased");
                }

                // if it's of function type, then call it passing the key name
                if (lua_type(L, -1) == LUA_TFUNCTION) {
                    lua_pushstring(L, SDL_GetKeyName(e.key.keysym.sym));
                    if (lua_pcall(L, 1, 0, 0)) {
                        printf("OnKeyPressed error: %s\n", lua_tostring(L, -1));
                        lua_pop(L, 1);
                    }
                }
                else
                {
                    lua_pop(L, 1);
                }
            }
        }

        // call Update on the Lua side
        lua_getglobal(L, "Update");
        lua_pushnumber(L, frameDeltaTime);
        if (lua_pcall(L, 1, 0, 0)) {
            printf("Update error: %s\n", lua_tostring(L, -1));
            lua_pop(L, 1);
        }

        // Read player position
        lua_getglobal(L, "gPlayerPosition");
        if (lua_type(L, -1) == LUA_TTABLE)
        {
            lua_getfield(L, -1, "x");
            lua_getfield(L, -2, "y");
            playerX = luaL_checknumber(L, -2);
            playerY = luaL_checknumber(L, -1);
            lua_pop(L, 2);
        }
        lua_pop(L, 1);

        // Read player direction
        lua_getglobal(L, "gPlayerDirection");
        if (lua_type(L, -1) == LUA_TSTRING)
        {
            const char *direction = lua_tostring(L, -1);
            if (strcmp(direction, "up") == 0) {
                playerDirection = PLAYER_DIRECTION_UP;
            } else if (strcmp(direction, "down") == 0) {
                playerDirection = PLAYER_DIRECTION_DOWN;
            } else if (strcmp(direction, "left") == 0) {
                playerDirection = PLAYER_DIRECTION_LEFT;
            } else if (strcmp(direction, "right") == 0) {
                playerDirection = PLAYER_DIRECTION_RIGHT;
            } else {
                printf("Unknown direction: %s\n", direction);
            }
        }
        lua_pop(L, 1);

        // Read player state
        lua_getglobal(L, "gPlayerState");
        if (lua_type(L, -1) == LUA_TSTRING)
        {
            const char *state = lua_tostring(L, -1);
            PlayerState newState;
            int wasValidState = 1;
            if (strcmp(state, "idle") == 0) {
                newState = PLAYER_STATE_IDLE;
            } else if (strcmp(state, "walking") == 0) {
                newState = PLAYER_STATE_WALKING;
            } else {
                printf("Unknown state: %s\n", state);
                wasValidState = 0;
            }
            if (wasValidState) {
                if (playerState != newState) {
                    timeOfLastStateSwitch = SDL_GetTicks();
                }
                playerState = newState;
            }
        }
        lua_pop(L, 1);

        // adjust animation to position
        playerDstRect.x = (int) playerX;
        playerDstRect.y = (int) playerY;

        // adjust animation to direction
        playerSrcRect.y = playerDirection * 16;
        
        // adjust animation to state and time
        if (playerState == PLAYER_STATE_WALKING)
        {
            int frameDuration = 1000/6; // animation at 6fps
            int framesSinceStateSwitch = (SDL_GetTicks() - timeOfLastStateSwitch) / frameDuration;
            // base animation on odd/even frames.
            if (framesSinceStateSwitch & 1) {
                playerSrcRect.x = 0;
            } else {
                playerSrcRect.x = 16;
            }
        }

        // clear the screen
        if (SDL_RenderClear(gRenderer)) {
            printf("SDL_RenderClear: %s\n", SDL_GetError());
            return -1;
        }
        
        // finally, do the real rendering
        if (SDL_RenderCopy(
                gRenderer,
                playerTexture,
                &playerSrcRect,
                &playerDstRect)) {
            printf("SDL_RenderCopy: %s\n", SDL_GetError());
            return -1;
        }

        // flip the display
        SDL_RenderPresent(gRenderer);

        // throttle the frame rate to 60fps
        SDL_Delay(1000/60);
    }

    // throw everything away
    SDL_Quit();

    return 0;
}

// Routine for loading color keyed textures and doing all the stupid stuff that comes with it
SDL_Texture * LoadTexture(const char *spriteSheetPath)
{
    // load the image
    SDL_Surface *surface = SDL_LoadBMP(spriteSheetPath);
    if (!surface) {
        return NULL;
    }

    // create a duplicate of the surface with the same pixel format as the window
    SDL_Surface *newSurface = SDL_ConvertSurfaceFormat(surface, SDL_GetWindowPixelFormat(gWindow), 0);

    // no longer need to hang on to the original image
    SDL_FreeSurface(surface);

    if (!newSurface) {
        return NULL;
    }

    // set (255,0,255) as the transparent color
    if (SDL_SetColorKey(newSurface, SDL_TRUE, SDL_MapRGB(newSurface->format, 255, 0, 255))) {
        return NULL;
    }

    // create a renderable texture from the image
    SDL_Texture *texture = SDL_CreateTextureFromSurface(gRenderer, newSurface);

    // no longer need to hang on to the converted image
    SDL_FreeSurface(newSurface);

    return texture;
}
