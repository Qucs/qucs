/*
 * m_interface.h - m-code interface class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $Id: analysis.h 1869 2013-03-06 12:50:21Z crobarcro $
 *
 */

/*! \file m_interface.h
 * \brief The m-code interface class header file.
 *
 * Contains the m-code interface class definition.
 */

#ifndef __M_INTERFACE_H__
#define __M_INTERFACE_H__

/*! \class qucsmint
 * \brief class for performing circuit analyses.
 *
 * This class is used for performing all the anlyses specified in
 * a circuit description. The actual solver classes for specific
 * analysis types inheirit from this class and override it's
 * methods.
 *
 */
class qucsmint
{

public:
    qucsmint(char * infile);
    ~qucsmint();

    int evaluate();


private:

  char * outfile = NULL;
  net * subnet;
  input * in;
  circuit * gnd;
  dataset * out;
  environment * root;

};

#endif /* __ANALYSIS_H__ */
