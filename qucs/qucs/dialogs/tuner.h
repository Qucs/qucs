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
    protected:
    private:
        Property* prop;
        QString originalValue;
        QSlider *slider;
        QTextEdit *maximum;
        QTextEdit *minimum;
        QTextEdit *value;
    private slots:
        void slotSliderValueChanged(int);
        void slotMinValueChanged();
        void slotMaxValueChanged();

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

};

#endif // TUNER_H
