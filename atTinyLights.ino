#include <SoftwareSerial.h>

String MESSAGE = "";
String LIGHTS_ON = "1";
String LIGHTS_OFF = "0";
String PIN = "1234";           // PIN to enter upon bluetooth pairing
String NAME = "Lights!";       // Name of device
// int LED = 4;                // Output pin lights
// int IR_SENSOR = 2;          // Input pin IR sensor
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
//  delay(500);
//  mySerial.print("AT+NAME" + NAME);
//  delay(1000);
//  mySerial.print("AT+PIN" + PIN);
//  delay(1000);
//  mySerial.flush();
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
    /*
    The logic here will trigger on the light switch via motion sensor only if the time
    between now and turning it off manually is greater than 60 minutes. The first "or"
    is needed to divide millis() down to less than the max int type value, which is 65,536
    (seconds) unsigned, which is also only 18.2 hours. If not divided, the int type value
    would overflow before the value that millis() returns, which overflows after about
    49.7 days. The second "or" is needed to run the logic even if the time from program
    start is less than 100 seconds.
    */
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
      mySerial.flush();
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
  for (int pwm = 0; pwm <= 255; pwm++){
    analogWrite(4, pwm);
    delay(4);
  }
  SWITCHED_ON = true;
}

void fadeOut() {
  mySerial.println(LIGHTS_OFF);
  for (int pwm = 255; pwm > -1; pwm--){
    analogWrite(4, pwm);
    delay(4);
  }
  SWITCHED_ON = false;
}

void startUp(){
  mySerial.println(LIGHTS_ON);
  mySerial.flush();
  // Uncomment for flicker effect on startup
//  onOff(35, 100);
//  onOff(35, 300);
//  onOff(40, 300);
  fadeIn();
}

//void onOff(int on, int off) {
//  digitalWrite(4, HIGH);
//  delay(on);
//  digitalWrite(4, LOW);
//  delay(off);
//}
