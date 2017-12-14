#include <PID_v1.h>

#define OUTPUT_MIN -1
#define OUTPUT_MAX 1
#define KP 0.6
#define KI .0005
#define KD .01

#define SHUTDOWN_TEMP 0 //This would not work for a module that can reach 0 degrees
bool tempSending = false;
int lastSend = 0;

int PRINT_FREQ_MILLI = 1000;

double temperature, setPoint, outputVal, fanOutput;
PID myPID(&temperature, &outputVal, &setPoint, KP, KI, KD,  DIRECT);


void setup_pins() {
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
}

void print_temp(double temp){
	if(tempSending && PRINT_FREQ_MILLI < (millis()-lastSend) ) {
		Serial.print("Current temperature: ");
    Serial.println(temp);
    lastSend = millis();
  }
}

void setup() {
  Serial.begin(9600);
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(-1, 1);
	setup_pins();
	setPoint = 0; //Module turned off at start
  setupFanPWM();
}

void loop() {
  checkCommand();
  temperature = read_temp();
	print_temp(temperature);

	if(setPoint > SHUTDOWN_TEMP)
	{
		myPID.Compute();
		float ran = random(0, 100)/(float)100;
		fanOutput = (39 + (-outputVal * 30)); // (total range = 0-79 = 1.25-100%)
		fanPwmDuty(fanOutput);


		//hacky pmw below - use valid pwm values with high current transistors
		if (ran < abs(outputVal))
		{
			if (outputVal > 0) { heat(); }
			else if (outputVal < 0) { cool(); }
		}
		else {
		off(); }
	}
	else { off(); }
}
