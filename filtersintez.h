#ifndef FILTERSINTEZ_H
#define FILTERSINTEZ_H

#include <QtGui>
#include <QtSvg>
#include <QMainWindow>
#include <complex>

class FilterSintez : public QMainWindow
{
    Q_OBJECT

private:
    QLabel *lblInputData;
    QLabel *lblA1;
    QLabel *lblA2;
    QLabel *lblF1;
    QLabel *lblF2;
    QLabel *lblTyp;
    QLabel *lblKv;
    QLabel *lblRpl1;
    QLabel *lblRpl2;

    QLineEdit *edtA1; // passband attenuation A1
    QLineEdit *edtA2; // stopband attenuation A2
    QLineEdit *edtF1; // passband cutoff frequency F1
    QLineEdit *edtF2; // stopband cutoff frequency F2
    QLineEdit *edtKv;
    QLineEdit *edtPassbRpl;
    QLineEdit *edtStopbRpl;

    QComboBox *cbxFilterFunc;
    QPushButton *btnCalcSchematic;

    QLabel *lblResult;
    QTextEdit *txtResult;

    QSvgWidget *imgAFR;
    QLabel *lblSch;

    QRadioButton *btnLowPass;
    QRadioButton *btnHighPass;
    QRadioButton *btnBandPass;
    QRadioButton *btnBandStop;

    QComboBox *cbxFilterType;

    QPushButton *btnElements;
    //QPushButton *btnPassive;

    QHBoxLayout *top;
    QVBoxLayout *left;
    QVBoxLayout *center;
    QVBoxLayout *right;
    
    QLabel *sch_pic;

    QWidget *zenter;

    void errorMessage(QString s);

    QVector< std::complex<float> > Poles;

    float Fc;
    int Nfil;

    QVector<float> coeffB;
    QVector<float> coeffC;


private slots:

    void slotUpdateSchematic();
    void slotCalcSchematic();

public:
    FilterSintez(QWidget *parent = 0);
    ~FilterSintez();
};

#endif // FILTERSINTEZ_H
