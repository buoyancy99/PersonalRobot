#include <Servo.h>

#define FL_motor_pin 4
#define FR_motor_pin 5
#define BL_motor_pin 6
#define BR_motor_pin 7

#define CH1_pin 8
#define CH2_pin 9
#define CH3_pin 10
#define CH4_pin 11
#define CH5_pin 12
#define CH6_pin 13

#define RADIAN_COEF 57.3f
#define WHEELTRACK 394
#define WHEELBASE 415
#define ROTATE_X_OFFSET 7
#define ROTATE_Y_OFFSET 0

float CH1, CH2, CH3, CH4, CH5, CH6;
Servo FL_motor, FR_motor, BL_motor, BR_motor;

//float rotate_ratio_fr = ((WHEELBASE + WHEELTRACK) / 2.0f - ROTATE_X_OFFSET + ROTATE_Y_OFFSET) / RADIAN_COEF;
//float rotate_ratio_fl = ((WHEELBASE + WHEELTRACK) / 2.0f - ROTATE_X_OFFSET - ROTATE_Y_OFFSET) / RADIAN_COEF;
//float rotate_ratio_bl = ((WHEELBASE + WHEELTRACK) / 2.0f + ROTATE_X_OFFSET - ROTATE_Y_OFFSET) / RADIAN_COEF;
//float rotate_ratio_br = ((WHEELBASE + WHEELTRACK) / 2.0f + ROTATE_X_OFFSET + ROTATE_Y_OFFSET) / RADIAN_COEF;
float rotate_ratio_fr = 1; 
float rotate_ratio_fl = 1; 
float rotate_ratio_bl = 1; 
float rotate_ratio_br = 1;

void setup() {
  Serial.begin(9600);
  pinMode(CH1_pin, INPUT);
  pinMode(CH2_pin, INPUT);
  pinMode(CH3_pin, INPUT);
  pinMode(CH4_pin, INPUT);
  pinMode(CH5_pin, INPUT);
  pinMode(CH6_pin, INPUT);
  FL_motor.attach(FL_motor_pin, 500, 2500);
  FR_motor.attach(FR_motor_pin, 500, 2500);
  BL_motor.attach(BL_motor_pin, 500, 2500);
  BR_motor.attach(BR_motor_pin, 500, 2500);
}

void loop() {
  CH1 = (pulseIn(CH1_pin, HIGH) - 996) / 989.0; // right horizontal
  CH2 = (pulseIn(CH2_pin, HIGH) - 997) / 980.0; // right vertical
  CH3 = (pulseIn(CH3_pin, HIGH) - 996) / 989.0; // left vertical
  CH4 = (pulseIn(CH4_pin, HIGH) - 997) / 988.0; // left horizontal
  CH5 = (pulseIn(CH5_pin, HIGH) - 995) / 994.0; // left knob
  CH6 = (pulseIn(CH6_pin, HIGH) - 995) / 994.0; // right knob

  float vx = CH1 * 2 - 1;
  float vy = CH2 * 2 - 1;
  float vw = CH4 * 2 - 1;

  float wheel_v[4];
  float max_v = 0;
  
  wheel_v[0] = - vx - vy - vw * rotate_ratio_fr;
  wheel_v[1] = vx - vy - vw * rotate_ratio_fl;
  wheel_v[2] = vx + vy - vw * rotate_ratio_bl;
  wheel_v[3] = -vx + vy - vw * rotate_ratio_br;

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
  
  Serial.println('=');
  Serial.println(CH1);
  Serial.println(CH2);
  Serial.println(CH3);
  Serial.println(CH4);
  Serial.println(CH5);
  Serial.println(CH6);
  delay(20);
}
