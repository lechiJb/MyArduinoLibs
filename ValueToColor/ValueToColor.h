/*
  ValueToColor.cpp - Library to convert a value into a color.
  Created by Christian Lechner, August 6, 2017.
  Released into the public domain.
*/
#ifndef VALUETOCOLOR_H
#define VALUETOCOLOR_H

#include "Arduino.h"

class ValueToColor
{
  public:
    ValueToColor(float setvalue, float tolerance, int brightness);
    uint32_t convert(float value);
    bool convert(float value, int* r, int* g, int* b);
    void set_setvalue(float setvalue);
    void set_tolerance(float tolerance);
    void set_brightness(int brightness);

  private:
    float m_setvalue;
    float m_tolerance;
    int m_brightness;
    
};



#endif
