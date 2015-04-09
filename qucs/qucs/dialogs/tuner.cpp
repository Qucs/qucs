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

    QLabel *Label1 = new QLabel(tr("Value"));
    gbox->addWidget(Label1);
    value = new QTextEdit();
    value->setText(prop->Value);
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

    double numericValue = value->text().split(' ').first().toDouble();
    double maxValue = numericValue + (numericValue * 0.10);
    double minValue = numericValue - (numericValue * 0.10);

    maximum->setText(QString::number(maxValue));
    minimum->setText(QString::number(minValue));

    slider->setMaximum(numericValue + (numericValue * 0.10));
    slider->setMinimum(numericValue - (numericValue * 0.10));
    slider->setValue(numericValue);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderValueChanged(int)));
    connect(maximum, SIGNAL(textChanged()), this, SLOT(slotMaxValueChanged()));
    connect(minimum, SIGNAL(textChanged()), this, SLOT(slotMinValueChanged()));
}

Property* tunerElement::getElementProperty()
{
    return prop;
}

void tunerElement::slotSliderValueChanged(int v)
{

    value->setText(QString::number(v));
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

tunerElement::~tunerElement()
{
    //dtor
    delete this;
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
    gbox->addWidget(updateValues);
    gbox->addWidget(closeButton);

    connect(closeButton, SIGNAL(released()), this, SLOT(close()));

}
void tuner::addTunerElement(tunerElement *element)
{
    if (!element)
        return;

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
          }
    }

}

void tuner::removeTunerElement(tunerElement* element)
{
    currentProps->remove(element->getElementProperty());
    gbox->remove(element);
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
