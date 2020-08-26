// 回転速度(35-255)
// ※値が大きいほど高速,余りにも小さい値は回転しません。
const uint8_t slow = 100;
const uint8_t norm = 150;
const uint8_t high = 250;


void setup() {
  Serial.begin(9600);
}
 
void loop() {
  // 正転(回転)
  Serial.println("Slow");
  analogWrite(6,slow);
  analogWrite(5,0);
  delay(5000);
 
  // 逆転(逆回転)
  Serial.println("Normal");
  analogWrite(6,norm);
  analogWrite(5,0);
  delay(5000);
  
  // ブレーキ
  Serial.println("High");
  analogWrite(6,high);
  analogWrite(5,0);
  delay(5000);
}
 
