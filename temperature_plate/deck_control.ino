#define FAN_PIN 8


void cool() {
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
}

void heat() {
  digitalWrite(FAN_PIN, HIGH); // turn fans on
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
}


void off() {
  digitalWrite(FAN_PIN, HIGH); // turn fans off
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
}
