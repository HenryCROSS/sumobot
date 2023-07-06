#include "strategy.h"
#include "ultrasonic.h"

bool is_obj_exist(Ultrasonic ultra, unsigned long max_distance, double range)
{
    double distance = detect_obj_distance(ultra.writer, ultra.reader, max_distance);

    if (distance < range)
    {
        return true;
    }
    else
    {
        return false;
    }
}
