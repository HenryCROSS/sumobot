#include "qtr_sensor.h"
#include "direction.h"

QTRSensor::QTRSensor(uint8_t pin) : pin(pin)
{
}

Edge_direction determine_edge(QTRSensor qtr_sensor_front_left, QTRSensor qtr_sensor_front_right)
{
    int frontL = analogRead(qtr_sensor_front_left.pin);
    int frontR = analogRead(qtr_sensor_front_right.pin);

    if (frontL <= 400 && frontR <= 400)
    {
        return Edge_direction::FRONT;
    }
    else if (frontL <= 400)
    {
        return Edge_direction::FRONT_LEFT;
    }
    else if (frontR <= 400)
    {
        return Edge_direction::FRONT_RIGHT;
    }
    else
    {
        return Edge_direction::NO_EDGE;
    }
}
