#ifndef TEMP_HUMIDITY_H_
#define TEMP_HUMIDITY_H_


#include <dht11.h>

// glabal variables
dht11     DHT11;
const int DHT11_PIN = 5;
int       chk;

void dht11Reading()
{
  // read DHT11 data
  chk = DHT11.read(DHT11_PIN);

#if ENABLE_SERIAL
  Serial.print(F("DHT reading from pin "));
  Serial.println(DHT11_PIN);
  Serial.print(F("DHT status: "));
  switch (chk)
  {
  case  0:  
    Serial.println(F("OK"));
    break;
  case -1: 
    Serial.println(F("Checksum error"));
    break;
  case -2: 
    Serial.println(F("Time out error"));
    break;
  default: 
    Serial.println(F("Unknown error"));
    break;
  }
#endif

#if ENABLE_SERIAL
  Serial.print(F("Temperature (oC): "));
  Serial.println((int)DHT11.temperature);

  Serial.print(F("Humidity (%): "));
  Serial.println((int)DHT11.humidity);
#endif
}


#endif

