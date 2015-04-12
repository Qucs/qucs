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
        void resetValue();
        virtual ~tunerElement();
    signals:
        void elementValueUpdated();
        void removeElement(tunerElement*);

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
        void updateProperty(double v);

    private slots:
        void slotSliderValueChanged(int);
        void slotMinValueChanged();
        void slotMaxValueChanged();
        void slotStepChanged();
        void slotValueChanged();
        void slotDelete();

};

class TunerDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TunerDialog(QWidget *parent = 0);
    virtual ~TunerDialog();
    void addTunerElement(tunerElement *element);
    bool containsProperty(Property *prop);

signals:
    void addTunerElement(Element *e);
public slots:

private:
    QPushButton *closeButton;
    QGridLayout *gbox;
    QList<tunerElement*> *currentElements;
    QList<Property*> *currentProps;
    void closeEvent(QCloseEvent *event);

private slots:
    void slotElementValueUpdated();
    void slotRemoveTunerElement(tunerElement*);
    void slotUpdateValues();
    void slotResetValues();
};

#endif // TUNER_H
