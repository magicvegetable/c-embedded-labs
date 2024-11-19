#ifndef SRC_ANIMATION_H_
#define SRC_ANIMATION_H_

#include "stm32f4xx_hal.h"

typedef struct {
  int index;
} Circle;

typedef struct {
  GPIO_PinState state;
} Filling;

typedef struct {
  int dir;
  int index;
} Steps;

typedef struct {
  int index;
} Cross;

typedef enum {
  kAnimateCircle,
  kAnimateFilling,
  kAnimateSteps,
  kAnimateCross
} Animation;

#define ANIMATIONS_AMOUNT (kAnimateCross + 1)

typedef struct {
  union {
    Circle circle;
    Filling filling;
    Steps steps;
    Cross cross;
  };
  uint32_t delay;
  uint32_t tick;
  Animation index;
} AnimationsState;

void next_animation();
AnimationsState init_animations_state();
void animate(AnimationsState *state);

#endif /* SRC_ANIMATION_H_ */
