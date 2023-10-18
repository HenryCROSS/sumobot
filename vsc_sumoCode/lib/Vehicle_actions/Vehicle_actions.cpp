#include <Arduino.h>
#include <Configs.h>
#include <Vehicle_utils.hpp>

#include "Vehicle_actions.hpp"

static bool rotation(int distance, int speed, unsigned long ms);

bool search_strategy(Strategy strategy, int distance, int speed, unsigned long ms)
{
    switch (strategy)
    {
    case Strategy::ROTATION:
        return rotation(distance, speed, ms);
        break;

    default:
        break;
    }

    return false;
}

static bool rotation(int distance, int speed, unsigned long ms)
{
    static uint16_t last = 0;
    static Maybe<Edge_Signal> signal = Maybe<Edge_Signal>::Nothing();

    bool found = false;


    while (last < ms && !found)
    {
        if (signal.hasValue())
        {
            switch (signal.getValue())
            {
            case Edge_Signal::FRONT:
                car_turn_left_by_speed(speed, speed);
                break;
            case Edge_Signal::FRONT_LEFT:
                car_turn_right_by_speed(speed, speed);
                break;
            case Edge_Signal::FRONT_RIGHT:
                car_turn_left_by_speed(speed, speed);
                break;
            case Edge_Signal::BACK:
                car_go_forward(speed);
                break;
            default:
                break;
            }
        }
        else
        {
            car_turn_left_by_speed(speed, speed);
        }

        last += TIMESLICE;
        delay(TIMESLICE);

        signal = determine_edge(QTR_SENSOR_FL, QTR_SENSOR_FR, QTR_SENSOR_B);

        auto info = obj_detection_info();
        found = is_obj_in_distance(info, distance);
    }

    last = 0;

    return found;
}

bool attack_strategy(int speed, unsigned long ms){
    car_go_backward(speed * 0.7);
    delay(ms);
    car_go_forward(speed);
}