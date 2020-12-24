/***************************************************************************
                                textdoc.h
                               -----------
Copyright (C) 2006 by Michael Margraf <michael.margraf@alumni.tu-berlin.de>
Copyright (C) 2014 by Guilherme Brondani Torri <guitorri@gmail.com>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TEXTDOC_H
#define TEXTDOC_H

#include <QPlainTextEdit>
#include <QFont>

#include "qucsdoc.h"
#include "searchdialog.h" // BUG

/*!
 * \file textdoc.h
 * \brief Definition of the TextDoc class.
 */

class SyntaxHighlighter;
class QString;

// device type flags
#define DEV_BJT      0x0001
#define DEV_MOS      0x0002
#define DEV_MASK_DEV 0x00FF
#define DEV_DIG      0x0100
#define DEV_ANA      0x0200
#define DEV_ALL      0x0300
#define DEV_MASK_TYP 0xFF00
#define DEV_DEF      0x0200 // default value

/*!
 * \brief The TextDoc class definition
 */
class TextDoc : public QPlainTextEdit, public QucsDoc {
  Q_OBJECT
public:
  TextDoc (QucsApp * /*BUG*/, const QString&, QWidget* owner);
 ~TextDoc ();

  Object::Type type() const override {return Object::Type::TextDoc;}

  void  setName (const QString&);
  bool  load ();
  int   save ();
  float zoomBy (float);
  void  showNoZoom ();
  void  becomeCurrent (bool);
  bool  loadSimulationTime (QString&);
  void  commentSelected ();
  void  insertSkeleton ();
  void  setLanguage (int);
  void  setLanguage (const QString&);
  QString getModuleName (void);


  QFont TextFont;

  bool simulation;   // simulation or module
  QString Library;   // library this document belongs to
  QString Libraries; // libraries to be linked with
  QString ShortDesc; // icon description
  QString LongDesc;  // component description
  QString Icon;      // icon file
  bool recreate;     // recreate output file
  int devtype;       // device type

  bool SetChanged;
  int language;

  bool loadSettings (void);
  bool saveSettings (void);
  void refreshLanguage(void);

  QMenu* createStandardContextMenu();

signals:
  void signalCursorPosChanged(int, int);
  void signalFileChanged(bool);
  void signalUndoState(bool);
  void signalRedoState(bool);

public slots:
  void search(const QString &str, bool CaseSensitive, bool wordOnly, bool backward);
  void replace(const QString &str, const QString &str2, bool needConfirmed,
               bool CaseSensitive, bool wordOnly, bool backward);
  void slotCursorPosChanged ();
  void slotSetChanged ();

private:
  SyntaxHighlighter * syntaxHighlight;
  QucsApp *App;

private: // actions. here?
  void actionCopy(QAction*) override{
	  copy();
  }
  void actionCut(QAction*) override{
	  cut();
  }
  void actionSelect(QAction*) override;
  void actionEditActivate(QAction*) override;
  void actionEditDelete(QAction*) override;
  void actionEditPaste(QAction*) override;
  void actionZoomIn(QAction*) override;

  // BUG: qucsDoc
  void actionEditUndo(QAction*) override{
    viewport()->setFocus();
	 QPlainTextEdit::undo();
  }
  void actionEditRedo(QAction*) override{
	  viewport()->setFocus();
	  QPlainTextEdit::redo();
  }

  void actionInsertEntity(QAction*) override;
  void actionSelectAll(QAction*) override{
	  viewport()->setFocus();
	  //->selectAll(true);
	  selectAll();
  }
  void actionChangeProps(QAction*) override;


private slots:
  void highlightCurrentLine();
  bool baseSearch(const QString &, bool, bool, bool);

private: // TODO
  float Scale;
};

inline void TextDoc::actionInsertEntity(QAction*)
{
  viewport()->setFocus ();
  //TODO Doc->clearParagraphBackground (Doc->tmpPosX);
  insertSkeleton ();

  //int x, y;
  //Doc->getCursorPosition (&x, &y);
  //x = Doc->textCursor().blockNumber();
  //y = Doc->textCursor().columnNumber();
  slotCursorPosChanged();
}

#endif
