#include "Vehicle_actions.hpp"

void search_strategy(Maybe<Edge_Signal> edge)
{
    if (edge.hasValue())
    {
        switch (edge.getValue())
        {
        case Edge_Signal::FRONT:
        case Edge_Signal::FRONT_LEFT:
            // Serial.print("turn right ");
            car_turn_right_by_speed(80, 80);
            break;

        case Edge_Signal::BACK:
            car_go_random(80);
            break;

        case Edge_Signal::FRONT_RIGHT:
            car_turn_left_by_speed(80, 80);
        default:
            // Never exec
            break;
        }
    }
    else
    {
        car_turn_left_by_speed(80, 80);
    }
}