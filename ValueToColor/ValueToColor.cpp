/*
  ValueToColor.cpp - Library to convert a value into a color.
  Created by Christian Lechner, August 6, 2017.
  Released into the public domain.
*/
#include "Arduino.h"
#include "ValueToColor.h"


ValueToColor::ValueToColor(float setvalue, float tolerance, int brightness)
{
	m_setvalue = setvalue;
	m_tolerance = tolerance;
	m_brightness = brightness;

}

void ValueToColor::set_setvalue(float setvalue)
{
	m_setvalue = setvalue;
}

void ValueToColor::set_tolerance(float tolerance)
{
	m_tolerance = tolerance;
}

void ValueToColor::set_brightness(int brightness)
{
	m_brightness = brightness;
}


bool ValueToColor::convert(float value, int* r, int* g, int* b) {

  float blue;
  float green;
  float red;
  
  float bright_step = ((m_brightness / 2.0) / m_tolerance); 


  if(value < m_setvalue) {
    //smaller
     if(value < m_setvalue - m_tolerance)
     {
      //outside tolerance
      if(value < m_setvalue - 2*m_tolerance)
      {
        red = 0.0;
        green = 0.0;
        blue =  m_brightness;
      }
      else
      {
        red = 0.0;
        green = (m_brightness / 2.0) - bright_step*((m_setvalue - value) - m_tolerance);
        blue =  (m_brightness / 2.0) + bright_step*((m_setvalue - value) - m_tolerance);
      }
     }
     else
     {
      //inside tolerance
      red = 0.0;
      green = m_brightness - bright_step*(m_setvalue - value);
      blue = bright_step*(m_setvalue - value);
     }
  }
  else if(value > m_setvalue) {
    //greater
     if(value > m_setvalue + m_tolerance)
     {
      //outside tolerance
      if(value > m_setvalue + 2*m_tolerance)
      {
        red = m_brightness;
        green = 0.0;
        blue = 0.0;
      }
      else
      {
        red = (m_brightness / 2.0) + bright_step*((value - m_setvalue) - m_tolerance);
        green = (m_brightness / 2.0) - bright_step*((value - m_setvalue) - m_tolerance);
        blue = 0.0;
      }
     }
     else
     {
      //inside tolerance
      red = bright_step*(value - m_setvalue);
      green = m_brightness - bright_step*(value - m_setvalue);
      blue = 0.0;
     }
    
  }
  else
  {
    red = 0.0;
    green = m_brightness;
    blue = 0.0;
  }


  *r = (int)(red + 0.5);
  *g = (int)(green + 0.5);
  *b = (int)(blue + 0.5);


  return true;

  
}



uint32_t ValueToColor::convert(float value) {
	int r, g, b;
	convert(value, &r, &g, &b);

	return ((uint32_t)r << 16) | ((uint32_t)g <<  8) | (uint32_t)b;
}
