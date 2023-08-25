#include <Arduino.h>

#include "pin_config.h"

#define MAX_DISTANCE 200

enum class Edge_direction
{
    FRONT,
    FRONT_RIGHT,
    FRONT_LEFT,
    BACK,
    NONE,
};

enum class Adjust_attck_direction
{
    TURN_LEFT,
    TURN_RIGHT,
    KEEP_GOING,
};

template <typename FunctorType, typename ValueType>
class Functor
{
public:
    virtual FunctorType fmap(ValueType (*func)(ValueType)) const = 0;
};

template <typename M, typename T>
class Monad
{
public:
    virtual M returnM(const T &value) const = 0;
    virtual M bind(M (*func)(T)) const = 0;
};

template <typename T1, typename T2>
class Tuple
{
private:
    T1 fst;
    T2 snd;

public:
    Tuple(const T1 &a, const T2 &b)
        : fst(a), snd(b) {}

    T1 getFst() const
    {
        return fst;
    }
    T2 getSnd() const
    {
        return snd;
    }
};

template <class T1, class T2>
class Pair
{
public:
    T1 fst;
    T2 snd;

    Pair(const T1 &fst, const T2 &snd)
        : fst(fst), snd(snd) {}

    Pair(const Pair &src)
        : fst(src.fst), snd(src.snd) {}
    Pair &operator=(const Pair &src)
    {
        if (this != &src)
        {
            fst = src.fst;
            snd = src.snd;
        }
        return *this;
    }
};

template <typename T>
class Maybe : public Functor<Maybe<T>, T>, public Monad<Maybe<T>, T>
{
private:
    bool isJust;
    T value;

public:
    Maybe()
        : isJust(false) {}

    explicit Maybe(const T &val)
        : isJust(true), value(val) {}

    bool hasValue() const
    {
        return isJust;
    }

    // call this after call hasValue()
    T getValue() const
    {
        if (!isJust)
        {
            return T();
        }
        return value;
    }

    static Maybe Nothing()
    {
        return Maybe();
    }

    // Testing
    Maybe<T> fmap(T (*func)(T)) const override
    {
        if (isJust)
        {
            return Maybe<T>(func(value));
        }
        else
        {
            return Maybe<T>::Nothing();
        }
    }

    Maybe<T> returnM(const T &val) const override
    {
        return Maybe<T>(val);
    }

    // m a -> (a -> m b) -> m b
    Maybe<T> bind(Maybe<T> (*func)(T)) const override
    {
        if (isJust)
        {
            return func(value);
        }
        else
        {
            return Maybe<T>::Nothing();
        }
    }
};

// testing
template <typename L, typename R>
class Either : public Functor<Either<L, R>, R>, public Monad<Either<L, R>, R>
{
private:
    enum class Tag
    {
        LEFT,
        RIGHT
    } tag;

    union
    {
        L leftValue;
        R rightValue;
    };

public:
    static Either Left(const L &value)
    {
        return Either(Tag::LEFT, value, R());
    }

    static Either Right(const R &value)
    {
        return Either(Tag::RIGHT, L(), value);
    }

    bool isLeft() const
    {
        return tag == Tag::LEFT;
    }

    bool isRight() const
    {
        return tag == Tag::RIGHT;
    }

    L getLeft() const
    {
        if (isRight())
        {
            R();
        }
        return leftValue;
    }

    R getRight() const
    {
        if (isLeft())
        {
            L();
        }
        return rightValue;
    }

    Either<L, R> fmap(R (*func)(R)) const override
    {
        if (isRight())
        {
            return Either<L, R>::Right(func(rightValue));
        }
        else
        {
            return *this; // return the Left value unchanged
        }
    }

    Either<L, R> returnM(const R &val) const override
    {
        return Either<L, R>::Right(val);
    }

    Either<L, R> bind(Either<L, R> (*func)(R)) const override
    {
        if (isRight())
        {
            return func(rightValue);
        }
        else
        {
            return *this; // Return the Left value unchanged.
        }
    }

private:
    Either(Tag t, const L &left, const R &right)
        : tag(t)
    {
        if (t == Tag::LEFT)
        {
            leftValue = left;
        }
        else
        {
            rightValue = right;
        }
    }
};

template <typename T, class Tag>
class NewType
{
    T value;

public:
    explicit NewType(const T &v)
        : value(v) {}

    T &get()
    {
        return value;
    }
    const T &get() const
    {
        return value;
    }
};

// should i go c++17?
// BEGIN compose
/* Example:
  auto f = [](int x) { return x + 1; };
  auto g = [](int x) { return x * 2; };
  auto h = [](int x) { return x - 3; };

  auto combined = compose(f, g, h);
  std::cout << combined(3);  // ouput 4, because ((3 - 3) * 2) + 1 = 4
*/
template <typename F, typename G>
auto compose(F f, G g)
{
    return [f, g](auto x)
    {
        return f(g(x));
    };
}

template <typename F, typename... Funcs>
auto compose(F f, Funcs... funcs)
{
    return [f, funcs...](auto x)
    {
        return f(compose(funcs...)(x));
    };
}
// END compose

// Tag
struct PinIdTag
{
};
struct ErrIdTag
{
};

using PinId = NewType<uint8_t, PinIdTag>;
using ErrId = NewType<int, ErrIdTag>;

struct Obj_direction
{
    Maybe<double> left_sensor;
    Maybe<double> right_sensor;
};

void setup()
{
    // put your setup code here, to run once:
    pinMode(TRIGGER_PIN_L, OUTPUT);
    pinMode(ECHO_PIN_L, INPUT);
    pinMode(TRIGGER_PIN_R, OUTPUT);
    pinMode(ECHO_PIN_R, INPUT);
    pinMode(LEFT_WHEEL_FORWARD, OUTPUT);
    pinMode(LEFT_WHEEL_BACKWARD, OUTPUT);
    pinMode(RIGHT_WHEEL_FORWARD, OUTPUT);
    pinMode(RIGHT_WHEEL_BACKWARD, OUTPUT);
    pinMode(LEFT_MOTOR, OUTPUT);
    pinMode(RIGHT_MOTOR, OUTPUT);
    // pinMode(QTR_SENSOR_FL, INPUT);
    // pinMode(QTR_SENSOR_FR, INPUT);
    // pinMode(QTR_SENSOR_B, INPUT);

    randomSeed(analogRead(0));
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
    wheel_move_speed(LEFT_MOTOR, speed);
    wheel_move_speed(RIGHT_MOTOR, speed);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_forward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_go_backward(int speed)
{
    wheel_move_speed(LEFT_MOTOR, speed);
    wheel_move_speed(RIGHT_MOTOR, speed);
    wheel_backward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_turn_right_by_speed(int left_wheel_forward_speed, int right_wheel_backward_speed)
{
    wheel_move_speed(LEFT_MOTOR, left_wheel_forward_speed);
    wheel_move_speed(RIGHT_MOTOR, right_wheel_backward_speed);
    wheel_forward(LEFT_WHEEL_FORWARD, LEFT_WHEEL_BACKWARD);
    wheel_backward(RIGHT_WHEEL_FORWARD, RIGHT_WHEEL_BACKWARD);
}

void car_turn_left_by_speed(int left_wheel_backward_speed, int right_wheel_forward_speed)
{
    wheel_move_speed(LEFT_MOTOR, left_wheel_backward_speed);
    wheel_move_speed(RIGHT_MOTOR, right_wheel_forward_speed);
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

void car_go_random(int speed)
{
    long rand_num = random(3);
    switch (rand_num)
    {
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

// TODO: 函数不应该再次计算是否应该要拐弯才对，应该要计算拐多少，或许需要重新考虑
Pair<Adjust_attck_direction, int> car_adjustment_measurement(Obj_direction info, int tolerance)
{
    if (info.left_sensor.hasValue() && info.right_sensor.hasValue())
    {
        int gap = info.left_sensor.getValue() - info.right_sensor.getValue();
        if (gap > tolerance)
            return Pair<Adjust_attck_direction, int>(Adjust_attck_direction::TURN_RIGHT, gap);

        gap = abs(gap);
        if (gap > tolerance)
            return Pair<Adjust_attck_direction, int>(Adjust_attck_direction::TURN_LEFT, gap);
    }

    return Pair<Adjust_attck_direction, int>(Adjust_attck_direction::KEEP_GOING, 0);
}

void car_adjust_attack_direction(Obj_direction info, double tolerance, int speed)
{
    auto result = car_adjustment_measurement(info, tolerance);
    double factor = (abs(result.snd) - tolerance) / result.snd;

    switch (result.fst)
    {
    case Adjust_attck_direction::TURN_LEFT:
        car_turn_left(speed * factor);
        break;
    case Adjust_attck_direction::TURN_RIGHT:
        car_turn_right(speed * factor);
        break;
    default:
        break;
        ;
    }
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

Edge_direction determine_edge(uint8_t qtr_sensor_front_left, uint8_t qtr_sensor_front_right, uint8_t qtr_sensor_back)
{
    int frontL = analogRead(qtr_sensor_front_left);
    int frontR = analogRead(qtr_sensor_front_right);
    // int back = analogRead(qtr_sensor_back);
    // int frontR = 0;
    int back = 0;
    Serial.print("FrontL: ");
    // Serial.println(frontL);

    Serial.print(frontL);
    Serial.print(":L /");
    Serial.print(frontR);
    Serial.print(":R /");
    Serial.print(back);
    Serial.print(":B /");
    Serial.println("Front");
    if (frontL <= 400 && frontR <= 400)
    {
        return Edge_direction::FRONT;
    }
    else if (frontL <= 400)
    {
        // Serial.print(frontR);
        // Serial.println("FrontLeft");
        return Edge_direction::FRONT_LEFT;
    }
    else if (frontR <= 400)
    {
        // Serial.print(frontR);
        // Serial.println("FrontRight");
        return Edge_direction::FRONT_RIGHT;
    }
    else if (back <= 400)
    {
        // Serial.print(back);
        // Serial.println("Back");
        return Edge_direction::BACK;
    }
    else
    {
        // Serial.println("None");
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
template <typename Fn, typename... Args>
void do_actions_duration(unsigned long ms, Fn fn, Args... args)
{
    fn(args...);
    delay(ms);
}

bool is_obj_in_distance(Obj_direction info, double range)
{
    return (info.left_sensor.hasValue() && info.left_sensor.getValue() <= range) || (info.right_sensor.hasValue() && info.right_sensor.getValue() <= range);
}

// range ~= 60
Obj_direction obj_detected_info(double range)
{
    double distance_l = detect_obj_distance(TRIGGER_PIN_L, ECHO_PIN_L);
    double distance_r = detect_obj_distance(TRIGGER_PIN_R, ECHO_PIN_R);

    return (Obj_direction){
        .left_sensor = distance_l < range ? Maybe<double>(distance_l) : Maybe<double>::Nothing(),
        .right_sensor = distance_r < range ? Maybe<double>(distance_r) : Maybe<double>::Nothing(),
    };
}

bool is_adjusting_needed(Obj_direction info, double max_range, double tolerance)
{
    if (info.left_sensor.hasValue() && info.right_sensor.hasValue())
    {
        auto lv = info.left_sensor.getValue();
        auto rv = info.right_sensor.getValue();

        if (abs(lv - rv) > tolerance)
        {
            return true;
        }
    }
    return false;
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

struct Test
{
    static void motor_forward(bool left_wheel, bool right_wheel, int speed)
    {
        if (left_wheel && right_wheel)
        {
            car_go_forward(speed);
        }
        else if (left_wheel)
        {
            car_turn_right(speed);
        }
        else if (right_wheel)
        {
            car_turn_left(speed);
        }
    }
    static void motor_backward(bool left_wheel, bool right_wheel, int speed)
    {
        if (left_wheel && right_wheel)
        {
            car_go_backward(speed);
        }
        else if (left_wheel)
        {
            car_turn_left_by_speed(speed, 0);
        }
        else if (right_wheel)
        {
            car_turn_right_by_speed(0, speed);
        }
    }
    static void ultrasonic_test(bool left, bool right, int range, int speed)
    {
        auto info = obj_detected_info(range);
        if (left && info.left_sensor.hasValue() && right && info.right_sensor.hasValue())
        {
            car_go_backward(speed);
        }
        else if (left && info.left_sensor.hasValue())
        {
            car_go_forward(speed);
        }
        else if (right && info.right_sensor.hasValue())
        {
            car_go_forward(speed);
        }
    }
    static void qtr_test(bool frontL, bool frontR, bool back, bool noEdge, int speed)
    {
        auto info = determine_edge(QTR_SENSOR_FL, QTR_SENSOR_FR, QTR_SENSOR_B);
        switch (info)
        {
        case Edge_direction::FRONT_LEFT:
            if (frontL)
            {
                car_go_forward(speed);
            }
            break;
        case Edge_direction::FRONT_RIGHT:
            if (frontR)
            {
                car_go_forward(speed);
            }
            break;
        case Edge_direction::BACK:
            if (back)
            {
                car_go_forward(speed);
            }
            break;
        case Edge_direction::NONE:
            if (noEdge)
            {
                car_go_backward(speed);
            }
            break;
        default:
            break;
        }
    }
    static void trace_mode()
    {
        auto speed = 70;
        auto range = 20.0;
        auto tolerance = 1.2;
        auto info = obj_detected_info(range);
        auto delay_ms = 10;

        if (is_obj_in_distance(info, range))
        {
            if (is_adjusting_needed(info, range, tolerance))
            {
                car_adjust_attack_direction(info, tolerance, speed);
            }
            else
            {
                car_go_forward(speed);
            }
        }

        delay(delay_ms);
    }
};

void loop()
{
    Serial.println("working");
    Test::trace_mode();
}