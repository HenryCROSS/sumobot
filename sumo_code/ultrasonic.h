#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#include <cstdint>

#define MAX_DETECTED_DISTANCE 200

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

double detect_obj_distance(USWriter writer, USReader reader);

#endif // ULTRASONIC_H_
