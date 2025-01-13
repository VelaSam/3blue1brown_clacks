#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>

#define SCREEN_WIDTH 900
#define SCREEN_HEIGHT 600

#define SCREEN_LEFT_BORDER 0
#define SCREEN_TOP_BORDER 0

#define BIG_RECT_WIDTH 300
#define BIG_RECT_HEIGHT 300
#define BIG_RECT_MASS 100

#define SMALL_RECT_WIDTH 100
#define SMALL_RECT_HEIGHT 100
#define SMALL_RECT_MASS 10

typedef struct SIM_Rect {
  SDL_Rect shape; // x, y, w, h
  int mass;
  int width;
  int height;
  int direction;
  int velocity;
} SIM_Rect;

void apply_bound_collisions(SIM_Rect *rect);
void apply_collisions(SIM_Rect *rect_a, SIM_Rect *rect_b);

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Window *window =
      SDL_CreateWindow("Bouncy Ball", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  if (window == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Surface *surface = SDL_GetWindowSurface(window);

  SIM_Rect big_rect;
  big_rect.shape =
      (SDL_Rect){BIG_RECT_WIDTH + 100, 0, BIG_RECT_WIDTH, BIG_RECT_HEIGHT};
  big_rect.width = BIG_RECT_WIDTH;
  big_rect.height = BIG_RECT_HEIGHT;
  big_rect.mass = BIG_RECT_MASS;
  big_rect.direction = 1;
  big_rect.velocity = 1;

  SIM_Rect small_rect;
  small_rect.shape = (SDL_Rect){SMALL_RECT_WIDTH + 100, 0, SMALL_RECT_WIDTH,
                                SMALL_RECT_HEIGHT};
  small_rect.width = SMALL_RECT_WIDTH;
  small_rect.height = SMALL_RECT_HEIGHT;
  small_rect.mass = SMALL_RECT_MASS;
  small_rect.direction = 1;
  small_rect.velocity = 1;

  while (1) {
    SDL_Event event;
    if (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        break;
      }
    }

    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

    small_rect.shape.x += small_rect.direction * small_rect.velocity;
    big_rect.shape.x += big_rect.direction * big_rect.velocity;

    apply_bound_collisions(&big_rect);
    apply_bound_collisions(&small_rect);

    apply_collisions(&small_rect, &big_rect);

    SDL_FillRect(surface, &(big_rect.shape),
                 SDL_MapRGB(surface->format, 150, 0, 255));
    SDL_FillRect(surface, &(small_rect.shape),
                 SDL_MapRGB(surface->format, 255, 0, 0));
    SDL_UpdateWindowSurface(window);
    SDL_Delay(1);
  }

  SDL_UpdateWindowSurface(window);
}

void apply_collisions(SIM_Rect *rect_a, SIM_Rect *rect_b) {
  if (rect_a->shape.x + rect_a->width == rect_b->shape.x) {
    rect_a->velocity *= -1;
    rect_b->velocity *= -1;
  }
  if (rect_b->shape.x + rect_b->width == rect_a->shape.x) {
    rect_a->velocity *= -1;
    rect_b->velocity *= -1;
  }
}

void apply_bound_collisions(SIM_Rect *rect) {
  if (rect->shape.x + rect->width >= SCREEN_WIDTH && rect->velocity > 0) {
    rect->velocity = -1;
  } else if (rect->shape.x <= SCREEN_LEFT_BORDER && rect->velocity < 0) {
    rect->velocity = 1;
  }
}
