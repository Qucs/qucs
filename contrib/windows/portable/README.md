
## QUCS - Quite Universal Circuit Simulator - version 0.0.19

Qucs is an integrated circuit simulator which means you are able to setup a circuit with a graphical user interface (GUI) and simulate the large-signal, small-signal and noise behaviour of the circuit. After that simulation has finished you can view the simulation results on a presentation page or window.

## Windows portable version

It is recommended to use the script `qucs.bat` to launch QUCS.
Please modify the script if necessary.

This package is compiled with:
  - Qt 4.6.8 [1]
  - GCC 4.8.2 (i686-posix-dwarf-rev3) [2]
  
For convenience, the following packages are also included:
  - ASCO 0.4.10 <http://asco.sourceforge.net/index.html> 
  - FreeHDL 0.0.8  <http://www.freehdl.seul.org/>  
    - VHDL simulations require GCC 4.8.2 from [2] or [3]
  - ADMS 2.3.6
    - Compilation of Verilog-A modules must performed with GCC 4.8.2 from [2] or [3]
  
Optional packages can be obtained elsewhere:
  - Icarus Verilog <http://bleyer.org/icarus/>
  - Octave <https://www.gnu.org/software/octave/download.html>
  
[1] <http://download.qt.io/official_releases/qt/4.8/4.8.6/qt-opensource-windows-x86-mingw482-4.8.6-1.exe>

[2] <http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/Personal%20Builds/mingw-builds/4.8.2/threads-posix/dwarf/i686-4.8.2-release-posix-dwarf-rt_v3-rev3.7z/>

[3] <https://sourceforge.net/projects/qucs/files/extra/mingw-w64-i686-4.8.2-release-posix-dwarf-rt_v3-rev3-setup.exe>


## Resources
  - Website: <http://qucs.sourceforge.net/>
  - Main repository: <http://sourceforge.net/p/qucs/git/>
  - Mirror repository: <https://github.com/Qucs/qucs>
  - Mailing lists: <http://sourceforge.net/p/qucs/mailman/>
  - Forum: <http://sourceforge.net/p/qucs/discussion/>
  - Bug trackers:
    - <http://sourceforge.net/p/qucs/_list/tickets>
    - <https://github.com/Qucs/qucs/issues>
  - Source code documentation:
    - <http://qucs.github.io/qucs-doxygen/qucs/index.html>
    - <http://qucs.github.io/qucs-doxygen/qucs-core/index.html>
  - Downloads: <http://sourceforge.net/projects/qucs/files/>
  - Translation platform: <https://www.transifex.com/qucs/public/>


## License

QUCS is under GNU General Public License, version 2.0+. See COPYING file.
