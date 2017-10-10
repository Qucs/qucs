Rem Wrapper script for QUCS
@echo off

Rem Silently unset environment variables used until 0.0.18
set QUCSDIR=
set ASCOBINDIR=
set FREEHDL=
set OCTAVEDIR=
set MINGWDIR=

Rem Get the location of this batch file.
Rem Note: QUCSPREFIX ends with a backslash
set QUCSPREFIX=%~dp0

Rem Set QUCS into the path
set PATH=%QUCSPREFIX%bin;%PATH%

Rem Set up admsXml
Rem   - Override needed for now. Else it could be using QucsSettings from version 0.0.18
set ADMSXMLBINDIR=%QUCSPREFIX%bin

Rem Set up FreeHDL variable
rem   - Portable version assume FreeHDL installed in the same prefix as QUCS
Rem   - qucsdigi.bat and qucsdigilib.bat set %FREEHDL%\bin on the path
set FREEHDL=%QUCSPREFIX%

Rem Set up MingGW
Rem   - GCC 4.8.2 / MinGW must be on the path for FreeHDL and Verilog-A module compilation
set MINGWDIR=C:\mingw32
set PATH=%MINGWDIR%\bin;%PATH%

Rem Set up Icarus into path, 
Rem   - upstream installer does not set the path
set ICARUS_PREFIX=C:\iverilog
set PATH=%ICARUS_PREFIX%\bin;%PATH%

Rem  Start Qucs-GUI
start qucs.exe

Rem Close cmd.exe window
exit