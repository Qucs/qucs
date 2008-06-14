/*
 * EKV26pMOS.h - device definitions for EKV26pMOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef EKV26pMOS_H
#define EKV26pMOS_H

#include "component.h"

class EKV26pMOS : public Component
{
  public:
    EKV26pMOS();
    ~EKV26pMOS() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* EKV26pMOS_H */
