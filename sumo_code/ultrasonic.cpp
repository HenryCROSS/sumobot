#include "ultrasonic.h"

USWriter::USWriter(uint8_t pin) : pin(pin)
{
}

USReader::USReader(uint8_t pin) : pin(pin)
{
}

Ultrasonic::Ultrasonic(uint8_t reader_pin, uint8_t writer_pin) : writer(writer_pin), reader(reader_pin)
{
}

Ultrasonic::Ultrasonic(USReader reader_src, USWriter writer_src) : writer(writer_src.pin), reader(reader_src.pin)
{
}

// double detect_obj_distance(uint8_t trigger_pin, uint8_t echo_pin)
double detect_obj_distance(USWriter writer, USReader reader)
{
    digitalWrite(writer.pin, LOW);
    delayMicroseconds(2);
    digitalWrite(writer.pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(writer.pin, LOW);
    return pulseIn(reader.pin, HIGH, MAX_DISTANCE * 58) / 58.0;
}
