/***************************************************************************
                                ohmmeter.cpp
                               -----------
    begin                : Fri Sep 25 2016
    copyright            : (C) 2016 by Nelson Martins
    email                : macedo.pp@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef __OHMMETER_H__
#define __OHMMETER_H__

class ohmmeter : public qucs::circuit
{
 public:
  CREATOR (ohmmeter);
  void initSP (void);
  void initDC (void);
  void initDC2 (void);
  void initAC (void);
  void initAC2 (void);
  void initTR (void);
  //void saveOperatingPoints (void);
  //void calcDC (void);
  void calcOperatingPoints (void);
  void setstate(int);
  int getstate();
 private:
  int on;
};

#endif /* __OHMMETER_H__ */
