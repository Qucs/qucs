/***************************************************************************
                                 node.h
                                --------
    copyright            : (C) 2003 by Michael Margraf
                               2018 Felix Salfelder / QUCS team
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QUCS_PORT_H
#define QUCS_PORT_H

class Node;

struct Port {
  Port() {};
  Port(int _x, int _y, bool _avail=true) : x(_x), y(_y), avail(_avail) {
    Type=""; Name=""; Connection=0;};

// private:
  int   x, y;
  bool  avail;
  QString Type;
  QString Name;
  Node *Connection;

public: // perhaps not here
  Node* value() const{return Connection;}
  QString const& name() const{return Name;}
};


#endif
