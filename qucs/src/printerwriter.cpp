/*
 * printerwriter.cpp - implementation of writer to printer
 *
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
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
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "printerwriter.h"
#include "schematic_doc.h"
#include "textdoc.h"
#include "qucs.h"

#include <QPrinter>
#include <QPainter>
#include <QPrintDialog>

PrinterWriter::PrinterWriter()
{
  //default setting
  Printer = new QPrinter(QPrinter::HighResolution);

  Printer->setPageSize(QPageSize(QPageSize::A4));
  Printer->setColorMode(QPrinter::Color);
  Printer->setFullPage(true);

  fitToPage = false;
}

PrinterWriter::~PrinterWriter()
{
  delete Printer;
}

//allow user pass parameter and print document
void PrinterWriter::noGuiPrint(
		SchematicDoc* doc, QString printFile,
		QString page, int dpi, QString color,
		QString orientation)
{
  //set property
  Printer->setOutputFileName(printFile);

  //page size
  if (page == "A3") {
    Printer->setPageSize(QPageSize(QPageSize::A3));
  } else if (page == "B4") {
    Printer->setPageSize(QPageSize(QPageSize::B4));
  } else if (page == "B5") {
    Printer->setPageSize(QPageSize(QPageSize::B5));
  } else {
    Printer->setPageSize(QPageSize(QPageSize::A4));
  }
  //dpi
  Printer->setResolution(dpi);
  //color
  if (color == "BW") {
    Printer->setColorMode(QPrinter::GrayScale);
  } else {
    Printer->setColorMode(QPrinter::Color);
  }
  //orientation
  if (orientation == "landscape") {
    Printer->setPageOrientation(QPageLayout::Orientation::Landscape);
  } else {
    Printer->setPageOrientation(QPageLayout::Orientation::Portrait);
  }
  QPainter Painter(Printer);
  if(!Painter.device()) {      // valid device available ?
    return;
  }

  doc->print(Printer, &Painter,
    Printer->printRange() == QPrinter::AllPages, fitToPage);
}

void PrinterWriter::print(QucsDoc *doc)
{
	(void)doc;
  QPrintDialog *dialog = new QPrintDialog(Printer, 0);
  dialog->setOption(QAbstractPrintDialog::PrintSelection, true);
  dialog->setOption(QAbstractPrintDialog::PrintPageRange, false);
  dialog->setOption(QAbstractPrintDialog::PrintToFile, true);
  dialog->setWindowTitle(QObject::tr("Print Document"));

#if 0
  if (QucsApp::isTextDocument(doc)) {
    if (dialog->exec() == QDialog::Accepted) {
       static_cast<QPlainTextEdit *>(doc)->print(Printer);
    }
  } else {
    Printer->setOrientation(QPrinter::Landscape);

    if (dialog->exec() == QDialog::Accepted)
    {
      QPainter Painter(Printer);
      if(!Painter.device()) {     // valid device available ?
        delete dialog;
        return;
      }
      for (int z = Printer->numCopies(); z > 0; --z) {
        if (Printer->printerState() == QPrinter::Aborted) {
          break;
        }

        static_cast<SchematicDoc *>(doc)->print(Printer, &Painter,
                Printer->printRange() == QPrinter::AllPages, fitToPage);
        if (z > 1 && !Printer->newPage()) {
          delete dialog;
          return;
        }
      }
    }
  }
#endif
  delete dialog;
}
