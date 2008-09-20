/*
 * hic0p2_full.h - device definitions for hic0p2_full module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef hic0p2_full_H
#define hic0p2_full_H

#include "component.h"

class hic0p2_full : public MultiViewComponent
{
  public:
    hic0p2_full();
    ~hic0p2_full() { };
    Component* newOne();
    static Element* info(QString&, char* &, bool getNewOne=false);
    static Element* info_pnp(QString&, char* &, bool getNewOne=false);
  protected:
    void createSymbol();
    QString netlist();
};

#endif /* hic0p2_full_H */
