/* Copyright (C) 2013 Albert Davis
 *               2020 Felix Salfelder
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *------------------------------------------------------------------
 * print a message at sign-on.
 */
//#include "io_.h"
//#include "patchlev.h"
#include <iostream>

#define PATCHLEVEL "wip"
/*--------------------------------------------------------------------------*/
static class SIGN_ON {
public:
  SIGN_ON() {
    std::cout << "legacy plugins: " PATCHLEVEL "\n";
  }
} sign_on;
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
