#include <Arduino.h>

const uint8_t DIAC_PIN = 3;

bool enablePhaseControl = false;
int speedVal = 0;
uint8_t prevStatus = 0;

void setup() { 
  //enable PCMSK0 scan
  PCICR |= (1 << PCIE0);
  //Set pin D8 trigger an interrupt on state change.
  PCMSK0 |= (1 << PCINT0);
  pinMode(DIAC_PIN, OUTPUT);
  // Setting baud rate to 9600 for serial communication from bluetooth module.
  Serial.begin(9600); 
}

void loop() {
   /*Read the speed value via Bluetooth and map it from 10 to 10000 us.
     AC frequency is 50Hz, so period is 20ms. We want to control the power
     of each half period, so the maximum is 10ms or 10000us.
   */
   if(Serial.available() > 0) { 
      speedVal = map(Serial.read(), 0, 255, 10000, 10);
   }
    if (enablePhaseControl) {
      //This delay controls the power
      delayMicroseconds(speedVal); 
      digitalWrite(DIAC_PIN, HIGH);
      delayMicroseconds(100);
      digitalWrite(DIAC_PIN, LOW);
      enablePhaseControl = 0;
    } 
}

// This is the interruption routine
ISR(PCINT0_vect) {
  // If pin D8 is high
  if(PINB & 0x01){
    // If previous state was low
    if(prevStatus == 0){
      // We have detected rising edge. Enabling phase control
      enablePhaseControl = true;
      // updating preStatus to current state i.e., high (1).
      prevStatus = 1;
    }
  }
  // Else D8 is low and if previous state was high
  else if(prevStatus == 1){
    // We have detected a falling edge.
    // updating preStatus to current state i.e., low (0).
    prevStatus = 0;
  }
}
