#include "strategy.h"
#include "ultrasonic.h"

bool is_obj_exist(double range)
{
    double distance = detect_obj_distance(TRIGGER_PIN, ECHO_PIN);

    if (distance < range)
    {
        return true;
    }
    else
    {
        return false;
    }
}
