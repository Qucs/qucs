/*
 * qucspowercombiningtool.h - Power combining tool definition
 *
 * copyright (C) 2017 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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
#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <complex>
#include <QtSvg>
#include<QDebug>
#include <QDesktopWidget>
struct tSubstrate {
  double er;
  double height;
  double thickness;
  double tand;
  double resistivity;
  double roughness;
  double minWidth, maxWidth;
};


static const double Z_FIELD = 376.73031346958504364963;
static const double SPEED_OF_LIGHT = 299792458.0;
static const double pi = 3.141592653589793238462643383279502884197169399375105820974944;

/*! coth function */
static inline double coth(const double x) {
  return (1.0 + 2.0 / (exp(2.0*(x)) - 1.0));
}

/*! sech function */
static inline double sech(const double x) {
  return  (2.0 / (exp(x) + exp(-(x))));
}


/*
 References:
 [1] "High Efficiency RF and Microwave Solid State Power Amplifiers". Paolo Colantonio, 
      Franco Giannini and Ernesto Limiti. 2009. John Wiley and Sons Inc.
 [2] "RF and Microwave Transmitter Design, First Edition". Andrei Grebennikov. 2011. John Wiley and Sons Inc.
*/

class QucsPowerCombiningTool : public QMainWindow
{
  Q_OBJECT
public:
     QucsPowerCombiningTool();
    ~QucsPowerCombiningTool();
     QLabel *NLabel,*RefImp,*FreqLabel,*K1Label, *K1LabeldB, *RelPermlabel, *SubstrateHeightlabel, *SubstrateMMlabel, *ThicknessLabel,
            *ThicknessumLabel, *MinWidthLabel, *MinWidthmmLabel, *MaxWidthLabel, *MaxWidthmmLabel, *tanDLabel,
            *ResistivityLabel, *RoughnessLabel,*TopoLabel, *OhmLabel, *NStagesLabel, *AlphaLabel, *AlphadBLabel, *UnitsLabel;

     QComboBox *TopoCombo, *BranchesCombo, *FreqScaleCombo, *RelPermcomboBox, *NStagesCombo, *UnitsCombo;

     QLineEdit *RefImplineEdit, *FreqlineEdit, *K1lineEdit, *SubstrateHeightlineEdit, *ThicknesslineEdit, *MinWidthlineEdit,
                 *MaxWidthlineEdit, *tanDlineEdit, *ResistivitylineEdit, *RoughnesslineEdit, *AlphalineEdit;

     QCheckBox *AddSparcheckBox,*MicrostripcheckBox, *LumpedcheckBox;
     QPushButton *GenerateButton;
     QGroupBox *ImplementationgroupBox,*MicrostripgroupBox, *ImagegroupBox;
     QWidget *centralWidget;
     QStatusBar *statusBar;
     QGridLayout *gboxImage;
     QSvgWidget *imgWidget;

     private slots:
         void on_TopoCombo_currentIndexChanged(int index);
         double getScaleFreq();
         void getMicrostrip(double Z0, double freq, tSubstrate *substrate, double &width, double &er_eff);

         void on_GenerateButton_clicked();
         QString CalculateWilkinson(double Z0, double K);
         int Wilkinson(double Z0, double Freq, double K, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha, bool LumpedElements);
         int MultistageWilkinson(double Z0, double Freq, int NStages, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha, bool LumpedElements);
         int Tee(double Z0, double Freq, double K, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha);
         int Branchline(double Z0, double Freq, double K, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha);
         int DoubleBoxBranchline(double Z0, double Freq, double K, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha);
         int Bagley(double Z0, double Freq, int N, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha);
         int Gysel(double Z0, double Freq, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha);
         int TravellingWave(double Z0, double Freq, int N, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha);
         int Tree(double Z0, double Freq, int N, bool SP_block, bool microcheck, tSubstrate Substrate, double Alpha);
         QString calcChebyLines(double RL, double Z0, double gamma, int NStages);
         QString calcMultistageWilkinsonIsolators(double Freq, QString Zlines, double L, std::complex<double> gamma, int NStages, double Z0);
         void on_MicrostripcheckBox_clicked();
         void on_LCcheckBox_clicked();

private:
    QSize DefaultSize, ExtendedSize;
    QString ConvertLengthFromM(double);
    QString RoundVariablePrecision(double);
    QString num2str(double);

};
