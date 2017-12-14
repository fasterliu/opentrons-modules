#define FAN_PIN 8


void heat() {
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
}

void cool() {
  digitalWrite(FAN_PIN, HIGH); // turn fans on
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
}


void off() {
  digitalWrite(FAN_PIN, LOW); // turn fans off
  digitalWrite(9, LOW);
  digitalWrite(10, HIGH);
}
