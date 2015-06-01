/*
 * dmux3to8.h - device definitions for dmux3to8 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef DMUX3TO8_H
#define DMUX3TO8_H
#include "component.h"

class dmux3to8 : public Component
{
  public:
    dmux3to8();
    ~dmux3to8() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString vhdlCode(int);
    QString verilogCode(int);
};

#endif /* DMUX3TO8_H */
