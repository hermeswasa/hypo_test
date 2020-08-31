/*Descriptions*********************************
***********************************************
Motor Pins:       D9, D10, 5V, GND
Motor Function:   drv(speed);
speed must be 100(low) to 255(high).

SSR Names:        ssr1: (Pin7)   ssr2: (Pin8)   ssr3: (Pin11)
SSR Function:     ssr(n)(0(off) or 1(on);
SSR Ref:          ssr1: LED      ssr2: PMP     ssr3: HSN

LED Names:        LED1:(5V), LED2(D12)


Error Codes
1: SSR1 ERROR
2: SSR2 ERROR
3: SSR3 ERROR

***********************************************
***********************************************/
//Timer Settings
int w_cnt = 0;
unsigned long water_timer;
unsigned long check_timer;


//DRV8835 Settings
int spd = 0;

//DS18B20 Settings
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(4);
DallasTemperature sensors(&oneWire);
DeviceAddress wtemp1 = {0x28,0x16,0xC8,0x56,0xB5,0x1,0x3C,0x6A};
DeviceAddress wtemp2 = {0x28,0x39,0x49,0x56,0xB5,0x1,0x3C,0xFE};
DeviceAddress wtemp3 = {0x28,0xA7,0x3B,0x56,0xB5,0x1,0x3C,0xB2};
double w_tmp1 = 0, w_tmp2 = 0, w_tmp3 = 0;

//AM2320 Settings
#include <Wire.h>
#define DEV_ADR 0x5c
uint8_t data[8];
double a_hum = 0, a_tmp = 0;

//Water Level Settings
int waterLevel = 0;     // holds the wator level value
int waterLevelPin = A0; // wator level sensor pin used
int w_lev = 0;

//Water Flow Settings
volatile int flow_frequency; // Measures flow sensor pulses
unsigned int l_hour; // Calculated litres/hour
unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;

void flow () // Interrupt function
{
   flow_frequency++;
}


////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  //Timer Setup
  water_timer = millis();
  check_timer = water_timer;
  
  //SSR Setup
  pinMode(7,  OUTPUT);
  pinMode(8,  OUTPUT);
  pinMode(11, OUTPUT);
  //SD18B20 Setup
  sensors.begin();
  //AM2023 Setup
  Wire.begin(); 
  //Water Flow Setup
  pinMode(flowsensor, INPUT);
  digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
  attachInterrupt(0, flow, RISING); // Setup Interrupt
  sei(); // Enable interrupts
  currentTime = millis();
  cloopTime = currentTime;
  
}
////////////////////////////////////////////////
////////////////////////////////////////////////
 
void loop() {
  
  w_tmp1 = water_temp1();
  w_tmp2 = water_temp2();
  w_tmp3 = water_temp3();

  a_hum = air_humid();
  a_tmp = air_temp();

  w_lev = water_level();

  Serial.println();

  if(water_timer >= (check_timer + 60000)){
    spd = 0;
    drv(spd);
    ssr1(1);
    ssr2(0);
    ssr3(1);
    w_cnt++;
    Serial.println("STATUS: WATER");
    if(w_cnt > 5){
      check_timer = water_timer;
      w_cnt = 0;
    }
  }else if(w_lev==0){
    spd = 200;
    drv(spd);
    ssr1(1);
    ssr2(0);
    ssr3(0);
    Serial.println("ERROR: CHECK WATER LEVEL");
    drv_rat(spd);
  }else if(w_tmp1 > 30.0 ||w_tmp2 > 30.0 ||w_tmp3 > 30.0 ||a_tmp > 30.0 || a_hum > 70.0){
    spd = 255;
    drv(spd);
    ssr1(1);
    ssr2(1);
    ssr3(0);
    Serial.println("WARNING: HIGH TEMPERATURE");
    drv_rat(spd);  
  }else if(w_tmp1 > 32.0 ||w_tmp2 > 32.0 ||w_tmp3 > 32.0 ||a_tmp > 32.0 || a_hum > 75.0){
    spd = 255;
    drv(spd);
    ssr1(1);
    ssr2(0);
    ssr3(0); 
    Serial.println("WARNING: VERY HIGH TEMPERATURE");
    drv_rat(spd); 
  }else if(w_tmp1 <= 30.0 ||w_tmp2 <= 30.0 ||w_tmp3 <= 30.0 ||a_tmp <= 30.0 || a_hum <= 70.0){
    spd = 200;
    drv(spd);
    ssr1(1);
    ssr2(1);
    ssr3(0);
    Serial.println("STATUS: FINE");
    drv_rat(spd); 
  }else{
    spd = 0;
    drv(spd);
    ssr1(0);
    ssr2(0);
    ssr3(0);
    Serial.println("ERROR: SOMETHING IS WRONG");
    drv_rat(spd);
  }
  water_timer = millis();
  currentTime = millis();
   // Every second, calculate and print litres/hour
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_hour = (flow_frequency * 60 / 7.5); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      flow_frequency = 0; // Reset Counter
      Serial.print(l_hour, DEC); // Print litres/hour
      Serial.println(" L/hour");
   }
  Serial.print("Current Time: ");
  Serial.println(currentTime);

  delay(5000);
}
////////////////////////////////////////////////
////////////////////////////////////////////////
void drv_rat(int spd){
  int n = (double)spd*100/255;
  Serial.print("Mtr_PW: ");
  Serial.print(spd);
  Serial.print(" : ");
  Serial.print(n);
  Serial.println("/100");
}

////////////////////////////////////////////////
////////////////////////////////////////////////

void drv(int spd){
  
  analogWrite(10,spd);
  analogWrite(9,0);
}

///////////////////////////////////////////
///////////////////////////////////////////
void ssr1(int sw){
  if(sw==0){
    digitalWrite(7,LOW);
  }else{
    digitalWrite(7,HIGH);
  }
}
void ssr2(int sw){
  if(sw==0){
    digitalWrite(8,LOW);
  }else{
    digitalWrite(8,HIGH);
  }
}
void ssr3(int sw){
  if(sw==0){
    digitalWrite(11,LOW);
  }else{
    digitalWrite(11,HIGH);
  }
}
///////////////////////////////////////////
///////////////////////////////////////////
double water_temp1(){
  sensors.requestTemperatures();
  double w_tmp = sensors.getTempC(wtemp1);
  Serial.print("water_temp1: ");
  Serial.print(w_tmp);
  return w_tmp;
}
double water_temp2(){
  sensors.requestTemperatures();
  double w_tmp = sensors.getTempC(wtemp1);
  Serial.print(" water_temp2: ");
  Serial.print(w_tmp);
  return w_tmp;
}
double water_temp3(){
  sensors.requestTemperatures();
  double w_tmp = sensors.getTempC(wtemp1);
  Serial.print(" water_temp3: ");
  Serial.println(w_tmp);
  return w_tmp;
}
///////////////////////////////////////////
///////////////////////////////////////////
double air_humid(){
  float rh;
  // AM2320センサーのウェイクアップ
  Wire.beginTransmission(DEV_ADR);
  Wire.endTransmission();

  // 湿度・温度データ取得要求
  Wire.beginTransmission(DEV_ADR);
  Wire.write(0x03);         // レジスタ読み取りコマンド
  Wire.write(0x00);         // 読み取り開始レジスタ番号
  Wire.write(0x04);         // 読み取りデータ数    
  Wire.endTransmission();

  // データの取得
  Wire.requestFrom(DEV_ADR,8); 
  if (Wire.available() >= 8) {
    for (uint8_t i=0; i<8; i++) {
      data[i] = Wire.read();
    }
    
    rh = ((float)(data[2]*256+data[3]))/10; // 湿度
    
    // 湿度の表示
    Serial.print("H=");
    Serial.print(rh);
    Serial.print("%");
  }
    return rh;
}
///////////////////////////////////////////
///////////////////////////////////////////
double air_temp(){
  float tp = 0;
  // AM2320センサーのウェイクアップ
  Wire.beginTransmission(DEV_ADR);
  Wire.endTransmission();

  // 湿度・温度データ取得要求
  Wire.beginTransmission(DEV_ADR);
  Wire.write(0x03);         // レジスタ読み取りコマンド
  Wire.write(0x00);         // 読み取り開始レジスタ番号
  Wire.write(0x04);         // 読み取りデータ数    
  Wire.endTransmission();

  // データの取得
  Wire.requestFrom(DEV_ADR,8); 
  if (Wire.available() >= 8) {
    for (uint8_t i=0; i<8; i++) {
      data[i] = Wire.read();
    }
    
    tp = ((float)(data[4]*256+data[5]))/10; // 温度
    
    // 温度の表示
    Serial.print(" T=");
    Serial.print(tp);
    Serial.println("c");
  }
  return tp;
}
///////////////////////////////////////////
///////////////////////////////////////////
int water_level(){
  int state = 0;
  waterLevel = analogRead(waterLevelPin);

  if(waterLevel >= 200){
    Serial.println("Water Level: OK(" + String(waterLevel) + ")");
    state = 1;
  }
  
  else {
    Serial.println("Water Level: Empty(" + String(waterLevel) + ")");
    state = 0;
  }
  return state;
}

///////////////////////////////////////////
///////////////////////////////////////////
