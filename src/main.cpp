#include <Arduino.h>

const int DIAC_PIN = 3;

int myDelay = 0;
int myvalue = 0;
int last_CH1_state = 0;

void setup() {
  /*
   * Port registers allow for lower-level and 
   * faster manipulation of the i/o pins of the microcontroller on an Arduino board. 
   * The chips used on the Arduino board 
   * (the ATmega8 and ATmega168) have three ports:
     -B (digital pin 8 to 13)
     -C (analog input pins)
     -D (digital pins 0 to 7)
  //All Arduino (Atmega) digital pins are inputs when you begin
  */
  
  //enable PCMSK0 scan
  PCICR |= (1 << PCIE0);
  //Set pin D8 trigger an interrupt on state change. Input from optocoupler
  PCMSK0 |= (1 << PCINT0);
  //Define D3 as output for the DIAC pulse
  pinMode(DIAC_PIN, OUTPUT);
  //Start serial com with the BT module (RX and TX pins)
  Serial.begin(9600); 
}

void loop() {
   /*Read the speed value via Bluetooth and map it from 10 to 10.000 us.
     AC frequency is 50Hz, so period is 20ms. We want to control the power
     of each half period, so the maximum is 10ms or 10.000us.
   */
   if(Serial.available() > 0) { 
      myvalue = map(Serial.read(), 0, 255, 10000, 10);
   }
    if (myDelay) {
      //This delay controls the power
      delayMicroseconds(myvalue); 
      digitalWrite(DIAC_PIN, HIGH);
      delayMicroseconds(100);
      digitalWrite(DIAC_PIN, LOW);
      myDelay = 0;
    } 
}




//This is the interruption routine
ISR(PCINT0_vect) {
  //Input from optocoupler
  //We make an AND with the pin state register, We verify if pin 8 is HIGH?
  if(PINB & B00000001){
    //If the last state was 0, then we have a state change
    if(last_CH1_state == 0){
      //We have detected a state change!
      myDelay = 1;
      last_CH1_state = 1;
    }
  }
  //If pin 8 is LOW and the last state was HIGH then we have a state change
  else if(last_CH1_state == 1){
    //We have detected a state change!
    myDelay = 1;
    //Store the current state into the last state for the next loop
    last_CH1_state = 0;
  }
}
