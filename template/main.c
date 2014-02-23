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

    // load the mario image
    SDL_Surface *marioSurface = SDL_LoadBMP("mario.bmp");
    if (!marioSurface) {
        printf("SDL_LoadBMP: %s\n", SDL_GetError());
        return -1;
    }

    // create a duplicate of the mario surface with the same pixel format as the window
    SDL_Surface *newMarioSurface = SDL_ConvertSurfaceFormat(marioSurface, SDL_GetWindowPixelFormat(window), 0);

    // no longer need to hang on to the original image
    SDL_FreeSurface(marioSurface);

    // set (255,0,255) as the transparent color
    if (SDL_SetColorKey(newMarioSurface, SDL_TRUE, SDL_MapRGB(newMarioSurface->format, 255, 0, 255))) {
        printf("SDL_SetColorKey: %s\n", SDL_GetError());
        return -1;
    }

    // create a renderable texture from the image
    SDL_Texture *marioTexture = SDL_CreateTextureFromSurface(renderer, newMarioSurface);
    if (!marioTexture) {
        printf("SDL_CreateTextureFromSurface: %s\n", SDL_GetError());
        return -1;
    }

    // no longer need to hang on to the converted image
    SDL_FreeSurface(newMarioSurface);

    typedef enum MarioState {
        MARIO_JUMPING,
        MARIO_WALKING,
        MARIO_IDLE
    } MarioState;

    // current state of mario
    MarioState marioState = MARIO_IDLE;

    // position of mario in the world
    float marioXPosition = 200;
    float marioYPosition = 200;
    float marioMovementSpeed = 50; // pixels per second
    
    // stores whether or not the mario state should be flipped
    SDL_RendererFlip marioFlipping = SDL_FLIP_NONE;

    // will use the time between state switches to know what frame of animation to use
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
            }
        }

        // store the old state to know if the state has changed
        MarioState oldMarioState = marioState;

        // get a lookup table to know if keys are pressed
        const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

        // handle left movement key
        if (keyboardState[SDL_SCANCODE_LEFT]) {
            marioXPosition -= marioMovementSpeed * (frameDeltaTime / 1000.0f);
            marioFlipping = SDL_FLIP_NONE;
            
            // switch to walking state
            marioState = MARIO_WALKING;
        }
        
        // handle right movement key
        if (keyboardState[SDL_SCANCODE_RIGHT]) {
            marioXPosition += marioMovementSpeed * (frameDeltaTime / 1000.0f);
            marioFlipping = SDL_FLIP_HORIZONTAL;
            
            // switch to walking state
            marioState = MARIO_WALKING;
        }

        // if nothing's being pressed, then we're in the idle state
        if (!keyboardState[SDL_SCANCODE_LEFT] && !keyboardState[SDL_SCANCODE_RIGHT]) {
            marioState = MARIO_IDLE;
        }

        // update the time of the last state switch
        if (marioState != oldMarioState) {
            timeOfLastStateSwitch = SDL_GetTicks();
        }

        // render everything
        if (SDL_RenderClear(renderer)) {
            printf("SDL_RenderClear: %s\n", SDL_GetError());
            return -1;
        }

        // decide frame of animation to use
        int currentSpriteFrame;
        if (marioState == MARIO_IDLE) {
            currentSpriteFrame = 2;
        } else if (marioState == MARIO_WALKING) {
            // get the index of the frame we're currently at (12 fps animation speed)
            Uint32 millisecondsPerFrame = 1000/12;
            Uint32 framesPassedSinceStateSwitch = (SDL_GetTicks() - timeOfLastStateSwitch) / millisecondsPerFrame;
            // if we're on an odd frame, use the standing animation.
            // if we're on an even frame, use the walking animation.
            if (framesPassedSinceStateSwitch & 1) {
                currentSpriteFrame = 2;
            } else {
                currentSpriteFrame = 1;
            }
        } else {
            currentSpriteFrame = 0;
        }

        // set up the rendering for mario
        // note: each frame in the animation has size 16x32 in the sprite sheet
        
        // the source rectangle defines which region of the sprite sheet to use
        SDL_Rect renderSourceRect; 
        renderSourceRect.x = currentSpriteFrame * 16;
        renderSourceRect.y = 0;
        renderSourceRect.w = 16;
        renderSourceRect.h = 32;

        // the destination rectangle defines which region of the screen to draw it to
        SDL_Rect renderDestinationRect;
        renderDestinationRect.x = marioXPosition;
        renderDestinationRect.y = marioYPosition - 32 * 2; // sprite is scaled 2x
        renderDestinationRect.w = 16 * 2; // sprite is scaled 2x
        renderDestinationRect.h = 32 * 2; // sprite is scaled 2x

        // finally, do the real rendering
        if (SDL_RenderCopyEx(
                renderer,
                marioTexture,
                &renderSourceRect,
                &renderDestinationRect,
                0.0, // no rotation
                NULL, // no center of rotation
                marioFlipping)) {
            printf("SDL_RenderCopy: %s\n", SDL_GetError());
            return -1;
        }

        // flip the display
        SDL_RenderPresent(renderer);

        // throttle the frame rate to 60fps
        SDL_Delay(1000/60);
    }

    // throw everything away
    SDL_Quit();

    return 0;
}
