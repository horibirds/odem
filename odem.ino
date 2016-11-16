#include "MsTimer2.h"
#include "TimerOne.h"

#define ODEM_NUM 3

#define ODEM_PIN_NUM ODEM_NUM * 3
#define ODEM1_R_PIN 2
#define ODEM1_G_PIN 3
#define ODEM1_B_PIN 4
#define ODEM2_R_PIN 5
#define ODEM2_G_PIN 6
#define ODEM2_B_PIN 7
#define ODEM3_R_PIN 8
#define ODEM3_G_PIN 9
#define ODEM3_B_PIN 10

#define ODEM_TOUCH_OUT_PIN 11
#define ODEM_TOUCH_IN_PIN 12

#define ODEM_MAGNET_IN_PIN 13

#define DUTY_MAX 255

int odem_pins[ODEM_PIN_NUM] = {
  ODEM1_R_PIN,
  ODEM1_G_PIN,
  ODEM1_B_PIN,
  ODEM2_R_PIN,
  ODEM2_G_PIN,
  ODEM2_B_PIN,
  ODEM3_R_PIN,
  ODEM3_G_PIN,
  ODEM3_B_PIN
};

int led_duty_nums[ODEM_PIN_NUM] = {
  0
};

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

void write_odem(int odem_index, int R, int G, int B) {
  if (odem_index >= 0 && odem_index <= 2) {
      int pin_index = odem_index * 3;
      led_duty_nums[pin_index] = map(R, 0, 255, 0, DUTY_MAX);
      pin_index++;
      led_duty_nums[pin_index] = map(G, 0, 255, 0, DUTY_MAX);
      pin_index++;
      led_duty_nums[pin_index] = map(B, 0, 255, 0, DUTY_MAX);
    }
}

int timer_count = 0;
int door_count = 0;
bool b_door = true;
bool ex_b_door = true;
bool door_int_flag = false;

int duty_rate = 0;
void led_duty(){
  if(duty_rate >= DUTY_MAX){
    duty_rate = 0;
    for(int i=0; i<ODEM_PIN_NUM; i++){
      if(led_duty_nums[i] == 0){
        digitalWrite(odem_pins[i],LOW);
      }
      else{
        digitalWrite(odem_pins[i],HIGH);
      }
    }
    return;
  }
  for(int i=0; i<ODEM_PIN_NUM; i++){
    if(duty_rate == led_duty_nums[i]){
      digitalWrite(odem_pins[i],LOW);
    }
  }
  duty_rate++;
}


void led_timer() {
  if (odem_st == SLEEP) {
    if (timer_count > 255) {
      timer_count = 0;
    }
    write_odem(0, 0, 0, 0);
    write_odem(1, 0, 0, 0);
    write_odem(2, 0, 0, 0);
  }
  else if (odem_st == WAKE) {
    write_odem(0, led_pattern[pattern_index][0], led_pattern[pattern_index][1], led_pattern[pattern_index][2]);
    write_odem(1, led_pattern[pattern_index][0], led_pattern[pattern_index][1], led_pattern[pattern_index][2]);
    write_odem(2, led_pattern[pattern_index][0], led_pattern[pattern_index][1], led_pattern[pattern_index][2]);
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
    if (timer_count < 50)
    {
      int pos = timer_count;
      write_odem(0, map(pos, 0, interval, 0, led_pattern[pattern_index][0]), map(pos, 0, interval, 0, led_pattern[pattern_index][1]), map(pos, 0, interval, 0, led_pattern[pattern_index][2]));
      write_odem(1, 0, 0, 0);
      write_odem(2, 0, 0, 0);
    }
    else if (timer_count < 100)
    {
      int pos = timer_count - interval;
      write_odem(0, map(pos, 0, interval, led_pattern[pattern_index][0], 0), map(pos, 0, interval, led_pattern[pattern_index][1], 0), map(pos, 0, interval, led_pattern[pattern_index][2], 0));
      write_odem(1, 0, 0, 0);
      write_odem(2, 0, 0, 0);
    }
    else if (timer_count < 150)
    {
      int pos = timer_count - (2 * interval);
      write_odem(1, map(pos, 0, interval, 0, led_pattern[pattern_index][0]), map(pos, 0, interval, 0, led_pattern[pattern_index][1]), map(pos, 0, interval, 0, led_pattern[pattern_index][2]));
      write_odem(0, 0, 0, 0);
      write_odem(2, 0, 0, 0);
    }
    else if (timer_count < 200)
    {
      int pos = timer_count - (3 * interval);
      write_odem(1, map(pos, 0, interval, led_pattern[pattern_index][0], 0), map(pos, 0, interval, led_pattern[pattern_index][1], 0), map(pos, 0, interval, led_pattern[pattern_index][2], 0));
      write_odem(0, 0, 0, 0);
      write_odem(2, 0, 0, 0);
    }
    else if (timer_count < 250)
    {
      int pos = timer_count - (4 * interval);
      write_odem(2, map(pos, 0, interval, 0, led_pattern[pattern_index][0]), map(pos, 0, interval, 0, led_pattern[pattern_index][1]), map(pos, 0, interval, 0, led_pattern[pattern_index][2]));
      write_odem(0, 0, 0, 0);
      write_odem(1, 0, 0, 0);
    }
    else if (timer_count <= 300)
    {
      int pos = timer_count - (5 * interval);
      write_odem(2, map(pos, 0, interval, led_pattern[pattern_index][0], 0), map(pos, 0, interval, led_pattern[pattern_index][1], 0), map(pos, 0, interval, led_pattern[pattern_index][2], 0));
      write_odem(0, 0, 0, 0);
      write_odem(1, 0, 0, 0);
    }
  }
  else if (odem_st == WHEEL) {
    if (timer_count > 255) {
      timer_count = 0;
    }
    int wheel_pos = 255 - timer_count;
    if (wheel_pos < 85)
    {
      write_odem(0, 255 - wheel_pos * 3, 0, wheel_pos * 3);
      write_odem(1, wheel_pos * 3, 255 - wheel_pos * 3, 0);
      write_odem(2, 0, wheel_pos * 3, 255 - wheel_pos * 3);
    }
    else if (wheel_pos < 170)
    {
      wheel_pos -= 85;
      write_odem(0, 0, wheel_pos * 3, 255 - wheel_pos * 3);
      write_odem(1, 255 - wheel_pos * 3, 0, wheel_pos * 3);
      write_odem(2, wheel_pos * 3, 255 - wheel_pos * 3, 0);
    }
    else if (wheel_pos <= 255)
    {
      wheel_pos -= 170;
      write_odem(0, wheel_pos * 3, 255 - wheel_pos * 3, 0);
      write_odem(1, 0, wheel_pos * 3, 255 - wheel_pos * 3);
      write_odem(2, 255 - wheel_pos * 3, 0, wheel_pos * 3);
    }
  }
  timer_count++;
  if (door_int_flag == true) {
    door_count++;
    if (door_count > (100 * 10)) {
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
  for (int i = 0; i < ODEM_PIN_NUM; i++) {
    pinMode(odem_pins[i], OUTPUT);
  }
  pinMode(ODEM_TOUCH_IN_PIN, INPUT);
  pinMode(ODEM_TOUCH_OUT_PIN, OUTPUT);

  pinMode(ODEM_MAGNET_IN_PIN, INPUT);

  write_odem(0, 0, 0, 0);
  write_odem(1, 0, 0, 0);
  write_odem(2, 0, 0, 0);


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
  Timer1.initialize();
  MsTimer2::set(10, led_timer);
  Timer1.attachInterrupt(led_duty,1);
  MsTimer2::start();
  write_odem(2, 0, 0, 0);
  write_odem(1, 0, 0, 0);
  write_odem(0, 0, 0, 0);
  
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
