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
    : QWidget(parent)
{
    int index, row, column, rowSpan, colSpan;
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
    step = new QLineEdit();
    gbox->addWidget(step);

    QLabel *Label1 = new QLabel(tr("Value"));
    gbox->addWidget(Label1);
    value = new QLineEdit();
    originalValue = prop->Value.copy();
    gbox->addWidget(value);

    QLabel *Label2 = new QLabel(tr("Minimum"));
    gbox->addWidget(Label2);
    minimum = new QLineEdit();
    gbox->addWidget(minimum);

    QLabel *Label3 = new QLabel(tr("Maximum"));
    gbox->addWidget(Label3);
    maximum = new QLineEdit();
    gbox->addWidget(maximum);

    QPushButton *remove = new QPushButton(tr("Remove"), this);
    gbox->addWidget(remove);


    QStringList lst = prop->Value.split(' ');
    unit = lst.last();

    if (lst.count() == 1)
    {
        // Probably a value without unit (like Epsilon r)
        // or a value with unit but without space
        // TODO: needs to be handled
    }

    index = gbox->indexOf(value);
    gbox->getItemPosition(index, &row, &column, &rowSpan, &colSpan);
    QLabel *unitLabel = new QLabel(unit, this);
    gbox->addWidget(unitLabel, row, column+1, 1, 1); // Value

    QPushButton *up = new QPushButton(tr("Up"), this);
    gbox->addWidget(up, 1, gbox->columnCount() -1, 1, 1);
    QPushButton *down = new QPushButton(tr("Down"), this);
    gbox->addWidget(down, 2, gbox->columnCount() - 1, 1, 1);

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

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderValueChanged(int)));
    connect(maximum, SIGNAL(editingFinished()), this, SLOT(slotMaxValueChanged()));
    connect(minimum, SIGNAL(editingFinished()), this, SLOT(slotMinValueChanged()));
    connect(step, SIGNAL(editingFinished()), this, SLOT(slotStepChanged()));
    connect(value, SIGNAL(editingFinished()), this, SLOT(slotValueChanged()));
    connect(remove, SIGNAL(released()), this, SLOT(slotDelete()));
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
    slider->setMaximum(numericValue*10);
}

void tunerElement::slotMinValueChanged()
{
    double numericValue = minimum->text().split(' ').first().toDouble();
    slider->setMinimum(numericValue*10);
}

void tunerElement::slotStepChanged()
{
    bool ok;
    double stepValue = step->text().toDouble(&ok);

    if (!ok)
        return;
    if (stepValue*10 > slider->maxValue())
    {
        maximum->setText(QString::number(stepValue*10));
    }
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

void tunerElement::slotDelete()
{
    this->setVisible(false);
    emit removeElement(this);
}

tunerElement::~tunerElement()
{
    //dtor
}

/*
 *
 *
 * TUNER Dialog Class
 * contains tunerElements
 *
 *
 *
 */

TunerDialog::TunerDialog(QWidget *parent) :
    QDialog(parent)
{
    this->setName("Tuner");
    this->setCaption("Tuner");
    gbox = new QGridLayout();
    this->setLayout(gbox);

    currentElements = new QList<tunerElement*>();
    currentProps = new QList<Property*>();

    closeButton = new QPushButton("Close", this);
    QPushButton *updateValues = new QPushButton("Update Values", this);
    QPushButton *resetValues = new QPushButton("Reset Values", this);
    gbox->addWidget(resetValues);
    gbox->addWidget(updateValues);
    gbox->addWidget(closeButton);

    connect(closeButton, SIGNAL(released()), this, SLOT(close()));
    connect(resetValues, SIGNAL(released()),this, SLOT(slotResetValues()));
}

bool TunerDialog::containsProperty(Property* prop)
{
    if (currentProps->contains(prop))
        return true;
    else
        return false;
}

void TunerDialog::addTunerElement(tunerElement *element)
{
    if (!element)
        return;

    connect(element, SIGNAL(elementValueUpdated()), this, SLOT(slotElementValueUpdated()));
    connect(element, SIGNAL(removeElement(tunerElement*)), this, SLOT(slotRemoveTunerElement(tunerElement*)));

    int column = gbox->columnCount();

    if (!currentProps->contains(element->getElementProperty()))
    {
        gbox->addWidget(element, 0, column, gbox->rowCount(), 1);
        currentProps->append(element->getElementProperty());
    }
    else
    {
        QMessageBox::message("Already Found", "Already tuning this element", "OK");
    }

}

void TunerDialog::slotRemoveTunerElement(tunerElement *e)
{
    currentProps->removeAll(e->getElementProperty());
    delete e;
    this->adjustSize();
    this->update();

}

void TunerDialog::slotElementValueUpdated()
{
    QucsMain->slotSimulate();
}

void TunerDialog::slotResetValues()
{
    for (int i = 0; i < currentElements->count(); i++)
    {
        currentElements->at(i)->resetValue();
    }
}

void TunerDialog::slotUpdateValues()
{}

void TunerDialog::slotResetTunerDialog()
{
    // Document closed. Reset tuner
    for (int i = 0; i < gbox->count(); i++)
    {
        slotRemoveTunerElement((tunerElement*)gbox->itemAt(i));
    }
}

void TunerDialog::closeEvent(QCloseEvent *event)
{
    //Undo changes to mouse actions when closing tuner window
    QucsMain->MousePressAction = &MouseActions::MPressSelect;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
    event->accept();
}

TunerDialog::~TunerDialog()
{

}
