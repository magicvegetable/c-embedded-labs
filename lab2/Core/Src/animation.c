#include "animation.h"

#define ARRLEN(arr) (sizeof(arr) / sizeof(*arr))

#define DEFAULT_DELAY ((uint32_t)750)
#define PRESSED_DELAY ((uint32_t)50)

static uint16_t leds[] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};

void reset_leds() {
	for (int i = 0; i < ARRLEN(leds); i++)
		HAL_GPIO_WritePin(GPIOD, leds[i], GPIO_PIN_RESET);
}

Circle init_circle() {
  Circle circle = {.index = 0};
  return circle;
}

void animate_circle(Circle *circle) {
	HAL_GPIO_WritePin(GPIOD, leds[circle->index], GPIO_PIN_RESET);
	circle->index = (circle->index + 1) % ARRLEN(leds);
	HAL_GPIO_WritePin(GPIOD, leds[circle->index], GPIO_PIN_SET);
}

Filling init_filling() {
  Filling filling = {.state = GPIO_PIN_SET};
  return filling;
}

void animate_filling(Filling *filling) {
	filling->state = (filling->state + 1) % 2;

	for (int i = 0; i < ARRLEN(leds); i++)
		HAL_GPIO_WritePin(GPIOD, leds[i], filling->state);
}

Steps init_steps() {
  Steps steps = {.dir = 1, .index = 0};
  return steps;
}

void animate_steps(Steps *steps) {
	GPIO_PinState state = steps->dir == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET;

	HAL_GPIO_WritePin(GPIOD, leds[steps->index], state);

	steps->index += steps->dir;

	if (steps->index == ARRLEN(leds) ||
      steps->index == -1) {
		steps->dir *= -1;
		steps->index += steps->dir;
	}
}

Cross init_cross() {
  Cross cross = {.index = 0};
  return cross;
}

void animate_cross(Cross *cross) {
	int first_i = cross->index % ARRLEN(leds);
	int second_i = (cross->index + 2) % ARRLEN(leds);

	HAL_GPIO_WritePin(GPIOD, leds[first_i], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, leds[second_i], GPIO_PIN_RESET);

	int third_i = (cross->index + 1) % ARRLEN(leds);
	int fourth_i = (cross->index + 3) % ARRLEN(leds);

	HAL_GPIO_WritePin(GPIOD, leds[third_i], GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, leds[fourth_i], GPIO_PIN_SET);

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
  case kCircle:
    state->circle = init_circle();
    break;
  case kFilling:
    state->filling = init_filling();
    break;
  case kSteps:
    state->steps = init_steps();
    break;
  case kCross:
    state->cross = init_cross();
    break;
  };
}

AnimationsState init_animations_state() {
  AnimationsState state = {
    .index = kCircle,
    .circle = init_circle(),
    .tick = HAL_GetTick(),
    .delay = DEFAULT_DELAY,
  };
  return state;
}

void animate(AnimationsState *state) {
  if (pressed) {
    pressed = 0;
    update_animations_state(state);
    reset_leds();
  }

  uint32_t tick = HAL_GetTick();
  if (tick < state->tick + state->delay)
    return;

  switch (state->index) {
  case kCircle:
    animate_circle(&state->circle);
    break;
  case kFilling:
    animate_filling(&state->filling);
    break;
  case kSteps:
    animate_steps(&state->steps);
    break;
  case kCross:
    animate_cross(&state->cross);
    break;
  }

  state->tick = tick;

  if (pressed_pass)
    return;

  if (!pressed_tick) {
    pressed_tick = tick;
    return;
  }

  if (tick > pressed_tick + PRESSED_DELAY)
    pressed_pass = 1;
}
