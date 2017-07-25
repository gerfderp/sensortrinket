#include <Wire.h>
#include "Adafruit_LiquidCrystal.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
#include <SoftwareSerial.h>
#endif
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
//#include "Adafruit_BLEGatt.h"
#include "BluefruitConfig.h"


#include <OneWire.h>
#include <DallasTemperature.h>

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, 7, BLUEFRUIT_SPI_RST);
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);
//Adafruit_BLEGatt gatt(ble);
Adafruit_LiquidCrystal lcd(0);

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 3
#define TEMPERATURE_PRECISION 9

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress oneThermometer, twoThermometer, threeThermometer;
//
//#define TEST_STRING     "01234567899876543210"
//int32_t htsServiceId;
//int32_t htsMeasureCharId;
/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */

  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
  lcd.begin(16, 2);
  /* Initialise the sensor */
  if (!tsl.begin())
  {
    /* There was a problem detecting the ADXL345 ... check your connections */
    while (1);
  }
  boolean success;

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  if (! ble.factoryReset() ) {
    error(F("Couldn't factory reset"));
  }
  /* Disable command echo from Bluefruit */
  ble.echo(false);
  //Serial.println("Requesting Bluefruit info:");
  //  /* Print Bluefruit information */
  ble.info();
   ble.setMode(BLUEFRUIT_MODE_DATA);
 ble.verbose(false);
//  ble.reset();


  /* Display some basic information on this sensor */
  //  displaySensorDetails();

  /* Setup the sensor gain and integration time */
  configureSensor();


  //  Serial.println();
  sensors.begin();

  if (!sensors.getAddress(oneThermometer, 0)) Serial.println("Unable to find address for Device 0");
  if (!sensors.getAddress(twoThermometer, 1)) Serial.println("Unable to find address for Device 1");
  if (!sensors.getAddress(threeThermometer, 2)) Serial.println("Unable to find address for Device 1");
  sensors.setResolution(oneThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(twoThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(threeThermometer, TEMPERATURE_PRECISION);
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)
{
  String data = "";
  String str;
  /* Get a new sensor event */
  sensors_event_t event;
  tsl.getEvent(&event);
  //  ble.print("AT+BLEUARTTX=");
  //    ble.println("loop");
  /* Display the results (light is measured in lux) */
  int light;
  if (event.light)
  {
    //    Serial.print(event.light); Serial.println(" lux");
    //    ble.print("AT+BLEUARTTX=");
    //    ble.println(event.light);
    //    ble.print("lux");
    light = int(event.light);
    char buff[5];
    sprintf(buff, "L:%5d", light);
//    data += light;
//    data += 'lux: ';
    String str(buff);
    data += str;
    lcd.setCursor(0, 1);
    // print the number of seconds since reset:
    lcd.print(buff);

    //  lcd.setCursor(0,0);
    //  lcd.print(event.light);
  }
  else
  {
    /* If event.light = 0 lux the sensor is probably saturated
       and no reliable data could be generated! */
    //    Serial.println("Sensor overload");
    //    ble.print("AT+BLEUARTTX=");
    //    ble.println("no data");
  }
  sensors.requestTemperatures();

  int one = int(DallasTemperature::toFahrenheit(sensors.getTempC(oneThermometer)));
  char buff[3];
  sprintf(buff, "1:%3d", one);
  lcd.setCursor(0, 0);
  lcd.print(buff);
  String st1(buff);
    data += st1;
  int two = int(DallasTemperature::toFahrenheit(sensors.getTempC(twoThermometer)));

  sprintf(buff, "2:%3d", two);
  lcd.setCursor(6, 0);
  lcd.print(buff);
  String st2(buff);
    data += st2;
  int three = int(DallasTemperature::toFahrenheit(sensors.getTempC(threeThermometer)));

  sprintf(buff, "3:%2d", three);
  lcd.setCursor(12, 0);
  lcd.print(buff);
  String st3(buff);
    data += st3;

   if (ble.isConnected()){
     
     ble.print(data);
   }

  delay(1000);
}
void error(const __FlashStringHelper*err) {
  lcd.setCursor(0, 0);
  lcd.print(err);

  while (1);
}


