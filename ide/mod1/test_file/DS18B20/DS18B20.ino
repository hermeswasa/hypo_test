#include <OneWire.h>
#include <DallasTemperature.h>

OneWire oneWire(4);
DallasTemperature sensors(&oneWire);

DeviceAddress temp1 = {0x28,0x16,0xC8,0x56,0xB5,0x1,0x3C,0x6A};
DeviceAddress temp2 = {0x28,0x39,0x49,0x56,0xB5,0x1,0x3C,0xFE};
DeviceAddress temp3 = {0x28,0xA7,0x3B,0x56,0xB5,0x1,0x3C,0xB2};


void setup(void)
{
  Serial.begin(9600);
  sensors.begin();
}

void loop(void)
{ 
  sensors.requestTemperatures();
  Serial.print("temp1: ");
  Serial.println(sensors.getTempC(temp1));
  Serial.print("temp2: ");
  Serial.println(sensors.getTempC(temp2));
  Serial.print("temp3: ");
  Serial.println(sensors.getTempC(temp3));
  delay(1000);
}
