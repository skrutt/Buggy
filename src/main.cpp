/*
  ESP8266 Blink by Simon Peter
  Blink the blue LED on the ESP-01 module
  This example code is in the public domain

  The blue LED on the ESP-01 module is connected to GPIO1
  (which is also the TXD pin; so we cannot use Serial.print() at the same time)

  Note that this sketch uses LED_BUILTIN to find the pin with the internal LED
*/
#define LED_BUILTIN PB1

#include <avr/io.h>
#include <DigiCDC.h> //serial
#include <wiring.h>
//#include <SoftRcPulseOut.h>
//SoftRcPulseOut servo1;


void setup() 
{

  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  //  servo1.attach(LED_BUILTIN);
  //servo1.setMinimumPulse(1000);
  //servo1.setMaximumPulse(1900);

  //Set timer
  //TCCROA &= ~(b1111 << 4); // clear COM0A1, COM0A0, COM0B0, COM0B1. "normal port operation"
  TCCR0A = 0; //also set wgm0&1 normal 0-255
  
  TCCR0B = 0; //stop timer by writing no prescaler
  
  // 2000/ 15.5 = 129.0322 1000 / 15.5 = 64.51 , 129*15.5= 1999.5 us 65*15.5 =1007.5 us
  /*
   * Time taken per increment = 1/16.11KHz = 62us
    Time taken for 0 to 255 count in TCNT0 register = 62us x 255 = 15.81ms
    FOC0A and FOC0B bits are worth noting as writing 1 to these bits will force compare match and as a result the output pins OC0A and OC0B pins can be set, cleared or toggled on demand based on the desire of programmer.
   */
  // TIMSK |= 1 << 4; // set OCIE0A, compare enable
   TIMSK |= (1 << OCIE0A); // set OCIE0A, compare enable
   OCR0A = 129; //compare value
   TCCR0B = 1 << 2;  // set wgm2 & CS02-00, 0011. prescaler /256 = 15,5 us, starts timer
  
}
void startTimer0()
{
  digitalWrite(LED_BUILTIN, HIGH); 
  TCNT0 = 0;  //Reset timer counter
  TCCR0B = 1 << 2;  // set wgm2 & CS02-00, 0011. prescaler /256 = 15,5 us, starts timer
}

volatile uint32_t twomsi = 0;
ISR (TIMER0_COMPA_vect)
{
  digitalWrite(LED_BUILTIN, LOW); 
  twomsi++;
  TCCR0B = 0; //stop timer by writing no prescaler
}

int pulse = 0;
  
// the loop function runs over and over again forever
void loop() 
{
//  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on (Note that LOW is the voltage level
  // but actually the LED is on; this is because
  // it is active low on the ESP-01)
  //delay(1000);                      // Wait for a second
//  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  /*if(SoftRcPulseOut::refresh()) //without argument, refresh occurs every 20 ms (internal to the lib)
  {
   pulse++;
  }
  if(pulse > 180)
  {
    pulse = 0;
  }
  servo1.write(pulse);*/
  
  uint16_t i = 0;
  int16_t fwdcount = 0, rwdcount = 0;
  
  int starttime = millis();
  //for(; starttime + 19 >= millis();i++)
  for(; i< 2500;i++)
  {
    //analogRead(0);
    //analogRead(1);
    //accumulate reading of input pwm pins, ~2500 times
    fwdcount += digitalRead(2);
    rwdcount += digitalRead(0);
    
  }
  int stoptime = millis();
  
  //outputvalue
  //range 65-129, 64 steps
  //center = 65+32 = 97, 1503.5us
  
  //filter section
  static int filterfwd = 0, filterrwd = 0;

  if(fwdcount > filterfwd)
  {
    filterfwd = (filterfwd + fwdcount) / 2;   //50% addition each time
  }
  else 
  {
    filterfwd = fwdcount;
  }
  //reverse
  if(rwdcount > filterrwd)
  {
    filterrwd = (filterrwd + rwdcount) / 2;   //50% addition each time
  }
  else 
  {
    filterrwd = rwdcount;
  }
  
  
  //Collect counts
  int32_t count = filterfwd - filterrwd;

  //scale?
  count = count * 3 / 4;
  
  
  //Set output value
  OCR0A = 97 +(count * 32 / i) ; 
  
  //Fire a shot
  startTimer0();
  
  //Start over

}/*
void debug_loop()
{
//Start writing ouput
  SerialUSB.begin();
  while(1)
  {
    SerialUSB.delay(1000);
    SerialUSB.println("1");
    SerialUSB.println(starttime);
    SerialUSB.println("2");
    SerialUSB.println(stoptime);
    SerialUSB.println("3");
    SerialUSB.println(i);
    SerialUSB.println("4");
    SerialUSB.println(twomsi);    
    SerialUSB.println("5");
    SerialUSB.println( millis()); 
    SerialUSB.println("6");
    SerialUSB.println( fwdcount);
  }
}//*/
void handlePWMPPM()
{
  //90deg is neutral, 0deg = -100%, 180deg = 100%
  //loop
  //read analog inputs
  
  //filter and compare

  //Write update on output
 // servo1.write(pulse);
  //force update
  //SoftRcPulseOut::refresh(1);
}
//
void setupTimer()
{
  
}
