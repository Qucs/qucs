/*
 * bsim6v10pMOS.h - device definitions for bsim6v10pMOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef bsim6v10pMOS_H
#define bsim6v10pMOS_H

#include "component.h"

class bsim6v10pMOS : public Component
{
  public:
    bsim6v10pMOS();
    ~bsim6v10pMOS() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* bsim6v10pMOS_H */
