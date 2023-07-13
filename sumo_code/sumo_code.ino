#include "direction.h"
#include "qtr_sensor.h"
#include "strategy.h"
#include "type.h"
#include "ultrasonic.h"
#include "vehicle.h"

#define SPEEDA 10               // Motor A enable pin
#define SPEEDB 9                // Motor B enable pin
#define RIGHT_WHEEL_FORWARD 3   // Motor B In1 pin
#define RIGHT_WHEEL_BACKWARD 2  // Motor B In2 pin
#define LEFT_WHEEL_FORWARD 5    // Motor A In1 pin
#define LEFT_WHEEL_BACKWARD 4   // Motor A In2 pin
#define TRIGGER_PIN 6           // Ultrasonic sensor trigger pin
#define ECHO_PIN_LEFT 7         // Ultrasonic sensor echo pin
#define ECHO_PIN_RIGHT 8        // Ultrasonic sensor echo pin
#define QTR_SENSOR_FL A5        // qtr sensor
#define QTR_SENSOR_FR A4        // qtr sensor
#define QTR_SENSOR_B 15         // waiting for qtr sensor

#define MAX_DETECTED_DISTANCE 200



void setup() {
  // put your setup code here, to run once:
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN_LEFT, INPUT);
  pinMode(ECHO_PIN_RIGHT, INPUT);
  pinMode(LEFT_WHEEL_FORWARD, OUTPUT);
  pinMode(LEFT_WHEEL_BACKWARD, OUTPUT);
  pinMode(RIGHT_WHEEL_FORWARD, OUTPUT);
  pinMode(RIGHT_WHEEL_BACKWARD, OUTPUT);
  pinMode(SPEEDA, OUTPUT);
  pinMode(SPEEDB, OUTPUT);
  pinMode(QTR_SENSOR_FL, INPUT);
  pinMode(QTR_SENSOR_FR, INPUT);
  pinMode(QTR_SENSOR_B, INPUT);

  delay(5000);
  Serial.begin(9600);
}

void loop() {
}
