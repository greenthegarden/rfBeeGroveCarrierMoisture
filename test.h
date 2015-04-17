#ifndef TEST_H_
#define TEST_H_


#include <avr/pgmspace.h>


#define ENABLE_WATCHDOG_TIMER        true
#define ENABLE_VOLTAGE_MEASUREMENT   false  // false

#define ENABLE_SERIAL                true
#define ENABLE_MEMORY_REPORTING      true

#define ENABLE_RFBEE                 true

#define ENABLE_DHT11                 true  // false
#define ENABLE_SEND_DHT11_CHK        true  // false

#define ENABLE_MOISTURE              false

#define SEND_EXTENDED_HEADER         true
#define SEND_DUPLICATE_DATA          false

#define ALLOW_RECEIVE                false


//const unsigned long MEASUREMENT_INTERVAL = 30000;    // take a measurement every 30 seconds
const unsigned long MEASUREMENT_INTERVAL = 60000;    // take a measurement every 1 minute
//const unsigned long MEASUREMENT_INTERVAL = 120000;    // take a measurement every 2 minutes
//const unsigned long MEASUREMENT_INTERVAL = 300000;    // take a measurement every 5 minutes

//const char          DEPLOYMENT_TYPE[]    = "TEST";
static const char DEPLOYMENT_TYPE[]     = "TEST";

#endif

