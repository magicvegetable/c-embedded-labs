#include "animation.h"

#define ARRLEN(arr) (sizeof(arr) / sizeof(*arr))

#define DEFAULT_DELAY ((uint32_t)750)
#define PRESSED_DELAY ((uint32_t)100)
#define LEDS ((const uint16_t []){GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15})

void reset_leds() {
  for (int i = 0; i < ARRLEN(LEDS); i++)
    HAL_GPIO_WritePin(GPIOD, LEDS[i], GPIO_PIN_RESET);
}

void init_circle(Circle *circle) {
  circle->index = 0;
}

void animate_circle(Circle *circle) {
  HAL_GPIO_WritePin(GPIOD, LEDS[circle->index], GPIO_PIN_RESET);
  circle->index = (circle->index + 1) % ARRLEN(LEDS);
  HAL_GPIO_WritePin(GPIOD, LEDS[circle->index], GPIO_PIN_SET);
}

void init_filling(Filling *filling) {
  filling->state = GPIO_PIN_SET;
}

void animate_filling(Filling *filling) {
  filling->state = (filling->state + 1) % 2;

  for (int i = 0; i < ARRLEN(LEDS); i++)
    HAL_GPIO_WritePin(GPIOD, LEDS[i], filling->state);
}

void init_steps(Steps *steps) {
  steps->dir = 1;
  steps->index = 0;
}

void animate_steps(Steps *steps) {
  GPIO_PinState state = steps->dir == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET;

  HAL_GPIO_WritePin(GPIOD, LEDS[steps->index], state);

  steps->index += steps->dir;

  if (steps->index == ARRLEN(LEDS) ||
      steps->index == -1) {
    steps->dir *= -1;
    steps->index += steps->dir;
  }
}

void init_cross(Cross *cross) {
  cross->index = 0;
}

void animate_cross(Cross *cross) {
  int first_i = cross->index % ARRLEN(LEDS);
  int second_i = (cross->index + 2) % ARRLEN(LEDS);

  HAL_GPIO_WritePin(GPIOD, LEDS[first_i], GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD, LEDS[second_i], GPIO_PIN_RESET);

  int third_i = (cross->index + 1) % ARRLEN(LEDS);
  int fourth_i = (cross->index + 3) % ARRLEN(LEDS);

  HAL_GPIO_WritePin(GPIOD, LEDS[third_i], GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOD, LEDS[fourth_i], GPIO_PIN_SET);

  cross->index++;
}

static int pressed = 0;
static int pressed_tick = 1;
static int pressed_pass = 1;

void next_animation() {
  if (!pressed_pass)
    return;

  pressed = 1;
  pressed_pass = 0;
  pressed_tick = 0;
}

void update_animations_state(AnimationsState *state) {
  state->index = (state->index + 1) % ANIMATIONS_AMOUNT;
  switch (state->index) {
  case kAnimateCircle:
    init_circle(&state->circle);
    break;
  case kAnimateFilling:
    init_filling(&state->filling);
    break;
  case kAnimateSteps:
    init_steps(&state->steps);
    break;
  case kAnimateCross:
    init_cross(&state->cross);
    break;
  };
}

void init_animations_state(AnimationsState *state) {
  state->tick = HAL_GetTick();
  state->delay = DEFAULT_DELAY;
  state->index = kAnimateCircle;
  init_circle(&state->circle);
}

void update_pressed_pass() {
  uint32_t tick = HAL_GetTick();

  if (!pressed_tick) {
    pressed_tick = tick;
    return;
  }

  if (tick > pressed_tick + PRESSED_DELAY)
    pressed_pass = 1;
}

void animate(AnimationsState *state) {
  if (pressed) {
    pressed = 0;
    update_animations_state(state);
    reset_leds();
  }

  if (!pressed_pass)
    update_pressed_pass();

  uint32_t tick = HAL_GetTick();
  if (tick < state->tick + state->delay)
    return;

  switch (state->index) {
  case kAnimateCircle:
    animate_circle(&state->circle);
    break;
  case kAnimateFilling:
    animate_filling(&state->filling);
    break;
  case kAnimateSteps:
    animate_steps(&state->steps);
    break;
  case kAnimateCross:
    animate_cross(&state->cross);
    break;
  }

  state->tick = tick;
}
