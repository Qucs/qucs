/*
 * photodiode.h - device definitions for photodiode module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef PHOTODIODE_H
#define PHOTODIODE_H

#include "component.h"

class photodiode : public Component
{
  public:
    photodiode();
    ~photodiode() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* PHOTODIODE_H */
