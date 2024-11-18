#include "animation.h"

#define arrlen(arr) (sizeof(arr) / sizeof(*arr))

#define DEFAULT_DELAY ((uint32_t)750)
#define PRESSED_DELAY ((int)50)

static uint16_t leds[] = {GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};

void reset_leds() {
	for (int i = 0; i < arrlen(leds); i++)
		HAL_GPIO_WritePin(GPIOD, leds[i], GPIO_PIN_RESET);
}

Circle init_circle() {
  Circle circle = {.tick = HAL_GetTick(), .index = 0, .delay = DEFAULT_DELAY};
  return circle;
}

void animate_circle(uint32_t tick, Circle *circle) {
	if (tick < circle->delay + circle->tick)
		return;

	circle->tick = tick;

	HAL_GPIO_WritePin(GPIOD, leds[circle->index], GPIO_PIN_RESET);
	circle->index = (circle->index + 1) % arrlen(leds);
	HAL_GPIO_WritePin(GPIOD, leds[circle->index], GPIO_PIN_SET);
}

Filling init_filling() {
  Filling filling = {.tick = HAL_GetTick(), .state = GPIO_PIN_SET, .delay = DEFAULT_DELAY};
  return filling;
}

void animate_filling(uint32_t tick, Filling *filling) {
	if (tick < filling->delay + filling->tick)
		return;

	filling->tick = tick;

	filling->state = (filling->state + 1) % 2;

	for (int i = 0; i < arrlen(leds); i++)
		HAL_GPIO_WritePin(GPIOD, leds[i], filling->state);
}

Steps init_steps() {
  Steps steps = {.tick = HAL_GetTick(), .dir = 1, .index = 0, .delay = DEFAULT_DELAY};
  return steps;
}

void animate_steps(uint32_t tick, Steps *steps) {
	if (tick < steps->delay + steps->tick)
		return;

	steps->tick = tick;

	GPIO_PinState state = steps->dir == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET;

	HAL_GPIO_WritePin(GPIOD, leds[steps->index], state);

	steps->index += steps->dir;

	if (steps->index == arrlen(leds) ||
      steps->index == -1) {
		steps->dir *= -1;
		steps->index += steps->dir;
	}
}

Cross init_cross() {
  Cross cross = {.tick = HAL_GetTick(), .index = 0, .delay = DEFAULT_DELAY};
  return cross;
}

void animate_cross(uint32_t tick, Cross *cross) {
	if (tick < cross->delay + cross->tick)
		return;

	cross->tick = tick;

	int first_i = cross->index % arrlen(leds);
	int second_i = (cross->index + 2) % arrlen(leds);

	HAL_GPIO_WritePin(GPIOD, leds[first_i], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, leds[second_i], GPIO_PIN_RESET);

	int third_i = (cross->index + 1) % arrlen(leds);
	int fourth_i = (cross->index + 3) % arrlen(leds);

	HAL_GPIO_WritePin(GPIOD, leds[third_i], GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOD, leds[fourth_i], GPIO_PIN_SET);

	cross->index++;
}

#define animations_amount ((int)4)

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
  state->index = (state->index + 1) % animations_amount;
  switch (state->index) {
  case 0:
    state->circle = init_circle();
    state->circle.delay = state->delay;
    break;
  case 1:
    state->filling = init_filling();
    state->filling.delay = state->delay;
    break;
  case 2:
    state->steps = init_steps();
    state->steps.delay = state->delay;
    break;
  case 3:
    state->cross = init_cross();
    state->cross.delay = state->delay;
    break;
  };
}

AnimationsState init_animations_state() {
  AnimationsState state = {
    .index = 0,
    .circle = init_circle(),
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
  switch (state->index) {
  case 0:
    animate_circle(tick, &state->circle);
    break;
  case 1:
    animate_filling(tick, &state->filling);
    break;
  case 2:
    animate_steps(tick, &state->steps);
    break;
  case 3:
    animate_cross(tick, &state->cross);
    break;
  };

  if (pressed_pass)
    return;

  if (!pressed_tick) {
    pressed_tick = tick;
    return;
  }

  if (tick > pressed_tick + PRESSED_DELAY)
    pressed_pass = 1;
}
