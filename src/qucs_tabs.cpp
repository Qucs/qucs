#include "qucs_tabs.h"
#include "qucsdoc.h"
#include "qucs_app.h" // gaah
#include "io_trace.h"

#include <QTabBar>
#include <QMenu>
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
void QucsTabWidget::slotDCbias()
{ untested();
  current()->showBias = 0; //???
  incomplete();
  // slotSimulate();
  current()->slotDCbias();
}
/*--------------------------------------------------------------------------*/
QucsTabWidget::QucsTabWidget(QucsApp *parent) : QTabWidget(parent)
{itested();
  App = parent;
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showContextMenu(const QPoint&)));
}
/*--------------------------------------------------------------------------*/
void QucsTabWidget::showContextMenu(const QPoint& point)
{ untested();
  if (point.isNull()) { untested();
	  incomplete();
    return;
  }else{
  }

  contextTabIndex = tabBar()->tabAt(point);
  if (contextTabIndex >= 0) { // clicked over a tab
    QMenu menu(this);

    // get the document where the context menu was opened
    QucsDoc *d = App->getDoc(contextTabIndex);
    // save the document name (full path)
    docName = d->docName();

#define APPEND_MENU(action, slot, text)         \
  QAction *action = new QAction(tr(text), &menu);    \
  connect(action, SIGNAL(triggered()), SLOT(slot())); \
  menu.addAction(action);

  APPEND_MENU(ActionCxMenuClose, slotCxMenuClose, "Close")
  APPEND_MENU(ActionCxMenuCloseOthers, slotCxMenuCloseOthers, "Close all but this")
  APPEND_MENU(ActionCxMenuCloseLeft, slotCxMenuCloseLeft, "Close all to the left")
  APPEND_MENU(ActionCxMenuCloseRight, slotCxMenuCloseRight, "Close all to the right")
  APPEND_MENU(ActionCxMenuCloseAll, slotCxMenuCloseAll, "Close all")
  menu.addSeparator();
  APPEND_MENU(ActionCxMenuCopyPath, slotCxMenuCopyPath, "Copy full path")
  APPEND_MENU(ActionCxMenuOpenFolder, slotCxMenuOpenFolder, "Open containing folder")
#undef APPEND_MENU

    // a not-yet-saved document does not have a name/full path
    // so copying full path or opening containing folder does not make sense
    ActionCxMenuCopyPath->setEnabled(!docName.isEmpty());
    ActionCxMenuOpenFolder->setEnabled(!docName.isEmpty());

    menu.exec(tabBar()->mapToGlobal(point));
  }
}
/*--------------------------------------------------------------------------*/
QucsDoc* QucsTabWidget::current()
{
	QWidget* w = currentWidget();
	auto d = dynamic_cast<QucsDoc*>(w);
	assert(d);
	return d;
}
/*--------------------------------------------------------------------------*/
