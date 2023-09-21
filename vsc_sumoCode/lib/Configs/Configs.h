#ifndef _PIN_H_
#define _PIN_H_

#define LEFT_MOTOR 7             // Motor A enable pin
#define RIGHT_MOTOR 12           // Motor B enable pin
#define RIGHT_WHEEL_FORWARD 10   // Motor B In1 pin
#define RIGHT_WHEEL_BACKWARD 11  // Motor B In2 pin
#define LEFT_WHEEL_FORWARD 8     // Motor A In1 pin
#define LEFT_WHEEL_BACKWARD 9    // Motor A In2 pin
#define TRIGGER_PIN_L 2          // Ultrasonic sensor trigger pin
#define ECHO_PIN_L 3             // Ultrasonic sensor echo pin
#define TRIGGER_PIN_R 4          // Ultrasonic sensor trigger pin
#define ECHO_PIN_R 5             // Ultrasonic sensor echo pin
#define QTR_SENSOR_FL A2         // qtr sensor
#define QTR_SENSOR_FR A0         // qtr sensor
#define QTR_SENSOR_B A1          // qtr sensor

#define MAX_DISTANCE (200)        // Max detect distance
#define TIMESLICE (10)            // time for each action lasts
#define E (2.71828)               // value of e

#define QTR_THRESHOLD (500)

#define MAX_TASKS 20

#endif // !_PIN_H_