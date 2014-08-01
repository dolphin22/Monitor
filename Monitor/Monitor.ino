/*
Temperature, Humidity and Wind Speed Monitor

to collecte data and send it back to people for monitoring & preventing purposes.  

Hardward & Pins

* DHT22 Setup *
DHT22    Arduino
VCC      +3v3
DATA     D2
N.C      -
GND      GND

* Wind Sensor Setup *
Wind Sensor    Arduino
GND            GND
+V             +5
RV             A1
TMP            A0

References:
o DHT22 - http://playground.arduino.cc//Main/DHTLib
o Wind Sensor - https://github.com/moderndevice/Wind_Sensor

*/
#include <dht.h>

dht DHT;  // dht type

#define DHT22_PIN 2  // DHT22 data signal pin 2

#define RV_PIN 1  // Wind Sensor RV pin
#define TMP_PIN 0  // Wind Sensor TMP pin

int check;  // check for DHT22 sensor error

const float zeroWindAdjustment =  .2; // negative numbers yield smaller wind speeds and vice versa.
float zeroWind_ADunits;  // Zero wind speed for calibration
float zeroWind_volts;
int TMP_Therm_ADunits;  // Temperature termistor value from wind sensor
float RV_Wind_ADunits;    // RV output from wind sensor 
float RV_Wind_Volts;
float WindSpeed_MPH;  // Wind speed MPH

void setup()
{
    Serial.begin(57600);
    Serial.println("Temperature, Humidity & Wind Speed Monitor");
    Serial.println("--------------------------------------------");
    Serial.println("Status, Humidity (%), Temperature (C), TMP (V), RV (V), ZeroWind (V), Wind Speed (MPH)");
}

void loop()
{
    // DHT22 Sensor
    int check = DHT.read22(DHT22_PIN);

    // Check for sensor error
    switch (check)
    {
    case DHTLIB_OK:
        Serial.print("OK,\t");
        break;
    case DHTLIB_ERROR_CHECKSUM:
        Serial.print("Checksum error,\t");
        break;
    case DHTLIB_ERROR_TIMEOUT:
        Serial.print("Time out error,\t");
        break;
    default:
        Serial.print("Unknown error,\t");
        break;
    }
    
    // Output data to serial
    Serial.print(DHT.humidity, 1);
    Serial.print(",\t\t");
    Serial.print(DHT.temperature, 1);
    Serial.print(",\t\t");
    
    // Wind Sensor
    TMP_Therm_ADunits = analogRead(TMP_PIN);  // read TMP pin value from the sensor
    // these are all derived from regressions from raw data as such they depend on a lot of experimental factors
    // such as accuracy of temp sensors, and voltage at the actual wind sensor, (wire losses) which were unaccouted for.
    //TempCtimes100 = (0.005 *((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits)) - (16.862 * (float)TMP_Therm_ADunits) + 9075.4;  
    zeroWind_ADunits = -0.0006*((float)TMP_Therm_ADunits * (float)TMP_Therm_ADunits) + 1.0727 * (float)TMP_Therm_ADunits + 47.172;  // calibration zero wind speed
    zeroWind_volts = (zeroWind_ADunits * 0.0048828125) - zeroWindAdjustment;  // display value in Voltages
    
    RV_Wind_ADunits = analogRead(RV_PIN);  // read RV pin value from the sensor
    RV_Wind_Volts = (RV_Wind_ADunits *  0.0048828125);  // display value in voltages

    WindSpeed_MPH =  pow(((RV_Wind_Volts - zeroWind_volts) /.2300) , 2.7265);  // calculate wind speed
    
    // Output data to serial
    Serial.print(TMP_Therm_ADunits * 0.0048828125);
    Serial.print(",\t");
    Serial.print((float)RV_Wind_Volts);
    Serial.print(",\t");
    Serial.print(zeroWind_volts);
    Serial.print(",\t\t");
    Serial.print((float)WindSpeed_MPH);
    Serial.println();
    
    delay(2000);
}
