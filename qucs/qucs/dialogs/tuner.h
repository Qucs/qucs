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
#include <QLineEdit>
#include <QString>
#include <QSlider>
#include <QLabel>
#include <QToolButton>

extern QucsApp *QucsMain;  // the Qucs application itself

class tunerElement : public QWidget
{
    Q_OBJECT
    public:
        tunerElement(QWidget *parent, Component*, int);
        Property* getElementProperty();
        void resetValue();
        void updateProperty();

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
        QLineEdit *maximum;
        QLineEdit *minimum;
        QLineEdit *value;
        QLineEdit *step;
        QToolButton *up;
        QToolButton *down;
        float minValue;
        float maxValue;
        float numValue;

    private slots:

        void slotMinValueChanged();
        void slotMaxValueChanged();
        void slotStepChanged();
        void slotValueChanged();
        void slotDelete();
        void slotDownClicked();
        void slotUpClicked();

        void updateSlider();
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
    void slotResetTunerDialog();
protected:
    virtual void showEvent(QShowEvent *);

private:
    QPushButton *closeButton;
    QGridLayout *gbox;
    QList<tunerElement*> *currentElements;
    QList<Property*> *currentProps;
    QLabel *info;

    Schematic* doc;

    void blockInput(bool enabled);
    void closeEvent(QCloseEvent *event);
    void infoMsg(const QString msg);

private slots:
    void slotElementValueUpdated();
    void slotRemoveTunerElement(tunerElement*);
    void slotUpdateValues();
    void slotResetValues();
    void slotSimulationEnded();
};

#endif // TUNER_H
