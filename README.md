# Ikea_Environmental_Sensor
Replacement Board for [Ikea Vindriktning PM2.5 Sensor](https://www.ikea.com/us/en/p/vindriktning-air-quality-sensor-60515911/)

## This board is meant as a complete replacement for the board rather than adding in a new board. 

## This board will add the following:

- ESP32-C3 processor with onboard antenna
- [Sensirion SGP40 VOC Sensor](https://www.sensirion.com/en/environmental-sensors/gas-sensors/sgp40/)
  - ~~SGP30 also works~~   SGP30 is the same package and pinout but SGP30 is a 1.8V part!
- [TI HDC1080DMBR Temp/Humidity Sensor](https://www.ti.com/store/ti/en/p/product/?p=HDC1080DMBR)
  - Same package and pinout as the [Sensirion SHT21](https://www.sensirion.com/en/environmental-sensors/humidity-sensors/humidity-temperature-sensor-sht2x-digital-i2c-accurate/)
  - The Sensirion SHT20, SHT21, and SHT25 have increasing accuracy and are pin/package compatible
  - All the the SHT2x parts have the same I2C address of 0x40 which is the same as TI HDC1080DMBR
- [QWiiC Connector](https://www.jst-mfg.com/product/pdf/eng/eSR.pdf) 1mm pitch standard I2C connector
- [Everlight ALS-PT19 Light Sensor](https://en.everlight.com/sensor/category-ambient_light_sensor/analog_ambient_light_sensor/)
- The PM2.5 Sensor has a fan with a 2-pin power connector on it. After testing many different samples the [CAX 1.25-2A] (https://www.lcsc.com/product-detail/Wire-To-Board-Wire-To-Wire-Connector_CAX-1-25-2A_C722594.html) has a good fit.
  - The [CAX 1.25T-2A] (https://www.lcsc.com/product-detail/Wire-To-Board-Wire-To-Wire-Connector_CAX-1-25T-2A_C722633.html) looks and fits the same. Not sure what the difference is.
  - The CAX parts seem to maybe be a knock off of the [BooM Precision Electric 1.25T-2A] (https://www.lcsc.com/product-detail/Wire-To-Board-Wire-To-Wire-Connector_BOOMELE-Boom-Precision-Elec-1-25T-2A_C10819.html) but I didn't get samples to verify

For the existing PM2.5 Sensor I decided to add power control for the fan so new code to control the speed of the fan as well as when to turn it on. There is a level shifter between the PM2.5 communication header and the ESP32 since the fan operates at 5V and the ESP32 operates at 3.3V

I kept a USB C connector for the power but since the ESP32-C3 has embedded USB I connected D+/D- so you can program and communicate with the board over USB. As a backup I also included a UART header.

I kept the LEDs on the back of the board and placed them in the same locations so you can still use them as a visual status of the sensor.

I did some initial testing with test code and everything appears to be working as expected but the complete software package has not been developed yet, life keeps getting in the way.

![Prototype](https://github.com/GeorgeIoak/Ikea_Environmental_Sensor/blob/master/images/2021-10-28%2016.59.06.jpg "Prototype Fit")

## Some useful links to additional information:

- https://github.com/pulsar256/vindriktning_esphome
- https://blakadder.com/vindriktning-tasmota/
