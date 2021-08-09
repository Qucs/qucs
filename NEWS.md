Qucs 0.0.20 Release Notes
=========================


Qucs
----

* [NEW] Matching Circuit dialog. Add impedance matching techniques: Single matching,
  double stub matching, real to real broadband transformers, cascaded L-sections
  and lambda/8 + lambda/4 matching ([PR#666])

* [NEW] Add tapered line component. S-param behaviour of a tapered line.
  It provides 4 impedance profiles: Exponential, Linear, Triangular and Klopfenstein. ([PR#657])

* [NEW] Add circular waveguide component. Model the TE11 mode of a circular waveguide. ([PR#654])

* [NEW] RF lumped components. ([PR#416])
  - add INDQ, CAPQ (inductor/capacitor with finite quality factor)
  - spiral inductor, planar circular loop inductor
  - create new palette for RF devices

* [NEW] Wattmeter probe. [(PR#682)]
  -  Calculate apparent power, real power, reactive power and power factor.

* [NEW] Add context menu to document tabs ([PR#708])

* [NEW] Show schematic description while hovering over file name ([#764])

* [NEW] N-port S-parameters de-embedding ([PR#693])

* Initial support for dynamic loading of shared libraries. ([PR#458])

* Fix behavior of push button used for color picking. ([#74])

* Let Markers show impedance/admittance data when used with Smith chart. ([PR#701])

* Fix "export as image" with non integer scale factor ([PR#715])

* Fix PDF bounding box when using "File->Export as image..." ([#882])

Qucsator
--------

* Fix parser fail on big S-parameter files.
  Tested with up to 100 ports with 1000 frequency points. ([#789])

* Fix propagation of initial solution to non-linear components. ([PR#206])

* Expand number of branches on equation defined device (EDD) to 20 sections. ([#658])

* Fix bug on polar() function ([#704])

* Fix crash when using glibc-2.26 ([#710])

* Fix crash in sortChildAnalyses, avoid removing from list while traversing. ([PR#712])

* Fix AC and SP sweep with a single value and Type=list ([#211])

* [NEW] add smooth() function, similar to what is often used in VNAs
   to remove some noise from group delay measurements. ([PR#747])

* [NEW] add groupdelay(S) function, returns the delay group between ports 1 and 2,
  and groupdelay(S, P2, P1) does the same but for ports P2 and P1. ([PR#751])

* [NEW] N-port S-parameters de-embedding ([PR#693])

Qucsconv
--------

* Fix crash when writing a MATLAB file. ([PR#758])


Qucs-Filter
-----------

* [NEW] Add two filter topologies, the quarter wave side coupled ring filter and the capacitive
  coupled shunt resonators bandpass filter ([PR#415])

[NEW] Qucs-PowerCombining
-------------------------

* [NEW] Tool for synthesizing power combining networks.
  It has some basic structures (Wilkinson, branchline, Gysel, travelling wave,... ). ([PR#509])

Qucs-Transcalc
--------------

* [NEW] Add stripline analysis/synthesis capability. ([#661])

Qucs-ActiveFilter
--------------

* [NEW] Add Legendre filter synthesis capability. ([PR#688])


Miscellaneous
-------------

* Removal of Qucs-Help application. Redirect to <http://qucs-help.readthedocs.io/> pages. ([PR#669])

* Removal of Qucs-Edit application. ([PR#652])

* Restructure Qucs libraries. Create new shared library libqucsschematic. ([PR#727])

* Rename Qucsator library libqucs to libqucsator.([PR#727])

* Add support to Qucs for unit tests using QtTest.([PR#727])


---

Qucs 0.0.19 Release Notes
=========================

This release brings improvements on many parts of Qucs.
The port from Qt3 to Qt4 is still ongoing and many bugs and usability issues have been fixed in the process.
A few new features were added and others are in development.
See below a short summary of the enhancements.

Qucs
----

* Update translations for various languages

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

* [NEW] Enable use of `QUCSATOR` environment variable to override `qucsator` ([PR#209])

* Fix bug on deactivation (short-circuiting) a component ([#216])

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

* Changing in Qucs Home no longer requires a restart ([PR#456])

* Fix the "Open recent Files" which was not updated after save-as ([PR#444])

* Fix issue that allowed to create projects with an empty name ([#311])

* Update authors and date in the About dialog ([PR#427])

* Remove non-GPL Verilog-AMS models: HICUM, BSIM, FBH_HBT ([#421])

* Fix typo in Voltage Controlled Resistor ([PR#406])

* Fix keypad up/down on component dialog ([#319])

* Fix crash when reloading Verilog-A symbols ([#478])

* Fix matching circuit tool number conversion ([PR#482])

* Fix the handling of project names containing `_prj` in them ([PR#486])

* Remove usage of LANGUAGEDIR, LIBRARYDIR variables ([PR#501], [PR#508])

* Enable loading unknown components, add sub-circuit place hoder ([PR#507])

* Fix "Export as image" behavior that was clipping markers on selected diagrams ([PR#523])

* Improve handling of Octave ([PR#530])
  - removal of hardcoded path
  - override environment variable renamed from OCTAVEBINDIR to QUCS_OCTAVE

* Fix "Move Up/Down" buttons greyed out in the equation property window ([PR#548])

* Fix Component search selecting wrong component ([#543])

* Fix cross-hair behavior of insert wire mode, related to zoom/scrool and RMB click ([PR#504])

Qucsator
--------

* Fix segmentation fault during noise enabled S-parameter simulation ([#133])

* Fix regression on tswitch component ([#34], [PR#203])

* Fix coplanar waveguide formulae ([SF#183], [PR#306])

* Fix several segmentation faults during fuzzed input ([PR#274])

* Remove null and free checks ([PR#267])

* Adopt to standard library in several classes.

* Enable gcov for coverage

* Fix S-parameters renormalization formula ([PR#481])

* Remove non-GPL Verilog-AMS models ([#421])

* Several fixes concerning rectangular waveguides ([PR#349])

* Correct CPW formulae in QucsTranscalc and qucsator ([PR#306])

* Fix regression on EDD component ([#296])

* Fix several segmentation faults, caught by AFL (american fuzzy lop) ([PR#274])

* Fix excess phase calculations on BJT device ([PR#493])

* Fix regression on SVD calculation ([PR#493])

* Fix turn off behavior of thyristor device when reversed biased ([PR#493])

* Require automake 1.12 for .ypp->.hpp rules ([PR#515])

* Disable parallel build of Verilo-A modules, temporary fix to avoid race condition ([PR#575])

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

* Fix erroneous preview of symbols when using the searchbox ([#550])

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

* Fix out-of-tree and parallel build for LaTeX documentation in qucs-doc directory ([PR#524])

Translations
----------

* Updated DE ([PR#128])

* Setup Transifex for Qucs desktop applications ([PR#357])


Port
----

* Ported Diagram classes from Qt3PtrList to QList

* Port Qt3PtrList to QVector on Diagrans, amongst other improvements

* Ported Component member variables Lines, Arcs, Rects, Ellips, Texts from Qt3PtrList to QList


Miscellaneous
------------

* new: top VERSION files shall be used for versioning (Autotools, CMake). TODO, package versioning.

* new: add tag with the latest Git commit hash: Qucs (About and -v), Qucsator, Qucsconv

* Move from math.h to cmath.h ([PR#168])

* Corrected behavior of `--with-mkadms` ([PR#436])

* Remove ADMS as submodule

* Add support for preferred number series with ASCO, fix variables search string ([PR#412])

* Fix Mac OSX uninstall script ([PR#414])

* Integrate qucs-test into qucs package ([PR#389])

* New top-level make, make check, make distcheck

* Simplify Verilog-A compilation rules

* Enable continuous automated builds (AppVeyor) of Windows binary packages, for test purposes ([PR#561])


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
   on a Octave library of plotting functions for Qucs post simulation data visualization)

 * First steps in developing a combined Qucs/Python Qucs simulation capability with
   high quality date visualization using matplolib. Applications include basic sensitivity analysis
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


Miscellaneous
------------

 * Project website is now under version control.

 * Source code is now under Travis continuous integration service.

 * Source code documentation (Doxygen) available on the website.

 * Preliminary regression and test suite (qucs-test)


Bug fixes
---------

 * Hundreds of bug fixes and code improvements.

<!-- Cross reference generated with NEWS_crossref.py -->
[#30]: https://github.com/Qucs/qucs/issues/30
[#710]: https://github.com/Qucs/qucs/issues/710
[#31]: https://github.com/Qucs/qucs/issues/31
[#467]: https://github.com/Qucs/qucs/issues/467
[#32]: https://github.com/Qucs/qucs/issues/32
[#445]: https://github.com/Qucs/qucs/issues/445
[#15]: https://github.com/Qucs/qucs/issues/15
[#311]: https://github.com/Qucs/qucs/issues/311
[#74]: https://github.com/Qucs/qucs/issues/74
[#882]: https://github.com/Qucs/qucs/issues/882
[#112]: https://github.com/Qucs/qucs/issues/112
[#310]: https://github.com/Qucs/qucs/issues/310
[#43]: https://github.com/Qucs/qucs/issues/43
[#73]: https://github.com/Qucs/qucs/issues/73
[#211]: https://github.com/Qucs/qucs/issues/211
[#319]: https://github.com/Qucs/qucs/issues/319
[#204]: https://github.com/Qucs/qucs/issues/204
[#57]: https://github.com/Qucs/qucs/issues/57
[#34]: https://github.com/Qucs/qucs/issues/34
[#133]: https://github.com/Qucs/qucs/issues/133
[#764]: https://github.com/Qucs/qucs/issues/764
[#421]: https://github.com/Qucs/qucs/issues/421
[#296]: https://github.com/Qucs/qucs/issues/296
[#661]: https://github.com/Qucs/qucs/issues/661
[#38]: https://github.com/Qucs/qucs/issues/38
[#550]: https://github.com/Qucs/qucs/issues/550
[#55]: https://github.com/Qucs/qucs/issues/55
[#658]: https://github.com/Qucs/qucs/issues/658
[#335]: https://github.com/Qucs/qucs/issues/335
[#39]: https://github.com/Qucs/qucs/issues/39
[#54]: https://github.com/Qucs/qucs/issues/54
[#300]: https://github.com/Qucs/qucs/issues/300
[#789]: https://github.com/Qucs/qucs/issues/789
[#704]: https://github.com/Qucs/qucs/issues/704
[#272]: https://github.com/Qucs/qucs/issues/272
[#253]: https://github.com/Qucs/qucs/issues/253
[#543]: https://github.com/Qucs/qucs/issues/543
[#216]: https://github.com/Qucs/qucs/issues/216
[#478]: https://github.com/Qucs/qucs/issues/478
[#132]: https://github.com/Qucs/qucs/issues/132
[PR#306]: https://github.com/Qucs/qucs/pull/306
[PR#375]: https://github.com/Qucs/qucs/pull/375
[PR#209]: https://github.com/Qucs/qucs/pull/209
[PR#507]: https://github.com/Qucs/qucs/pull/507
[PR#575]: https://github.com/Qucs/qucs/pull/575
[PR#701]: https://github.com/Qucs/qucs/pull/701
[PR#86]: https://github.com/Qucs/qucs/pull/86
[PR#232]: https://github.com/Qucs/qucs/pull/232
[PR#657]: https://github.com/Qucs/qucs/pull/657
[PR#252]: https://github.com/Qucs/qucs/pull/252
[PR#292]: https://github.com/Qucs/qucs/pull/292
[PR#416]: https://github.com/Qucs/qucs/pull/416
[PR#682]: https://github.com/Qucs/qucs/pull/682
[PR#523]: https://github.com/Qucs/qucs/pull/523
[PR#509]: https://github.com/Qucs/qucs/pull/509
[PR#412]: https://github.com/Qucs/qucs/pull/412
[PR#666]: https://github.com/Qucs/qucs/pull/666
[PR#357]: https://github.com/Qucs/qucs/pull/357
[PR#715]: https://github.com/Qucs/qucs/pull/715
[PR#482]: https://github.com/Qucs/qucs/pull/482
[PR#124]: https://github.com/Qucs/qucs/pull/124
[PR#153]: https://github.com/Qucs/qucs/pull/153
[PR#206]: https://github.com/Qucs/qucs/pull/206
[PR#669]: https://github.com/Qucs/qucs/pull/669
[PR#389]: https://github.com/Qucs/qucs/pull/389
[PR#458]: https://github.com/Qucs/qucs/pull/458
[PR#85]: https://github.com/Qucs/qucs/pull/85
[PR#501]: https://github.com/Qucs/qucs/pull/501
[PR#98]: https://github.com/Qucs/qucs/pull/98
[PR#371]: https://github.com/Qucs/qucs/pull/371
[PR#414]: https://github.com/Qucs/qucs/pull/414
[PR#515]: https://github.com/Qucs/qucs/pull/515
[PR#436]: https://github.com/Qucs/qucs/pull/436
[PR#168]: https://github.com/Qucs/qucs/pull/168
[PR#349]: https://github.com/Qucs/qucs/pull/349
[PR#652]: https://github.com/Qucs/qucs/pull/652
[PR#444]: https://github.com/Qucs/qucs/pull/444
[PR#456]: https://github.com/Qucs/qucs/pull/456
[PR#693]: https://github.com/Qucs/qucs/pull/693
[PR#688]: https://github.com/Qucs/qucs/pull/688
[PR#548]: https://github.com/Qucs/qucs/pull/548
[PR#712]: https://github.com/Qucs/qucs/pull/712
[PR#708]: https://github.com/Qucs/qucs/pull/708
[PR#493]: https://github.com/Qucs/qucs/pull/493
[PR#481]: https://github.com/Qucs/qucs/pull/481
[PR#727]: https://github.com/Qucs/qucs/pull/727
[PR#128]: https://github.com/Qucs/qucs/pull/128
[PR#504]: https://github.com/Qucs/qucs/pull/504
[PR#654]: https://github.com/Qucs/qucs/pull/654
[PR#87]: https://github.com/Qucs/qucs/pull/87
[PR#530]: https://github.com/Qucs/qucs/pull/530
[PR#508]: https://github.com/Qucs/qucs/pull/508
[PR#561]: https://github.com/Qucs/qucs/pull/561
[PR#524]: https://github.com/Qucs/qucs/pull/524
[PR#758]: https://github.com/Qucs/qucs/pull/758
[PR#274]: https://github.com/Qucs/qucs/pull/274
[PR#95]: https://github.com/Qucs/qucs/pull/95
[PR#203]: https://github.com/Qucs/qucs/pull/203
[PR#751]: https://github.com/Qucs/qucs/pull/751
[PR#29]: https://github.com/Qucs/qucs/pull/29
[PR#303]: https://github.com/Qucs/qucs/pull/303
[PR#747]: https://github.com/Qucs/qucs/pull/747
[PR#207]: https://github.com/Qucs/qucs/pull/207
[PR#486]: https://github.com/Qucs/qucs/pull/486
[PR#399]: https://github.com/Qucs/qucs/pull/399
[PR#427]: https://github.com/Qucs/qucs/pull/427
[PR#415]: https://github.com/Qucs/qucs/pull/415
[PR#156]: https://github.com/Qucs/qucs/pull/156
[PR#406]: https://github.com/Qucs/qucs/pull/406
[PR#267]: https://github.com/Qucs/qucs/pull/267
[SF#171]: https://sourceforge.net/p/qucs/bugs/171
[SF#183]: https://sourceforge.net/p/qucs/bugs/183
