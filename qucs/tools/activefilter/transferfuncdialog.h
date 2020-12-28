/***************************************************************************
                            transferfuncdialog.h
                              ----------------
    begin                : Wed Apr 10 2014
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

#ifndef TRANSFERFUNCDIALOG_H
#define TRANSFERFUNCDIALOG_H

#include <QWidget>
#include <QtGui>
#include <QDialog>

class QLabel;
class QTableWidget;
class QHBoxLayout;
class QVBoxLayout;

class TransferFuncDialog : public QDialog
{
    Q_OBJECT

private:
    QLabel *lblA{nullptr};
    QLabel *lblB{nullptr};
    QLabel *imgTrfuncEq{nullptr};
    QTableWidget *tblA{nullptr};
    QTableWidget *tblB{nullptr};

    QPushButton *btnAccept{nullptr};
    QPushButton *btnCancel{nullptr};

    QHBoxLayout *top{nullptr};
    QVBoxLayout *top1{nullptr};
    QVBoxLayout *low1{nullptr};
    QVBoxLayout *low2{nullptr};
    QHBoxLayout *low3{nullptr};

public:
    explicit TransferFuncDialog(QVector<long double> &a, QVector<long double> &b, QWidget *parent = 0);

    void getCoeffs(QVector<long double> &a, QVector<long double> &b);
    
signals:
    
private slots:
    void slotCheckCoeffs();

public slots:
    
};

#endif // TRANSFERFUNCDIALOG_H
