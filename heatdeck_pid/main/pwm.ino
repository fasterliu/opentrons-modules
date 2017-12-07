word VentPin = 3;

void setupFanPWM() {
  pinMode(VentPin, OUTPUT);
  pwm25kHzBegin();
}

//void loopFan() {
//  pwmDuty(19); // 25% (range = 0-79 = 1.25-100%)
//  delay(10000);
//  pwmDuty(39); // 50% (range = 0-79 = 1.25-100%)
//  delay (10000);
//  pwmDuty(79); // 75% (range = 0-79 = 1.25-100%)
//  delay (10000);
//  pwmDuty(59); // 75% (range = 0-79 = 1.25-100%)
//  delay (10000);
//}

void pwm25kHzBegin() {
  TCCR2A = 0;                               // TC2 Control Register A
  TCCR2B = 0;                               // TC2 Control Register B
  TIMSK2 = 0;                               // TC2 Interrupt Mask Register
  TIFR2 = 0;                                // TC2 Interrupt Flag Register
  TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);  // OC2B cleared/set on match when up/down counting, fast PWM
  TCCR2B |= (1 << WGM22) | (1 << CS21);     // prescaler 8
  OCR2A = 79;                               // TOP overflow value (Hz)
  OCR2B = 0;
}

void fanPwmDuty(byte ocrb) {
  OCR2B = ocrb;                             // PWM Width (duty)
}
