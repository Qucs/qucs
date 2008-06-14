/*
 * EKV26nMOS.h - device definitions for EKV26nMOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef EKV26nMOS_H
#define EKV26nMOS_H

#include "component.h"

class EKV26nMOS : public Component
{
  public:
    EKV26nMOS();
    ~EKV26nMOS() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
};

#endif /* EKV26nMOS_H */
