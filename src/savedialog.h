/***************************************************************************
 * Copyright (C) 2006 by Gopala Krishna A <krishna.ggk@gmail.com>          *
 *                                                                         *
 * This is free software; you can redistribute it and/or modify            *
 * it under the terms of the GNU General Public License as published by    *
 * the Free Software Foundation; either version 3, or (at your option)     *
 * any later version.                                                      *
 *                                                                         *
 * This software is distributed in the hope that it will be useful,        *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 * GNU General Public License for more details.                            *
 *                                                                         *
 * You should have received a copy of the GNU General Public License       *
 * along with this package; see the file COPYING.  If not, write to        *
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,   *
 * Boston, MA 02110-1301, USA.                                             *
 ***************************************************************************/

#ifndef QUCS_SAVEDIALOG_H
#define QUCS_SAVEDIALOG_H

#include <QVariant>
#include <QDialog>
#include <QMap>
#include <QBoxLayout>
#include <QGridLayout>
#include <QBoxLayout>
#include <QLabel>
#include <QCheckBox>

#include "qt_compat.h"

class QBoxLayout;
class QBoxLayout;
class QGridLayout;
class QSpacerItem;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;

namespace qucs{
	class Doc;
	class App;
}

class SaveDialog : public QDialog {
   Q_OBJECT
public:
	enum {
		AbortClosing = 0,
		DontSave,
		SaveSelected
	};

	SaveDialog(QWidget* p = 0, const char* n = 0, bool modal = true, Qt::WFlags fl = 0 );
	~SaveDialog();
	void addUnsavedDoc(qucs::Doc *doc);
	void setApp(qucs::App *a);
	bool isEmpty() const;

	protected slots:
		void dontSaveClicked();
	void saveSelectedClicked();
	void reject();

private:
	void initDialog();

	QMap<qucs::Doc*, QListWidgetItem*> unsavedDocs;

	QLabel* label;
	QListWidget* fileView;
	QPushButton* abortClosingButton;
	QPushButton* dontSaveButton;
	QPushButton* saveSelectedButton;
	QBoxLayout* SaveDialogLayout;
	QHBoxLayout* buttonsLayout;
	QSpacerItem* spacer;
	qucs::App *app;
};

#endif // guard
