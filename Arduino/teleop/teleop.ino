#include <Servo.h>
#include <EnableInterrupt.h>

#define FL_motor_pin 4
#define FR_motor_pin 5
#define BL_motor_pin 6
#define BR_motor_pin 7

#define RC_NUM_CHANNELS 6

#define RC_CH1  0
#define RC_CH2  1
#define RC_CH3  2
#define RC_CH4  3
#define RC_CH5  4
#define RC_CH6  5

//#define CH1_pin 8
//#define CH2_pin 9
//#define CH3_pin 10
//#define CH4_pin 11
//#define CH5_pin 12
//#define CH6_pin 13

#define CH1_pin 10
#define CH2_pin 11
#define CH3_pin 12
#define CH4_pin 13
#define CH5_pin 2
#define CH6_pin 3

int rc_values[RC_NUM_CHANNELS];
int rc_start[RC_NUM_CHANNELS];
volatile int rc_shared[RC_NUM_CHANNELS];

float CH1, CH2, CH3, CH4, CH5, CH6;
Servo FL_motor, FR_motor, BL_motor, BR_motor;

float rotate_ratio = 0.5; 

/* =========================== utils ==================================*/
void rc_read_values() {
  noInterrupts();
  memcpy(rc_values, (const void *)rc_shared, sizeof(rc_shared));
  interrupts();
}

void calc_input(uint8_t channel, uint8_t input_pin) {
  if (digitalRead(input_pin) == HIGH) {
    rc_start[channel] = micros();
  } else {
    uint16_t rc_compare = (uint16_t)(micros() - rc_start[channel]);
    rc_shared[channel] = rc_compare;
  }
}

void calc_ch1(){calc_input(RC_CH1, CH1_pin);}
void calc_ch2(){calc_input(RC_CH2, CH2_pin);}
void calc_ch3(){calc_input(RC_CH3, CH3_pin);}
void calc_ch4(){calc_input(RC_CH4, CH4_pin);}
void calc_ch5(){calc_input(RC_CH5, CH5_pin);}
void calc_ch6(){calc_input(RC_CH6, CH6_pin);}


/* =========================== code ==================================*/
void setup() {
  Serial.begin(9600);
  pinMode(CH1_pin, INPUT);
  pinMode(CH2_pin, INPUT);
  pinMode(CH3_pin, INPUT);
  pinMode(CH4_pin, INPUT);
  pinMode(CH5_pin, INPUT);
  pinMode(CH6_pin, INPUT);

  enableInterrupt(CH1_pin, calc_ch1, CHANGE);
  enableInterrupt(CH2_pin, calc_ch2, CHANGE);
  enableInterrupt(CH3_pin, calc_ch3, CHANGE);
  enableInterrupt(CH4_pin, calc_ch4, CHANGE);
  enableInterrupt(CH5_pin, calc_ch5, CHANGE);
  enableInterrupt(CH6_pin, calc_ch6, CHANGE);

  FL_motor.attach(FL_motor_pin, 500, 2500);
  FR_motor.attach(FR_motor_pin, 500, 2500);
  BL_motor.attach(BL_motor_pin, 500, 2500);
  BR_motor.attach(BR_motor_pin, 500, 2500);
}

void loop() {
  rc_read_values();
  
  CH1 = (rc_values[RC_CH1] - 984) / 1004.0; // right horizontal
  CH2 = (rc_values[RC_CH2] - 972) / 1004.0; // right vertical
  CH3 = (rc_values[RC_CH3] - 964) / 980.0; // left vertical
  CH4 = (rc_values[RC_CH4] - 952) / 1004.0; // left horizontal
  CH5 = (rc_values[RC_CH5] - 980) / 1000.0; // left knob
  CH6 = (rc_values[RC_CH6] - 995) / 984.0; // right knob

  CH1 = max(CH1, 0);
  CH2 = max(CH2, 0);
  CH3 = max(CH3, 0);
  CH4 = max(CH4, 0);
  CH5 = max(CH5, 0);
  CH6 = max(CH6, 0);

//  
//  Serial.println('=');
//  Serial.println(rc_values[RC_CH1]);
//  Serial.println(rc_values[RC_CH2]);
//  Serial.println(rc_values[RC_CH3]);
//  Serial.println(rc_values[RC_CH4]);
//  Serial.println(rc_values[RC_CH5]);
//  Serial.println(rc_values[RC_CH6]);

  Serial.println('=');
  Serial.println('=');
  Serial.println('=');
  Serial.println('=');
  Serial.println(CH1);
  Serial.println(CH2);
  Serial.println(CH3);
  Serial.println(CH4);
  Serial.println(CH5);
  Serial.println(CH6);  

  float vx = CH2 * 2 - 1;
  float vy = - (CH1 * 2 - 1);
  float vw = - (CH4 * 2 - 1);
  float power_switch= CH5 * 2 -1;

  if (fabs(vx) < 0.1)
  {
    vx = 0.0;
  }

  if (fabs(vy) < 0.1)
  {
    vy = 0.0;
  }

  if (fabs(vw) < 0.1)
  {
    vw = 0.0;
  }

  if (power_switch < 0)
  {
    vx = 0.0;
    vy = 0.0;
    vw = 0.0;
  }

  float wheel_v[4];
  float max_v = 0;
  
  wheel_v[0] = - vx - vy - vw * rotate_ratio;
  wheel_v[1] = vx - vy - vw * rotate_ratio;
  wheel_v[2] = vx + vy - vw * rotate_ratio;
  wheel_v[3] = -vx + vy - vw * rotate_ratio;

  for (uint8_t i = 0; i < 4; i++)
  {
    if (fabs(wheel_v[i]) > max_v)
      max_v = fabs(wheel_v[i]);
  }

  //equal proportion
  if (max_v > 1)
  {
    float rate = 1 / max_v;
    for (uint8_t i = 0; i < 4; i++)
      wheel_v[i] *= rate;
  }

  FR_motor.writeMicroseconds(int(wheel_v[0] * 1000 + 1500));
  FL_motor.writeMicroseconds(int(wheel_v[1] * 1000 + 1500));
  BL_motor.writeMicroseconds(int(wheel_v[2] * 1000 + 1500));
  BR_motor.writeMicroseconds(int(wheel_v[3] * 1000 + 1500));
  
//  Serial.println('=');
//  Serial.println(int(wheel_v[0] * 1000 + 1500));
//  Serial.println(int(wheel_v[1] * 1000 + 1500));
//  Serial.println(int(wheel_v[2] * 1000 + 1500));
//  Serial.println(int(wheel_v[3] * 1000 + 1500));

//  delay(20);
}
