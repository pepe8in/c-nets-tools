#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "interface.c"

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() < 0) {
        fprintf(stderr, "Erreur d'initialisation SDL ou TTF : %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("C-NETS T00LS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
    if (!window) {
        fprintf(stderr, "Erreur de création de la fenêtre : %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        fprintf(stderr, "Erreur de création du renderer : %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("/usr/share/fonts/nerd-fonts-git/TTF/HackNerdFont-Regular.ttf", 20);
    if (!font) {
        fprintf(stderr, "Erreur de chargement de la police : %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Color text_color = {0, 0, 0};
    SDL_Color bg_color = {226, 233, 192, 255};
    Button buttons[6] = {
        {{150, 200, 200, 50}, "porteye"},
        {{450, 200, 200, 50}, "packetsnoop"},
        {{150, 300, 200, 50}, "filesecure"},
        {{450, 300, 200, 50}, "ddoswatcher"},
        {{150, 400, 200, 50}, "urlspy"},
        {{450, 400, 200, 50}, "xmlbuilder"}
    };

    SDL_StartTextInput();
    char user_input[256] = "";
    SDL_Event event;
    int running = 1, current_screen = 0;
    while (running) {
        while (SDL_PollEvent(&event)) {
            running = manageEvent(&event, buttons, 6, renderer, font, bg_color, &current_screen, user_input);
        }
        SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
        SDL_RenderClear(renderer);
        switch (current_screen) {
            case 0:
                displayHome(renderer, buttons, 6, font, text_color); 
            break;
            case 1:
                displayPorteye(renderer, font, user_input);
            break;
            case 2:
            break;
            case 3:
            break;
            case 4:
            break;
            case 5:
            break;
            case 6: 
            break;
            default:
        }
        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

