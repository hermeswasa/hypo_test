int watorLevel = 0;     // holds the wator level value
int watorLevelPin = A0; // wator level sensor pin used

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9800);
}

void loop() {
  // put your main code here, to run repeatedly:
  // read wator level
  watorLevel = analogRead(watorLevelPin);
  Serial.println("Wator Level: Empty(" + String(watorLevel) + ")");
  if(watorLevel > 700) {
    Serial.println("水が空になりそうだよ (" + String(watorLevel) + ")");
  }
  delay(1000);
}
