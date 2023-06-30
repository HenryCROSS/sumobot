#include "wheel.h"

void wheel_forward(uint8_t forward_pin, uint8_t backward_pin)
{
    digitalWrite(forward_pin, HIGH);
    digitalWrite(backward_pin, LOW);
}

void wheel_backward(uint8_t forward_pin, uint8_t backward_pin)
{
    digitalWrite(forward_pin, LOW);
    digitalWrite(backward_pin, HIGH);
}

void wheel_stop(uint8_t forward_pin, uint8_t backward_pin)
{
    digitalWrite(forward_pin, LOW);
    digitalWrite(backward_pin, LOW);
}

void wheel_move_speed(uint8_t pin, int speed)
{
    analogWrite(pin, speed);
}

void car_go_forward(int speed)
{
    wheel_move_speed(SPEEDA, speed);
    wheel_move_speed(SPEEDB, speed);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_go_backward(int speed)
{
    wheel_move_speed(SPEEDA, speed);
    wheel_move_speed(SPEEDB, speed);
    wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_turn_right_by_speed(int left_wheel_forward_speed, int right_wheel_backward_speed)
{
    wheel_move_speed(SPEEDA, left_wheel_forward_speed);
    wheel_move_speed(SPEEDB, right_wheel_backward_speed);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_turn_left_by_speed(int left_wheel_backward_speed, int right_wheel_forward_speed)
{
    wheel_move_speed(SPEEDA, left_wheel_backward_speed);
    wheel_move_speed(SPEEDB, right_wheel_forward_speed);
    wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
    wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
}

void car_turn_left(int speed)
{
    car_turn_left_by_speed(speed, 0);
}

void car_turn_right(int speed)
{
    car_turn_right_by_speed(0, speed);
}

void car_stop(void)
{
    wheel_stop(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_stop(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}
