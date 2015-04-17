// Use to send temperature and humidty measurements from a DHT11 sensor
// using a RFBee in a Grove Carrier

// Note DHT11 sensor needs to be connected to D5/D6 grove connector

// DHT11 Grove
// http://www.seeedstudio.com/wiki/Grove-_Temperature_and_Humidity_Sensor

// XBee Carrier
// http://www.seeedstudio.com/wiki/index.php?title=Bee_Stem
// Pins
// When using an RFBee, the following pinouts apply for using the arduino IDE
// Pin 5 is the Grove connector for I/O - Yellow wire
// Pin 6 is the Grove connector for I/O - White wire
// Note: you can use the x2 Grove cable with the white and yellow wires swapped on one to access both I/O.
// Pin 16 may need to be driven low to provide enough power to the I/O Grove [via mosfet]
// Pin 17 may need to be driven low to provide enough power to the I2C Grove [via mosfet]

// RfBee
// http://www.seeedstudio.com/wiki/RFbee_V1.1_-_Wireless_Arduino_compatible_node
// Pins
// GDO0          2  defined in RFBeeSendRev.cpp
// SCK_PIN   	13  defined in RFBeeSpi.h
// MISO_PIN  	12  defined in RFBeeSpi.h
// MOSI_PIN  	11  defined in RFBeeSpi.h
// SS_PIN    	10  defined in RFBeeSpi.h



// Compile using
// Arduino Pro or Pro Mini (3.3v, 8MHz) w/ATmega168


#define DEPLOYMENT false


#if DEPLOYMENT
#include "deploy.h"
#else
#include "test.h"
#endif


#include <Arduino.h>

#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>

// See http://www.engblaze.com/hush-little-microprocessor-avr-and-arduino-sleep-mode-basics/
// for information about sleep

void sleepNow()
{
  // Choose our preferred sleep mode:
  set_sleep_mode(SLEEP_MODE_IDLE);

  // Set sleep enable (SE) bit:
  sleep_enable();

  // Put the device to sleep:
  sleep_mode();

  // Upon waking up, sketch continues from this point.
  sleep_disable();
}



#if ENABLE_WATCHDOG_TIMER
// Source:

#include <avr/wdt.h>

// Function Pototype
void wdt_init(void) __attribute__((naked)) __attribute__((section(".init3")));


// Function Implementation
void wdt_init(void)
{
  MCUSR = 0;
  wdt_disable();

  return;
}

#endif

#if ENABLE_VOLTAGE_MEASUREMENT
long Vcc = 0;

// Source: https://code.google.com/p/tinkerit/wiki/SecretVoltmeter
// Note: The voltage is returned in millivolts. So 5000 is 5V, 3300 is 3.3V
long readVcc()
{
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA,ADSC));
  result = ADCL;
  result |= ADCH<<8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}
#endif

#if ENABLE_MEMORY_REPORTING

#include <MemoryFree.h>

#if ENABLE_SERIAL
void printFreeMemory()
{
  Serial.print(F("freeMemory()= "));
  Serial.println(freeMemory());
}
#endif

#endif


#include <EEPROM.h>
#include <RFBeeSendRev.h>
#include <RFBeeCore.h>
//#include <Sleep_n0m1.h>

#define power_adc_enable()      (PRR &= (uint8_t)~(1 << PRADC))
#define power_adc_disable()     (PRR |= (uint8_t)(1 << PRADC))

#define power_spi_enable()      (PRR &= (uint8_t)~(1 << PRSPI))
#define power_spi_disable()     (PRR |= (uint8_t)(1 << PRSPI))

#define power_usart0_enable()   (PRR &= (uint8_t)~(1 << PRUSART0))
#define power_usart0_disable()  (PRR |= (uint8_t)(1 << PRUSART0))

#define power_timer0_enable()   (PRR &= (uint8_t)~(1 << PRTIM0))
#define power_timer0_disable()  (PRR |= (uint8_t)(1 << PRTIM0))

#define power_timer1_enable()   (PRR &= (uint8_t)~(1 << PRTIM1))
#define power_timer1_disable()  (PRR |= (uint8_t)(1 << PRTIM1))

#define power_timer2_enable()   (PRR &= (uint8_t)~(1 << PRTIM2))
#define power_timer2_disable()  (PRR |= (uint8_t)(1 << PRTIM2))

#define power_twi_enable()      (PRR &= (uint8_t)~(1 << PRTWI))
#define power_twi_disable()     (PRR |= (uint8_t)(1 << PRTWI))

void rfBeeSendData();



#if ENABLE_DHT11
#include <dht11.h>
#include "tempHumidity.h"
#endif

#if ENABLE_MOISTURE
#include "moisture.h"
#endif


#include "data.h"

void takeMeansurements ()
{
#if ENABLE_MOISTURE
  moisture_level = getSoilMoistureLevel();
#endif

#if ENABLE_DHT11
  dht11Reading();
#endif

#if ENABLE_VOLTAGE_MEASUREMENT
  Vcc = readVcc();
#endif
}



/*--------------------------------------------------------------------------------------
 setup()
 Called by the Arduino framework once, before the main loop begins
 --------------------------------------------------------------------------------------*/
void setup()
{

#if ENABLE_WATCHDOG_TIMER
  wdt_init();
#endif

#if ENABLE_SERIAL && ENABLE_MEMORY_REPORTING
  Serial.println(F("Begin setup()"));
  printFreeMemory();
#endif

  RFBEE.init();
  Serial.begin(38400);

  power_adc_disable();
  power_timer1_disable();
  power_timer2_disable();
  power_twi_disable();

#if ENABLE_SERIAL
  Serial.println(F("RFBee Grove Carrier Remote Module"));
  Serial.print(F("Running in deployment mode: "));
  Serial.println(DEPLOYMENT_TYPE);
#if ENABLE_DHT11
  Serial.print(F("DHT11 library version "));
  Serial.println(DHT11LIB_VERSION);
#endif
  Serial.print(F("Measurement interval set to "));
  Serial.print(MEASUREMENT_INTERVAL/60000);
  Serial.println(F(" minutes"));
#endif

#if ENABLE_MOISTURE
  pinMode(MOISTURE_SENSOR_PIN, INPUT);
  pinMode(DIVIDER_TOP, OUTPUT);
  pinMode(DIVIDER_BOTTOM, OUTPUT);
#endif

#if ENABLE_SERIAL && ENABLE_VOLTAGE_MEASUREMENT
  Serial.print(F("Vcc voltage: "));
  Serial.println(readVcc(), DEC);
#endif

#if ENABLE_WATCHDOG_TIMER
  wdt_enable (WDTO_4S);  // reset after four seconds, if no "pat the dog" received
#endif

#if ENABLE_SERIAL && ENABLE_MEMORY_REPORTING
  Serial.println(F("End setup()"));
  printFreeMemory();
#endif

}    // end setup()


/*--------------------------------------------------------------------------------------
 loop()
 Arduino main loop
 --------------------------------------------------------------------------------------*/
void loop()
{

#if ENABLE_WATCHDOG_TIMER
  wdt_reset ();  // give me another second to do stuff (pat the dog)
#endif

  // at beginning of loop get time program has been running
  unsigned long runTime = millis();

  // Check to see if it is time to get another temperature reading
  if ( runTime - measurementPrev > MEASUREMENT_INTERVAL )
  {
    measurementPrev = runTime;
    measurementCount++;

#if ENABLE_SERIAL
    Serial.print(F("\n\n"));
    Serial.print(F("Measurement "));
    Serial.print(measurementCount);
    Serial.print(F(" at "));
    Serial.print((long)runTime/1000);
    Serial.print(F(" seconds"));
    Serial.println();
#endif

    takeMeansurements();

    // create message string
    createMessageStr();

    // Send data via RFBee
    rfBeeSendData();

#if ENABLE_SERIAL && ENABLE_MEMORY_REPORTING
    Serial.println(F("End loop()"));
    printFreeMemory();
#endif

  } // end if ( runTime - measurementPrev > measurementInterval )

}  // end loop()


