#ifndef SRC_ANIMATION_H_
#define SRC_ANIMATION_H_

#include "stm32f4xx_hal.h"

typedef struct Circle {
  uint32_t tick;
  uint32_t delay;
  int index;
} Circle;

typedef struct Filling {
  uint32_t tick;
  uint32_t delay;
  GPIO_PinState state;
} Filling;

typedef struct Steps {
  uint32_t tick;
  uint32_t delay;
  int dir;
  int index;
} Steps;

typedef struct Cross {
  uint32_t tick;
  uint32_t delay;
  int index;
} Cross;

typedef struct AnimationsState {
  union {
    Circle circle;
    Filling filling;
    Steps steps;
    Cross cross;
  };
  uint32_t delay;
  int index;
} AnimationsState;

void next_animation();
AnimationsState init_animations_state();
void animate(AnimationsState *state);

#endif /* SRC_ANIMATION_H_ */
