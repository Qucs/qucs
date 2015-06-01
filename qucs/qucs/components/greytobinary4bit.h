/*
 * greytobinary4bit.h - device definitions for greytobinary4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef GREYTOBINARY4BIT_H
#define GREYTOBINARY4BIT_H

#include "component.h"

class greytobinary4bit : public Component
{
  public:
    greytobinary4bit();
    ~greytobinary4bit() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* GREYTOBINARY4BIT_H */
