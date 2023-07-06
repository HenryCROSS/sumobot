#ifndef WHEEL_H_
#define WHEEL_H_

#include "common.h"

struct Motor {
  const uint8_t pin;
  Motor(uint8_t pin);
};

struct Wheel {
  const uint8_t pin;
  Wheel(uint8_t pin);
};

struct Vehicle {
  const Motor left_wheel_motor;
  const Wheel left_wheel_forward;
  const Wheel left_wheel_backward;

  const Motor right_wheel_motor;
  const Wheel right_wheel_forward;
  const Wheel right_wheel_backward;

  Vehicle(Motor left, Motor right, Wheel left_forward, Wheel left_backward,
      Wheel right_forward, Wheel right_backward);
};

void wheel_forward(Wheel forward, Wheel backward);

void wheel_backward(Wheel forward, Wheel backward);

void wheel_stop(Wheel forward, Wheel backward);

void wheel_move_speed(Motor motor, int speed);

void car_go_forward(Vehicle car, int speed);

void car_go_backward(Vehicle car, int speed);

void car_turn_right_by_speed(Vehicle car, int left_wheel_forward_speed, int right_wheel_backward_speed);

void car_turn_left_by_speed(Vehicle car, int left_wheel_backward_speed, int right_wheel_forward_speed);

void car_turn_left(Vehicle car, int speed);

void car_turn_right(Vehicle car, int speed);

void car_stop(Vehicle car);

#endif  // WHEEL_H_
