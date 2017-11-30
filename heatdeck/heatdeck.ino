/*
  To set the target temperature (celsius), change the value below.
  For example, if I wanted 45 degrees C, I would set:

    const double target_temperature = 45;

  After editing the value to your desired temperature, connect
  the heat deck over USB, and upload from the Arduino IDE.
  Make sure to set the "Tools > Boards" setting to "Uno"

  You can see a printout of the currently read temperature
  by opening the Serial Monitor at baudrate 115200

  This sketch was created by Adafruit:
  https://learn.adafruit.com/thermistor/using-a-thermistor
  and modified by Opentrons to work with the Heat Deck.
*/



const double target_temperature = 50;




// which analog pin to connect
#define THERMISTORPIN A0
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 5
// The beta coefficient of the thermistor (usually 3000-4000)
double BCOEFFICIENT = 3250;
// the value of the 'other' resistor
#define SERIESRESISTOR 4700

float running_sum = 0;
int run = 0;
int samples[NUMSAMPLES];

void setup(void) {
  Serial.begin(115200);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
}

float read_temp(void) {

  uint8_t i;
  float therm_reading;
	float temp;

  // take N samples in a row, with a slight delay
	therm_reading = 1023 - analogRead(THERMISTORPIN);

  // convert the value to resistance
  temp = 1023 / therm_reading - 1;
  temp = SERIESRESISTOR / temp;

//  Serial.print("Thermistor resistance ");
//  Serial.println(temp);

  float steinhart;
  steinhart = temp / THERMISTORNOMINAL;     // (R/Ro)
  steinhart = log(steinhart);                  // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
  steinhart = 1.0 / steinhart;                 // Invert
  steinhart -= 273.15;                         // convert to C

  // scaling to get better accuracy from Heat Deck
  steinhart = ((steinhart - TEMPERATURENOMINAL) * 1.05) + TEMPERATURENOMINAL;

  if (run % 1000 == 0) {
    Serial.print("Temperature ");
    Serial.print(steinhart);
    Serial.println(" *C");
  }
  return steinhart;

}


float integral_component(float error){
  float integ_comp, perc_error;
  float integ_const = 10000;

  perc_error = error / target_temperature;
  if (abs(perc_error) < 0.20) {running_sum += error/integ_const;}
  return running_sum; 
}

//Calculate output for heating/cooling
float pid(float error) {
  float prop_const = 10;
  float deriv_const = 115200;
  float prop_comp, integ_comp, deriv_comp;
  // proportional component
	prop_comp =  prop_const*error / target_temperature;
  
  
	float output = prop_comp + integ_comp;
//	Serial.print("OUTPUT: ");
//  Serial.println(output);
	return output;

}


void heat() {
//  digitalWrite(8, HIGH); // turn fans off
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
}


void cool() {
//  digitalWrite(8, HIGH); // turn fans on
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
}

void off() {
//  digitalWrite(8, HIGH); // turn fans on
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
}



void loop(void) {
	float temp = read_temp();
	float error = target_temperature - temp;
	float pid_output = pid(error);

	float ran = random(0, 100)/(float)100;

//  Serial.print("PID OUTPUT: ");
//  Serial.print(pid_output);

	if (ran < pid_output) {
		heat();
	}
	else {
		  off();
	}
  run += 1;
}
