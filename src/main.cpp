/*
*   RTES Challenge - Need For Speed
*   Fall 2021 - New York University
*   Submitted By:- KUNAL KASHYAP (kk4564)
*/


#include <mbed.h>
#include "gyroDriver.h"
#include "TS_DISCO_F429ZI.h"
#include "LCD_DISCO_F429ZI.h"

#define WFACTOR 1.8
#define RFACTOR 4
 
LCD_DISCO_F429ZI lcd;
TS_DISCO_F429ZI ts;

L3GD20 gyro(PF_9, PF_8, PF_7, PC_1); // MOSI, MISO, SCLK, CS (Pins are for the inbuilt Gyroscope)

volatile bool sample = true;
volatile int16_t cycles = 0;
volatile int16_t dataSamples[400][3];
volatile int16_t dataZeroSamples[100][3];

volatile float ans;
volatile float w = 0, linDist = 0, linVel = 0;

volatile uint8_t text[50];
volatile uint8_t text1[50];
volatile uint8_t text2[50];

void setSampleFlag() {
  if(cycles < 400)
    sample = true;
}

// This function handles the touch event and changes the screen to show different data
void changeScreen(uint8_t curr_disp) {
  switch (curr_disp)
  {
  case 0:
    sprintf((char*)text1, "DPS=%d", (int)ans);
    sprintf((char*)text2, "Speed=%d", (int)linVel);
    sprintf((char*)text, "%dm", (int)linDist);
    lcd.DisplayStringAt(0, LINE(4), (uint8_t *)&text1, CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)&text2, CENTER_MODE);
    if(ans < 180) {
      lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"A GOOD WALK OF", CENTER_MODE);
    } else {
      lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"A GOOD RUN OF", CENTER_MODE);
    }
    lcd.DisplayStringAt(0, LINE(7), (uint8_t *)&text, CENTER_MODE);
    break;
  case 1:
    sprintf((char*)text, "%dm", (int)linDist);
    lcd.Clear(LCD_COLOR_RED);
    lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"DISTANCE", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"COVERED", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(6), (uint8_t *)&text, CENTER_MODE);
    break;
  case 2:
    sprintf((char*)text, "%dm/s", (int)linVel);
    lcd.Clear(LCD_COLOR_RED);
    lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"SPEED", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)&text, CENTER_MODE);
    break;
  case 3:
    sprintf((char*)text, "%d dps", (int)ans);
    lcd.Clear(LCD_COLOR_RED);
    lcd.DisplayStringAt(0, LINE(4), (uint8_t *)"ANGULAR", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"VELOCITY", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(6), (uint8_t *)&text, CENTER_MODE);
    break;
  
  default:
    break;
  }
}

int main() {

  // put your setup code here, to run once:
  TS_StateTypeDef TS_State;
  uint8_t status;
  uint8_t curr_disp = 0;
  BSP_LCD_SetFont(&Font20);
  
  //Init screen
  lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"RTES CHALLENGE", CENTER_MODE);
  lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"NEED FOR SPEED", CENTER_MODE);
  wait_us(3000000);

  status = ts.Init(lcd.GetXSize(), lcd.GetYSize());
  if (status != TS_OK)
  {
    lcd.Clear(LCD_COLOR_RED);
    lcd.SetBackColor(LCD_COLOR_RED);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"INIT FAIL", CENTER_MODE);
  }
  else
  {
    lcd.Clear(LCD_COLOR_GREEN);
    lcd.SetBackColor(LCD_COLOR_GREEN);
    lcd.SetTextColor(LCD_COLOR_WHITE);
    lcd.DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN", CENTER_MODE);
    lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"INIT OK", CENTER_MODE);
  }
  
  wait_us(2000000);
  lcd.SetBackColor(LCD_COLOR_BLACK);
  lcd.SetTextColor(LCD_COLOR_WHITE);

  bool clor = 0;

  Ticker ticker;

  ticker.attach(&setSampleFlag, 50ms);
  
  while(1) {
    if(sample) {
      // READ X-AXIS DATA
      // Calling the read function we have created for the Gyroscope
      int8_t out_x_l = gyro.read_register(L3GD20::Registers::OUT_X_L_ADDR);
      int8_t out_x_h = gyro.read_register(L3GD20::Registers::OUT_X_H_ADDR);

      dataSamples[cycles][0] = GYRO_SENSITIVITY_250*0.001*(out_x_h << 8 | out_x_l);

      // READ Y-AXIS DATA
      int8_t out_y_l = gyro.read_register(L3GD20::Registers::OUT_Y_L_ADDR);
      int8_t out_y_h = gyro.read_register(L3GD20::Registers::OUT_Y_H_ADDR);

      dataSamples[cycles][1] = GYRO_SENSITIVITY_250*0.001*(out_y_h << 8 | out_y_l);

      // READ Z-AXIS DATA
      int8_t out_z_l = gyro.read_register(L3GD20::Registers::OUT_Z_L_ADDR);
      int8_t out_z_h = gyro.read_register(L3GD20::Registers::OUT_Z_H_ADDR);

      dataSamples[cycles][2] = GYRO_SENSITIVITY_250*0.001*(out_z_h << 8 | out_z_l);

      cycles++;
      sample = false;
    }

    // Detecting Touch
    ts.GetState(&TS_State);      
    if(TS_State.TouchDetected) {
      if(curr_disp == 3) {
        curr_disp = 0;
      } else curr_disp++;
      changeScreen(curr_disp);
    }

    if(cycles >= 400) {
      printf("\n\r\n\r");
			printf("data = [");
      int sum_400 = 0;
			for(int m = 0; m < 400; m++) {
        if(dataSamples[m][2] < 0) {
          sum_400 += -1*dataSamples[m][2];
        } else sum_400 += dataSamples[m][2];
        printf("%d,", dataSamples[m][2]);
			}
      printf("]");

      ans = (float)sum_400/400;
      if(ans <= 95.0) {
        ans = ans*WFACTOR;
        w = (ans*3.142)/180;  // w(in rad/s) = dps*(pi/180)
        linVel = w*0.51;      // v=wr ; 0.51 = radius
        linDist = linVel*20;  // distance = v*time ; time = 20 secs

        sprintf((char*)text1, "DPS=%d", (int)ans);
        sprintf((char*)text2, "Speed=%d", (int)linVel);
        sprintf((char*)text, "%dm", (int)linDist);
        lcd.Clear(LCD_COLOR_CYAN);
        if(!clor) {
          lcd.Clear(LCD_COLOR_ORANGE);
          clor = 1;
        } else {
          lcd.Clear(LCD_COLOR_CYAN);
          clor = 0;
        }
        lcd.DisplayStringAt(0, LINE(4), (uint8_t *)&text1, CENTER_MODE);
        lcd.DisplayStringAt(0, LINE(5), (uint8_t *)&text2, CENTER_MODE);
        lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"A GOOD WALK OF", CENTER_MODE);
        lcd.DisplayStringAt(0, LINE(7), (uint8_t *)&text, CENTER_MODE);
      } else {
        ans = ans*RFACTOR;
        w = (ans*3.142)/180;
        linVel = w*0.51;
        linDist = linVel*20;

        sprintf((char*)text1, "DPS=%d", (int)ans);
        sprintf((char*)text2, "Speed=%d", (int)linVel);
        sprintf((char*)text, "%dm", (int)linDist);
        lcd.Clear(LCD_COLOR_CYAN);
        if(!clor) {
          lcd.Clear(LCD_COLOR_ORANGE);
          clor = 1;
        } else {
          lcd.Clear(LCD_COLOR_CYAN);
          clor = 0;
        }
        lcd.DisplayStringAt(0, LINE(4), (uint8_t *)&text1, CENTER_MODE);
        lcd.DisplayStringAt(0, LINE(5), (uint8_t *)&text2, CENTER_MODE);
        lcd.DisplayStringAt(0, LINE(6), (uint8_t *)"A GOOD RUN OF", CENTER_MODE);
        lcd.DisplayStringAt(0, LINE(7), (uint8_t *)&text, CENTER_MODE);
      }
      cycles = 0; //reset
    }
  }
}