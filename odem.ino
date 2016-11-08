#define SRCLK 2 // clock
#define RCLK 4 // 書き換え
#define SER 7 // signal

#define ODEM_NUM 3

int odem_player = 1;

int time_sum = 0;
int time1=0;
int time2=0;

int odem_start_pins[ODEM_NUM] = { 12, 8, 4 };

// duty比の大きさ
#define MAX_DUTY_COUNT 40
int duty_count = 0;
int duty_count_now = 0;
int duty_count_now_stat = 0;

#define PATTERN_NUM 16
float max_duty_rate[PATTERN_NUM][3] = {
  {1.0,1.0,1.0},
  {0.8,0.0,1.0},
  {1.0,0.0,0.0},
  {1.0,0.15,0.15},
  {0.0,1.0,0.0},
  {0.15,1.0,0.15},
  {0.0,0.0,1.0},
  {0.15,0.15,1.0},
  {0.5,3.0,3.0},
  {3.0,0.3,0.3},
  {3.0,0.3,3.0},
  {3.0,3.0,1.0},
  {1.0,3.0,1.0},
  {1.0,1.0,3.0},
  {0.8,0.5,0.0},
  {0.8,0.0,1.0}
};

int pattern_stat = 0;

int R_duty_count = 0;
int G_duty_count = 0;
int B_duty_count = 0;

// duty比の大きさを何回繰り返すか（時間の長さ、調節）
#define MAX_XXX_COUNT 2
int xxx_count = 0;


void write_SR(int start_pin, bool R, bool G, bool B){
  digitalWrite(SER, LOW);
  for(int i = 0; i < start_pin; i++){
    digitalWrite(SRCLK, LOW);
    digitalWrite(SRCLK, HIGH);
  }
  
  digitalWrite(SER, R);
  digitalWrite(SRCLK, LOW);
  digitalWrite(SRCLK, HIGH);
  
  digitalWrite(SER, B);
  digitalWrite(SRCLK, LOW);
  digitalWrite(SRCLK, HIGH);
  
  digitalWrite(SER, G);
  digitalWrite(SRCLK, LOW);
  digitalWrite(SRCLK, HIGH);
    
  digitalWrite(SER, LOW);
  for(int i = start_pin+3; i < 16; i++){
    digitalWrite(SRCLK, LOW);
    digitalWrite(SRCLK, HIGH);
  }
  digitalWrite(RCLK, LOW);
  digitalWrite(RCLK, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(SRCLK, OUTPUT);
  pinMode(RCLK, OUTPUT);
  pinMode(SER, OUTPUT);
  
//  Serial.begin(9600);
  
  write_SR(0, LOW, LOW, LOW);
}


void loop() {
//  time1 = micros();
  int odem_start_pin = odem_start_pins[odem_player];
  if(duty_count >= MAX_DUTY_COUNT){
    // duty 1回分
    duty_count = 0;
    xxx_count++;
    if(xxx_count >= MAX_XXX_COUNT){
      // duty比を変更する 1回分
      if(duty_count_now_stat == 0){
        duty_count_now++;
        if(duty_count_now >= MAX_DUTY_COUNT){
          duty_count_now = MAX_DUTY_COUNT;
          duty_count_now_stat = 1;
        }
      }
      else{
        duty_count_now--;
        if(duty_count_now < 0){
          duty_count_now = 0;
          duty_count_now_stat = 0;
          
          // odemを変更
          odem_player++;
          if(odem_player >= 3){
            odem_player = 0;
            pattern_stat++;
            if(pattern_stat>=PATTERN_NUM){
              pattern_stat = 0;
            }
          }
        }
      }
      R_duty_count = max_duty_rate[pattern_stat][0] * duty_count_now;
      G_duty_count = max_duty_rate[pattern_stat][1] * duty_count_now;
      B_duty_count = max_duty_rate[pattern_stat][2] * duty_count_now;
      xxx_count = 0;
    }
  }
  
  write_SR(odem_start_pin, duty_count < R_duty_count, duty_count < G_duty_count, duty_count < B_duty_count);
//  time2 = micros();

  duty_count++;
}
