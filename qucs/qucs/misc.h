/***************************************************************************
                                  misc.h
                                 --------
    begin                : Wed Nov 12 2004
    copyright            : (C) 2014 by YodaLee
    email                : lc85301@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*!
 * \file misc.h
 * \Declaration of some miscellaneous function
 */

class QString;

namespace misc {
  QString complexRect(double, double, int Precision=3);
  QString complexDeg (double, double, int Precision=3);
  QString complexRad (double, double, int Precision=3);
  QString StringNum  (double, char form='g', int Precision=3);
  void    str2num    (const QString&, double&, QString&, double&);
  QString num2str    (double);
  QString StringNiceNum(double);
  void    convert2Unicode(QString&);
  void    convert2ASCII(QString&);
  QString properName(const QString&);
  QString properAbsFileName(const QString&);
  QString properFileName(const QString&);
  bool    VHDL_Time(QString&, const QString&);
  bool    VHDL_Delay(QString&, const QString&);
  bool    Verilog_Time(QString&, const QString&);
  bool    Verilog_Delay(QString&, const QString&);
  QString Verilog_Param(const QString);
  bool    checkVersion(QString&);
}



// Fix Visual Studio 2010 lack of std::isfinite
// https://gist.github.com/ennetws/fb4d02dbf476557a3c71
#if (_MSC_VER == 1600)
namespace std{
  template<typename T> bool isfinite(T arg){
    return arg == arg &&
           arg != std::numeric_limits<T>::infinity() &&
           arg != -std::numeric_limits<T>::infinity();
  }
}
#endif
