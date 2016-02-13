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
#include <QAction>
#include <QDialog>
#include <QLabel>
#include <QTextEdit>
#include <QMessageBox>
#include <QDebug>
#include <QToolButton>

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
    if (lst.count() > 1)
        unit = lst.last();
    else
        unit = ""; // No unit

    index = gbox->indexOf(value);
    gbox->getItemPosition(index, &row, &column, &rowSpan, &colSpan);
    QLabel *unitLabel = new QLabel(unit, this);
    gbox->addWidget(unitLabel, row, column+1, 1, 1); // Value

    up = new QToolButton(this, "Up");
    up->setArrowType(Qt::UpArrow);
    gbox->addWidget(up, 1, gbox->columnCount() -1, 1, 1);

    down = new QToolButton(this, "Down");
    down->setArrowType(Qt::DownArrow);
    gbox->addWidget(down, 2, gbox->columnCount() - 1, 1, 1);

    numValue = lst.first().toFloat();
    maxValue = ceil(numValue + (numValue * 0.15));
    minValue = floor(numValue - (numValue * 0.15));

    maximum->setText(QString::number(maxValue));
    minimum->setText(QString::number(minValue));
    value->setText(QString::number(numValue));
    step->setText(QString::number(numValue/10));

    slider->setRange(0,100);
    float v = (numValue - minValue) / (maxValue - minValue);
    slider->setValue(v*100);

    slider->setTickmarks(QSlider::Both);
    slider->setTickInterval(0);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slotValueChanged()));
    connect(maximum, SIGNAL(editingFinished()), this, SLOT(slotMaxValueChanged()));
    connect(minimum, SIGNAL(editingFinished()), this, SLOT(slotMinValueChanged()));
    connect(step, SIGNAL(editingFinished()), this, SLOT(slotStepChanged()));
    connect(value, SIGNAL(editingFinished()), this, SLOT(slotValueChanged()));
    connect(remove, SIGNAL(released()), this, SLOT(slotDelete()));
    connect(up, SIGNAL(clicked()), this, SLOT(slotUpClicked()));
    connect(down, SIGNAL(clicked()), this, SLOT(slotDownClicked()));
}

Property* tunerElement::getElementProperty()
{
    return prop;
}

void tunerElement::resetValue()
{
    prop->Value = originalValue;

    value->setText(originalValue.split(' ').first());
    this->slotValueChanged();
}

void tunerElement::updateProperty()
{

    prop->Value = value->text().append(tr(" ") + unit);
}

void tunerElement::slotMaxValueChanged()
{
    bool ok;
    float v = maximum->text().toFloat(&ok);

    if (!ok)
    {
        QMessageBox::warning(this, "ERROR", "Maximum value not correct", QMessageBox::Ok);
        return;
    }

    maxValue = v;
    qDebug() << "tunerElement::slotMaxValueChanged() " << v;

    updateSlider();
}

void tunerElement::slotMinValueChanged()
{
    bool ok;
    float v = minimum->text().toFloat(&ok);

    if (!ok)
    {
        QMessageBox::warning(this, "ERROR", "Minimum value not correct", QMessageBox::Ok);
        return;
    }

    minValue = v;
    qDebug() << "slotMinValueChanged() " << v;

    updateSlider();
}

void tunerElement::slotStepChanged()
{
    bool ok;
    float v = step->text().toFloat(&ok);
    float range = maxValue - minValue;

    qDebug() << "tunerElement::slotStepChanged()" << v;

    if (!ok)
    {
        QMessageBox::warning(this, "ERROR", "Entered step is not correct", QMessageBox::Ok);
        return;
    }

    slider->blockSignals(true);
    slider->setTickInterval(100 / (range/v));
    slider->blockSignals(false);
}

void tunerElement::slotValueChanged()
{
    bool ok;
    float v = value->text().toFloat(&ok);
    float slider_v = minValue + ((slider->value()/100.0) * (maxValue - minValue));
    qDebug() << "tunerElement::slotValueChanged()" << v;

    if (!ok)
    {
        QMessageBox::warning(this, "ERROR", "Entered value not correct", QMessageBox::Ok);
        return;
    }

    if (v > maxValue)
    {
        value->setText(QString::number(maxValue));
        v = maxValue;
    }
    else if (v < minValue)
    {
        value->setText(QString::number(minValue));
        v = minValue;
    }

    numValue = v;

    if ( (int)v*1000 != (int)slider_v*1000 )
        updateSlider();

    updateProperty();
    emit elementValueUpdated();
}

void tunerElement::slotUpClicked()
{
    float s = step->text().toFloat();
    qDebug() << "tunerElement::slotUpClicked() " << s;

    numValue += s;

    value->setText(QString::number(numValue, 'f', 3));

    slotValueChanged();
    updateSlider();
}

void tunerElement::slotDownClicked()
{
    float s = step->text().toFloat();
    qDebug() << "tunerElement::slotDownClicked() " << s;

    numValue -= s;
    value->setText(QString::number(numValue, 'f', 3));

    slotValueChanged();
    updateSlider();
}

void tunerElement::slotDelete()
{
    this->setVisible(false);
    emit removeElement(this);
}

void tunerElement::updateSlider()
{
    float v = (numValue - minValue) / (maxValue - minValue);
    float s = step->text().toFloat();
    slider->blockSignals(true);
    slider->setValue(v*100);
    slider->setTickInterval(100 / ((maxValue - minValue)/s));
    slider->blockSignals(false);
}

tunerElement::~tunerElement()
{
    //dtor
}

/* \brief
 * TUNER Dialog Class
 *
 *
 * Tuner dialog contains tuner elements
 * Updates property values
 * Resets values
 *
 */

TunerDialog::TunerDialog(QWidget *parent) :
    QDialog(parent)
{
    qDebug() << "Tuner::TunerDialog";
    this->setName("Tuner");
    this->setCaption("Tuner");
    gbox = new QGridLayout();
    this->setLayout(gbox);

    currentElements = new QList<tunerElement*>();
    currentProps = new QList<Property*>();

    closeButton = new QPushButton("Close", this);
    QPushButton *updateValues = new QPushButton("Update Values", this);
    QPushButton *resetValues = new QPushButton("Reset Values", this);

    info = new QLabel(this);
    gbox->addWidget(resetValues);
    gbox->addWidget(updateValues);
    gbox->addWidget(closeButton);
    gbox->addWidget(info);
    info->setAlignment(Qt::AlignmentFlag::AlignBottom);

    info->setText("Please select a component to tune");
    valuesUpated = false;
    connect(closeButton, SIGNAL(released()), this, SLOT(close()));
    connect(resetValues, SIGNAL(released()),this, SLOT(slotResetValues()));
    connect(updateValues, SIGNAL(released()), this, SLOT(slotUpdateValues()));
}

void TunerDialog::infoMsg(const QString msg)
{
    info->clear();
    info->setText(msg);
}

bool TunerDialog::containsProperty(Property* prop)
{
    if (currentProps->contains(prop)) return true;
    else return false;
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
        currentElements->append(element);
        this->infoMsg(QString("Added element\r\n"));
    }
    else
    {
        QMessageBox::message("Already Found", "Already tuning this element", "OK");
    }

}

void TunerDialog::slotRemoveTunerElement(tunerElement *e)
{
    qDebug() << "Tuner::slotRemoveTunerElement()";
    this->infoMsg("Removed element from Tuner");
    currentProps->removeAll(e->getElementProperty());
    currentElements->removeAll(e);
    this->infoMsg(QString("Removed element: ") + e->name());
    delete e;
    this->adjustSize();
    this->update();
}

void TunerDialog::slotElementValueUpdated()
{
    qDebug() << "Tuner::slotElementValueUpdated()";

    this->setEnabled(false);
    QucsMain->slotSimulate();
}

void TunerDialog::slotSimulationEnded()
{
    qDebug() << "Tuner::slotSimulationEnded()";

    this->setEnabled(true);
}

void TunerDialog::slotResetValues()
{
    qDebug() << "Tuner::slotResetValues()";
    for (int i = 0; i < currentElements->count(); i++)
    {
        currentElements->at(i)->resetValue();
    }
}

void TunerDialog::slotUpdateValues()
{
    qDebug() << "Tuner::slotUpdateValues()";
    for (int i = 0; i < currentElements->count(); i++)
    {
        qDebug() << "Tuner::slotUpdateValues()::updateProperty()";
        currentElements->at(i)->updateProperty();
    }
    this->infoMsg("Updated Schematic values");
    valuesUpated = true;
}

void TunerDialog::slotResetTunerDialog()
{
    // Document closed. Reset tuner
    qDebug() << "Tuner::slotResetTunerDialog()";
    infoMsg("Document closed");
    for (int i = 0; i < currentElements->count(); i++)
    {
       qDebug() << "Tuner::slotResetTunerDialog()::delete";
       delete currentElements->at(i);
       currentElements->removeAt(i);
    }
    this->update();
}

void TunerDialog::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton msg;
    if (!valuesUpated)
    {
    msg = QMessageBox::question(this, "Update values before closing?",
                                "Do you want to update the component values before closing?",
                               QMessageBox::Yes | QMessageBox::No );

    if (msg == QMessageBox::Yes)
    {
        qDebug() << "slotCloseClicked::User clicked yes";
        slotUpdateValues();
    }
    else
        slotResetValues();
    }
    this->valuesUpated = false;
    //Undo changes to mouse actions when closing tuner window
    QucsMain->MousePressAction = &MouseActions::MPressSelect;
    QucsMain->MouseReleaseAction = &MouseActions::MReleaseSelect;
    QucsMain->tune->setChecked(false);
    event->accept();
}

void TunerDialog::showEvent(QShowEvent *e)
{
    for (int i = 0; i < currentElements->count(); i++)
    {
        if (currentElements->at(i)->getElementProperty() == nullptr)
            currentElements->removeAt(i);
    }

}

TunerDialog::~TunerDialog()
{

}
