#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <qbuffer.h>
#include <qtextstream.h>
#include <qobject.h>
#include <qapplication.h>
#include <qtextedit.h>
#include <qdir.h>
#include <qtextcodec.h>

#include "qf_common.h"
#include "qf_poly.h"
#include "qf_comp.h"
#include "qf_filter.h"
#include "qf_capacity.h"
#include "qf_tform.h"
#include "qf_box.h"
#include "qf_settings.h"

void compute_lumped (qf_spec* spec_p, QTextStream& out) {

  qf_tform* T = qf_tform_apis [spec_p -> tform] -> cons (spec_p);

  T -> dump (out);
}

int main (int argc, char * argv []) {

  // apply default settings
  QucsSettings.x = 200;
  QucsSettings.y = 100;
  QucsSettings.font = QFont("Helvetica", 12);

  // is application relocated?
  char * var = getenv ("QUCSDIR");
  if (var != NULL) {
    QDir QucsDir = QDir (var);
    QString QucsDirStr = QucsDir.canonicalPath ();
    QucsSettings.BitmapDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/bitmaps/");
    QucsSettings.LangDir =
      QDir::convertSeparators (QucsDirStr + "/share/qucs/lang/");
  } else {
    QucsSettings.BitmapDir = BITMAPDIR;
    QucsSettings.LangDir = LANGUAGEDIR;
  }

  loadSettings();

  QApplication	    app (argc, argv);
  qf_spec*	    spec_p;

  int               result = 0;
  
  app.setFont( QucsSettings.font );
  QTranslator tor( 0 );
  QString lang = QucsSettings.Language;
  if(lang.isEmpty())
    lang = QTextCodec::locale();
  tor.load( QString("qucs_") + lang, QucsSettings.LangDir);
  app.installTranslator( &tor );

  QTextEdit res;
  res. setReadOnly (true);
  res. setTextFormat (Qt::PlainText);
  res. setAlignment (Qt::AlignLeft);
  res. setFont (QucsSettings.font);
  res. setTabStopWidth (12 * 4);
  res. show ();

  qf_box	    Filterbox;
  Filterbox. setFont (QucsSettings.font);
  Filterbox. move (QucsSettings.x, QucsSettings.y);
  Filterbox. show ();

  while (true) {
    if (Filterbox. exec () != QDialog::Accepted) break;
    
    spec_p = Filterbox. get_spec ();

    QByteArray	buf;
    QTextStream s (buf, IO_ReadWrite);

    compute_lumped (spec_p, s);
    s. device () -> flush ();

    res. insert (buf);
  }

  saveApplSettings (&Filterbox);
  return result;
}
