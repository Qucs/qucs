/***************************************************************************
                                wprobe.cpp
                               -----------
    begin                : Sat Sep 18 2004
    copyright            : (C) 2015 by Pedro Macedo
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

#ifndef __WPROBE_H__
#define __WPROBE_H__

class wprobe : public qucs::circuit
{
 public:
  CREATOR (wprobe);
  void initDC (void);
  void initAC (void);
  void initTR (void);
  void saveOperatingPoints (void);
  void calcOperatingPoints (void);
};

#endif /* __WPROBE_H__ */
