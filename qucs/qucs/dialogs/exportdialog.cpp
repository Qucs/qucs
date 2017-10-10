/***************************************************************************
                               exportdiagramdialog.cpp
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
#include <cmath>
#include "exportdialog.h"

#include <QValidator>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileInfo>
#include <QFileDialog>

ExportDialog::ExportDialog(int w, int h, int wsel, int hsel, QString filename_, bool nosel_, QWidget *parent) :
    QDialog(parent)
{

    setWindowTitle(tr("Export graphics"));
    dwidth = w;
    dheight = h;
    dwidthsel = wsel;
    dheightsel = hsel;
    svg = false;
    noselected = nosel_;

    filename = filename_;

    lblFilename = new QLabel(tr("Save to file (Graphics format by extension)"));
    lblResolutionX = new QLabel(tr("Width in pixels"));
    lblResolutionY = new QLabel(tr("Height in pixels"));
    lblRatio = new QLabel(tr("Scale factor: "));
    lblFormat = new QLabel(tr("Image format:"));

    ExportButt = new QPushButton(tr("Export"));
    connect(ExportButt,SIGNAL(clicked()),this,SLOT(accept()));
    CancelButt = new QPushButton(tr("Cancel"));
    connect(CancelButt,SIGNAL(clicked()),this,SLOT(reject()));
    SaveButt = new QPushButton(tr("Browse"));
    connect(SaveButt,SIGNAL(clicked()),this,SLOT(setFileName()));

    editFilename = new QLineEdit(filename);
    connect(editFilename,SIGNAL(textChanged(QString)),this,SLOT(setSvg(QString)));

    editResolutionX = new QLineEdit(QString::number(dwidth));
    QIntValidator *val = new QIntValidator(0,64000,this);
    editResolutionX->setValidator(val);
    editResolutionX->setEnabled(false);
    editResolutionY = new QLineEdit(QString::number(dheight));
    editResolutionY->setValidator(val);
    editResolutionY->setEnabled(false);
    editScale = new QLineEdit(QString::number(1.0));
    QDoubleValidator *val1 = new QDoubleValidator(0,20.0,2,this);
    editScale->setValidator(val1);

    cbxImgType = new QComboBox(this);
    QStringList lst;
    lst<<tr("Colour")<<tr("Monochrome");
    cbxImgType->addItems(lst);

    cbRatio = new QCheckBox(tr("Original width to height ratio"));
    cbRatio->setChecked(true);
    connect(cbRatio,SIGNAL(toggled(bool)),this,SLOT(recalcRatio()));

    cbResolution = new QCheckBox(tr("Original size"));
    connect(cbResolution,SIGNAL(toggled(bool)),editResolutionX,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),editResolutionY,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),cbRatio,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),editScale,SLOT(setDisabled(bool)));
    connect(cbResolution,SIGNAL(toggled(bool)),this,SLOT(restoreOriginalWtoH()));

    connect(editResolutionX,SIGNAL(textEdited(QString)),this,SLOT(calcHeight()));
    connect(editResolutionY,SIGNAL(textEdited(QString)),this,SLOT(calcWidth()));
    connect(editScale,SIGNAL(textChanged(QString)),this,SLOT(recalcScale()));

    cbSelected = new QCheckBox(tr("Export selected only"));
    connect(cbSelected,SIGNAL(toggled(bool)),this,SLOT(setSelectedWH()));
    cbSelected->setChecked(false);
    if (noselected) cbSelected->setDisabled(true);
    
    cbResolution->setChecked(true);
    //cbResolution->setEnabled(false);
    cbRatio->setEnabled(false);

    top = new QVBoxLayout;
    lower1 = new QHBoxLayout;
    lower2 = new QHBoxLayout;
    lower3 = new QHBoxLayout;
    lower4 = new QHBoxLayout;

    top->addWidget(lblFilename);
    lower1->addWidget(editFilename);
    lower1->addWidget(SaveButt);
    top->addLayout(lower1);
    lower4->addWidget(lblFormat);
    lower4->addWidget(cbxImgType);
    top->addLayout(lower4);
    top->addWidget(cbResolution);
    //top->addWidget(cbRatio);
    lower3->addWidget(lblRatio);
    lower3->addWidget(editScale);
    top->addLayout(lower3);
    top->addWidget(lblResolutionX);
    top->addWidget(editResolutionX);
    top->addWidget(lblResolutionY);
    top->addWidget(editResolutionY);
    top->addWidget(cbSelected);

    lower2->addWidget(ExportButt);
    lower2->addWidget(CancelButt);
    top->addLayout(lower2);
    this->setLayout(top);

    this->layout()->setSizeConstraint(QLayout::SetFixedSize);
    this->setWindowTitle(tr("Export schematic to raster or vector image"));

    this->setSvg(editFilename->text());
}

QString ExportDialog::FileToSave()
{
    return editFilename->text();
}

bool ExportDialog::isOriginalSize()
{
    return cbResolution->isChecked();
}

int ExportDialog::Xpixels()
{
    return editResolutionX->text().toInt();
}

int ExportDialog::Ypixels()
{
    return editResolutionY->text().toInt();
}

void ExportDialog::setFileName()
{
  QString selectedFilter;
  QString currentExtension;
  QString filterExtension;

  QString fileName = QFileDialog::getSaveFileName(this, tr("Export Schematic to Image"),
      editFilename->text(),
      "PNG images (*.png);;"
      "JPEG images (*.jpg *.jpeg);;"
      "SVG vector graphics (*.svg);;"
      "PDF (*.pdf);;"
      "PDF + LaTeX (*.pdf_tex);;"
      "EPS Encapsulated Postscript (*.eps)",
      &selectedFilter);

  if (fileName.isEmpty()) return;

  if (selectedFilter.contains("*.png", Qt::CaseInsensitive)) filterExtension = QString(".png");
  if (selectedFilter.contains("*.jpg", Qt::CaseInsensitive)) filterExtension = QString(".jpg");
  if (selectedFilter.contains("*.svg", Qt::CaseInsensitive)) filterExtension = QString(".svg");
  if (selectedFilter.contains("*.pdf", Qt::CaseInsensitive)) filterExtension = QString(".pdf");
  if (selectedFilter.contains("*.pdf_tex", Qt::CaseInsensitive)) filterExtension = QString(".pdf_tex");
  if (selectedFilter.contains("*.eps", Qt::CaseInsensitive)) filterExtension = QString(".eps");

  QFileInfo fileInfo(fileName);

  currentExtension = fileInfo.suffix();

  QString allowedExtensions = "png;jpg;jpeg;svg;pdf;pdf_tex;eps";
  QStringList extensionsList = allowedExtensions.split (';');

  if (currentExtension.isEmpty()) {
    fileName.append(filterExtension);
  } else {
    if (!extensionsList.contains(currentExtension)) {
      fileName.append(filterExtension);
    }
  }

  editFilename->setText(fileName);
}

void ExportDialog::calcWidth()
{
    if (cbRatio->isChecked()) {
        float h = editResolutionY->text().toFloat();
        float w =  (h*dwidth)/dheight;
        editResolutionX->setText(QString::number(w, 'f', 0)); // integer number of pixels
    }
}


void ExportDialog::calcHeight()
{
    if (cbRatio->isChecked()) {
        float w = editResolutionX->text().toFloat();
        float h =  (w*dheight)/dwidth;
        editResolutionY->setText(QString::number(h, 'f', 0)); // integer number of pixels
    }

}

void ExportDialog::recalcRatio()
{
    if (cbRatio->isChecked()) {
        calcHeight();
    }
}

void ExportDialog::restoreOriginalWtoH()
{
    if (cbResolution->isChecked()) {
      if (cbSelected->isChecked()) { // Export selected only
            editResolutionX->setText(QString::number(dwidthsel));
            editResolutionY->setText(QString::number(dheightsel));
        } else {
            editScale->setText(QString::number(1.0));
            editResolutionX->setText(QString::number(dwidth));
            editResolutionY->setText(QString::number(dheight));
        }
    }
}

bool ExportDialog::isSvg()
{
    return svg;
}

bool ExportDialog::isExportSelected()
{
    return cbSelected->isChecked();
}

void ExportDialog::setSvg(QString filename)
{
    QFileInfo graphics_file(filename);
    QString ext = graphics_file.suffix();
    ext = ext.toLower();
    if ((ext=="svg")||(ext=="pdf")||(ext=="eps")||(ext=="pdf_tex")) {
        svg = true;
        cbResolution->setChecked(true);
        cbResolution->setDisabled(true);
        cbxImgType->setDisabled(true);
        cbRatio->setChecked(true);
    } else {
        svg = false;
        cbResolution->setEnabled(true);
        cbxImgType->setEnabled(true);
    }
}

bool ExportDialog::isValidFilename()
{
    QString nam = editFilename->text();
    QStringList filetypes;
    QFileInfo inf(nam);
    filetypes<<"png"<<"svg"<<"jpeg"<<"jpg"<<"pdf"<<"pdf_tex"<<"eps"
            <<"PNG"<<"JPG"<<"SVG"<<"JPEG"<<"PDF"
            <<"PDF_TEX"<<"EPS";

    if (filetypes.contains(inf.suffix())) {
        return true;
    } else {
        return false;
    }
}

bool ExportDialog::needsInkscape()
{
    QString nam = editFilename->text();
    QStringList filetypes;
    QFileInfo inf(nam);
    filetypes<<"pdf"<<"pdf_tex"<<"eps"<<"PDF"<<"PDF_TEX"<<"EPS";

    if (filetypes.contains(inf.suffix())) {
        return true;
    } else {
        return false;
    }
}

bool ExportDialog::isPdf()
{
    QFileInfo inf(editFilename->text());
    if (inf.suffix().toLower()=="pdf") return true;
    else return false;
}

bool ExportDialog::isPdf_Tex()
{
    QFileInfo inf(editFilename->text());
    if (inf.suffix().toLower()=="pdf_tex") return true;
    else return false;
}

bool ExportDialog::isEps()
{
    QFileInfo inf(editFilename->text());
    if (inf.suffix().toLower()=="eps") return true;
    else return false;
}

void ExportDialog::setSelectedWH()
{
    if (cbSelected->isChecked()) {
        editResolutionX->setText(QString::number(dwidthsel));
        editResolutionY->setText(QString::number(dheightsel));
    } else {
        editResolutionX->setText(QString::number(dwidth));
        editResolutionY->setText(QString::number(dheight));
    }
    recalcScale();
}

void ExportDialog::setDiagram()
{
    cbSelected->setChecked(true);
    cbSelected->setDisabled(true);
    this->setWindowTitle(tr("Export diagram to raster or vector image"));
}

float ExportDialog::getScale()
{
    scale = editScale->text().toFloat();
    return scale;
}

void ExportDialog::recalcScale()
{
    scale = editScale->text().toFloat();
    if (cbSelected->isChecked()) {
        editResolutionX->setText(QString::number(scale*dwidthsel, 'f', 0));  // integer number of pixels
        editResolutionY->setText(QString::number(scale*dheightsel, 'f', 0)); // integer number of pixels
    } else {
        editResolutionX->setText(QString::number(scale*dwidth, 'f', 0));
        editResolutionY->setText(QString::number(scale*dheight, 'f', 0));
    }

}

ExportDialog::ImgFormat ExportDialog::getImgFormat()
{
    // default
    ExportDialog::ImgFormat ImgFormat = ExportDialog::Coloured;

    switch (cbxImgType->currentIndex()) {
    case 0 :
        ImgFormat = ExportDialog::Coloured;
        break;
    case 1 :
        ImgFormat = ExportDialog::Monochrome;
        break;
    default : break;
    }

    return ImgFormat;
}
