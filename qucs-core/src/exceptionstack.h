/*
 * exceptionstack.h - exception stack class definitions
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: exceptionstack.h,v 1.3 2004-08-19 19:44:23 ela Exp $
 *
 */

#ifndef __EXCEPTIONSTACK_H__
#define __EXCEPTIONSTACK_H__

namespace qucs {

class exception;

class exceptionstack
{
 public:
  exceptionstack ();
  exceptionstack (const exceptionstack &);
  ~exceptionstack ();
  void push (exception *);
  exception * pop (void);
  exception * top (void);
  void print (const char * prefix = NULL);

 private:
  exception * root;
};

// Global exception stack.
extern exceptionstack estack;

} /* namespace */

// Some macros used with exception handling.
#define try_running()        do {
#define catch_exception()    } while (0); if (estack.top ()) \
                                       switch (estack.top()->getCode ())
#define throw_exception(e)   estack.push (e)
#define top_exception()      estack.top ()
#define pop_exception()      estack.pop ()

#endif /* __EXCEPTIONSTACK_H__ */
