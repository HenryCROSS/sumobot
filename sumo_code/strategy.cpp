#include "strategy.h"
#include "ultrasonic.h"

bool is_obj_exist(Ultrasonic ultra, double range)
{
    double distance = detect_obj_distance(ultra.writer, ultra.reader);

    if (distance < range)
    {
        return true;
    }
    else
    {
        return false;
    }
}
