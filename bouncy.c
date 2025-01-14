#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <stdlib.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 300

#define SCREEN_LEFT_BORDER 0
#define SCREEN_TOP_BORDER 0

#define BIG_RECT_WIDTH 100
#define BIG_RECT_HEIGHT 100
#define BIG_RECT_MASS 10
#define BIG_RECT_INITIAL_VEL_SCALAR 1
#define BIG_RECT_INITIAL_DIRECTION 1

#define SMALL_RECT_WIDTH 100
#define SMALL_RECT_HEIGHT 100
#define SMALL_RECT_MASS 1
#define SMALL_RECT_INITIAL_VEL_SCALAR 0
#define SMALL_RECT_INITIAL_DIRECTION 1

int collision_counter = 0;

typedef struct SIM_Rect {
  SDL_Rect shape; // x, y, w, h
  int mass;
  int width;
  int height;
  int direction;
  float velocity;
} SIM_Rect;

void apply_bound_collisions(SIM_Rect *rect);
void apply_collisions(SIM_Rect *rect_a, SIM_Rect *rect_b);
float solve_for_va2(int ma, int mb, float va1, float vb1);
float solve_for_vb2(int ma, int mb, float va1, float vb1);
void print_square_info(SIM_Rect *rect);

int main(int argc, char *argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Window *window =
      SDL_CreateWindow("Tacks", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  if (window == NULL) {
    printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    return 1;
  }
  SDL_Surface *surface = SDL_GetWindowSurface(window);

  SIM_Rect big_rect;
  big_rect.shape = (SDL_Rect){SCREEN_WIDTH - BIG_RECT_WIDTH, 0, BIG_RECT_WIDTH,
                              BIG_RECT_HEIGHT};
  big_rect.width = BIG_RECT_WIDTH;
  big_rect.height = BIG_RECT_HEIGHT;
  big_rect.mass = BIG_RECT_MASS;
  big_rect.direction = BIG_RECT_INITIAL_DIRECTION;
  big_rect.velocity = BIG_RECT_INITIAL_VEL_SCALAR;

  SIM_Rect small_rect;
  small_rect.shape = (SDL_Rect){SMALL_RECT_WIDTH + 100, 0, SMALL_RECT_WIDTH,
                                SMALL_RECT_HEIGHT};
  small_rect.width = SMALL_RECT_WIDTH;
  small_rect.height = SMALL_RECT_HEIGHT;
  small_rect.mass = SMALL_RECT_MASS;
  small_rect.direction = SMALL_RECT_INITIAL_DIRECTION;
  small_rect.velocity = SMALL_RECT_INITIAL_VEL_SCALAR;

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
    SDL_Delay(5);
  }

  SDL_UpdateWindowSurface(window);
}

void apply_collisions(SIM_Rect *rect_a, SIM_Rect *rect_b) {
  if (rect_a->shape.x + rect_a->width >= rect_b->shape.x &&
          rect_a->shape.x + rect_a->width <= rect_b->shape.x + rect_b->width ||
      rect_b->shape.x + rect_b->width >= rect_a->shape.x &&
          rect_b->shape.x + rect_b->width <= rect_a->shape.x + rect_a->width) {
    float a_initial_v = rect_a->direction * rect_a->velocity;
    float b_initial_v = rect_b->direction * rect_b->velocity;
    float va_ans =
        solve_for_va2(rect_a->mass, rect_b->mass, a_initial_v, b_initial_v);
    if (va_ans < 0) {
      rect_a->direction = -1;
      rect_a->velocity = fabs(va_ans);
    } else {
      rect_a->direction = 1;
      rect_a->velocity = fabs(va_ans);
    }

    float vb_ans =
        solve_for_vb2(rect_a->mass, rect_b->mass, a_initial_v, b_initial_v);
    if (vb_ans < 0) {
      rect_b->direction = -1;
      rect_b->velocity = fabs(vb_ans);
    } else {
      rect_b->direction = 1;
      rect_b->velocity = fabs(vb_ans);
    }
    printf("collisions : %d\n", ++collision_counter);
  }
}

void apply_bound_collisions(SIM_Rect *rect) {
  if (rect->shape.x + rect->width >= SCREEN_WIDTH && rect->direction > 0) {
    printf("first if \n");
    print_square_info(rect);
    rect->direction = -1;
    rect->shape.x = SCREEN_WIDTH - rect->width - 1;
    print_square_info(rect);
  }
  if (rect->shape.x <= SCREEN_LEFT_BORDER && rect->direction < 0) {
    printf("second if \n");
    print_square_info(rect);
    rect->direction = 1;
    rect->shape.x = SCREEN_LEFT_BORDER + 1;
    print_square_info(rect);
  }
}

float solve_for_va2(int ma, int mb, float va1, float vb1) {
  return ((float)(ma - mb) / (ma + mb)) * va1 +
         ((float)2 * mb / (ma + mb)) * vb1;
}
float solve_for_vb2(int ma, int mb, float va1, float vb1) {
  return ((float)(2 * ma) / (ma + mb)) * va1 +
         ((float)(mb - ma) / (ma + mb)) * vb1;
}

void print_square_info(SIM_Rect *rect) {
  printf("x: %d, y: %d, w: %d, h: %d, m: %d, v: %f, d: %d\n", rect->shape.x,
         rect->shape.y, rect->shape.w, rect->shape.h, rect->mass,
         rect->velocity, rect->direction);
}
