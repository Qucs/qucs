# Test C++ namespace conformance
#
# Copyright (C) 2008 Bastien ROUCARIES <bastien.roucaries+qucs@gmail.com>
#
# Copying and distribution of this file, with or without modification, 
# are permitted in any medium without royalty provided the copyright notice 
# and this notice are preserved. 
#  
# This script use script from http://autoconf-archive.cryp.to/
# under the following licence
#
# Copyright © 2004 Todd Veldhuizen
# Copyright © 2004 Luc Maisonobe <luc@spaceroots.org> 
# Copying and distribution of this file, with or without modification, 
# are permitted in any medium without royalty provided the copyright notice 
# and this notice are preserved. 

dnl Namespace
AC_DEFUN([AC_CXX_NAMESPACES],
[AC_CACHE_CHECK(whether the compiler implements namespaces,
ac_cv_cxx_namespaces,
[AC_LANG_PUSH(C++)
 AC_TRY_COMPILE([namespace Outer { namespace Inner { int i = 0; }}],
                [using namespace Outer::Inner; return i;],
 ac_cv_cxx_namespaces=yes, ac_cv_cxx_namespaces=no)
 AC_LANG_POP
])
if test "$ac_cv_cxx_namespaces" = yes; then
  AC_DEFINE(HAVE_NAMESPACES,,[define if the compiler implements namespaces])
else
  AC_MSG_ERROR([The $PACKAGE package requires an c++ compiler with namespace.])	
fi
])
