#ifndef QTR_SENSOR_H_
#define QTR_SENSOR_H_

enum class Edge_direction;

struct QTRSensor
{
    const uint8_t pin;
    QTRSensor(uint8_t pin);
};

Edge_direction determine_edge(QTRSensor qtr_sensor_front_left, QTRSensor qtr_sensor_front_right);

#endif // QTR_SENSOR_H_
