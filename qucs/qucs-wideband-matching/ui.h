#ifndef UI_H
#define UI_H

#include <QClipboard>
#include <QApplication>
#include <QGridLayout>
#include <QPixmap>
#include <QMainWindow>
#include <QGroupBox>
#include <QRadioButton>
#include <QLabel>
#include <QWidget>
#include <QApplication>
#include <QString>
#include <QPushButton>
#include <QtSvg>
#include <QObject>
#include <QFileDialog>
#include "io.h"
#include <sstream>
#include <complex>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

using namespace std;


class ui :public QMainWindow
{
     Q_OBJECT
public:
    ui();

private:
    QPushButton * SourceFileButton, * LoadFileButton;
    QPushButton * RunButton, * CancelButton;
    QPushButton * GNUplotButton;
    QPushButton * TopoScriptButton;
    QWidget * centralWidget;
    QSvgWidget *imgWidget;
    QComboBox *Topology, *SearchModeCombo;

    QComboBox *minFUnitsCombo, * maxFUnitsCombo;
    QLabel *minFLabel, *maxFLabel, *TopoScriptLabel, *SearchModeLabel;
    QString  SourceFile, LoadFile;
    QString GNUplot_path, TopoScript_path;
    QLineEdit * minFEdit, * maxFEdit;

    QCheckBox * FixedZLCheckbox, * FixedZSCheckbox;
    QLineEdit * FixedZSLineedit, *FixedZLLineedit;

    QLabel *ZLOhmLabel, *ZSOhmLabel;


 private slots:

    void go_clicked();
    void cancel_clicked();
    void SourceImpedance_clicked();
    void LoadImpedance_clicked();
    void GNUplotOutput_clicked();
    void FixedZSCheckbox_clicked();
    void FixedZLCheckbox_clicked();
    void TopoScriptButton_clicked();
    void TopoCombo_clicked(int);

    complex<double> getComplexImpedanceFromText(char *);

    double getFreqScale(int);
    QString getTopoScriptPath();
};

#endif // UI_H

