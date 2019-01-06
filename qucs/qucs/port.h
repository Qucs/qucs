
#ifndef QUCS_PORT_H
#define QUCS_PORT_H

class Node;

struct Port {
  Port() {};
  Port(int _x, int _y, bool _avail=true) : x(_x), y(_y), avail(_avail) {
    Type=""; Name=""; Connection=0;};
  int   x, y;
  bool  avail;
  QString Type;
  QString Name;
  Node *Connection;

public: // perhaps not here
  Node* value() const{return Connection;}
};


#endif
