/***************************************************************************
                           ngspicesimdialog.h
                             ----------------
    begin                : Sun Nov 9 2014
    copyright            : (C) 2014 by Vadim Kuznetsov
    email                : ra3xdh@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EXTERNSIMDIALOG_H
#define EXTERNSIMDIALOG_H

#include <QtGui>
#include <QtCore>
#include "schematic.h"
#include "ngspice.h"
#include "xyce.h"

class ExternSimDialog : public QDialog
{
    Q_OBJECT
private:

    Schematic *Sch;

    QLabel *lblSimulator;
    QComboBox *cbxSimualor;
    QPushButton *buttonSimulate;
    QPushButton *buttonStopSim;
    QPushButton *buttonSimSettings;
    QPushButton *buttonSaveNetlist;

    QTextEdit *editSimConsole;

    QProgressBar *simProgress;

    QString workdir;

    Ngspice *ngspice;
    Xyce *xyce;

    enum Simulator {simNgspice=0, simXyceSer = 1, simXycePar =2};

public:
    explicit ExternSimDialog(Schematic *sch,QWidget *parent = 0);
    ~ExternSimDialog();
    
signals:
    
public slots:

private slots:
    void slotProcessNgspiceOutput();
    void slotProcessXyceOutput();
    void slotNgspiceStarted();
    void slotNgspiceStartError();
    void slotStart();
    void slotStop();
    void slotSetSimulator();
    void slotSimSettings();
    void slotSaveNetlist();
    
};

#endif // EXTERNSIMDIALOG_H
