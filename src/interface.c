#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

typedef struct {
    SDL_Rect rect;
    const char *label;
} Button;

void writeText(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, const char *text, SDL_Rect *rect) {
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, text, color);
    if (text_surface == NULL) {
        fprintf(stderr, "Erreur de rendu du texte : %s\n", TTF_GetError());
        return;
    }

    SDL_Texture *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
    if (!text_texture) {
        fprintf(stderr, "Erreur de création de la texture : %s\n", SDL_GetError());
        SDL_FreeSurface(text_surface);
        return;
    }

    rect->w = text_surface->w;
    rect->h = text_surface->h;
    SDL_RenderCopy(renderer, text_texture, NULL, rect);
    SDL_FreeSurface(text_surface);
    SDL_DestroyTexture(text_texture);
}

void drawButton(SDL_Renderer *renderer, Button *button, TTF_Font *font, SDL_Color text_color) {
    SDL_SetRenderDrawColor(renderer, 122, 169, 92, 255);
    SDL_RenderFillRect(renderer, &button->rect);
    int text_width, text_height;
    TTF_SizeText(font, button->label, &text_width, &text_height);
    SDL_Rect textRect = { button->rect.x + (button->rect.w - text_width) / 2, button->rect.y + (button->rect.h - text_height) / 2, text_width, text_height };
    writeText(renderer, font, text_color, button->label, &textRect);
}

void displayHome(SDL_Renderer *renderer, Button *buttons, int nb_buttons, TTF_Font *font, SDL_Color text_color) {
    for (int i = 0; i < nb_buttons; i++) {
        drawButton(renderer, &buttons[i], font, text_color);
    }
}

void displayPorteye(SDL_Renderer *renderer, TTF_Font *font, const char *input_text) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect input_rect = {300, 250, 200, 50};
    SDL_RenderFillRect(renderer, &input_rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &input_rect);
    SDL_Color text_color = {0, 0, 0};
    SDL_Rect text_rect = {input_rect.x + 10, input_rect.y + 15, 0, 0};
    if (strlen(input_text) > 0) {
        writeText(renderer, font, text_color, input_text, &text_rect);
    } else {
        SDL_Color placeholder_color = {128, 128, 128};
        writeText(renderer, font, placeholder_color, "Saisissez l'adresse IP ici ...", &text_rect);
    }
}

int manageEvent(SDL_Event *event, Button *buttons, int nb_buttons, SDL_Renderer *renderer, TTF_Font *font, SDL_Color bg_color, int *current_screen, char *user_input) {
    if (event->type == SDL_QUIT) {
        return 0;
    } else if (event->type == SDL_MOUSEBUTTONDOWN) {
        int mouseX = event->button.x;
        int mouseY = event->button.y;
        for (int i = 0; i < nb_buttons; i++) {
            Button *button = &buttons[i];
            if (mouseX >= button->rect.x && mouseX <= button->rect.x + button->rect.w && mouseY >= button->rect.y && mouseY <= button->rect.y + button->rect.h) {
                if (strcmp(button->label, "porteye") == 0) {
                    *current_screen = 1;
                }
                return -1;
            }
        }
    } else if (event->type == SDL_TEXTINPUT && *current_screen == 1) {
        if (strlen(user_input) + strlen(event->text.text) < 256) {
            strcat(user_input, event->text.text);
        }
    } else if (event->type == SDL_KEYDOWN && *current_screen == 1) {
        if (event->key.keysym.sym == SDLK_BACKSPACE && strlen(user_input) > 0) {
            user_input[strlen(user_input) - 1] = '\0';
        } else if (event->key.keysym.sym == SDLK_RETURN) {
            printf("Texte saisi : %s\n", user_input);
            *current_screen = 0; 
        }
    }
    return 1;
}

