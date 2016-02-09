/***************************************************************************
                               tuner.cpp
                              -----------------
    begin                : Sat April 11 2015
    copyright            : (C) 2015 by Kevin Voet
    email                : kevin.voet@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef TUNER_H
#define TUNER_H

#include <QWidget>
#include "qucs.h"
#include "components/components.h"

#include <QWidget>
#include <QDialog>
#include <QPushButton>
#include <QGridLayout>
#include <QTextEdit>
#include <QString>
#include <QSlider>
#include <Q3PtrList>

extern QucsApp *QucsMain;  // the Qucs application itself

class tunerElement : public QWidget
{
    Q_OBJECT
    public:
        tunerElement(QWidget *parent, Component*, int);
        Property* getElementProperty();
        virtual ~tunerElement();
    signals:
        void elementValueUpdated();
    protected:
    private:
        Property* prop;
        QString originalValue;
        QString unit;
        QSlider *slider;
        QTextEdit *maximum;
        QTextEdit *minimum;
        QTextEdit *value;
        QTextEdit *step;
    private slots:
        void slotSliderValueChanged(int);
        void slotMinValueChanged();
        void slotMaxValueChanged();
        void slotStepChanged();
        void slotValueChanged();
};

class tuner : public QDialog
{
    Q_OBJECT
public:
    explicit tuner(QWidget *parent = 0);
    virtual ~tuner();
    void addTunerElement(tunerElement *element);

signals:
    void addTunerElement(Element *e);
public slots:

private:
    QPushButton *closeButton;
    QGridLayout *gbox;
    Q3PtrList<Property> *currentProps;
    void removeTunerElement(tunerElement *element);
    void closeEvent(QCloseEvent *event);

private slots:
    void slotElementValueUpdated();
};

#endif // TUNER_H
