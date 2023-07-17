#define LEFT_SPEEDA 10               // Motor A enable pin
#define SPEEDB 9                // Motor B enable pin
#define RIGHT_WHEEL_FORWARD 3   // Motor B In1 pin
#define RIGHT_WHEEL_BACKWARD 2  // Motor B In2 pin
#define LEFT_WHEEL_FORWARD 5    // Motor A In1 pin
#define LEFT_WHEEL_BACKWARD 4   // Motor A In2 pin
#define TRIGGER_PIN_L 6         // Ultrasonic sensor trigger pin
#define ECHO_PIN_L 7            // Ultrasonic sensor echo pin
#define TRIGGER_PIN_R 8         // Ultrasonic sensor trigger pin
#define ECHO_PIN_R 9            // Ultrasonic sensor echo pin
#define QTR_SENSOR_FL A5        // qtr sensor
#define QTR_SENSOR_FR A4        // qtr sensor
#define QTR_SENSOR_B A6         // qtr sensor

#define MAX_DISTANCE 200

enum class Edge_direction {
  FRONT,
  FRONT_RIGHT,
  FRONT_LEFT,
  BACK,
  NONE,
};

enum class Adjust_attck_direction {
  TURN_LEFT,
  TURN_RIGHT,
  NOTHING,
};

struct Obj_direction {
  double left_sensor;
  double right_sensor;
};

void setup() {
  // put your setup code here, to run once:
  pinMode(TRIGGER_PIN_L, OUTPUT);
  pinMode(ECHO_PIN_L, INPUT);
  pinMode(TRIGGER_PIN_R, OUTPUT);
  pinMode(ECHO_PIN_R, INPUT);
  pinMode(LEFT_WHEEL_FORWARD, OUTPUT);
  pinMode(LEFT_WHEEL_BACKWARD, OUTPUT);
  pinMode(RIGHT_WHEEL_FORWARD, OUTPUT);
  pinMode(RIGHT_WHEEL_BACKWARD, OUTPUT);
  pinMode(SPEEDA, OUTPUT);
  pinMode(SPEEDB, OUTPUT);
  pinMode(QTR_SENSOR_FL, INPUT);
  pinMode(QTR_SENSOR_FR, INPUT);
  pinMode(QTR_SENSOR_B, INPUT);

  randomSeed(analogRead(0));
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

void car_go_random(int speed) {
  long rand_num = random(3);
  switch (rand_num) {
    case 0:
      car_go_forward(speed);
      break;
    case 1:
      car_turn_left(speed);
      break;
    case 2:
      car_turn_right(speed);
      break;
  }
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

void car_turn_attack_direction(Obj_direction info, int speed) {
  //TODO: adjust direction based on the info
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

Edge_direction determine_edge(uint8_t qtr_sensor_front_left, uint8_t qtr_sensor_front_right, uint8_t qtr_sensor_back) {
  int frontL = analogRead(qtr_sensor_front_left);
  int frontR = analogRead(qtr_sensor_front_right);
  int back = analogRead(qtr_sensor_back);
  // Serial.print("FrontL: ");
  // Serial.println(frontL);

  if (frontL <= 400 && frontR <= 400) {
    return Edge_direction::FRONT;
  } else if (frontL <= 400) {
    return Edge_direction::FRONT_LEFT;
  } else if (frontR <= 400) {
    return Edge_direction::FRONT_RIGHT;
  } else if (back <= 400) {
    return Edge_direction::BACK;
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

bool is_obj_in_distance(Obj_direction info, double range) {
  return info.left_sensor <= range && info.right_sensor <= range;
}

// range ~= 60
Obj_direction obj_detected_info(double range) {
  double distance_l = detect_obj_distance(TRIGGER_PIN_L, ECHO_PIN_L);
  double distance_r = detect_obj_distance(TRIGGER_PIN_R, ECHO_PIN_R);

  return (Obj_direction){
    .left_sensor = distance_l < range ? distance_l : -1,
    .right_sensor = distance_r < range ? distance_r : -1,
  };
}

bool is_adjusting_needed(Obj_direction info, double max_range, double tolerance) {
  if (abs(info.left_sensor - info.right_sensor) < tolerance) {
    return false;
  } else if (info.left_sensor <= max_range || info.right_sensor <= max_range) {
    return true;
  }
  return false;
}

void search_strategy(Edge_direction edge) {
  switch (edge) {
    case Edge_direction::FRONT:
    case Edge_direction::FRONT_LEFT:
      // Serial.print("turn right ");
      car_turn_right_by_speed(80, 80);
      break;

    case Edge_direction::BACK:
      car_go_random(80);
      break;

    case Edge_direction::FRONT_RIGHT:
    default:
      // Serial.print("turn left ");
      car_turn_left_by_speed(80, 80);
      break;
  }
}

struct Test {
  static void motor_forward(bool left_wheel, bool right_wheel, int speed) {
    if (left_wheel && right_wheel) {
      car_go_forward(speed);
    } else if (left_wheel) {
      car_turn_right(speed);
    } else if (right_wheel) {
      car_turn_left(speed);
    }
  }
  static void motor_backward(bool left_wheel, bool right_wheel, int speed) {
    if (left_wheel && right_wheel) {
      car_go_backward(speed);
    } else if (left_wheel) {
      car_turn_left_by_speed(speed, 0);
    } else if (right_wheel) {
      car_turn_right_by_speed(0, speed);
    }
  }
  static void ultrasonic_test(bool left, bool right, int range, int speed) {
    auto info = obj_detected_info(range);
    if (left && info.left_sensor > -1 && right && info.right_sensor > -1) {
      car_go_backward(speed);
    } else if (left && info.left_sensor > -1) {
      car_go_forward(speed);
    } else if (right && info.right_sensor > -1) {
      car_go_forward(speed);
    }
  }
  static void qtr_test(bool frontL, bool frontR, bool back, bool noEdge, int speed) {
    auto info = determine_edge(QTR_SENSOR_FL, QTR_SENSOR_FR, QTR_SENSOR_B);
    switch (info) {
      case Edge_direction::FRONT_LEFT:
        if (frontL) {
          car_go_forward(speed);
        }
        break;
      case Edge_direction::FRONT_RIGHT:
        if (frontR) {
          car_go_forward(speed);
        }
        break;
      case Edge_direction::BACK:
        if (back) {
          car_go_forward(speed);
        }
        break;
      case Edge_direction::NONE:
        if(noEdge){
          car_go_backward(speed);
        }
        break;
      default:
        break;
    }
  }
};

void loop() {
  Serial.println("working");
  Test::motor_forward(true, true, 255);
  delay(3000);
  Test::motor_backward(true, true, 255);
  delay(3000);
}