/*
This code is to be used with an Adafruit Trinket, however, the pinout can be
modified to fit any Arduino, probably :)
The circuit also includes an IR sensor, HM-06 bluetooth module, and a
photoresistor.
*/

#include <SoftwareSerial.h>

String MESSAGE = "";
String LIGHTS_ON = "1";
String LIGHTS_OFF = "0";
unsigned int AUTO_TIMER = 300; // Seconds until lights turn off after motion (5 minutes)
unsigned long START_TIME = 0;
unsigned long OFF_SWITCH_TIMER;
boolean SWITCHED_ON = false;
char CHARACTER;

SoftwareSerial mySerial(0, 1); // RX, TX

void setup() {
  pinMode(2, INPUT);  // Set pin 2 as input for IR sensor
  pinMode(4, OUTPUT); // Set pin 4 as output for LEDs
  mySerial.begin(9600);
  
  /*
  Uncomment code below to set friendly
  name and PIN upon boot
  */
  // String PIN = "1234"; // The PIN to enter upon bluetooth pairing
  // String NAME = "Lights!";
  // delay(500);
  // mySerial.print("AT+NAME" + NAME);
  // delay(1000);
  // mySerial.print("AT+PIN" + PIN);
  // delay(1000);
  // mySerial.flush();
}

void loop() {
  while(mySerial.available() > 0){
    delay(50);
    CHARACTER = mySerial.read();
    MESSAGE = MESSAGE + CHARACTER;
    manualSwitch();
  }
  MESSAGE = "";
  if(digitalRead(2) && SWITCHED_ON == false){
    if(START_TIME + OFF_SWITCH_TIMER < millis()/1000){
      delay(20);
      irSensor();
    }
  }
}

void manualSwitch(){
  if(MESSAGE == "open"){
    if(SWITCHED_ON == false){
      MESSAGE = "";
      startUp();
      mySerial.flush();
    }else if(SWITCHED_ON == true){
      START_TIME = millis()/1000;
      OFF_SWITCH_TIMER = 60 * 60; // This value is set in seconds, in this case, 60 minutes
      MESSAGE = "";
      fadeOut();
    }
  }
}

void irSensor(){
  if(SWITCHED_ON == false){
    startUp();
  }
  for(int i=0; i<AUTO_TIMER*100; i++){
    delay(10);
    if(mySerial.available() > 0){
      break;
    }else if(digitalRead(2)){
      i=0;
    }else if(i == AUTO_TIMER*100 - 1 && SWITCHED_ON == true){
      OFF_SWITCH_TIMER = 0;
      fadeOut();
    }
  }
}

void fadeIn() {
  mySerial.println(LIGHTS_ON);
  mySerial.flush();
  for (int pwm = 0; pwm <= 255; pwm++){
    analogWrite(4, pwm);
    delay(4);
  }
  SWITCHED_ON = true;
}

void fadeOut() {
  mySerial.println(LIGHTS_OFF);
  mySerial.flush();
  for (int pwm = 255; pwm > -1; pwm--){
    analogWrite(4, pwm);
    delay(4);
  }
  SWITCHED_ON = false;
}

void startUp(){
  fadeIn();
}
