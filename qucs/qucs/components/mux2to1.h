/*
 * mux2to1.h - device definitions for mux2to1 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef MUX2TO1_H
#define MUX2TO1_H

#include "component.h"

class mux2to1 : public Component
{
  public:
    mux2to1();
    ~mux2to1() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* MUX2TO1_H */
