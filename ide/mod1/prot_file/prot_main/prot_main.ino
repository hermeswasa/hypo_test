/*Descriptions*********************************
***********************************************
Motor Pins:       D5, D6, 5V, GND
Motor Function:   drv(speed);
speed must be 100(low) to 255(high).

SSR Names:        ssr1: (Pin8)   ssr2: (Pin9)   ssr3: (Pin10)
SSR Function:     ssr(n)(0(off) or 1(on);
SSR Ref:          ssr1: LED      ssr2: Aero     ssr3: Pump


Error Codes
1: SSR1 ERROR
2: SSR2 ERROR
3: SSR3 ERROR

***********************************************
***********************************************/
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




////////////////////////////////////////////////
void setup() {
  Serial.begin(9600);
  //SSR Setup
  pinMode(8,  OUTPUT);
  pinMode(9,  OUTPUT);
  pinMode(10, OUTPUT);
  //SD18B20 Setup
  sensors.begin();
  //AM2023 Setup
  Wire.begin(); 
  
  
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

  if(w_lev==0){
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
    ssr1(0);
    ssr2(1);
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

  Serial.println();

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
  
  analogWrite(6,spd);
  analogWrite(5,0);
}

///////////////////////////////////////////
///////////////////////////////////////////
void ssr1(int sw){
  if(sw==0){
    digitalWrite(8,LOW);
  }else{
    digitalWrite(8,HIGH);
  }
}
void ssr2(int sw){
  if(sw==0){
    digitalWrite(9,LOW);
  }else{
    digitalWrite(9,HIGH);
  }
}
void ssr3(int sw){
  if(sw==0){
    digitalWrite(10,LOW);
  }else{
    digitalWrite(10,HIGH);
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

  if(waterLevel > 800){
    Serial.println("Water Level: OK(" + String(waterLevel) + ")");
    state = 1;
  }
  
  if(waterLevel < 500) {
    Serial.println("Water Level: Empty(" + String(waterLevel) + ")");
    state = 0;
  }
  return state;
}
