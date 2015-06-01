/*
 * hpribin4bit.h - device definitions for hpribin4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef HPRIBIN4BIT_H
#define HPRIBIN4BIT_H

#include "component.h"

class hpribin4bit : public Component
{
  public:
    hpribin4bit();
    ~hpribin4bit() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* HPRIBIN4BIT_H */
