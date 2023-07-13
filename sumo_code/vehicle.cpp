#include "vehicle.h"

Motor::Motor(uint8_t pin)
  : pin(pin) {
}

Wheel::Wheel(uint8_t pin)
  : pin(pin) {
}

Vehicle::Vehicle(Motor left, Motor right, Wheel left_forward, Wheel left_backward,
         Wheel right_forward, Wheel right_backward)
  : left_wheel_motor(left.pin),
    right_wheel_motor(right.pin),
    left_wheel_forward(left_forward.pin),
    left_wheel_backward(left_backward.pin),
    right_wheel_forward(right_forward.pin),
    right_wheel_backward(right_backward.pin) {
}

void wheel_forward(Wheel forward, Wheel backward) {
  digitalWrite(forward.pin, HIGH);
  digitalWrite(backward.pin, LOW);
}

void wheel_backward(Wheel forward, Wheel backward) {
  digitalWrite(forward.pin, LOW);
  digitalWrite(backward.pin, HIGH);
}

void wheel_stop(Wheel forward, Wheel backward) {
  digitalWrite(forward.pin, LOW);
  digitalWrite(backward.pin, LOW);
}

void wheel_move_speed(Motor motor, int speed) {
  analogWrite(motor.pin, speed);
}

void car_go_forward(Vehicle car, int speed) {
  wheel_move_speed(car.left_wheel_motor, speed);
  wheel_move_speed(car.right_wheel_motor, speed);
  wheel_forward(car.left_wheel_forward, car.left_wheel_forward);
  wheel_forward(car.right_wheel_forward, car.right_wheel_backward);
}

void car_go_backward(Vehicle car, int speed) {
  wheel_move_speed(car.left_wheel_motor, speed);
  wheel_move_speed(car.right_wheel_motor, speed);
  wheel_backward(car.left_wheel_forward, car.left_wheel_backward);
  wheel_backward(car.right_wheel_forward, car.right_wheel_backward);
}

void car_turn_right_by_speed(Vehicle car, int left_wheel_forward_speed, int right_wheel_backward_speed) {
  wheel_move_speed(car.left_wheel_motor, left_wheel_forward_speed);
  wheel_move_speed(car.right_wheel_motor, right_wheel_backward_speed);
  wheel_forward(car.left_wheel_forward, car.left_wheel_backward);
  wheel_backward(car.right_wheel_forward, car.right_wheel_backward);
}

void car_turn_left_by_speed(Vehicle car, int left_wheel_backward_speed, int right_wheel_forward_speed) {
  wheel_move_speed(car.left_wheel_motor, left_wheel_backward_speed);
  wheel_move_speed(car.right_wheel_motor, right_wheel_forward_speed);
  wheel_forward(car.right_wheel_forward, car.right_wheel_backward);
  wheel_backward(car.left_wheel_forward, car.left_wheel_backward);
}

void car_turn_left(Vehicle car, int speed) {
  car_turn_left_by_speed(car, speed, 0);
}

void car_turn_right(Vehicle car, int speed) {
  car_turn_right_by_speed(car, 0, speed);
}

void car_stop(Vehicle car) {
  wheel_stop(car.left_wheel_forward, car.left_wheel_backward);
  wheel_stop(car.right_wheel_forward, car.right_wheel_backward);
}
