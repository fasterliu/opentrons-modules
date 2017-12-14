#define THERMISTORNOMINAL 10000
#define THERMISTORPIN A0
#define TEMPERATURENOMINAL 25
double BCOEFFICIENT = 3250;
#define SERIESRESISTOR 10000

long lastSend;


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

  if(tempSending && 1000 < (millis()-lastSend) ) {
//    Serial.print("output: ");
//    Serial.println(outputVal);
//    Serial.print("fan output: ");
//    Serial.println(fanOutput);
    lastSend = millis();
    Serial.print(outputVal + setPoint);
    Serial.print(',');
    Serial.print(setPoint);
    Serial.print(',');
    Serial.println(steinhart);
    
    
  }
  return steinhart;

}
