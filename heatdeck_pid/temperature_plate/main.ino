#include <PID_v1.h>

#define OUTPUT_MIN -1
#define OUTPUT_MAX 1
#define KP 0.6
#define KI .0005
#define KD .01

#define SHUTDOWN_TEMP 0 //This would not work for a module that can reach 0 degrees
bool tempSending = false;

double temperature, setPoint, outputVal, fanOutput;
PID myPID(&temperature, &outputVal, &setPoint, KP, KI, KD,  DIRECT);


double plateDutyCycle = 10000;
double lastCycle = 0;

bool heating = false;

void setup() {
  setPoint = 4;
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-1, 1);
  setupFanPWM();

}

void loop() {
  checkCommand();
  temperature = read_temp();

 if(plateDutyCycle < (millis()-lastCycle) ) {

    lastCycle = millis();


      
    if(setPoint > SHUTDOWN_TEMP)
    {
      myPID.Compute();
      float ran = random(0, 100)/(float)100;
      fanOutput = (39 + (-outputVal * 30)); // (total range = 0-79 = 1.25-100%)
      fanPwmDuty(fanOutput);
    
      if (ran < abs(outputVal)) 
      {
        if (outputVal > 0) { heat(); }
        else if (outputVal < 0 && heating) { off(); }
        else if (outputVal < 0) { cool(); }
      }
      else { 
      off(); }
    }
    else { off(); }
  }
}
