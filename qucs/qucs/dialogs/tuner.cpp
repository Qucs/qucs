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
#include "tuner.h"

#include "qucs.h"
#include "mouseactions.h"
#include "components/component.h"

#include <QWidget>
#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QMessageBox>
#include <QDebug>

tunerElement::tunerElement(QWidget *parent, Component *component, int selectedPropertyId)
    : QWidget()
{

    //ctor

    prop = component->Props.at(selectedPropertyId);
    QGridLayout *gbox = new QGridLayout();
    this->setLayout(gbox);
    QLabel *tunerName = new QLabel(component->Name + ":" + prop->Name);
    gbox->addWidget(tunerName);


    slider = new QSlider(Qt::Vertical);
    gbox->addWidget(slider);
    slider->setMinimumHeight(200);

    QLabel *Label = new QLabel(tr("Step"));
    gbox->addWidget(Label);
    step = new QTextEdit();
    gbox->addWidget(step);

    QLabel *Label1 = new QLabel(tr("Value"));
    gbox->addWidget(Label1);
    value = new QTextEdit();

    originalValue = prop->Value.copy();
    gbox->addWidget(value);

    QLabel *Label2 = new QLabel(tr("Minimum"));
    gbox->addWidget(Label2);
    minimum = new QTextEdit();
    gbox->addWidget(minimum);

    QLabel *Label3 = new QLabel(tr("Maximum"));
    gbox->addWidget(Label3);
    maximum = new QTextEdit();
    gbox->addWidget(maximum);

    QStringList lst = prop->Value.split(' ');
    unit = lst.last();

    if (lst.count() == 1)
    {
        // Probably a value without unit (like Epsilon r)
        // or a value with unit but without space
        // needs to be handled
    }

    int nextColumn = gbox->columnCount();
    for (int i=gbox->rowCount()-1; i > gbox->rowCount() - 6; i = i-2 )
    {
        QLabel *label = new QLabel(unit);
        gbox->addWidget(label, i, nextColumn);
    }

    double numericValue = lst.first().toDouble();
    double maxValue = numericValue + (numericValue * 0.10);
    double minValue = numericValue - (numericValue * 0.10);

    maximum->setText(QString::number(maxValue));
    minimum->setText(QString::number(minValue));
    value->setText(QString::number(numericValue));
    step->setText(tr("1"));

    slider->setMaximum((numericValue + (numericValue * 0.10))*10);
    slider->setMinimum((numericValue - (numericValue * 0.10))*10);
    slider->setValue(numericValue*10);
    slider->setTickmarks(QSlider::Both);
    slider->setTickInterval(10);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderValueChanged(int)));
    connect(maximum, SIGNAL(textChanged()), this, SLOT(slotMaxValueChanged()));
    connect(minimum, SIGNAL(textChanged()), this, SLOT(slotMinValueChanged()));
    connect(step, SIGNAL(textChanged()), this, SLOT(slotStepChanged()));
    connect(value, SIGNAL(textChanged()), this, SLOT(slotValueChanged()));
}

Property* tunerElement::getElementProperty()
{
    return prop;
}

void tunerElement::resetValue()
{
    prop->Value = originalValue;

    value->setText(originalValue.split(' ').first());
}

void tunerElement::updateProperty(double v)
{
    prop->Value = QString::number(v).append(tr(" ") + unit);
}

void tunerElement::slotSliderValueChanged(int v)
{
    value->setText(QString::number(v/10.0));
    emit elementValueUpdated();
}

void tunerElement::slotMaxValueChanged()
{
    double numericValue = maximum->text().split(' ').first().toDouble();
    slider->setMaximum(numericValue);
}

void tunerElement::slotMinValueChanged()
{
    double numericValue = minimum->text().split(' ').first().toDouble();
    slider->setMinimum(numericValue);
}

void tunerElement::slotStepChanged()
{
    bool ok;
    double stepValue = step->text().toDouble(&ok);

    if (!ok)
        return;

    slider->setTickInterval(stepValue*10);
    slider->setSingleStep(stepValue*10);
}

void tunerElement::slotValueChanged()
{
    bool ok;
    double v = value->text().toDouble(&ok);

    if (!ok)
        return;

    slider->setValue(v*10);
    updateProperty(v);
}

tunerElement::~tunerElement()
{
    //dtor
}

//Tuner dialog class

tuner::tuner(QWidget *parent) :
    QDialog(parent)
{
    this->setName("Tuner");
    this->setCaption("Tuner");
    gbox = new QGridLayout();
    this->setLayout(gbox);

    currentElements = new Q3PtrList<tunerElement>();

    closeButton = new QPushButton("Close", this);
    QPushButton *updateValues = new QPushButton("Update Values", this);
    QPushButton *resetValues = new QPushButton("Reset Values", this);
    gbox->addWidget(resetValues);
    gbox->addWidget(updateValues);
    gbox->addWidget(closeButton);

    connect(closeButton, SIGNAL(released()), this, SLOT(close()));
    connect(resetValues, SIGNAL(released()),this, SLOT(slotResetValues()));

}
void tuner::addTunerElement(tunerElement *element)
{
    if (!element)
        return;

    connect(element, SIGNAL(elementValueUpdated()), this, SLOT(slotElementValueUpdated()));

    int column = gbox->columnCount();

    if (!currentElements->contains(element))
    {
        gbox->addWidget(element, 0, column, 1, 1);
        currentElements->append(element);
    }
    else
    {
        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Remove tuner Element", "Delete?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes)
          {
              removeTunerElement(element);
          }
    }

}

void tuner::removeTunerElement(tunerElement* element)
{
    currentElements->remove(element);
    delete element;
    this->update();
}

/*
 * Private Slots
 */

void tuner::slotElementValueUpdated()
{
    QucsMain->slotSimulate();
}

void tuner::slotResetValues()
{
    for (int i = 0; i < currentElements->count(); i++)
    {
        currentElements->at(i)->resetValue();
    }
}

void tuner::slotUpdateValues()
{}

void tuner::closeEvent(QCloseEvent *event)
{
    //Undo changes to mouse actions when closing tuner window
    QucsMain->MousePressAction = &MouseActions::MPressSelect;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
    event->accept();
}

tuner::~tuner()
{

}
