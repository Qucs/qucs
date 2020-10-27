
#ifndef QUCS_TABS_H
#define QUCS_TABS_H

#include <QTabWidget>

class QucsApp;
class QucsDoc;

class QucsTabWidget : public QTabWidget
{
  Q_OBJECT
public:
  QucsTabWidget(QucsApp *parent = 0);
  void setSaveIcon(bool state=true, int index=-1);
public: // BUG, obsolete.
  QucsDoc *createEmptySchematic(const QString &name);
  QucsDoc *createEmptyTextDoc(const QString &name);

  QucsDoc* current();

public slots:
  void showContextMenu(const QPoint& point);
private slots:
  void slotCxMenuClose();
  void slotCxMenuCloseOthers();
  void slotCxMenuCloseAll();
  void slotCxMenuCloseRight();
  void slotCxMenuCloseLeft();
  void slotCxMenuCopyPath();
  void slotCxMenuOpenFolder();

public slots:
  void slotDCbias();

private:
  int contextTabIndex; // index of tab where context menu was opened
  QString docName; // name of the document where context menu was opened
  QucsApp *App; // the main application - parent widget
};

#endif
