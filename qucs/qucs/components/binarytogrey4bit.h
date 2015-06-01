/*
 * binarytogrey4bit.h - device definitions for binarytogrey4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef BINARYTOGREY4BIT_H
#define BINARYTOGREY4BIT_H

#include "component.h"

class binarytogrey4bit : public Component
{
  public:
    binarytogrey4bit();
    ~binarytogrey4bit() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* BINARYTOGREY4BIT_H */
