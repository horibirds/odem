#include "MsTimer2.h"
#include <Adafruit_NeoPixel.h>

#define LED_NUM 3
#define LED_PIN 6
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);

#define ODEM_TOUCH_OUT_PIN 11
#define ODEM_TOUCH_IN_PIN 12
#define ODEM_MAGNET_IN_PIN 13

enum odem_st {
  SLEEP,
  WAKE,
  WHEEL,
  ACTIVE,
};
odem_st odem_st = ACTIVE;

int pattern_index = 0;
#define LED_PATTERN_NUM 8
int led_pattern[LED_PATTERN_NUM][3] = {
  {255, 70, 70}, // pink
  {255, 120, 0}, // purple
  {255, 0, 100}, // orange
  {255, 0, 190}, // yellow
  {150, 0, 200}, // yellow green
  {0, 0, 255}, // green
  {255, 0, 0}, //red
  {0, 255, 0}, //blue
};


uint32_t wheel_pattern(byte WheelPos)
{
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85)
  {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  else if (WheelPos < 170)
  {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  else
  {
    WheelPos -= 170;
    return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  }
}

int timer_count = 0;

int door_count = 0;
bool b_door = true;
bool ex_b_door = true;
bool door_int_flag = false;

void led_timer() {
  if (odem_st == SLEEP) {
    if (timer_count > 255) {
      timer_count = 0;
    }
    for (int i = 0; i < LED_NUM; i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
    strip.setBrightness(0);
  }
  else if (odem_st == WAKE) {
    for (int i = 0; i < LED_NUM; i++) {
      strip.setPixelColor(i, led_pattern[pattern_index][0], led_pattern[pattern_index][1], led_pattern[pattern_index][2]);
    }
    strip.setBrightness(255);
  }
  else if (odem_st == ACTIVE) {
    int interval = 50;
    if (timer_count >= 300) {
      timer_count = 0;
      pattern_index++;
      if (pattern_index >= LED_PATTERN_NUM) {
        pattern_index = 0;
      }
    }
    strip.setBrightness(255);
    for (int i = 0; i < LED_NUM; i++) {
      strip.setPixelColor(i, 0, 0, 0);
    }
    if (timer_count < 50)
    {
      int pos = timer_count;
      strip.setPixelColor(0, map(pos, 0, interval, 0, led_pattern[pattern_index][0]), map(pos, 0, interval, 0, led_pattern[pattern_index][1]), map(pos, 0, interval, 0, led_pattern[pattern_index][2]));
    }
    else if (timer_count < 100)
    {
      int pos = timer_count - interval;
      strip.setPixelColor(0, map(pos, 0, interval, led_pattern[pattern_index][0], 0), map(pos, 0, interval, led_pattern[pattern_index][1], 0), map(pos, 0, interval, led_pattern[pattern_index][2], 0));
    }
    else if (timer_count < 150)
    {
      int pos = timer_count - (2 * interval);
      strip.setPixelColor(1, map(pos, 0, interval, 0, led_pattern[pattern_index][0]), map(pos, 0, interval, 0, led_pattern[pattern_index][1]), map(pos, 0, interval, 0, led_pattern[pattern_index][2]));
    }
    else if (timer_count < 200)
    {
      int pos = timer_count - (3 * interval);
      strip.setPixelColor(1, map(pos, 0, interval, led_pattern[pattern_index][0], 0), map(pos, 0, interval, led_pattern[pattern_index][1], 0), map(pos, 0, interval, led_pattern[pattern_index][2], 0));
    }
    else if (timer_count < 250)
    {
      int pos = timer_count - (4 * interval);
      strip.setPixelColor(2, map(pos, 0, interval, 0, led_pattern[pattern_index][0]), map(pos, 0, interval, 0, led_pattern[pattern_index][1]), map(pos, 0, interval, 0, led_pattern[pattern_index][2]));
    }
    else if (timer_count <= 300)
    {
      int pos = timer_count - (5 * interval);
      strip.setPixelColor(2, map(pos, 0, interval, led_pattern[pattern_index][0], 0), map(pos, 0, interval, led_pattern[pattern_index][1], 0), map(pos, 0, interval, led_pattern[pattern_index][2], 0));
    }
  }
  else if (odem_st == WHEEL) {
    if (timer_count > 255) {
      timer_count = 0;
    }
    strip.setBrightness(255);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, wheel_pattern(i + timer_count));
    }
  }
  strip.show();
  timer_count++;
  if (door_int_flag == true) {
    door_count++;
    if (door_count > (100 * 30)) {
      door_count = 0;
      odem_st = SLEEP;
      door_int_flag = false;
    }
  }
}

int default_touch_count = 0;
int thre_up;
int thre_down;
void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  pinMode(ODEM_TOUCH_IN_PIN, INPUT);
  pinMode(ODEM_TOUCH_OUT_PIN, OUTPUT);

  pinMode(ODEM_MAGNET_IN_PIN, INPUT);

  // Adafruit
  strip.begin();
  strip.show();

  Serial.begin(9600);

  // calibration
  for (int i = 0; i < 10; i++) {
    digitalWrite(ODEM_TOUCH_OUT_PIN, HIGH);
    while (digitalRead(ODEM_TOUCH_IN_PIN) == LOW) {
      default_touch_count++;
    }
    digitalWrite(ODEM_TOUCH_OUT_PIN, LOW);
    delay(5);
  }
  default_touch_count /= 10;
  if (default_touch_count == 0) {
    default_touch_count = 1;
  }
  thre_up = default_touch_count * 1.5;
  thre_down = default_touch_count * 1.2;

  // timer
  MsTimer2::set(10, led_timer);
  MsTimer2::start();

}

int touch_count = 0;
bool b_touch = false;
int touch_vals[10] = {0};
int touch_ave = 0;


void loop() {
  digitalWrite(ODEM_TOUCH_OUT_PIN, HIGH);
  while (digitalRead(ODEM_TOUCH_IN_PIN) == LOW) {
    touch_count++;
    //    if(touch_count >= 100){
    //      break;
    //    }
  }
  digitalWrite(ODEM_TOUCH_OUT_PIN, LOW);
  for (int i = 9; i > 0; i--) {
    touch_vals[i] = touch_vals[i - 1];
    touch_ave += touch_vals[i];
  }
  touch_vals[0] = touch_count;
  touch_ave += touch_vals[0];
  touch_ave = touch_ave / 10;

  // ドアが開いたか
  ex_b_door = b_door;
  b_door = digitalRead(ODEM_MAGNET_IN_PIN);
  if (ex_b_door == false && b_door == true) {
    odem_st = ACTIVE;
    door_int_flag = true;
    door_count = 0;
  }
  delay(5); // 待たなきゃチャタリングが起きる
  if (door_int_flag == false) {
    if (touch_ave > thre_up) {
      if (b_touch == false) {
        b_touch = true;
        if (odem_st == SLEEP) {
          odem_st = WAKE;
        }
        else if (odem_st == WAKE) {
          odem_st = WHEEL;
        }
        else if (odem_st == WHEEL) {
          odem_st = ACTIVE;
        }
        else if (odem_st == ACTIVE) {
          odem_st = SLEEP;
        }
      }
    }
    else if (touch_ave < thre_down ) {
      b_touch = false;
    }
  }
  touch_count = 0;
  touch_ave = 0;
}
