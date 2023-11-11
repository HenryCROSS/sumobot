#include <Arduino.h>
// #include <OLED_utils.hpp>
#include <Configs.h>
#include <Debug.h>
#include <MK2System.hpp>
#include <Vehicle_actions.hpp>
#include <Vehicle_utils.hpp>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES 10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT 16
#define LOGO_WIDTH 16
static const unsigned char PROGMEM logo_bmp[] =
    {0b00000000, 0b11000000,
     0b00000001, 0b11000000,
     0b00000001, 0b11000000,
     0b00000011, 0b11100000,
     0b11110011, 0b11100000,
     0b11111110, 0b11111000,
     0b01111110, 0b11111111,
     0b00110011, 0b10011111,
     0b00011111, 0b11111100,
     0b00001101, 0b01110000,
     0b00011011, 0b10100000,
     0b00111111, 0b11100000,
     0b00111111, 0b11110000,
     0b01111100, 0b11110000,
     0b01110000, 0b01110000,
     0b00000000, 0b00110000};


///////////////////////
void testdrawline() {
  int16_t i;

  display.clearDisplay(); // Clear display buffer

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, 0, i, display.height()-1, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn line
    delay(1);
  }
  for(i=0; i<display.height(); i+=4) {
    display.drawLine(0, 0, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.width(); i+=4) {
    display.drawLine(0, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(0, display.height()-1, display.width()-1, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=display.width()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, i, 0, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=display.height()-1; i>=0; i-=4) {
    display.drawLine(display.width()-1, display.height()-1, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  delay(250);

  display.clearDisplay();

  for(i=0; i<display.height(); i+=4) {
    display.drawLine(display.width()-1, 0, 0, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }
  for(i=0; i<display.width(); i+=4) {
    display.drawLine(display.width()-1, 0, i, display.height()-1, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000); // Pause for 2 seconds
}

void testdrawrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testfillrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2; i+=3) {
    // The INVERSE color is used so rectangles alternate white/black
    display.fillRect(i, i, display.width()-i*2, display.height()-i*2, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn rectangle
    delay(1);
  }

  delay(2000);
}

void testdrawcircle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=2) {
    display.drawCircle(display.width()/2, display.height()/2, i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillcircle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=3) {
    // The INVERSE color is used so circles alternate white/black
    display.fillCircle(display.width() / 2, display.height() / 2, i, SSD1306_INVERSE);
    display.display(); // Update screen with each newly-drawn circle
    delay(1);
  }

  delay(2000);
}

void testdrawroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    display.drawRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfillroundrect(void) {
  display.clearDisplay();

  for(int16_t i=0; i<display.height()/2-2; i+=2) {
    // The INVERSE color is used so round-rects alternate white/black
    display.fillRoundRect(i, i, display.width()-2*i, display.height()-2*i,
      display.height()/4, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawtriangle(void) {
  display.clearDisplay();

  for(int16_t i=0; i<max(display.width(),display.height())/2; i+=5) {
    display.drawTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_WHITE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testfilltriangle(void) {
  display.clearDisplay();

  for(int16_t i=max(display.width(),display.height())/2; i>0; i-=5) {
    // The INVERSE color is used so triangles alternate white/black
    display.fillTriangle(
      display.width()/2  , display.height()/2-i,
      display.width()/2-i, display.height()/2+i,
      display.width()/2+i, display.height()/2+i, SSD1306_INVERSE);
    display.display();
    delay(1);
  }

  delay(2000);
}

void testdrawchar(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  for(int16_t i=0; i<256; i++) {
    if(i == '\n') display.write(' ');
    else          display.write(i);
  }

  display.display();
  delay(2000);
}

void testdrawstyles(void) {
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Hello, world!"));

  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.println(3.141592);

  display.setTextSize(2);             // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.print(F("0x")); display.println(0xDEADBEEF, HEX);

  display.display();
  delay(2000);
}

void testscrolltext(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 0);
  display.println(F("scroll"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}

void testdrawbitmap(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
  delay(1000);
}

#define XPOS   0 // Indexes into the 'icons' array in function below
#define YPOS   1
#define DELTAY 2

void testanimate(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  int8_t f, icons[NUMFLAKES][3];

  // Initialize 'snowflake' positions
  for(f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
    icons[f][YPOS]   = -LOGO_HEIGHT;
    icons[f][DELTAY] = random(1, 6);
    Serial.print(F("x: "));
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(F(" y: "));
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(F(" dy: "));
    Serial.println(icons[f][DELTAY], DEC);
  }

  for(;;) { // Loop forever...
    display.clearDisplay(); // Clear the display buffer

    // Draw each snowflake:
    for(f=0; f< NUMFLAKES; f++) {
      display.drawBitmap(icons[f][XPOS], icons[f][YPOS], bitmap, w, h, SSD1306_WHITE);
    }

    display.display(); // Show the display buffer on the screen
    delay(200);        // Pause for 1/10 second

    // Then update coordinates of each flake...
    for(f=0; f< NUMFLAKES; f++) {
      icons[f][YPOS] += icons[f][DELTAY];
      // If snowflake is off the bottom of the screen...
      if (icons[f][YPOS] >= display.height()) {
        // Reinitialize to a random position, just off the top
        icons[f][XPOS]   = random(1 - LOGO_WIDTH, display.width());
        icons[f][YPOS]   = -LOGO_HEIGHT;
        icons[f][DELTAY] = random(1, 6);
      }
    }
  }
}
////////////////////////


VehState g_state{};

void task_searching()
{
    g_state.ultra_info = obj_detection_info();
}

void task_qtr()
{
    g_state.edge_info = determine_edge(QTR_SENSOR_FL, QTR_SENSOR_FR, QTR_SENSOR_B);
}

void task_normal_attack()
{
    // it is useful when more than 1 mode
    // if (g_state.stage != Stage::BATTLE)
    //     return;

    int search_distance = 40;

    if (g_state.edge_info.hasValue())
    {
        g_state.motion = VehMotion::TURNING;
        switch (g_state.edge_info.getValue())
        {
        case Edge_Signal::BACK:
            // TODO: maybe go random?
            car_go_forward(SPEED);
            break;
        case Edge_Signal::FRONT:
            car_turn_left_by_speed(SPEED, SPEED);
            break;
        case Edge_Signal::FRONT_LEFT:
            car_turn_left_by_speed(SPEED, SPEED);
            break;
        case Edge_Signal::FRONT_RIGHT:
            car_turn_right_by_speed(SPEED, SPEED);
            break;
        default:
            break;
        }

        delay(TIMESLICE);
        g_state.speed = SPEED;
    }
    else if (is_obj_in_distance(g_state.ultra_info, search_distance))
    {
        g_state.motion = VehMotion::FORWARD;

        // calculation of the gap
        // auto gap = abs(g_state.ultra_info.left_sensor.getValue() - g_state.ultra_info.left_sensor.getValue());
        auto gap = calculate_gap(g_state.ultra_info);

        if (is_adjusting_needed(g_state.ultra_info, gap))
        {
            g_state.motion = VehMotion::ADJUST;
            g_state.speed = car_adjust_attack_direction(g_state.ultra_info, SPEED);
            delay(TIMESLICE * 3);
        }
        else
        {
            // attack strategy
            if (is_obj_in_distance(g_state.ultra_info, 10))
            {
                // attack_strategy(120, TIMESLICE * 10);
                car_go_forward(255);
                g_state.speed = 255;
            }
            else if (is_obj_in_distance(g_state.ultra_info, 20))
            {
                // attack_strategy(120, TIMESLICE * 10);
                car_go_forward(155);
                g_state.speed = 155;
            }
            else
            {
                car_go_forward(90);
                g_state.speed = 90;
                delay(TIMESLICE * 3);
            }
        }
    }
    else
    {
        g_state.motion = VehMotion::SEARCH;
        search_strategy(g_state.search_strategy, search_distance, SPEED, 500);
        // car_go_forward(SPEED);
        g_state.speed = SPEED;
    }
}

void task_oled_display()
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    // Display static text
    display.print("stage: ");
    switch (g_state.stage)
    {
    case Stage::INIT:
        display.println("INIT");
        break;
    case Stage::BATTLE:
        display.println("Battle");
        break;
    default:
        break;
    }

    display.print("L: ");
    display.println(g_state.ultra_info.left_sensor.getValue());
    display.print("R: ");
    display.println(g_state.ultra_info.right_sensor.getValue());

    display.print("Speed: ");
    display.println(g_state.speed);

    display.print("Edge: ");
    if (g_state.edge_info.hasValue())
    {
        switch (g_state.edge_info.getValue())
        {
        case Edge_Signal::BACK:
            display.println("Back");
            break;
        case Edge_Signal::FRONT:
            display.println("Front");
            break;
        case Edge_Signal::FRONT_LEFT:
            display.println("Front Left");
            break;
        case Edge_Signal::FRONT_RIGHT:
            display.println("Front Right");
            break;
        default:
            break;
        }
    }
    else
    {
        display.println("Void");
    }

    display.print("Motion: ");

    switch (g_state.motion)
    {
    case VehMotion::VOID:
        display.println("Void");
        break;
    case VehMotion::TURNING:
        display.println("Turing");
        break;
    case VehMotion::FORWARD:
        display.println("Forward");
        break;
    case VehMotion::BACKWARD:
        display.println("Backward");
        break;
    case VehMotion::ADJUST:
        display.println("Adjust");
        break;
    case VehMotion::SEARCH:
        display.println("Search");
    default:
        break;
    }

    display.display();
}

void setup()
{
    Serial.begin(9600);
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

    MK2System::init();
    MK2System::register_task(task_searching, Task_Type::PREEMPTIVE, -1);
    MK2System::register_task(task_qtr, Task_Type::PREEMPTIVE, -1);
    MK2System::register_task(task_normal_attack, Task_Type::PREEMPTIVE, -1);

    // if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    // {
    //     Serial.println(F("SSD1306 allocation failed"));
    //     for (;;)
    //         ;
    // }

    // delay(2000);
    // display.clearDisplay();

    // display.setTextSize(1);
    // display.setTextColor(WHITE);
    // display.setCursor(0, 10);
    // // Display static text
    // display.println("Hello, world!");
    // display.display();

    // MK2System::register_task(task_oled_display, Task_Type::PREEMPTIVE, -1);
    // if (display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    // { // Address 0x3D for 128x64
    //     delay(2000);

    //     g_state.monitor_running = true;

    //     display.clearDisplay();
    //     display.setTextSize(2);
    //     display.setTextColor(WHITE);
    //     display.setCursor(0, 10);
    //     // Display static text
    //     display.println("CHARGE!!");
    //     display.display();
    // }
    // else
    // {
    //     Serial.println(F("SSD1306 allocation failed"));
    //     delay(2000);
    // }

    // delay(4000);

    Serial.begin(9600);

    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ; // Don't proceed, loop forever
    }

    // Show initial display buffer contents on the screen --
    // the library initializes this with an Adafruit splash screen.
    display.display();
    delay(2000); // Pause for 2 seconds

    // Clear the buffer
    display.clearDisplay();

    // Draw a single pixel in white
    display.drawPixel(10, 10, SSD1306_WHITE);

    // Show the display buffer on the screen. You MUST call display() after
    // drawing commands to make them visible on screen!
    display.display();
    delay(2000);
    // display.display() is NOT necessary after every single drawing command,
    // unless that's what you want...rather, you can batch up a bunch of
    // drawing operations and then update the screen all at once by calling
    // display.display(). These examples demonstrate both approaches...

    testdrawline(); // Draw many lines

    testdrawrect(); // Draw rectangles (outlines)

    testfillrect(); // Draw rectangles (filled)

    testdrawcircle(); // Draw circles (outlines)

    testfillcircle(); // Draw circles (filled)

    testdrawroundrect(); // Draw rounded rectangles (outlines)

    testfillroundrect(); // Draw rounded rectangles (filled)

    testdrawtriangle(); // Draw triangles (outlines)

    testfilltriangle(); // Draw triangles (filled)

    testdrawchar(); // Draw characters of the default font

    testdrawstyles(); // Draw 'stylized' characters

    testscrolltext(); // Draw scrolling text

    testdrawbitmap(); // Draw a small bitmap image

    // Invert and restore display, pausing in-between
    display.invertDisplay(true);
    delay(1000);
    display.invertDisplay(false);
    delay(1000);

    testanimate(logo_bmp, LOGO_WIDTH, LOGO_HEIGHT); // Animate bitmaps
}

struct Test
{
    static void oled_display(const char *msg)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setTextColor(WHITE);
        display.setCursor(0, 10);
        // Display static text
        display.println(msg);
        display.display();
    }
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
        auto info = obj_detection_info();
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
        if (info.hasValue())
        {
            switch (info.getValue())
            {
            case Edge_Signal::FRONT_LEFT:
                if (frontL)
                {
                    car_go_forward(speed);
                }
                break;
            case Edge_Signal::FRONT_RIGHT:
                if (frontR)
                {
                    car_go_forward(speed);
                }
                break;
            case Edge_Signal::BACK:
                if (back)
                {
                    car_go_forward(speed);
                }
                break;
            default:
                // never exec
                break;
            }
        }
        else
        {
            if (noEdge)
            {
                car_go_backward(speed);
            }
        }
    }
    static void trace_mode()
    {
        auto speed = 130;
        auto range = 30.0;
        auto tolerance = 1;
        auto info = obj_detection_info();
        auto delay_ms = 10;

        Serial.println(String(">>L: ") + info.left_sensor.getValue());
        Serial.println(String(">>R: ") + info.right_sensor.getValue());

        if (is_obj_in_distance(info, range))
        {
            if (is_adjusting_needed(info, tolerance))
            {
                car_adjust_attack_direction(info, speed);
                Test::oled_display("TURNING");
            }
            else
            {
                car_go_forward(speed);
                Test::oled_display("FORWARD");
            }
        }
        else
        {
            Test::oled_display("NOT MOVE");
            debug::serial_println("VehAction: NOT MOVE");
        }

        delay(delay_ms);
    }

    static void monitor_test_mode()
    {
        if (g_state.monitor_running)
        {
            for (int i = 0; i < 100; i++)
            {
                display.clearDisplay();
                display.setTextSize(2);
                display.setTextColor(WHITE);
                display.setCursor(0, 10);
                // Display static text
                display.println(i);
                display.display();
                delay(500);
            }
        }
    }

    static void normal_mode()
    {
        MK2System::run();
    }
};

void loop()
{
    // debug::serial_println("===============");
    // Test::normal_mode();

    // Test::monitor_test_mode();
    // attack_strategy(100, 100);

    // car_turn_left_by_speed(SPEED, 0);
    // delay(480);

    // car_stop();
    // delay(1000);

    byte error, address;
    int nDevices;
    Serial.println("Scanning...");
    nDevices = 0;
    for (address = 1; address < 127; address++)
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        error = Wire.endTransmission();
        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" !");
            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("Unknow error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
        Serial.println("No I2C devices found\n");
    else
        Serial.println("done\n");
    delay(5000); // wait 5 seconds for next scan
}