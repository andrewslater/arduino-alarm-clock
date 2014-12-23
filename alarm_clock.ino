
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Time.h"
#include "Encoder.h"
#include "Button.h"
#include "ClockAlarm.h"

#define ENCODER_THRESHOLD 4           // Number of encoder events which corresponding to one 'click'
#define BUTTON_DEBOUNCE_DELAY_MS  25

#define MINUTE_ENCODER_LEFT_PIN   8
#define MINUTE_ENCODER_RIGHT_PIN  9
#define HOUR_ENCODER_LEFT_PIN     3
#define HOUR_ENCODER_RIGHT_PIN    4
#define MODE_BUTTON_PIN           12

enum ClockMode {CLOCK, SET_ALARM1, SET_ALARM2, SET_SNOOZE_DURATION};


Adafruit_7segment matrix = Adafruit_7segment();
Encoder minuteEncoder = Encoder(MINUTE_ENCODER_LEFT_PIN, MINUTE_ENCODER_RIGHT_PIN);
Encoder hourEncoder = Encoder(HOUR_ENCODER_LEFT_PIN, HOUR_ENCODER_RIGHT_PIN);
Button modeButton = Button(MODE_BUTTON_PIN, true, true, BUTTON_DEBOUNCE_DELAY_MS);
ClockMode mode = CLOCK;
ClockAlarm alarm1, alarm2;

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("Alarm Clock v0.1c");
#endif
  matrix.begin(0x70);
  alarm1.alarmHour = alarm2.alarmHour = 12;
  alarm1.alarmMinute = alarm2.alarmMinute = 0;
  alarm1.displayMask = 4;
  alarm2.displayMask = 8;
}

void loop() {
  // Display Time
  int curHour = hour();
  int curMin = minute();
  int curSecond = second();

  int minuteMovement = minuteEncoder.read() / ENCODER_THRESHOLD;
  int hourMovement = hourEncoder.read() / ENCODER_THRESHOLD;

  modeButton.read();
  
  if (modeButton.wasPressed()) {
    cycleMode();
  }

  switch(mode) {
    case CLOCK:
      displayClock(curHour, curMin, curSecond, minuteMovement, hourMovement);
      break;

    case SET_ALARM1:
      displayAlarm(alarm1, minuteMovement, hourMovement, curSecond);
      break;

    case SET_ALARM2:
      displayAlarm(alarm2, minuteMovement, hourMovement, curSecond);
      break;
      
    case SET_SNOOZE_DURATION:
    default:
      break;
  }
  

}

void displayAlarm(ClockAlarm alarm, int minuteMovement, int hourMovement, int currentSecond) {
  displayTime(alarm.alarmHour, alarm.alarmMinute, currentSecond % 2 == 0, matrix);
  setColon(currentSecond % 2 == 1, alarm.displayMask);
  matrix.writeDisplay();
  
  int newMinute = calculateWraparound(alarm.alarmMinute, minuteMovement, 60);    
  int newHour = calculateWraparound(alarm.alarmHour, hourMovement, 24);
  
  if (newMinute != alarm.alarmMinute || newHour != alarm.alarmHour) { 
    alarm.alarmMinute = newMinute;
    alarm.alarmHour = newHour;   
    minuteEncoder.write(0);      
    hourEncoder.write(0);
  }
}


// void displayAlarm1(int minuteMovement, int hourMovement, int currentSecond) {
//   displayTime(alarm1Hour, alarm1Minute, currentSecond % 2 == 0, matrix);
//   setAlarm1LED(currentSecond % 2 == 1);
//   matrix.writeDisplay();
  
//   int newMinute = calculateWraparound(alarm1Minute, minuteMovement, 60);    
//   int newHour = calculateWraparound(alarm1Hour, hourMovement, 24);
  
//   if (newMinute != alarm1Minute || newHour != alarm1Hour) { 
//     alarm1Minute = newMinute;
//     alarm1Hour = newHour;   
//     minuteEncoder.write(0);      
//     hourEncoder.write(0);
//   }
// }

// void displayAlarm2(int minuteMovement, int hourMovement, int currentSecond) {
//   displayTime(alarm2Hour, alarm2Minute, currentSecond % 2 == 0, matrix);
//   setAlarm2LED(currentSecond % 2 == 1);
//   matrix.writeDisplay();
  
//   int newMinute = calculateWraparound(alarm2Minute, minuteMovement, 60);    
//   int newHour = calculateWraparound(alarm2Hour, hourMovement, 24);
  
//   if (newMinute != alarm2Minute || newHour != alarm2Hour) { 
//     alarm2Minute = newMinute;
//     alarm2Hour = newHour;   
//     minuteEncoder.write(0);      
//     hourEncoder.write(0);
//   }
// }

// void setAlarm1LED(boolean enable) {
//   setColon(enable, 4);
// }

// void setAlarm2LED(boolean enable) {
//   setColon(enable, 8);
// }

void setColon(boolean enable, uint8_t mask) {
  if (enable) {
    matrix.displaybuffer[2] |= mask;  
  } else {    
    matrix.displaybuffer[2] &= ~mask;
  }  
}

void cycleMode() {
  switch(mode) {
    case CLOCK:
      mode = SET_ALARM1;
      break;

    case SET_ALARM1:
      mode = SET_ALARM2;
      break;

    case SET_ALARM2:
      mode = SET_SNOOZE_DURATION;
      break;

    case SET_SNOOZE_DURATION:
      mode = CLOCK;
      break;
  }
}

void displayClock(int currentHour, int currentMinute, int currentSecond, int minuteMovement, int hourMovement) {
  displayTime(currentHour, currentMinute, currentSecond % 2 == 0, matrix);
  
  int newMinute = calculateWraparound(currentMinute, minuteMovement, 60);    
  int newHour = calculateWraparound(currentHour, hourMovement, 24);
  
  if (newMinute != currentMinute || newHour != currentHour) {
    setTime(newHour, newMinute, 0, day(), month(), year());  
    minuteEncoder.write(0);      
    hourEncoder.write(0);
  }
  
  matrix.writeDisplay();
}

int calculateWraparound(int curValue, int delta, int maxValue) {
  int newValue = curValue + delta;

  if (newValue < 0) {
    return maxValue + newValue;
  } else if (newValue >= maxValue) {
    return maxValue - newValue;
  }

  return newValue;
}

void displayTime(int currentHour, int currentMinute, boolean drawColon, Adafruit_7segment &matrix) {
  if (currentHour < 10) {
      matrix.writeDigitRaw(0,0);
    } else {
      matrix.writeDigitNum(0, currentHour / 10);
    }

    matrix.writeDigitNum(1, currentHour % 10);
    matrix.writeDigitNum(3, currentMinute / 10);
    matrix.writeDigitNum(4, currentMinute == 0 ? 0 : (currentMinute % 10));  
    matrix.drawColon(drawColon);
    //matrix.writeDisplay();
}
