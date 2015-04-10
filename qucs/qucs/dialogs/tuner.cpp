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

    originalValue = prop->Value;
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

    currentProps = new Q3PtrList<Property>();

    closeButton = new QPushButton("Close", this);
    QPushButton *updateValues = new QPushButton("Update Values", this);
    QPushButton *resetValues = new QPushButton("Reset Values", this);
    gbox->addWidget(resetValues);
    gbox->addWidget(updateValues);
    gbox->addWidget(closeButton);

    connect(closeButton, SIGNAL(released()), this, SLOT(close()));

}
void tuner::addTunerElement(tunerElement *element)
{
    if (!element)
        return;

    connect(element, SIGNAL(elementValueUpdated()), this, SLOT(slotElementValueUpdated()));

    int column = gbox->columnCount();
    Property *prop = element->getElementProperty();
    if (!currentProps->contains(prop))
    {
        gbox->addWidget(element, 0, column, 1, 1);
        currentProps->append(prop);
    }
    else
    {
        QMessageBox::StandardButton reply;
          reply = QMessageBox::question(this, "Remove tuner Element", "Delete?",
                                        QMessageBox::Yes|QMessageBox::No);
          if (reply == QMessageBox::Yes)
          {
              removeTunerElement(element);
              this->update();
          }
    }

}

void tuner::slotElementValueUpdated()
{
    QucsMain->slotSimulate();
}

void tuner::removeTunerElement(tunerElement* element)
{
    currentProps->remove(element->getElementProperty());
    delete element;
}

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
