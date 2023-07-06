#define SPEEDA 10              // Motor A enable pin
#define SPEEDB 9               // Motor B enable pin
#define RIGHT_WHEEL_FORWARD 3  // Motor B In1 pin
#define RIGHT_WHEEL_BACKWARD 2 // Motor B In2 pin
#define LEFT_WHEEL_FORWARD 5   // Motor A In1 pin
#define LEFT_WHEEL_BACKWARD 4  // Motor A In2 pin
#define TRIGGER_PIN 6          // Ultrasonic sensor trigger pin
#define ECHO_PIN 7             // Ultrasonic sensor echo pin
#define QTR_SENSOR_FL A5       // qtr sensor
#define QTR_SENSOR_FR A4       // qtr sensor
// #define QTR_SENSOR_BL 15        // waiting for qtr sensor
// #define QTR_SENSOR_BR 16        // waiting for qtr sensor

#define MAX_DISTANCE 200

#include <stdint.h>

enum class Edge_direction
{
    FRONT,
    FRONT_RIGHT,
    FRONT_LEFT,
    NONE,
};

void setup()
{
    // put your setup code here, to run once:
    pinMode(TRIGGER_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
    pinMode(LEFT_WHEEL_FORWARD, OUTPUT);
    pinMode(LEFT_WHEEL_BACKWARD, OUTPUT);
    pinMode(RIGHT_WHEEL_FORWARD, OUTPUT);
    pinMode(RIGHT_WHEEL_BACKWARD, OUTPUT);
    pinMode(SPEEDA, OUTPUT);
    pinMode(SPEEDB, OUTPUT);
    pinMode(QTR_SENSOR_FL, INPUT);
    pinMode(QTR_SENSOR_FR, INPUT);
    // pinMode(QTR_SENSOR_BL, INPUT);
    // pinMode(QTR_SENSOR_BR, INPUT);

    delay(5000);
    Serial.begin(9600);
}

void wheel_forward(uint8_t forward_pin, uint8_t backward_pin)
{
    digitalWrite(forward_pin, HIGH);
    digitalWrite(backward_pin, LOW);
}

void wheel_backward(uint8_t forward_pin, uint8_t backward_pin)
{
    digitalWrite(forward_pin, LOW);
    digitalWrite(backward_pin, HIGH);
}

void wheel_stop(uint8_t forward_pin, uint8_t backward_pin)
{
    digitalWrite(forward_pin, LOW);
    digitalWrite(backward_pin, LOW);
}

void wheel_move_speed(uint8_t pin, int speed)
{
    analogWrite(pin, speed);
}

void car_go_forward(int speed)
{
    wheel_move_speed(SPEEDA, speed);
    wheel_move_speed(SPEEDB, speed);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_go_backward(int speed)
{
    wheel_move_speed(SPEEDA, speed);
    wheel_move_speed(SPEEDB, speed);
    wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_turn_right_by_speed(int left_wheel_forward_speed, int right_wheel_backward_speed)
{
    wheel_move_speed(SPEEDA, left_wheel_forward_speed);
    wheel_move_speed(SPEEDB, right_wheel_backward_speed);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_turn_left_by_speed(int left_wheel_backward_speed, int right_wheel_forward_speed)
{
    wheel_move_speed(SPEEDA, left_wheel_backward_speed);
    wheel_move_speed(SPEEDB, right_wheel_forward_speed);
    wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
    wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
}

void car_turn_left(int speed)
{
    car_turn_left_by_speed(speed, 0);
}

void car_turn_right(int speed)
{
    car_turn_right_by_speed(0, speed);
}

void car_stop(void)
{
    wheel_stop(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_stop(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

double detect_obj_distance(uint8_t trigger_pin, uint8_t echo_pin)
{
    digitalWrite(trigger_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigger_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigger_pin, LOW);
    return pulseIn(echo_pin, HIGH, MAX_DISTANCE * 58) / 58.0;
}

Edge_direction determine_edge(uint8_t qtr_sensor_front_left, uint8_t qtr_sensor_front_right)
{
    int frontL = analogRead(qtr_sensor_front_left);
    int frontR = analogRead(qtr_sensor_front_right);

    // Serial.print("FrontL: ");
    // Serial.println(frontL);

    if (frontL <= 400 && frontR <= 400)
    {
        return Edge_direction::FRONT;
    }
    else if (frontL <= 400)
    {
        return Edge_direction::FRONT_LEFT;
    }
    else if (frontR <= 400)
    {
        return Edge_direction::FRONT_RIGHT;
    }
    else
    {
        return Edge_direction::NONE;
    }
}

/*
  Example:
  actions_duration(1000, [](){
      car_stop();
  });
  actions_duration(1000, car_turn_left, 100);
  actions_duration(1000, car_stop());
*/
template <typename Fn, typename... Args> void do_actions_duration(unsigned long ms, Fn fn, Args... args)
{
    fn(args...);
    delay(ms);
}

// range ~= 60
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

void search_strategy(Edge_direction edge)
{
    switch (edge)
    {
    case Edge_direction::FRONT:
    case Edge_direction::FRONT_LEFT:
        // Serial.print("turn right ");
        car_turn_right_by_speed(80, 80);
        break;

    case Edge_direction::FRONT_RIGHT:
    default:
        // Serial.print("turn left ");
        car_turn_left_by_speed(80, 80);
        break;

    case Edge_direction::NONE:
        // Serial.println("???");
        break;
    }
}

void loop()
{
    // put your main code here, to run repeatedly:
    static Edge_direction prev_turning_edge;
    auto edge = determine_edge(QTR_SENSOR_FL, QTR_SENSOR_FR);

    Serial.println("======");
    switch (edge)
    {
    case Edge_direction::FRONT:
        // Serial.println("Go backward");
        do_actions_duration(300, car_go_backward, 100);
        do_actions_duration(150, car_turn_right_by_speed, 90, 90);
        prev_turning_edge = edge;
        break;

    case Edge_direction::FRONT_LEFT:
        // Serial.println("Front left");
        do_actions_duration(1, car_turn_right_by_speed, 100, 100);
        prev_turning_edge = edge;
        break;

    case Edge_direction::FRONT_RIGHT:
        // Serial.println("Front right");
        do_actions_duration(1, car_turn_left_by_speed, 100, 100);
        prev_turning_edge = edge;
        break;

    case Edge_direction::NONE:
        do_actions_duration(
            1,
            [&](Edge_direction e) mutable {
                if (is_obj_exist(2.0))
                {
                    car_go_forward(255);
                    Serial.println("go 255! ");
                }
                else if (is_obj_exist(5.0))
                {
                    car_go_forward(150);
                    Serial.println("go 150! ");
                }
                else if (is_obj_exist(60.0))
                {
                    car_go_forward(80);
                    Serial.println("go 90! ");
                }
                else
                {
                    Serial.println("dectect! ");
                    // search action
                    search_strategy(e);
                }
            },
            prev_turning_edge);
        break;

    default:
        Serial.println("default");
        car_stop();
        // do_actions_duration(100, []() {
        // });
        break;
    }
    delay(1);
}


/*
  Tasks:
  - Poll sensors (using a function that specifies states, given inputs)                         DONE
  - Create a function that handles: motor speed, motor direction, time motor is ran.            DONE
  - Create a second function that handles each flag and keeps track of changes in flags         DONE
  - Create basic homing function that "latches" onto the target                                 WIP/DONE
  - Find ideal moving speed to prevent "too fast" switching from edge -> arena -> edge

  Priority of functions:
  - Keep track of changes in flags. Call sensor-polling function to achieve this.               DONE
  -
*/

// // Choose your pins
// // Enable pins are needed to control motor speed (through PWM)

// // Pins for motor 1
// #define Pin1A 2
// #define Pin2A 3
// #define ENA 10

// // pins for motor 2
// #define Pin3A 4
// #define Pin4A 5
// #define ENB 11

// // pins for ultrasonic sensor
// #define TRIGGER 8
// #define ECHO 9

// // pins for the two reflectance sensors
// #define QTR_L 6
// #define QTR_R 7

// unsigned long start, interval = 0;
// float distance = 0;
// int prevDistance;
// volatile int flag, previousflag;
// int edgeLeft, edgeRight;

// // make similar arrays for doing fancier movements. To keep things simple, the size of the array is fixed to n, n >=
// 4;

// // Voltage must also be above a certain value, or there is not enough torque to turn the wheel. Roughly half of
// supply(50% duty cycle, 128).
// // Only solution is to reduce time, as much as possible.
// int ramSpeed[] = {200};
// int ramDirection[] = {1};
// int ramTime[] = {50};

// int homingSpeed[] = {0,80,0,60};
// int homingDirection1[] = {4,4,4,1};
// int homingDirection2[] = {3,3,3,1};
// int homingTime[] = {20,125,20,125};

// int scanSpeed[] = {160,160,160,160,160,160,160,170};
// int scanDirection[] = {3,3,3,3,4,4,4,4};
// int scanTime[] = {100,100,100,100,100,100,100,100};

// int edgeSpeed[] = {0,100,0,150};
// int edge_L_Direction[]  = {2,2,2,3};
// int edge_R_Direction[]  = {2,2,2,4};
// int edgeTime[] = {50,600,50,50}; // change to lower values to add some delay to responsiveness of border and arena

// int frictionFactor = 1.25; // 1 for same speed. > 1 for smoother surfaces, < 1 for rougher ones.

// void setup() {
//   // put your setup code here, to run once:
//   Serial.begin(9600);

//   // Set up your 6 motor-related pins(control)
//   pinMode(Pin1A, OUTPUT);   pinMode(Pin3A, OUTPUT);
//   pinMode(Pin2A, OUTPUT);   pinMode(Pin4A, OUTPUT);
//   pinMode(ENA, OUTPUT);     pinMode(ENB, OUTPUT);

//   // Set up your sensor pins
//   pinMode(TRIGGER, OUTPUT);
//   pinMode(ECHO, INPUT);
//   pinMode(QTR_L, INPUT);
//   pinMode(QTR_R, INPUT);

//   // Call once to get a starting flag
//   manualUSonic();
//   edgeSense();  // without this, the ADC is set to some high(?) value and immediately thinks the bot has hit arena
//   border. flagAssignment();

//   // Test single sets of moves
//   delay(5000);
// }

// void loop() {
//   stateMachine();
// }

// // Something about the array breaks the machine.
// // Update 5 mins after: Was never the array. Just bad placement of break when we are in the "border" state
// // Update day after: There is still an issue with the arrays. Best to keep them all the same size, but we can repeat
// entries so we can continue to play with unique movement.
// // Update day + 5 mins: Add a conditional break for if state changes, AND the standard break needed for every case at
// the end. TWO BREAKS.

// void stateMachine(){
//   // The reflectance sensor makes no distinction between all absorbed and no signal received(too far). Both are
//   considered HIGH and must be dealt with as soon as the board hits an edge. previousflag = flag; int i = 0;
//   // Serial.print("Distance: "); Serial.print(distance); Serial.print("| Flag: "); Serial.println(flag);
//   //   Serial.print("Before: "); Serial.print(distance);0 Serial.print("| Flag: "); Serial.println(flag);
//   //   Serial.print(edgeLeft); Serial.print("|"); Serial.println(edgeRight);

//   // *** !!!  FLAG FORCING to test out different arrays of timings !!! *** //
//   // flag = 0b010;

//     switch (flag)
//     {
//       case 0b111:
//         while (i < sizeof(edgeTime)/sizeof(int))
//         {
//           // Serial.print("Before: "); Serial.print(distance); Serial.print("| Flag: "); Serial.println(flag);
//           // Serial.print(edgeLeft); Serial.print("|"); Serial.println(edgeRight);
//           // Serial.println("edge");
//               // issue with case 0, it goes edge but goes to case 1 twice each cycle.
//           motorControl(edgeTime[i], edgeSpeed[i], edge_R_Direction[i]);
//           i++;

//           manualUSonic();
//           edgeSense();
//           flagAssignment(); // see edgeSense(). place outside while so it executes back+spin in full, instead of
//           breaking early.
//         }
//       break;

//       case 0b110:
//         while (i < sizeof(edgeTime)/sizeof(int))
//         {
//           //  Serial.print(edgeLeft); Serial.print("|"); Serial.println(edgeRight);
//           motorControl(edgeTime[i], edgeSpeed[i], edge_L_Direction[i]);
//           i++;

//           manualUSonic();
//           edgeSense();
//           flagAssignment();
//         }
//       break;

//       case 0b000:
//         while (i < sizeof(ramTime)/sizeof(int))
//         {
//           // Serial.print("Before: "); Serial.print(distance); Serial.print("| Flag: "); Serial.println(flag);
//           // Serial.print(edgeLeft); Serial.print("|"); Serial.println(edgeRight);
//           //  Serial.println("ram");
//           motorControl(ramTime[i], ramSpeed[i], ramDirection[i]);
//           i++;

//           manualUSonic();
//           edgeSense();
//           flagAssignment();
//           if (previousflag != flag)
//           {
//     //        Serial.println("break 1");
//             break;
//           }
//         }
//         break;

//       case 0b001:
//             // Serial.println("home");
//         while (i < sizeof(homingTime)/sizeof(int))
//         {
//           if (rand() % 2 == 0)
//           {
//             motorControl(homingTime[i], homingSpeed[i], homingDirection1[i]);
//           }
//           else if (rand() % 2 == 1)
//           {
//             motorControl(homingTime[i], homingSpeed[i], homingDirection2[i]);
//           }
//           i++;

//           manualUSonic();
//           edgeSense();
//           flagAssignment();
//           // // homing logic
//           // prevDistance = distance;
//           // // Serial.println(prevDistance);Serial.println("|");Serial.println(distance);

//           // manualUSonic();
//           // edgeSense();
//           // flagAssignment();
//           // // Serial.println("~~~");
//           // // Serial.println(prevDistance);Serial.println("|");Serial.println(distance);

//           // if (prevDistance > distance)
//           // {
//           //   Serial.println("farther");
//           // }
//           // else if (prevDistance <= distance)
//           // {
//           //   Serial.println("closer");
//           // }

//           if (previousflag != flag)
//           {
//             //  Serial.print  ln("break 2");
//             //  delay(1000);
//             break;
//           }
//         }
//       break;

//       case 0b010:
//       while (i < sizeof(scanTime)/sizeof(int))
//       {
//         // Can result in

//         //  Serial.println("scan");
//         motorControl(scanTime[i], scanSpeed[i], scanDirection[i]);
//         i++;
//         manualUSonic();
//         edgeSense();
//         flagAssignment();
//         if (previousflag != flag)
//         {
//           break;
//         }
//       }
//       break;

//       default:
//        Serial.println("default");
//         manualUSonic();
//         edgeSense();
//         flagAssignment();
//     }
//       previousflag = flag;
// }

// // with the issue in the state machine fixed, maybe expand back to 3 bits to differentiate between left and right
// detection.
// // does default cover all unused states? we will only use, at minimum, 6 states(3 usonic, 3 reflectance)

// void flagAssignment(){
//     // Serial.print("Before: "); Serial.print(distance); Serial.print("| Flag: "); Serial.println(flag);
//   //  Serial.print(edgeLeft); Serial.print("|"); Serial.println(edgeRight);
//   if (edgeLeft <= 100){
//   // light -> -lte 0-50 or 0-100, low. dark -> -gte 650-1000, high(with our distance)
//     flag = 0b110;
//   }
//   else if (edgeRight <= 100){
//     flag = 0b111;
//   }
//   else if (distance > 0 && distance <= 23){
//     flag = 0b000;
//   }
//   else if (distance > 23 && distance < 45){
//     flag = 0b001;
//   }
//   else if (distance >= 45){
//     flag = 0b010;
//   }
// //    Serial.print("After: "); Serial.print(distance); Serial.print("| Flag: "); Serial.println(flag);
// }

// void edgeSense(){
//   // when exiting, disregard flag changes until AFTER the full sequence has been executed. saves time on do-nothing
//   delay. edgeLeft = analogRead(QTR_L); edgeRight = analogRead(QTR_R);
// }

// void manualUSonic(){
//     // put your main code here, to run repeatedly:
//   digitalWrite(TRIGGER, LOW);
//   delayMicroseconds(2);
//   digitalWrite(TRIGGER,HIGH);

//   delayMicroseconds(10);
//   digitalWrite(TRIGGER,LOW);

//   // DISTANCE CALCULATIONS
//   distance = pulseIn(ECHO, HIGH, 30000);  // Change the sensor timeout to 35ms
//   distance = (int) (distance/68);   // experimenting with typecast int to eliminate float. maybe increase processing
//   speed?

//   // Timeout handling
//   if (distance >=2000) distance = 0;
// }

// // Use the contents inside of motorControl to better drive the motors while keeping all sensors active: changing
// states on the fly

//   // Issue with millis(), induced voltage is now a problem. Delay somehow fixes that, likely due to the "do-nothing"
//   // V2, we gut motor control and just do a looped delay like before. We will see how it turns out.
// void motorControl(unsigned long time, int spd, int dir){
//   // long j = 0;
//   // interval = 5; // adjust as needed. delay() parameter
//   // start = time / interval;
//   // motorSetup(spd,dir);
//   // while (j < start){
//   //   motorSetup(spd,dir);
//   //   delay(interval);
//   //   j++;
//   // }

//   interval = millis();
//   motorSetup(spd, dir);
//   while (interval - start <= time){
//    motorSetup(spd, dir);
//     interval = millis();
//     // poll sensors
//     // keep track of flag changes
//   }
//   start = interval;
// }

// // Renamed to motorSetup to make room for a new MotorControl()
// void motorSetup(int spd, int dir)
// {
//   int drive1Pin, drive2Pin, driveENxPin;
//   int drive3Pin, drive4Pin, driveENyPin;

//     drive1Pin = Pin1A;
//     drive2Pin = Pin2A;
//     driveENxPin = ENA;

//     drive3Pin = Pin3A;
//     drive4Pin = Pin4A;
//     driveENyPin = ENB;

//   switch(dir){
//     // Cheat Sheet:
//     /*
//         12 Low-High Forward | 12 High-Low Backward
//         34 High-Low Forward | 34 Low-High Backward
//     */
//   // Forward
//   case 1:
//     digitalWrite(drive1Pin,HIGH);//LOW signal
//     digitalWrite(drive2Pin,LOW);//HIGH signal
//     digitalWrite(drive3Pin,LOW);//LOW signal
//     digitalWrite(drive4Pin,HIGH);//HIGH signal
//     break;
//   // Backwards
//   case 2:
//     digitalWrite(drive1Pin,LOW);//HIGH signal
//     digitalWrite(drive2Pin,HIGH);//LOW signal
//     digitalWrite(drive3Pin,HIGH);//LOW signal
//     digitalWrite(drive4Pin,LOW);//HIGH signal
//     break;
//   // Turn Left
//   case 3:
//     digitalWrite(drive1Pin,LOW);
//     digitalWrite(drive2Pin,HIGH);
//     digitalWrite(drive3Pin,LOW);
//     digitalWrite(drive4Pin,HIGH);
//     break;
//   // Turn Right
//   case 4:
//     digitalWrite(drive1Pin,HIGH);
//     digitalWrite(drive2Pin,LOW);
//     digitalWrite(drive3Pin,HIGH);
//     digitalWrite(drive4Pin,LOW);
//     break;
//   }

//     // After choosing how the motors will turn, you can choose to "chop up" the voltage
//     // Use the enable pin to continuously connect and disconnect the motor. PWM

//     analogWrite(driveENxPin, spd*frictionFactor); // send PWM to driveENPin
//     analogWrite(driveENyPin, spd*frictionFactor); // ENA for left motor, ENB for right motor
// }
