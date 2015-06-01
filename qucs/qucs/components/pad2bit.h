/*
 * pad2bit.h - device definitions for pad2bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef PAD2BIT_H
#define PAD2BIT_H

#include "component.h"

class pad2bit : public Component
{
  public:
    pad2bit();
    ~pad2bit() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* PAD2BIT_H */
