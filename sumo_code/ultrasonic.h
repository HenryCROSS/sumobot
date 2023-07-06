#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define MAX_DETECTED_DISTANCE 200
#include "common.h"

struct USReader
{
    const uint8_t pin;
    USReader(uint8_t pin);
};

struct USWriter
{
    const uint8_t pin;
    USWriter(uint8_t pin);
};

struct Ultrasonic
{
    const USReader reader;
    const USWriter writer;

    Ultrasonic(uint8_t reader_pin, uint8_t writer_pin);
    Ultrasonic(USReader reader_src, USWriter writer_src);
};

enum class TargetZone
{
    ObjSomewhere,
    ObjExists,
    ObjFar,
    ObjClose,
};

double detect_obj_distance(USWriter writer, USReader reader, unsigned long distance);

#endif // ULTRASONIC_H_
