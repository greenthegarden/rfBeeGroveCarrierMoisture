#ifndef DATA_H_
#define DATA_H_


#include <avr/pgmspace.h>


//  data format
// $ , M , x x x \r \n
// 0 0 0 0 0 0 0 0  0  1 1 1 1 1 1  1
// 1 2 3 4 5 6 7 8  9  0 1 2 3 4 5  6

const char         DATA_PREFIX[]        = "$";
const char         DATA_SEPARATOR[]     = ",";
const char         SYSTEM_REF[]         = "M";

//PROGMEM const char DATA_PREFIX[]        = "$";
//PROGMEM const char DATA_SEPARATOR[]     = ",";
//PROGMEM const char SYSTEM_REF[]         = "M";

const unsigned int FLOAT_DECIMAL_PLACES = 1;
const unsigned int MAX_DATA_LENGTH      = 32;
const unsigned int MAX_HEADER_LENGTH    = 20;

static const int   ERROR_READING        = -1;
static const int   NO_READING           = -2;

static unsigned long measurementPrev    = 0;
static unsigned long measurementCount   = 0;


static char messageStr[MAX_HEADER_LENGTH + MAX_DATA_LENGTH + 1] = "";


void createMessageStr()
{
  
#if ENABLE_WATCHDOG_TIMER
  wdt_reset ();
#endif
  
  // reset messageStr
  messageStr[0] = '\0';

  // create a temporary character buffer
  char buf[12];
  
  // Construct data string
  char dataStr[MAX_DATA_LENGTH];
  dataStr[0] = '\0';

  int dataFieldCount = 0;

#if ENABLE_DHT11
#if ENABLE_SEND_DHT11_CHK
  strcat(dataStr, itoa(chk, buf, 10));
  strcat(dataStr, DATA_SEPARATOR);
  dataFieldCount++;
#endif
  if (chk == 0) {
    Serial.print("DHT11.temperature = ");
    Serial.println(DHT11.temperature);
    strcat(dataStr, itoa((int)DHT11.temperature, buf, 10));
    strcat(dataStr, DATA_SEPARATOR);
    dataFieldCount++;
    Serial.print("DHT11.humidity = ");
    Serial.println(DHT11.humidity);
    strcat(dataStr, itoa((int)DHT11.humidity, buf, 10));
    strcat(dataStr, DATA_SEPARATOR);
    dataFieldCount++;
  }
  else {    // deal with an error in DHT reading
    strcat(dataStr, itoa(ERROR_READING, buf, 10));    // send ERROR_READING for temperature
    strcat(dataStr, DATA_SEPARATOR);
    dataFieldCount++;
    strcat(dataStr, itoa(ERROR_READING, buf, 10));    // send ERROR_READING for humidity
    strcat(dataStr, DATA_SEPARATOR);
    dataFieldCount++;
  }
#endif
#if ENABLE_MOISTURE
  strcat(dataStr, itoa(moisture_level, buf, 10));
#else
  strcat(dataStr, itoa(NO_READING, buf, 10));
#endif
  strcat(dataStr, DATA_SEPARATOR);
  dataFieldCount++;
#if ENABLE_VOLTAGE_MEASUREMENT
  strcat(dataStr, itoa(Vcc, buf, 10));
#else
  strcat(dataStr, itoa(NO_READING, buf, 10));
#endif
  dataFieldCount++;

  strcat(messageStr, DATA_PREFIX);
  strcat(messageStr, DATA_SEPARATOR);
  strcat(messageStr, SYSTEM_REF);
  strcat(messageStr, DATA_SEPARATOR);
#if SEND_EXTENDED_HEADER
  strcat(messageStr, itoa((int)(MEASUREMENT_INTERVAL/1000), buf, 10));
  strcat(messageStr, DATA_SEPARATOR);
  strcat(messageStr, itoa(dataFieldCount, buf, 10));
  strcat(messageStr, DATA_SEPARATOR);
  strcat(messageStr, itoa(strlen(dataStr), buf, 10));
  strcat(messageStr, DATA_SEPARATOR);
#endif


#if ENABLE_SERIAL
  Serial.print(F("heasderStr is "));
  Serial.println(messageStr);
  Serial.print(F("heasderStr length is "));
  Serial.println(strlen(messageStr));
#endif

  // add dataStr to header info
  strcat(messageStr, dataStr);
  // add a carriage return
  strcat(messageStr, "\n");

#if ENABLE_SERIAL
  Serial.print(F("dataStr is "));
  Serial.println(dataStr);
  Serial.print(F("dataStr length is "));
  Serial.println(strlen(dataStr));
  
  Serial.print(F("messageStr is "));
  Serial.println(messageStr);
  Serial.print(F("messageStr length is "));
  Serial.println(strlen(messageStr));
#endif

}


void rfBeeSendData()
{
  
#if ENABLE_WATCHDOG_TIMER
  wdt_reset ();
#endif
  
  RFBEE.sendDta(strlen(messageStr), (unsigned char *)messageStr);
  
#if ENABLE_SERIAL
  Serial.println(F("Data sent by RFBee"));
#endif

#if ENABLE_WATCHDOG_TIMER
  wdt_reset ();
#endif

#if SEND_DUPLICATE_DATA
  delay(2000);          // delay to allow all data to be sent
  
#if ENABLE_WATCHDOG_TIMER
  wdt_reset ();
#endif

  RFBEE.sendDta(strlen(messageStr), (unsigned char *)messageStr);  

#if ENABLE_SERIAL
  Serial.println(F("Duplicate data sent by RFBee"));
#endif

#endif


}


#endif

