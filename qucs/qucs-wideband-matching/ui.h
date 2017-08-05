/*
 * ui.h - User interface class definition
 *
 * copyright (C) 2016 Andres Martinez-Mera <andresmartinezmera@gmail.com>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 *
 */

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
    ~ui();

private:
    QPushButton * SourceFileButton, * LoadFileButton;
    QPushButton * RunButton, *AmplifierS2Pbutton;
    QPushButton * GNUplotButton;
    QWidget * centralWidget;
    QSvgWidget *imgWidget;
    QComboBox *SearchModeCombo;
    QComboBox *minFUnitsCombo, * maxFUnitsCombo;
    QCheckBox *UseGNUplotCheckbox, *RefineCheckbox;
    QLabel *minFLabel, *maxFLabel, *SearchModeLabel, *LabelResult;
    QString  SourceFile, LoadFile, AmpFile;
    QString GNUplot_path;
    QLineEdit * minFEdit, * maxFEdit;
    QGroupBox *SourceImpGroupBox, *LoadImpGroupBox; 

    QCheckBox * FixedZLCheckbox, * FixedZSCheckbox, *TwoPortMatchingCheckbox;
    QLineEdit * FixedZSLineedit, *FixedZLLineedit;

    QLabel *ZLOhmLabel, *ZSOhmLabel;
    QLabel *SourceLabel, *LoadLabel;

    QVBoxLayout * mainLayout, * SourceLayout, * LoadLayout;
    QHBoxLayout * ConstantZSLayout, * ConstantZLLayout, *ButtonsLayout;
    QGridLayout *Impedancelayout, *vbox;
    QGridLayout *OptionsLayout;
    QGroupBox *FreqgroupBox;


 private slots:

    void go_clicked();
    void SourceImpedance_clicked();
    void LoadImpedance_clicked();
    void GNUplotOutput_clicked();
    void FixedZSCheckbox_clicked();
    void FixedZLCheckbox_clicked();
    void slotShowResult();
    void TwoPortMatchingCheckbox_state(int);
    void BrowseAmplifier_S2P();

  private:
    complex<double> getComplexImpedanceFromText(char *);
    int CheckInputText(string);
    double getFreqScale(int);
    int ResultState;
};

#endif // UI_H

