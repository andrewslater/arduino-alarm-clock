
#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Time.h"
#include "Encoder.h"


Adafruit_7segment matrix = Adafruit_7segment();
Encoder minEncoder = Encoder(8,9);
Encoder hourEncoder = Encoder(3,4);

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  Serial.println("7 Segment Backpack Test");
#endif
  matrix.begin(0x70);
  minEncoder.write(0);
}

void loop() {
  // Display Time
  int curHour = hour();
  int curMin = minute();

  if (curHour < 10) {
    matrix.writeDigitRaw(0,0);
  } else {
    matrix.writeDigitNum(0, curHour / 10);
  }
  matrix.writeDigitNum(1, curHour % 10);
  matrix.writeDigitNum(3, curMin / 10);
  matrix.writeDigitNum(4, curMin == 0 ? 0 : (curMin % 10));  
  matrix.drawColon(second() % 2 == 0);
  matrix.writeDisplay();

  int minuteMovement = minEncoder.read();
  int threshold = 4;
  int pauseDelay = 0;
  
  if (minuteMovement >= threshold) {
    Serial.println(minuteMovement, DEC);
    Serial.println("Adding 1 min!");
    curMin = (curMin + 1 > 59 ? 0 : curMin + 1);
    setTime(curHour, curMin, 0, day(), month(), year());  
    minEncoder.write(0);
    delay(pauseDelay);
  } else if (minuteMovement < -threshold) {
    Serial.println(minuteMovement, DEC);
    Serial.println("Subtracting 1 min!");
    curMin = (curMin -1 < 0 ? 59 : curMin - 1);
    setTime(curHour, curMin, 0, day(), month(), year());
    minEncoder.write(0);    
    delay(pauseDelay);
  }
  
  int hourMovement = hourEncoder.read();
  
  if (hourMovement >= threshold) {
    Serial.println(hourMovement, DEC);
    Serial.println("Adding 1 hour!");
    curHour = (curHour + 1 > 24 ? 0 : curHour + 1);
    setTime(curHour, curMin, 0, day(), month(), year());  
    hourEncoder.write(0);
    delay(pauseDelay);
  } else if (hourMovement <= -threshold) {
    Serial.println(hourMovement, DEC);
    Serial.println("Subtracting 1 hour!");
    curHour = (curHour - 1 < 0 ? 23 : curHour - 1);
    setTime(curHour, curMin, 0, day(), month(), year());
    hourEncoder.write(0);    
    delay(pauseDelay);
  }
  
}
