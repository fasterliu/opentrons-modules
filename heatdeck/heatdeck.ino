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



double target_temperature = 50;




// which analog pin to connect
#define THERMISTORPIN A0         
// resistance at 25 degrees C
#define THERMISTORNOMINAL 10000      
// temp. for nominal resistance (almost always 25 C)
#define TEMPERATURENOMINAL 25
// how many samples to take and average, more takes longer
// but is more 'smooth'
#define NUMSAMPLES 100
// The beta coefficient of the thermistor (usually 3000-4000)
double BCOEFFICIENT = 3250;
// the value of the 'other' resistor
#define SERIESRESISTOR 4700    
 
int samples[NUMSAMPLES];
float temperatures[NUMSAMPLES];
float prev_temperatures[NUMSAMPLES];
unsigned long timestamp_current = 0;
unsigned long timestamp_prev = 0;

float movement_samples[NUMSAMPLES] = {0,};

float prev_c_per_min = 0;
 
void setup(void) {
  Serial.begin(115200);
  Serial.setTimeout(30);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);

  read_temperatures_to_buffer();
  for (int i=0; i< NUMSAMPLES; i++) {
    prev_temperatures[i] = temperatures[i];
  }
  get_average_temperature();
  get_average_movement();
  is_moving_toward();
  get_average_temperature();
  is_moving_toward;
}
 
void loop(void) {
  check_serial();
  update_heat();
}

void check_serial(){
  if (Serial.available()>0){
    int val = Serial.parseInt();
    if (val > 0){
      target_temperature = val;
    }
  }
}

void update_heat(){
  read_temperatures_to_buffer();
  float avg = get_average_temperature();
  float mov = get_average_movement();
  Serial.print(avg);Serial.print('\t');Serial.print(int(mov));
  Serial.print('\t');Serial.println(is_moving_toward());
  if (avg < target_temperature){
    digitalWrite(10, LOW);
  }
  else {
    digitalWrite(10, HIGH);
  }
}

void read_temperatures_to_buffer() {
  // take N samples in a row
  float sample;
  timestamp_prev = timestamp_current;
  timestamp_current = millis();
  for (int i=0; i< NUMSAMPLES; i++) {
    samples[i] = 1023 - analogRead(THERMISTORPIN);
  }
  // convert to temperature
  for (int i=0; i< NUMSAMPLES; i++) {
    sample = float(samples[i]);
    // convert the value to resistance
    sample = 1023 / sample - 1;
    sample = SERIESRESISTOR / sample;

    sample = sample / THERMISTORNOMINAL;     // (R/Ro)
    sample = log(sample);                  // ln(R/Ro)
    sample /= BCOEFFICIENT;                   // 1/B * ln(R/Ro)
    sample += 1.0 / (TEMPERATURENOMINAL + 273.15); // + (1/To)
    sample = 1.0 / sample;                 // Invert
    sample -= 273.15;                         // convert to C

    // scaling to get better accuracy from Heat Deck
    sample = ((sample - TEMPERATURENOMINAL) * 1.05) + TEMPERATURENOMINAL;

    prev_temperatures[i] = temperatures[i];
    temperatures[i] = sample;
  }
}

float get_average_temperature(){
  float average = 0;
  for (int i=0; i< NUMSAMPLES; i++) {
    average += temperatures[i];
  }
  return average / NUMSAMPLES;
}

float get_average_movement(){
  float average = 0;
  for (int i=0; i< NUMSAMPLES; i++) {
    average += temperatures[i] - prev_temperatures[i];
  }
  float time_difference = timestamp_current - timestamp_prev;
  average = average / NUMSAMPLES;
  float c_per_min = (1.0 / time_difference) * average * 60.0 * 1000.0;
  return c_per_min;
}

boolean is_moving_toward(){

  float move_avg = get_average_movement();

  if (int(move_avg) == 0){
    return true;
  }
  if (temp_above() && move_avg > 0){
    return true;
  }
  else if (!temp_above() && move_avg < 0){
    return true;
  }
  else {
    return false;
  }
}

boolean temp_above(){
  return (target_temperature - get_average_temperature()) > 0;
}

