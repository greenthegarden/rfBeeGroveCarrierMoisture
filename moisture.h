#ifndef MOISTURE_H_
#define MOISTURE_H_


// global variable for reading
int       moisture_level      = 0;

// analogue pin for measurement
const int MOISTURE_SENSOR_PIN = A5;

// digital pins for sensor potential
const int DIVIDER_TOP         = 7;
const int DIVIDER_BOTTOM      = 8;

// Calibration parameters
// value at pin MOISTURE_PIN when open circuit (no moisture)
const int NO_MOISTURE         = 0;
// value at pin MOISTURE_PIN when sensor submerged in water (full moisture)
const int FULL_MOISTURE       = 990;

// time for capacitance to settle (milliseconds)
const int CAP_TIME            = 1000;


void setSensorPolarity(boolean flip)
{
  if(flip)
  {
    digitalWrite(DIVIDER_TOP, HIGH);
    digitalWrite(DIVIDER_BOTTOM, LOW);
  }
  else
  {
    digitalWrite(DIVIDER_TOP, LOW);
    digitalWrite(DIVIDER_BOTTOM, HIGH);
  }
}


int getSoilMoistureLevel()
{
  int reading;        // a value between 0 and 1023

  // enable analogue to digital reading
  power_adc_enable();

  // from http://gardenbot.org/howTo/soilMoisture/
  setSensorPolarity(true);
  delay(CAP_TIME);
  int val1 = analogRead(MOISTURE_SENSOR_PIN);
#if ENABLE_SERIAL
  Serial.print(F("val1: "));
  Serial.println(val1);
#endif
  delay(CAP_TIME);  
  setSensorPolarity(false);
  delay(CAP_TIME);
  // invert the reading
  int val2 = 1023 - analogRead(MOISTURE_SENSOR_PIN);
#if ENABLE_SERIAL
  Serial.print(F("val2: "));
  Serial.println(val2);
#endif

  // stop the current
  digitalWrite(DIVIDER_TOP,LOW);
  digitalWrite(DIVIDER_BOTTOM,LOW);

  // disable analogue to digital reading
  power_adc_disable();

  reading = (val1 + val2) / 2;

#if ENABLE_SERIAL
  Serial.print(F("Soil moisture raw reading is "));
  Serial.println(reading);
#endif

  return reading;
}


#endif
