#include <stdint.h>
#ifndef WHEEL_H_
#define WHEEL_H_

#include "common.h"

struct Motor
{
    const uint8_t pin;
    Motor(uint8_t pin);
};

struct Wheel
{
    const uint8_t pin;
    Wheel(uint8_t pin);
};

void wheel_forward(Wheel forward, Wheel backward);

void wheel_backward(Wheel forward, Wheel backward);

void wheel_stop(Wheel forward, Wheel backward);

void wheel_move_speed(Motor motor, int speed);

void car_go_forward(int speed);

void car_go_backward(int speed);

void car_turn_right_by_speed(int left_wheel_forward_speed, int right_wheel_backward_speed);

void car_turn_left_by_speed(int left_wheel_backward_speed, int right_wheel_forward_speed);

void car_turn_left(int speed);

void car_turn_right(int speed);

void car_stop(void);

#endif  // WHEEL_H_
