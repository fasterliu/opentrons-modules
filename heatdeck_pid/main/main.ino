#include <AutoPID.h>

#define OUTPUT_MIN -1
#define OUTPUT_MAX 1
#define KP 4
#define KI .0003
#define KD .05

#define ZERO_DEGREES 0 //This would not work for a module that can reach 0 degrees
bool tempSending = false;

double temperature, setPoint, outputVal;
AutoPID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);

void setup() {
  setPoint = 4;
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);

  //if temperature is more than 10 degrees below or above setpoint, OUTPUT will be set to min or max respectively
  myPID.setBangBang(10);
  
  //set PID update interval to 4000ms
  myPID.setTimeStep(400);
}

void loop() {
  checkCommand();
  temperature = read_temp();
  myPID.run();
  float ran = random(0, 100)/(float)100;
  
  if (ran < abs(outputVal) && setPoint > ZERO_DEGREES) {
    if (outputVal > 0) { 
      heat();
    }
    else if (outputVal < 0) {
      cool();
    }
  }
  else {
    off();
  }
}
