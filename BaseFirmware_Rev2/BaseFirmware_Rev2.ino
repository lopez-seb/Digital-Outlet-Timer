#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <avr/interrupt.h>
#include "MyTime.h"
//#include "Buttons.h"
//#include "SetTime.h"

//construct the lcd
LiquidCrystal_I2C lcd(0x27, 20, 4);
// construct the Real Time Clock - variables only
RTC rtc;
//Buttons button;
//SetTime time;

// button assignments
// Buttons declarations (using pins 2-8)

const int b1 = 1; // OVERRIDE
const int b2 = 2; // MODE
const int b3 = 3; // ENTER
const int b4 = 4; // RIGHT
const int b5 = 5; // DOWN
const int b6 = 6; // UP
const int b7 = 7; // LEFT
const int sig = 0; // LED

enum states { Idle, TimeSet, ProgSet, Run };
states MasterState;

void setup() {
  //power up screen
  lcd.init();
  // begin real time clock - interrupts turned on
  rtc.timerinit();

  pinMode(b1, INPUT);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(b4, INPUT);
  pinMode(b5, INPUT);
  pinMode(b6, INPUT);
  pinMode(b7, INPUT);

  MasterState = Idle;
}

// Button states (LOW)
int OR = 0;
int mode = 0;
int enter = 0;
int right = 0;
int down = 0;
int up = 0;
int left = 0;


//Functions
int debounce(int button)
{
  static bool lastPress, thisPress;
  int bstate = LOW;

  lastPress = thisPress;
  thisPress = button;
  if (thisPress && !lastPress) {
    bstate = HIGH;
  }
  return bstate;
}

void checkTime(int hourON, int hourOFF, int tensON, int tensOFF, int minsON, int minsOFF){
  if((rtc.hours() == hourON) && (rtc.tens() == tensON) && (rtc.mins() == minsON)){
        digitalWrite(sig, HIGH);
  }
  if((rtc.hours() == hourOFF) && (rtc.tens() == tensOFF) && (rtc.mins() == minsOFF)){
        digitalWrite(sig, LOW);
  }
}


int sub_state = 0;
int counter = 0;

int set_hours = 0;
int set_tens = 0;
int set_mins = 0;

int hourON = 0;
int tensON = 0;
int minsON = 0;
int hourOFF = 0;
int tensOFF = 0;
int minsOFF = 0;



void loop() {    //**************************** MAIN ****************************


  OR = digitalRead(b1);
  mode = digitalRead(b2);
  enter = digitalRead(b3);
  right = digitalRead(b4);
  down = digitalRead(b5);
  up = digitalRead(b6);
  left = digitalRead(b7);


  switch (MasterState) {

    case Idle:   //------------------------------ IDLE ------------------------------
      if (sub_state == 0) {
        lcd.setCursor(0, 0);
        lcd.print(rtc.hours()); lcd.print(":"); lcd.print(rtc.tens()); lcd.print(rtc.mins());
        lcd.print(":"); lcd.print(rtc.sec());
        lcd.setCursor(0, 1);
        lcd.print("                    ");
        lcd.setCursor(0, 2);
        lcd.print("                    ");
        lcd.setCursor(0, 3);
        if((hourON && tensON && minsON && hourOFF && tensOFF && minsOFF) != 0){
          lcd.print("ON:");lcd.print(hourON);lcd.print(":");
          lcd.print(tensON);lcd.print(minsON);
          lcd.print("    OFF:");lcd.print(hourOFF);lcd.print(":");
          lcd.print(tensOFF);lcd.print(minsOFF);
        }else{
          lcd.print("                    ");
        }
      }

      //  =========================== begin sub menus ===========================
      mode = debounce(mode);
      if (mode) {
        if (sub_state == 3) {
          sub_state = 0;
        }
        else {
          sub_state += 1;
        }
      }
      if (sub_state == 1) {   //  idle substate 1?: go to set time?
        lcd.setCursor(0, 1);
        lcd.print("     Set time ?     ");
        lcd.setCursor(0, 2);
        lcd.print("    press enter     ");

        enter = debounce(enter);
        OR = debounce(OR);
        if (enter) {
          sub_state = 0;
          MasterState = TimeSet;
        }
        if (OR) {
          sub_state = 0;
        }
      }
      if (sub_state == 2) {   // idle substate 2?: go to set program times?
        lcd.setCursor(0, 1);
        lcd.print(" Set Program Times? ");
        lcd.setCursor(0, 2);
        lcd.print("    press enter     ");

        enter = debounce(enter);
        OR = debounce(OR);
        if (enter) {
          sub_state = 0;
          MasterState = ProgSet;
        }
        if (OR) {
          sub_state = 0;
        }
      }
      if (sub_state == 3) {
        lcd.setCursor(0, 1);
        lcd.print("    Run Program?    ");
        lcd.setCursor(0, 2);
        lcd.print("    press enter     ");

        enter = debounce(enter);
        OR = debounce(OR);
        if (enter) {
          sub_state = 0;
          MasterState = Run;
        }
        if (OR) {
          sub_state = 0;
        }
      }
      break;    //------------------------------ end idle ------------------------------

    case TimeSet:   //------------------------------ SET TIME ------------------------------
      lcd.setCursor(0, 0);
      lcd.print(rtc.hours()); lcd.print(":"); lcd.print(rtc.tens()); lcd.print(rtc.mins());
      lcd.print(":"); lcd.print(rtc.sec());
      lcd.setCursor(0, 1);
      lcd.print("    set new time    ");
      lcd.setCursor(0, 2);
      lcd.print("       "); lcd.print(set_hours); lcd.print(":");
      lcd.print(set_tens); lcd.print(set_mins); lcd.print("        ");
      //        ^ 11 spot              ^12 spot

      left = debounce(left);
      if (left) {
        if (sub_state == 0) {
          sub_state = 2;
        }
        else {
          sub_state -= 1;
        }
      }
      right = debounce(right);
      if (right) {
        if (sub_state == 2) {
          sub_state = 0;
        }
        else {
          sub_state += 1;
        }
      }

      mode = debounce(mode);
      if (mode) {
        sub_state = 3;
      }

      switch (sub_state) {       //===================timeset substate switch=============

        case 0:                     //++++++++++++++set hours+++++++++++++++++++
          up = debounce(up);
          if (up) {
            if (set_hours == 23) {
              set_hours = 0;
            }
            else {
              set_hours += 1;
            }
          }
          down = debounce(down);
          if (down) {
            if (set_hours == 0) {
              set_hours = 23;
            }
            else {
              set_hours -= 1;
            }
          }
          lcd.setCursor(0, 3);
          lcd.print("       |             ");
          break;

        case 1:                     //++++++++++++++set tens+++++++++++++++++++++
          up = debounce(up);
          if (up) {
            if (set_tens == 5) {
              set_tens = 0;
            }
            else {
              set_tens += 1;
            }
          }
          down = debounce(down);
          if (down) {
            if (set_tens == 0) {
              set_tens = 5;
            }
            else {
              set_tens -= 1;
            }
          }

          lcd.setCursor(0, 3);
          lcd.print("          |          ");
          break;

        case 2:
          up = debounce(up);
          if (up) {
            if (set_mins == 9) {
              set_mins = 0;
            }
            else {
              set_mins += 1;
            }
          }
          down = debounce(down);
          if (down) {
            if (set_mins == 0) {
              set_mins = 9;
            }
            else {
              set_mins -= 1;
            }
          }

          lcd.setCursor(0, 3);
          lcd.print("            |          ");
          break;

        case 3:
          lcd.setCursor(0, 3);
          lcd.print("    Time Correct?   ");

          enter = debounce(enter);
          if (enter) {
            rtc.setTime(set_hours, set_tens, set_mins);
            set_hours = 0;
            set_tens = 0;
            set_mins = 0;
            lcd.clear();
            sub_state = 0;
            MasterState = Idle;

          }
          break;
      } //------------------------------end time set------------------------------
      break;

    case ProgSet: //==================== BEGIN PROGRAM SET ========================

      lcd.setCursor(0, 0);
      lcd.print(rtc.hours()); lcd.print(":"); lcd.print(rtc.tens()); lcd.print(rtc.mins());
      lcd.print(":"); lcd.print(rtc.sec());
      lcd.setCursor(0, 1);
      if(sub_state < 3){
        lcd.print("Set Program On Time");
        lcd.setCursor(0, 2);
        lcd.print("       "); lcd.print(hourON); lcd.print(":");
        lcd.print(tensON); lcd.print(minsON); lcd.print("        ");
      }else{
        lcd.print("Set Program Off Time");
        lcd.setCursor(0, 2);
        lcd.print("       "); lcd.print(hourOFF); lcd.print(":");
        lcd.print(tensOFF); lcd.print(minsOFF); lcd.print("        ");
      }

      left = debounce(left);
      if (left) {
        if (sub_state == 0) {
          sub_state = 5;
        }
        else {
          sub_state -= 1;
        }
      }
      right = debounce(right);
      if (right) {
        if (sub_state == 5) {
          sub_state = 0;
        }
        else {
          sub_state += 1;
        }
      }
      mode = debounce(mode);
      if (mode) {
        sub_state = 6;
      }

      switch(sub_state){ //+++++++++++++ Program set Substates +++++++++++++

        case 0:               //  --------hours ON
          up = debounce(up);
          if (up) {
            if (hourON == 23) {
              hourON = 0;
            }
            else {
              hourON += 1;
            }
          }
          down = debounce(down);
          if (down) {
            if (hourON == 0) {
              hourON = 23;
            }
            else {
              hourON -= 1;
            }
          }
          lcd.setCursor(0, 3);
          lcd.print("       |             ");
        break;

        case 1:             //  ----------tens ON
          up = debounce(up);
          if (up) {
            if (tensON == 5) {
              tensON = 0;
            }
            else {
              tensON += 1;
            }
          }
          down = debounce(down);
          if (down) {
            if (tensON == 0) {
              tensON = 5;
            }
            else {
              tensON -= 1;
            }
          }

          lcd.setCursor(0, 3);
          lcd.print("          |          ");
        break;

        case 2:             //  -----------mins ON
          up = debounce(up);
          if (up) {
            if (minsON == 9) {
              minsON = 0;
            }
            else {
              minsON += 1;
            }
          }
          down = debounce(down);
          if (down) {
            if (minsON == 0) {
              minsON = 9;
            }
            else {
              minsON -= 1;
            }
          }

          lcd.setCursor(0, 3);
          lcd.print("            |          ");
        break;

        case 3:             //  ----------hours OFF
           up = debounce(up);
          if (up) {
            if (hourOFF == 23) {
              hourOFF = 0;
            }
            else {
              hourOFF += 1;
            }
          }
          down = debounce(down);
          if (down) {
            if (hourOFF == 0) {
              hourOFF = 23;
            }
            else {
              hourOFF -= 1;
            }
          }
          lcd.setCursor(0, 3);
          lcd.print("       |             ");
        break;

        case 4:             //  --------------tens OFF
          up = debounce(up);
          if (up) {
            if (tensOFF == 5) {
              tensOFF = 0;
            }
            else {
              tensOFF += 1;
            }
          }
          down = debounce(down);
          if (down) {
            if (tensOFF == 0) {
              tensOFF = 5;
            }
            else {
              tensOFF -= 1;
            }
          }

          lcd.setCursor(0, 3);
          lcd.print("          |          ");
        break;

        case 5:           //  ---------------mins OFF
          up = debounce(up);
          if (up) {
            if (minsOFF == 9) {
              minsOFF = 0;
            }
            else {
              minsOFF += 1;
            }
          }
          down = debounce(down);
          if (down) {
            if (minsOFF == 0) {
              minsOFF = 9;
            }
            else {
              minsOFF -= 1;
            }
          }

          lcd.setCursor(0, 3);
          lcd.print("            |          ");
        break;

        case 6:
          lcd.setCursor(0, 3);
          lcd.print("  Program Correct?  ");

          enter = debounce(enter);
          if (enter) {
            lcd.clear();
            sub_state = 0;
            MasterState = Idle;
          }
        break;
      }

      break;

    case Run:
      // -=-=-=-=-=-=-=-=-=-Program Run Codes
      lcd.setCursor(0, 0);
      lcd.print(rtc.hours()); lcd.print(":"); lcd.print(rtc.tens()); lcd.print(rtc.mins());
      lcd.print(":"); lcd.print(rtc.sec());
      
      lcd.setCursor(0,1);
      lcd.print("   Program Running  ");
      
      lcd.setCursor(0,2);
      lcd.print("ON:");lcd.print(hourON);lcd.print(":");
      lcd.print(tensON);lcd.print(minsON);
      lcd.print("    OFF:");lcd.print(hourOFF);lcd.print(":");
      lcd.print(tensOFF);lcd.print(minsOFF);

      OR = debounce(OR);
      if(OR){
        digitalWrite(sig,!digitalRead(sig));
      }

      break;

  }

}

ISR(TIMER1_COMPA_vect) {
  rtc.updateTime();
  if(MasterState == Run){
    checkTime(hourON, hourOFF, tensON, tensOFF, minsON, minsOFF);
  }
}
