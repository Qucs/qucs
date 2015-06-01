/*
 * pad3bit.h - device definitions for pad3bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef PAD3BIT_H
#define PAD3BIT_H

#include "component.h"

class pad3bit : public Component
{
  public:
    pad3bit();
    ~pad3bit() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* PAD3BIT_H */
