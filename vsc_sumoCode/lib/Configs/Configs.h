#ifndef _PIN_H_
#define _PIN_H_

#define LEFT_MOTOR 10             // Motor A enable pin
#define RIGHT_MOTOR 11           // Motor B enable pin
// #define RIGHT_WHEEL_FORWARD 7   // Motor B In1 pin
// #define RIGHT_WHEEL_BACKWARD 8  // Motor B In2 pin
// #define LEFT_WHEEL_FORWARD 9     // Motor A In1 pin
// #define LEFT_WHEEL_BACKWARD 12    // Motor A In2 pin
#define RIGHT_WHEEL_FORWARD 9   // Motor B In1 pin
#define RIGHT_WHEEL_BACKWARD 12  // Motor B In2 pin
#define LEFT_WHEEL_FORWARD 7     // Motor A In1 pin
#define LEFT_WHEEL_BACKWARD 8    // Motor A In2 pin
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

#define QTR_THRESHOLD (650)
// #define QTR_THRESHOLD (250)

#define SPEED (75)

#define ROTATE_ONE_DEGREE_IN_MS (180.0 / 480.0) //in speed 75, not accurate

#define MAX_TASKS (6)

#define MAX_QTR_SCAN_TIME (10)

#define DEBUG (true)

#endif // !_PIN_H_