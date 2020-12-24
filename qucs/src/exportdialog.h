/***************************************************************************
                               exportdiagramdialog.h
                              ------------------
    begin                : Thu Nov 28 2013
    copyright            : (C) 2013 by Vadim Kuznetzov
    email                : <ra3xdh@gmail.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>

class QLabel;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QHBoxLayout;
class QVBoxLayout;

class ExportDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ExportDialog(int w, int h, int wsel, int hsel, QString filename_, bool nosel_=true, QWidget *parent = 0);
    
private:
    QPushButton* ExportButt{nullptr};
    QPushButton* CancelButt{nullptr};
    QPushButton* SaveButt{nullptr};

    QLabel* lblFilename{nullptr};
    QLabel* lblResolutionX{nullptr};
    QLabel* lblResolutionY{nullptr};
    QLabel* lblRatio{nullptr};
    QLabel* lblFormat{nullptr};

    QCheckBox* cbResolution{nullptr};
    QCheckBox* cbRatio{nullptr};
    QCheckBox* cbSelected{nullptr};

    QLineEdit* editFilename{nullptr};
    QLineEdit* editResolutionX{nullptr};
    QLineEdit* editResolutionY{nullptr};
    QLineEdit* editScale{nullptr};

    QComboBox* cbxImgType{nullptr};

    QHBoxLayout* lower1{nullptr};
    QHBoxLayout* lower2{nullptr};
    QHBoxLayout* lower3{nullptr};
    QHBoxLayout* lower4{nullptr};
    QVBoxLayout* top{nullptr};

    int dwidth, dheight;

    int dwidthsel, dheightsel;

    float scale;

    bool svg, noselected;

    QString filename;

public:

    enum ImgFormat {Coloured, Monochrome};

    QString FileToSave();
    bool isOriginalSize();
    bool isExportSelected();
    int Xpixels();
    int Ypixels();
    bool isSvg();
    bool needsInkscape();
    bool isValidFilename();
    bool isPdf();
    bool isPdf_Tex();
    bool isEps();
    void setDiagram();
    float getScale();

    ExportDialog::ImgFormat getImgFormat();

signals:
    
private slots:
    void setFileName();
    void calcWidth();
    void calcHeight();
    void recalcRatio();
    void restoreOriginalWtoH();
    void setSvg(QString filename);
    void setSelectedWH();
    void recalcScale();

public slots:
    
};

#endif // EXPORTDIALOG_H
