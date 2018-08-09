/*$Id: io_trace.h 2016.08.11 $ -*- C++ -*-
 * Copyright (C) 2001 Albert Davis,
 *               2015,16 Felix Salfelder
 * Author: Albert Davis <aldavis@gnu.org>, Felix Salfelder
 *
 * This file is part of "Gnucap", the Gnu Circuit Analysis Package
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
 * trace macros for debugging
 */
#include <iostream>

/* allow multiple inclusions with different DO_TRACE */
#undef trace_line
#undef trace
#undef trace0
#undef trace1
#undef trace2
#undef trace3
#undef trace4
#undef trace5
#undef trace6
#undef trace7
#undef trace8
#undef untested
#undef itested
#undef unreachable
#undef incomplete
/*--------------------------------------------------------------------------*/
#ifdef DO_TRACE

#ifdef __cplusplus

#define trace_line() (std::cerr << "@@#\n@#@:" \
              << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n" )
#define trace0(s) ( std::cerr << "@#@" << (s) << "\n")
#define trace1(s,x) ( \
	   std::cerr <<  "@#@" << (s) << "  " << #x << "=" << (x)  \
		     << "\n" )
#define trace2(s,x,y) ( \
	   std::cerr <<  "@#@" << (s) << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "\n" )
#define trace3(s,x,y,z) ( \
	   std::cerr <<  "@#@" << (s) << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "  " << #z << "=" << (z)  \
		     << "\n" )
#define trace4(s,w,x,y,z) ( \
	   std::cerr <<  "@#@" << (s) << "  " << #w << "=" << (w)  \
		     << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "  " << #z << "=" << (z)  \
		     << "\n" )
#define trace5(s,v,w,x,y,z) ( \
	   std::cerr <<  "@#@" << (s) << "  " << #v << "=" << (v)  \
		     << "  " << #w << "=" << (w)  \
		     << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "  " << #z << "=" << (z)  \
		     << "\n" )
#define trace6(s,u,v,w,x,y,z) ( \
	   std::cerr <<  "@#@" << (s) \
		     << "  " << #u << "=" << (u)  \
		     << "  " << #v << "=" << (v)  \
		     << "  " << #w << "=" << (w)  \
		     << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "  " << #z << "=" << (z)  \
		     << "\n" )
#define trace7(s,t,u,v,w,x,y,z) ( \
           std::cerr <<  "@#@" << (s)  \
		     << "  " << #t << "=" << (t)  \
		     << "  " << #u << "=" << (u)  \
		     << "  " << #v << "=" << (v)  \
		     << "  " << #w << "=" << (w)  \
		     << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "  " << #z << "=" << (z)  \
		     << "\n" )
#define trace8(s,r,t,u,v,w,x,y,z) ( \
		std::cerr <<  "@#@" << (s)  \
		     << "  " << #r << "=" << (r)  \
		     << "  " << #t << "=" << (t)  \
		     << "  " << #u << "=" << (u)  \
		     << "  " << #v << "=" << (v)  \
		     << "  " << #w << "=" << (w)  \
		     << "  " << #x << "=" << (x)  \
		     << "  " << #y << "=" << (y)  \
		     << "  " << #z << "=" << (z)  \
		     << "\n" )

#else // no __cplusplus

#define trace_line() (printf("@@#\n@#@:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))
#define trace0(s) (printf("@#@%s\n", s))
#define trace1(s,x) (printf("@#@%s  %s=%g\n", s, #x, (double)(x)))
#define trace2(s,x,y) (printf("@#@%s  %s=%g  %s=%g\n",\
	s, #x, (double)(x), #y, (double)(y)))
#define trace3(s,x,y,z) (printf("@#@%s  %s=%g  %s=%g  %s=%g\n",\
	s, #x, (double)(x), #y, (double)(y), #z, (double)(z)))
#define trace4(s,w,x,y,z)(printf("@#@%s  %s=%g  %s=%g  %s=%g  %s=%g\n",\
	s, #w, (double)(w), #x, (double)(x), #y, (double)(y), #z, (double)(z)))
#define trace5(s,v,w,x,y,z)\
	(printf("@#@%s  %s=%g  %s=%g  %s=%g  %s=%g  %s=%g\n",\
	s, #v, (double)(v), #w, (double)(w), #x, (double)(x),\
	#y, (double)(y), #z, (double)(z)))

#endif // __cplusplus

#else
#define USE(x) (1)?(void)(0):(void)(x)
#define trace_line()
#define trace0(s) USE(s)
#define trace1(s,x) (USE(s),USE(x))
#define trace2(s,x,y) USE(s);USE(x);USE(y)
#define trace3(s,x,y,z) USE(s);USE(x);USE(y);USE(z)
#define trace4(s,w,x,y,z) USE(s);USE(w);USE(x);USE(y);USE(z)
#define trace5(s,v,w,x,y,z) USE(s);USE(v);USE(w);USE(x);USE(y);USE(z)
#define trace6(s,u,v,w,x,y,z) USE(s);USE(u);USE(v);USE(w);USE(x);USE(y);USE(z)
#define trace7(s,t,u,v,w,x,y,z) USE(s);USE(t);USE(u);USE(v);USE(w);USE(x);USE(y);USE(z)
#define trace8(r,s,t,u,v,w,x,y,z) USE(r);USE(s);USE(t);USE(u);USE(v);USE(w);USE(x);USE(y);USE(z)
#endif

#ifdef __cplusplus

#define unreachable() ( \
    std::cerr << "@@#\n@@@\nunreachable:" \
              << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n" )

#define incomplete() ( \
    std::cerr << "@@#\n@@@\nincomplete:" \
              << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n" )

#else // no __cplusplus

#define unreachable() (fprintf(stderr,"@@#\n@@@unreachable:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))

#define incomplete() (fprintf(stderr,"@@#\n@@@incomplete:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))

#endif // __cplusplus

#ifdef TRACE_UNTESTED

#ifdef __cplusplus

#define untested() ( std::cerr <<  "@@#\n@@@:"<< __FILE__ << ":"<< __LINE__ \
          <<":" << __func__ << "\n" )
#define untested1(s,x) ( std::cerr <<  "@@#\n@@@:"<< __FILE__ << ":"<< __LINE__ \
          <<":" << __func__ << ": "  << s << "  " << #x << "=" << (x) << "\n" )
#define untested2(s,x,y) ( std::cerr <<  "@@#\n@@@:"<< __FILE__ << ":"<< __LINE__ \
            <<":" << __func__ << ": "  << s << "  " << #x << "=" << (x) << ", "\
                                                    << #y << "=" << (y) << "\n" )
#define untested3(s,x,y,z) ( std::cerr <<  "@@#\n@@@:"<< __FILE__ << ":"<< __LINE__ \
              <<":" << __func__ << ": "  << s << "  " << #x << "=" << (x) << ", "\
                                                      << #y << "=" << (y) << ", " \
                                                      << #z << "=" << (z) << "\n" )

#else // no __cplusplus

#define untested() (fprintf(stderr,"@@#\n@@@:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))

#endif // __cplusplus

#else
#define untested()
#define untested1(s,x)
#define untested2(s,x,y)
#define untested3(s,x,y,z)
#endif

#ifdef TRACE_ITESTED

#ifdef __cplusplus

#define itested() ( std::cerr << "@@#\n@@@:" \
     << __FILE__ << ":" << __LINE__ << ":" << __func__ << "\n" )

#else // no __cplusplus

#define itested() (fprintf(stderr,"@@#\n@@@:%s:%u:%s\n", \
			   __FILE__, __LINE__, __func__))
#endif // __cplusplus

#else
#define itested()
#endif


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
// vim:ts=8:sw=2:noet:
