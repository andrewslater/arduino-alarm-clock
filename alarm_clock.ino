
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Time.h"
#include "Encoder.h"
#include "Bounce2.h"

#define ENCODER_THRESHOLD 4

enum ClockMode {CLOCK, SET_ALARM1, SET_ALARM2, SET_SNOOZE_DURATION};

Adafruit_7segment matrix = Adafruit_7segment();
Encoder minuteEncoder = Encoder(8,9);
Encoder hourEncoder = Encoder(3,4);
ClockMode mode = CLOCK;

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("Alarm Clock v0.1b");
#endif
  matrix.begin(0x70);
}

void loop() {
  // Display Time
  int curHour = hour();
  int curMin = minute();

  switch(mode) {
    case CLOCK:
      displayClock(curHour, curMin);
      break;

    case SET_ALARM1:
    case SET_ALARM2:
    case SET_SNOOZE_DURATION:
    default:
      break;
  }
  

}

void displayClock(int currentHour, int currentMinute) {
  displayTime(currentHour, currentMinute, matrix);

  int minuteMovement = minuteEncoder.read() / ENCODER_THRESHOLD;
  int newMinute = calculateWraparound(currentMinute, minuteMovement, 60);  

  int hourMovement = hourEncoder.read() / ENCODER_THRESHOLD;
  int newHour = calculateWraparound(currentHour, hourMovement, 24);
  
  if (newMinute != currentMinute || newHour != currentHour) {
    setTime(newHour, newMinute, 0, day(), month(), year());  
    minuteEncoder.write(0);      
    hourEncoder.write(0);
  }
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

void displayTime(int currentHour, int currentMinute, Adafruit_7segment matrix) {
  if (currentHour < 10) {
      matrix.writeDigitRaw(0,0);
    } else {
      matrix.writeDigitNum(0, currentHour / 10);
    }

    matrix.writeDigitNum(1, currentHour % 10);
    matrix.writeDigitNum(3, currentMinute / 10);
    matrix.writeDigitNum(4, currentMinute == 0 ? 0 : (currentMinute % 10));  
    matrix.drawColon(second() % 2 == 0);
    matrix.writeDisplay();
}
