@echo off

REM
REM qucsdigilib.bat - wrapper script for digital modules and libraries
REM
REM Copyright (C) 2009 Stefan Jahn <stefan@lkcc.org>
REM
REM This is free software; you can redistribute it and/or modify
REM it under the terms of the GNU General Public License as published by
REM the Free Software Foundation; either version 2, or (at your option)
REM any later version.
REM 
REM This software is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU General Public License for more details.
REM 
REM You should have received a copy of the GNU General Public License
REM along with this package; see the file COPYING.  If not, write to
REM the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
REM Boston, MA 02110-1301, USA.  
REM

if not exist "%3" goto usage

set NAME=%1
set NAMEOUT=%2
set DIR=%3

if not exist "%DIR%" goto nodir

cd /d "%DIR%"

if not exist %NAME% goto nofile

copy %NAME% digi.vhdl > NUL
set NAME=digi

REM set MINGWDIR=H:/Daten/Misc/mingw
REM set FREEHDL=H:/Daten/Misc/freehdl
REM set QUCSDIR=H:/Daten/Misc/Qucs

set CXX=g++
set CXXFLAGS=-O2 -I"%FREEHDL%/include"

set PATH=%PATH%;%FREEHDL%/bin;%MINGWDIR%/bin;%QUCSDIR%/bin

echo running C++ conversion...
freehdl-v2cc -L"%FREEHDL%/share/freehdl/lib" -o %NAME%.cc %NAME%.vhdl

echo compiling functions...
%CXX% %CXXFLAGS% -c %NAME%.cc

echo copying modules to VHDL directory...
copy %NAME%.o %NAMEOUT%

goto end

:usage
echo Usage: %0 "<netlist.txt> <output.o> <directory>"
echo Directory has to contain the file 'netlist.txt'.
exit /b 1
goto end

:nodir
echo %DIR%: Not a directory
exit /b 1
goto end

:nofile
echo %NAME%: No such file
exit /b 1
goto end

:end
