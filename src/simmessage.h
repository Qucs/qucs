/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SIMMESSAGE_H
#define SIMMESSAGE_H

#include <QDialog>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include "qucsdoc.h" // stream
#include "qio.h"
#include "simulator.h"

class QPlainTextEdit;
class QTextStream;
class QVBoxLayout;
class QPushButton;
class QProgressBar;
class QFile;

namespace qucs {

// #define SPEEDUP_PROGRESSBAR

class SimMessage : public QDialog, private SimCtrl {
Q_OBJECT
public:
  SimMessage(Simulator* sim, Doc const* doc);
 ~SimMessage();

  void startProcess(istream_t&);

signals:
  void SimulationEnded(int, SimMessage*);
  void displayDataPage(QString&, QString&);

private: // simCtrl
  void stateChange() override;
  void message(int level, std::string msg) override;

public slots:
  void slotClose();

private slots:
  void slotDisplayMsg(std::string&);
  void slotDisplayErr(std::string&);
  void slotSimEnded();
  void slotDisplayButton();
  void AbortSim();

/* #ifdef SPEEDUP_PROGRESSBAR
  void slotUpdateProgressBar();
private:
  int  iProgress;
  bool waitForUpdate;
#endif
*/

private:
  void FinishSimulation();
//  void nextSPICE();
  void startSimulator(std::string const& which);
  QPlainTextEdit *ProgText, *ErrText;
  bool           wasLF;   // linefeed for "ProgText"
  bool           simKilled; // true if simulation was aborted by the user

public:
	void do_it(istream_t, Doc&);
	void setMode(std::string const&);
	int status()const{
		return 0;
//		return _sim->status;
	}
	QWidget* docWidget() const;

signals:
  void signalData(std::string const& what);

private:
	QString        ProgressText;

	Component      *SimOpt;
	int            SimPorts;
	bool           makeSubcircuit, insertSim;

public:
  QWidget const* _docWidget;
  int showBias;
  bool SimOpenDpl;
  bool SimRunScript;
  QString DocName, DataDisplay, Script;

	QPushButton    *Display, *Abort;
	QProgressBar   *SimProgress;
	QVBoxLayout  *all;

private:
  Simulator* _simulator;
  Doc const* _doc;
  Simulator::state_t oldState;
  // std::string _what;
}; // SimMessage

} // qucs

#endif
