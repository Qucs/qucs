Qucs 0.0.19 Release Notes
=========================

This release brings improvements on many parts of Qucs.
The port from Qt3 to Qt4 is still ongoing and many bugs and usability issues have been fixed in the process.
A few new features were added and others are in development.
See below a short summary of the enhancements.

Qucs
----

* Fix diagram color picker button ([#30])

* Fix zoom in a box ([#31])

* [NEW] Command line to save schematic as image (png, svg, pdf) ([#32])

* [NEW] Add component MutualX ([#54])

* [NEW] Enable anti-aliasing, provide it as option ([#73])

* [NEW] Add search file on component dock ([PR#86])

* Fix rendering of component text properties ([PR#87], [PR#98])

* [NEW] Add recently opened menu ([PR#95], [PR#207])

* Fix crash when loading timingdiagram ([#112])

* Refactor text search dialog ([PR#124])

* Correct simulator process exit check ([#132])

* Refactor project manager ([PR#153])

* Deprecated use of `HOME` environment variable ([#204])

* [NEW] Enable use of `QUCSATOR` environment variable to overide `qucsator` ([PR#209])

* Fix bug on deactivation (shor-circuiting) a component ([#216])

* Various usability enhancements ([PR#232])
  - Adjust various dialogs for a better table sizing
  - Results of Optimization dialog can be copied to clipboard and pasted as an Equation
  - Fix usage of font metrics in the schematic
  - Fix handling of simulation messages in case a simulation is aborted
  - Add `QT3_WARNINGS` to serve as a reminder of which Qt3 stuff still needs removal.

* [NEW] Redesigned About dialog ([PR#252])

* Fix context menu texts when pressed over toolbar ([#253])

* Fix crash when using DC bias annotation with a simulation sweep ([#272])

* Selection of a wire label would change the font elsewhere in the schematic ([#467])

* Fix crash when changing background color ([PR#456])

* Changing in Qucs Home no longuer requires a restart ([PR#456])

* Fix the "Open recent Files" which was not uptaded after save-as ([PR#444])

* Fix issue that allowed to create projects with an empty name ([#311])

* Update authors and date in the About dialog ([PR#427])

* Remove non-GPL Verilog-AMS models: HICUM, BSIM, FBH_HBT ([#421])

* Fix typo in Voltage Controlled Resistor ([PR#406])

* Fix keypad up/down on component dialog ([#319])

* Fix crash when reloading Verilo-A symbols ([#478])

* Fix matching circuit tool number conversion ([PR#482])

* Fix the handling of project names containing `_prj` in them ([PR#486])

Qucsator
--------

* Fix segmentation fault during noise enabled S-parameter simulaton ([#133])

* Fix regression on tswitch component ([#34], [PR#203])

* Fix coplanar waveguide formulae ([SF#183], [PR#306])

* Fix several segfaults during fuzzed input ([PR#274])

* Remove null and free checks ([PR#267])

* Adopt to standard library in several classes.

* Enable gcov for coverage

* Fix S-parameters renormalization formula ([PR#481)

* Remove non-GPL Verilog-AMS models ([#421])

* Several fixes concerning rectangular waveguides ([PR#349])

* Correct CPW formulae in QucsTranscalc and qucsator ([PR#306])

* Fix regression on EDD component ([#296])

* Fix several segmentation faults, caught by AFL (american fuzzy lop) ([PR#274])

* Fix excess phase calculations on BJT device ([PR#493])

* Fix regression on SVD calculation ([PR#493])

* Fix turn off behavior of thyristor device when reversed biased ([PR#493])


Qucs-Transcalc
-------------

* Fix issues with coupled microstrips ([SF#171],[PR#85])

* Fix coplanar waveguide formulae ([SF#183], [PR#306])

* Several fixes concerning rectangular waveguides ([PR#349])

* Correct CPW formulae in QucsTranscalc and qucsator ([PR#306])

[New] Qucs-ActiveFilter
--------------------------------

* Merge new qucs-activefilter tool ([#43], [#300])

Qucs-Filter
-------------

* Port QucsStudio Filter synthesis ([#15])

* Added quarter wavelength bandpass and notch filters ([#310])
  Transmission line and the microstrip are available.


Qucs-Lib
-------------

* Fix issue that corrupt netlist for item dragged from the library dock ([#39])

* Fixed library paths for user library components ([PR#292])

* Fix crash when creating symbols for unknown components ([PR#399])

* Fix pin assignment in LM317 regulator ([PR#375])

* Fix ideal OpAmp model ([PR#371])

* Add search box to search components in libraries ([PR#303])

Qucs-Rescodes
-------------

* Ported to Qt4 ([#38])

* Corrected qucsrescodes command name for Linux ([PR#29])

* Fix issues in the menu bar and fonts ([#445])


Qucs-Help
--------

* Fix issue on Qucs-Help leading to fail when searching for documentation files ([#335])

Qucsconv
--------

* Add further command line help documentation, `qucsconv -h` ([#57])

* Remove converter for non-GPL Verilog-AMS models ([#421])

Documentation
-------------

* Fix table of contents on Technical.pdf ([#55])

* Documentation for qucs-active filter ([PR#156])

Translations
----------

* Updated DE ([PR#128])

* Setup Transifex for Qucs desktop applications ([PR#357])


Port
----

* Ported Diagram classes from Qt3PtrList to QList

* Port Qt3PtrList to QVector on Diagrans, amonst other improvements

* Ported Component member variables Lines, Arcs, Rects, Ellips, Texts from Qt3PtrList to QList


Miscelaneous
------------

* new: top VERSION files shall be used for versioning (Autotools, CMake). TODO, package versioning.

* new: add tag with the latest Git commit hash: Qucs (About and -v), Qucsator, Qucsconv

* Move from math.h to cmath.h ([PR#168])

* Corrected behavior of `--with-mkadms` ([PR#436])

* Remove ADMS as submodule

* Add support for preferred number series with ASCO, fix variables search string ([PR#412])

* Fix Mac OSX uninstall script ([PR#414])

* Integrate qucs-test into qucs package ([PR#389])

* New toplevel make, make check, make distcheck

* Simplify Verilo-A compilation rules


---

Qucs 0.0.18 Release Notes
=========================

This release brings improvements on many parts of Qucs.
The port from Qt3 to Qt4 is still ongoing and any bugs and usability issues have been fixed in the process.
A few new features were added and others are in development.
See below a short summary of the most noticeable enhancements.

Qucs, Qucsator
--------------

 * new: Dynamic compilation and loading of Verilog-A modules (beta feature).
   Addition of a full ADMS/Qucs "turn key" Verilog-A compact device modelling system to
   Qucs. Users are no longer required to manually edit C++ code and the build system
   to be able to run Verilog-A models. Uses ADMS 2.3.4 (only a subset of Verilog-A is supported).

 * fix: Spice file handling and translator

 * new: Addition of BSIM 3 and BSIM 4 models to Qucs MOS models.

 * new: Release of the first model (RFresistor) in a new series of RF models for Harmonic Balance
   and other forms of Qucs simulation.

 * new: HICUM npn and pnp Level 0 V1.3 Verilog-A devices.

 * new: HICUM npn level 2 V2.32 Verilog-A device.


Qucs
----

 * new/fix : Ported several classes from Qt3Support to Qt4 (work in progress)

 * new: use QSettings to store user preferences and application settings

 * new: Copy/duplicate project content files

 * new: Export schematic and diagrams to SVG, PNG, JPG, JPEG, PDF, PDF+Latex, EPS (Inkscape required)

 * new: Component library is not included on the Dock Window

 * new: Added checkbox to settings, which allows to load schematics from future versions

 * new: Open Recent menu option

 * new: Set ADMS, ASCO and Octave paths on the application settings

 * new: Use environment variables ADMSXMLBINDIR, ASCOBINDIR, OCTAVEBINDIR to set tools paths

 * new: Updated translations: pt-BR, pt-PT, ru


Qucsator
--------

 * new: Refactoring of Qucsator namespace. Most of the core functions are under the namespace `qucs::`

 * new: Allow to run autotest by adding bugon/assert operator

 * new: initial support for unit testing with googletest.

 * new: added a Python scripts for parsing Qucs data files

 * fix: enable BJT models with (Vtf < 0)


QucsFilter, QucsAttenuator, QucsHelp, QucsEdit
----------------------------------------------

 * new/fix : Ported several classes from Qt3Support to Qt4 (work in progress)


QucsResCodes
------------

 * new: QucsResCodes is a program to compute color codes for resistors and resistance values from
   color codes. The program computes the closest standard resistor value. Users can paste the computed
   resistor in the schematic.


Interfaces
----------

 * Improved Qucs/Octave interface for GUI and Qucssator netlist entry (including a start
   on a Octave library of plotting functions for Qucs post simulation data visualisation)

 * First steps in developing a combined Qucs/Python Qucs simulation capability with
   high quality date visualisation using matplolib. Applications include basic sensitivity analysis
   and circuit performance optimisation.

 * Preliminary version of Qucs/Octave linked transient simulation capability.


Build system
------------

 * Add ADMS as a Git submodule to the Qucs Git repository

 * Added --disable-asco and --disable-adms option to skip the build of included sources

 * Added CMake (experimental) for advanced users building from Git repository

 * OSX: options to build against 10.5 to 10.9 SDK


Included packages
-----------------

 * ASCO 0.4.9 (patched)

 * ADMS 2.3.4


Miscelaneous
------------

 * Project website is now under version control.

 * Source code is now under Travis continuous integration service.

 * Source code documentation (Doxygen) available on the website.

 * Preliminary regression and test suite (qucs-test)


Bug fixes
---------

 * Hundreds of bug fixes and code improvements.

