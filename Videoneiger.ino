int pins[] = {53, 52, 51, 50};

void setup() {
  for (int i = 0; i < 4; i++) {
    pinMode(pins[i], OUTPUT);
  }
}

void loop() {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      digitalWrite(pins[j], LOW);
    }

    digitalWrite(pins[i], HIGH);
    delay(1000);
  }
}