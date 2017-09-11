/*
  I2C_Scanner.cpp - Library for Scanning I2C-Bus.
  Created by Christian Lechner, August 6, 2017.
  Released into the public domain.
*/
#include "Arduino.h"
#include "I2C_Scanner.h"
#include <Wire.h>


I2C_Scanner::I2C_Scanner()
{

}


void I2C_Scanner::scan()
{
  byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
    else
    {
      //Serial.print("No device found at 0x");
      //if (address<16)
      //  Serial.print("0");
      //Serial.println(address,HEX); 
    }
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
 
  delay(5000);           // wait 5 seconds for next scan
}
