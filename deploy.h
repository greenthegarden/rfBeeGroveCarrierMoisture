#ifndef DEPLOY_H_
#define DEPLOY_H_


#include <avr/pgmspace.h>


#define ENABLE_WATCHDOG_TIMER        true
#define ENABLE_VOLTAGE_MEASUREMENT   false

#define ENABLE_SERIAL                false
#define ENABLE_MEMORY_REPORTING      false

#define ENABLE_RFBEE                 true

#define ENABLE_DHT11                 true
#define ENABLE_SEND_DHT11_CHK        true

#define ENABLE_MOISTURE              false  // true

#define SEND_EXTENDED_HEADER         true
#define SEND_DUPLICATE_DATA          false

#define ALLOW_RECEIVE                false


const unsigned long MEASUREMENT_INTERVAL    = 300000;    // take a measurement every 5 minutes


static const char DEPLOYMENT_TYPE[] = "DEPLOYMENT";

#endif

