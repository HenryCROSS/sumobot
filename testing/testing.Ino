#define SPEEDA 10               // Motor A enable pin
#define SPEEDB 9               // Motor B enable pin
#define RIGHT_WHEEL_FORWARD 3   // Motor B In1 pin
#define RIGHT_WHEEL_BACKWARD 2  // Motor B In2 pin
#define LEFT_WHEEL_FORWARD 5    // Motor A In1 pin
#define LEFT_WHEEL_BACKWARD 4   // Motor A In2 pin
#define TRIGGER_PIN 6           // Ultrasonic sensor trigger pin
#define ECHO_PIN 7              // Ultrasonic sensor echo pin
#define QTR_SENSOR_FL A5         // qtr sensor
#define QTR_SENSOR_FR A4         // qtr sensor
// #define QTR_SENSOR_BL 15        // waiting for qtr sensor
// #define QTR_SENSOR_BR 16        // waiting for qtr sensor

#define MAX_DISTANCE 200

enum class Edge_direction {
  FRONT,
  FRONT_RIGHT,
  FRONT_LEFT,
  NONE,
};

void setup() {
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

void wheel_forward(uint8_t forward_pin, uint8_t backward_pin) {
  digitalWrite(forward_pin, HIGH);
  digitalWrite(backward_pin, LOW);
}

void wheel_backward(uint8_t forward_pin, uint8_t backward_pin) {
  digitalWrite(forward_pin, LOW);
  digitalWrite(backward_pin, HIGH);
}

void wheel_stop(uint8_t forward_pin, uint8_t backward_pin) {
  digitalWrite(forward_pin, LOW);
  digitalWrite(backward_pin, LOW);
}

void wheel_move_speed(uint8_t pin, int speed) {
  analogWrite(pin, speed);
}

void car_go_forward(int speed) {
  wheel_move_speed(SPEEDA, speed);
  wheel_move_speed(SPEEDB, speed);
  wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
  wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_go_backward(int speed) {
  wheel_move_speed(SPEEDA, speed);
  wheel_move_speed(SPEEDB, speed);
  wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
  wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_turn_right_by_speed(int left_wheel_forward_speed, int right_wheel_backward_speed) {
  wheel_move_speed(SPEEDA, left_wheel_forward_speed);
  wheel_move_speed(SPEEDB, right_wheel_backward_speed);
  wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
  wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_turn_left_by_speed(int left_wheel_backward_speed, int right_wheel_forward_speed) {
  wheel_move_speed(SPEEDA, left_wheel_backward_speed);
  wheel_move_speed(SPEEDB, right_wheel_forward_speed);
  wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
  wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
}

void car_turn_left(int speed) {
  car_turn_left_by_speed(speed, 0);
}

void car_turn_right(int speed) {
  car_turn_right_by_speed(0, speed);
}

void car_stop(void) {
  wheel_stop(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
  wheel_stop(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}



double detect_obj_distance(uint8_t trigger_pin, uint8_t echo_pin) {
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);
  return pulseIn(echo_pin, HIGH, MAX_DISTANCE * 58) / 58.0;
}

Edge_direction determine_edge(uint8_t qtr_sensor_front_left, uint8_t qtr_sensor_front_right) {
  int frontL = analogRead(qtr_sensor_front_left);
  int frontR = analogRead(qtr_sensor_front_right);

  // Serial.print("FrontL: ");
  // Serial.println(frontL);

  if (frontL <= 400 && frontR <= 400) {
    return Edge_direction::FRONT;
  } else if (frontL <= 400) {
    return Edge_direction::FRONT_LEFT;
  } else if (frontR <= 400) {
    return Edge_direction::FRONT_RIGHT;
  } else {
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
template<typename Fn, typename... Args>
void do_actions_duration(unsigned long ms, Fn fn, Args... args) {
  fn(args...);
  delay(ms);
}

// range ~= 60
bool is_obj_exist(double range) {
  double distance = detect_obj_distance(TRIGGER_PIN, ECHO_PIN);


  if (distance < range) {
    return true;
  } else {
    return false;
  }
}

void search_strategy(Edge_direction edge) {
  switch (edge) {
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

void loop() {
  // put your main code here, to run repeatedly:
  static Edge_direction prev_turning_edge;
  auto edge = determine_edge(QTR_SENSOR_FL, QTR_SENSOR_FR);

  Serial.println("======");
  switch (edge) {
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
        1, [&](Edge_direction e) mutable {
          if (is_obj_exist(2.0)) {
            car_go_forward(255);
            Serial.println("go 255! ");
          }
          else if (is_obj_exist(5.0)) {
            car_go_forward(150);
            Serial.println("go 150! ");
          }
          else if (is_obj_exist(60.0)){
            car_go_forward(80);            
            Serial.println("go 90! ");
          } else {
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