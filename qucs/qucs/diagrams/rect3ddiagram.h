/***************************************************************************
                              rect3ddiagram.h
                             -----------------
    begin                : Sat Mar 5 2005
    copyright            : (C) 2005 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef RECT3DDIAGRAM_H
#define RECT3DDIAGRAM_H

#include <QObject>
class Rect3DDiagram;

// map signals to Rect3DDiagram and its Dialogs
// TODO: this is a bit ugly. need more generic approach, for all Diagrams
class mySignalWrapper : public QObject
{
    Q_OBJECT
public:
    mySignalWrapper(QObject *parent=0) {}
    mySignalWrapper(Rect3DDiagram *target, void (Rect3DDiagram::*f)(int)) :
      QObject(), _target(target), _int(f) {}
    mySignalWrapper(Rect3DDiagram *target, void (Rect3DDiagram::*f)(const QString&)) :
      QObject(), _target(target), _str(f) {}
    ~mySignalWrapper(){}

public Q_SLOTS:
    void map(int x);
    void map(const QString& x);

private:
    Rect3DDiagram* _target;
    void (Rect3DDiagram::*_int)(int);
    void (Rect3DDiagram::*_str)(const QString&);
};

// ----------------------------------------
// BUG. belongs to rect3ddiagram.cpp
#include "diagram.h"


struct tPoint3D;
struct tPointZ;
struct tBound;

class QSlider;
class Cross3D;

class Rect3DDiagram : public Diagram  {
  friend class mySignalWrapper;
  Rect3DDiagram(const Rect3DDiagram&) { /*incomplete*/ }
public:
  Rect3DDiagram(int _cx=0, int _cy=0);
 ~Rect3DDiagram();

  Element* newOne() const {return new Rect3DDiagram(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
  int  calcDiagram();
  void calcLimits();
  void calcCoordinate(const double*, const double*, const double*, float*, float*, Axis const*) const;
  void finishMarkerCoordinates(float&, float&) const;

  void createAxisLabels();
  bool insideDiagram(float, float) const;

  tPoint3D *Mem;   // memory for all points during hidden line algorithm
  tPoint3D *pMem;  // current position in "Mem"


protected:
  void calcData(GraphDeque*);

private:
  bool setParamByIndex(unsigned index, QString value);

  int  calcAxis(Axis*, int, int, double, double, bool);
  void createAxis(Axis*, bool, int, int, int, int);
  // void paint(ViewPainter*);

  void   calcCoefficients();
  int    calcCross(int*, int*);
  double calcX_2D(double, double, double) const;
  double calcY_2D(double, double, double) const;
  double calcZ_2D(double, double, double) const;

  static int comparePoint3D(const void*, const void*);
  static int comparePointZ(const void*, const void*);
  bool isHidden(int, int, tBound*, char*);
  void enlargeMemoryBlock(tPoint3D* &);
  void calcLine(tPoint3D* &, tPoint3D* &, tBound*, char*);
  void calcCoordinate3D(double, double, double, double, tPoint3D*, tPointZ*);
  void removeHiddenLines(char*, tBound*);
  void removeHiddenCross(int, int, int, int, char*, tBound*);

  float  xorig, yorig; // where is the 3D origin with respect to cx/cy
  double cxx, cxy, cxz, cyx, cyy, cyz, czx, czy, czz; // coefficients 3D -> 2D
  double scaleX, scaleY;
  bool hideLines;
  int rotX, rotY, rotZ;

  QCheckBox *hideInvisible;
  QLineEdit *rotationX, *rotationY, *rotationZ;
  QSlider *SliderRotX, *SliderRotY, *SliderRotZ;
  // FIXME: rotate diagram, not tinylittlecross
  Cross3D *DiagCross;

  void slotNewRotX(int);
  void slotNewRotY(int);
  void slotNewRotZ(int);
  void slotEditRotX(const QString&);
  void slotEditRotY(const QString&);
  void slotEditRotZ(const QString&);

  bool applyDialog();
  void grid_layout_stuff(DiagramDialog*, QGridLayout*, QWidget*, unsigned);
  void save_some_char(QTextStream& str) const;
  void save_rot_hack(QTextStream& str) const;
  void set_hide_lines_hack(bool h){hideLines=h;}
  mySignalWrapper* m_sigmapper;

private:
  QList<QObject *> callbacks;
};


#endif
// vim:ts=8:sw=2:noet
