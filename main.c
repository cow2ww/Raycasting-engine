#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

const int screenWidth = 1600;
const int screenHeight = 900;

int main() {
    float playerPos[] = {2.5f, 2.5f};
    int map[7][7] = {
                {1, 1, 1, 1, 1, 1, 1},
                {1, 0, 0, 0, 1, 0, 1},
                {1, 0, 0, 0, 1, 0, 1},
                {1, 0, 1, 0, 0, 0, 1},
                {1, 0, 1, 0, 0, 0, 1},
                {1, 0, 1, 0, 1, 0, 1},
                {1, 1, 1, 1, 1, 1, 1}};

    float rayPos[] =    {0.0f, 0.0f};
    float stepSize =     0.01f;
    float rayAngle, rayInitAngle, playerAngle, playerAngleY;
    float     dx;
    float     dy;
    int rayLength = 0;
    int scaleFactor = 50000;

    int playerSize = 5;
    int tileSize =   20;

    int mouseSensX = 10;
    int mouseSensY = 5;

    // init SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL");
    }

    // Init window
    SDL_Window *window = SDL_CreateWindow(
        "window",
        0, 0, 
        screenWidth, screenHeight, 
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        printf("Failed to create window");
    }

    SDL_SetWindowResizable(window, SDL_TRUE);

    SDL_ShowCursor(SDL_FALSE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    // Init renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {
        printf("Failed to create renderer");
    }

    SDL_Event event;
    bool running = true;

    // 90 degrees = 7.851
    // 180 degrees = 15.702
    // 360 degrees = 31.404

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            playerAngle = (rayAngle + rayInitAngle) / 2;

            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_w) {
                    dx = cos(playerAngle);
                    dy = sin(playerAngle);
                    playerPos[0] += dx / 10;
                    playerPos[1] += dy / 10;
                } else if (event.key.keysym.sym == SDLK_a) {
                    dx = cos(playerAngle - 7.851);
                    dy = sin(playerAngle - 7.851);
                    playerPos[0] += dx / 10;
                    playerPos[1] += dy / 10;
                } else if (event.key.keysym.sym == SDLK_d) {
                    dx = cos(playerAngle + 7.851);
                    dy = sin(playerAngle + 7.851);
                    playerPos[0] += dx / 10;
                    playerPos[1] += dy / 10;
                } else if (event.key.keysym.sym == SDLK_s) {
                    dx = cos(playerAngle + 15.702);
                    dy = sin(playerAngle + 15.702);
                    playerPos[0] += dx / 10;
                    playerPos[1] += dy / 10;
                } else if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }
            }
            
            if (event.type == SDL_MOUSEMOTION) {
                rayInitAngle = event.motion.x / mouseSensX / 31.404;
                playerAngleY = (event.motion.y - screenHeight / 2) * mouseSensY;
            }
        }

        // clear renderer
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        
        // draw sky and floor
        SDL_Rect skyrect = {
            0, 
            0, 
            screenWidth, 
            screenHeight / 2 - playerAngleY};
        SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
        SDL_RenderFillRect(renderer, &skyrect);

        SDL_Rect floorRect = {
            0, 
            screenHeight / 2 - playerAngleY, 
            screenWidth, 
            10000};
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderFillRect(renderer, &floorRect);

        // draw rays (3d)

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        rayPos[0] = playerPos[0];
        rayPos[1] = playerPos[1];

        for (float i = 0.0f; i <= screenWidth; i++) {
            rayLength = 0;
            rayAngle = rayInitAngle + i * 1 / screenWidth;
            rayPos[0] = playerPos[0];
            rayPos[1] = playerPos[1];
            
            while (rayPos[0] < 7 && rayPos[0] >= 0 && 
                   rayPos[1] < 7 && rayPos[1] >= 0) {

                if (map[(int)rayPos[1]][(int)rayPos[0]] != 0) {
                    // corect fish-eye lens effect
                    rayLength = rayLength * cos(rayAngle - playerAngle);

                    int lineHeight = scaleFactor / rayLength;
                    int drawStart = ((screenHeight - lineHeight) / 2) - playerAngleY;
                    int drawEnd = (drawStart + lineHeight);

                    int shade = scaleFactor / rayLength;
                    if (shade > 255) {shade = 255;}

                    SDL_SetRenderDrawColor(renderer, 
                                        shade,
                                        shade,
                                        shade,
                                        255);
                    SDL_RenderDrawLine(renderer, 
                                        i, drawStart, 
                                        i, drawEnd);
                    
                    // draw black outlines
                    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                    SDL_RenderDrawPoint(renderer, i, drawStart);
                    SDL_RenderDrawPoint(renderer, i, drawEnd);
                    break;
                }
                
                dx = cos(rayAngle);
                dy = sin(rayAngle);
                
                rayPos[0] += dx * stepSize;
                rayPos[1] += dy * stepSize;

                rayLength += 1;
            }
        }

        // draw tiles
        for (int i = 0; i <= 7; i++) {
            for (int j = 0; j <= 6; j++) {
                if (map[i][j] == 1) {
                    SDL_Rect rect = {j * tileSize, i * tileSize, tileSize, tileSize};
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }

        // draw player
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

        SDL_Rect rect = {playerPos[0] * tileSize - playerSize / 2, playerPos[1] * tileSize - playerSize / 2, playerSize, playerSize};
        SDL_RenderFillRect(renderer, &rect);

        // draw rays (2d)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        rayPos[0] = playerPos[0];
        rayPos[1] = playerPos[1];

        for (float i = 0.0f; i <= screenWidth; i++) {
            rayAngle = rayInitAngle + i * 1 / screenWidth;
            rayPos[0] = playerPos[0];
            rayPos[1] = playerPos[1];
            
            while (rayPos[0] < 7 && rayPos[0] >= 0 && 
                   rayPos[1] < 7 && rayPos[1] >= 0) {

                if (map[(int)rayPos[1]][(int)rayPos[0]] == 1) {
                    break;
                } else {
                    SDL_RenderDrawPoint(renderer, rayPos[0] * tileSize, rayPos[1] * tileSize);
                }
                
                dx = cos(rayAngle);
                dy = sin(rayAngle);
                
                rayPos[0] += dx * stepSize;
                rayPos[1] += dy * stepSize;

                rayLength += 1;
            }
        }
        

        // render
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return 0;
}
