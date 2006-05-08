@echo off

if not exist "%5" goto usage

set NAME=%1
set NAMEOUT=%2
set TIME=%3
set DIR=%4
set BINDIR=%5
set OPTION=%6

REM Remove leading and trailing "
set TIME=%TIME:~1,-1%

if not exist "%DIR%" goto nodir

cd /d "%DIR%"

if not exist %NAME% goto nofile

copy %NAME% digi.vhdl > NUL
set NAME=digi

REM set MINGWDIR=H:/Daten/Misc/mingw
REM set FREEHDL=H:/Daten/Misc/freehdl
REM set QUCSDIR=H:/Daten/Misc/Qucs

set CXX=%MINGWDIR%/bin/g++
set CXXFLAGS=-O2 -g -I"%FREEHDL%/include"
REM set NAMEOUT=digi.dat
set LDFLAGS=-L"%FREEHDL%/lib" -Wl,--enable-auto-import -s
set LIBS=-lfreehdl-kernel -lfreehdl-std -lregex

set PATH=%PATH%;%FREEHDL%/bin;%MINGWDIR%/bin

echo running C++ conversion...
%FREEHDL%/bin/freehdl-v2cc -m %NAME%._main_.cc -L "%FREEHDL%/share/freehdl/lib" -o %NAME%.cc %NAME%.vhdl

echo compiling functions...
%CXX% %CXXFLAGS% -c %NAME%.cc

echo compiling main...
%CXX% %CXXFLAGS% -c %NAME%._main_.cc

echo linking...
%CXX% %NAME%._main_.o %NAME%.o %LDFLAGS% %LIBS% -o %NAME%.exe

echo simulating...
%NAME%.exe -cmd "dc -f %NAME%.vcd -q;d;run %TIME%;q;" > NUL

echo running VCD conversion...
%QUCSDIR%/bin/qucsconv.exe %OPTION% -if vcd -of qucsdata -i %NAME%.vcd -o %NAMEOUT%

goto end

:usage
echo Usage: %0 "<netlist.txt> <output.dat> <time> <directory> <bindirectory> <convoption>"
echo Directory has to contain netlist.txt
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
