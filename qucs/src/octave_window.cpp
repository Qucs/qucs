/***************************************************************************
    copyright            : (C) 2010 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/
#include "octave_window.h"
#include "qucs.h"
#include "misc.h"

#include <QSize>
#include <QColor>
#include <QKeyEvent>
#include <QWidget>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QLineEdit>
#include <QDockWidget>
#include <QDebug>
#include <QMessageBox>


OctaveWindow::OctaveWindow(QDockWidget *parent_): QWidget()
{
  QFont font;
  font = QFont("Courier New");
  font.setPointSize(QucsSettings.font.pointSize()-1);
  font.setStyleHint(QFont::Courier);
  font.setFixedPitch(true);
  setFont(font);

  QWidget *all = new QWidget();
  QVBoxLayout *allLayout = new QVBoxLayout();

  output = new QTextEdit(this);
  output->setReadOnly(true);
  output->setUndoRedoEnabled(false);
  output->toPlainText();
  output->setLineWrapMode(QTextEdit::NoWrap);
  misc::setWidgetBackgroundColor(output, QucsSettings.BGColor);
  allLayout->addWidget(output);

  input = new QLineEdit(this);
  connect(input, SIGNAL(returnPressed()), SLOT(slotSendtaskElement()));
  allLayout->addWidget(input);
  all->setLayout(allLayout);

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  parent_->setWidget(all);

  //parent_->setResizeEnabled(true);
  //parent_->setHorizontallyStretchable(true);
  histPosition = 0;

  input->installEventFilter(this);
}

// -----------------------------------------------------------------
OctaveWindow::~OctaveWindow()
{
  if(octProcess.state()==QProcess::Running)
    octProcess.kill();
}

// -----------------------------------------------------------------
QSize OctaveWindow::sizeHint() const
{
  QSize Size;
  int w=0, h=0;

  Size = output->sizeHint();
  w = Size.width();
  h = Size.height() + input->sizeHint().height();
  return QSize(w, h);
}

// ------------------------------------------------------------------------
bool OctaveWindow::startOctave()
{
  if(octProcess.state()==QProcess::Running)
    return true;

  QString Program;
  QStringList Arguments;

  QString OctavePath = QDir::toNativeSeparators(QucsSettings.OctaveExecutable);

  // Override Octave path by enviroment variable if it is set
  if (QucsSettings.QucsOctave.isEmpty()) Program = OctavePath;
  else Program = QucsSettings.QucsOctave;

  Arguments << "--no-history" << "-i" << "-f" << "-p"
            << QDir::toNativeSeparators(QucsSettings.OctaveDir); // m-files location

  disconnect(&octProcess, 0, 0, 0);
  connect(&octProcess, SIGNAL(readyReadStandardError()), SLOT(slotDisplayErr()));
  connect(&octProcess, SIGNAL(readyReadStandardOutput()), SLOT(slotDisplayMsg()));
  connect(&octProcess, SIGNAL(finished(int)), SLOT(slotOctaveEnded(int)));
#ifdef __MINGW32__
  QString sep(";"); // path separator
#else
  QString sep(":");
#endif

  // append process PATH, othewise Octave does not find gnuplot
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert("PATH", env.value("PATH") + sep + QucsSettings.BinDir );
  octProcess.setProcessEnvironment(env);
  output->clear();

  qDebug() << "taskElement :" << Program << Arguments.join(" ");
  octProcess.start(Program, Arguments);
  octProcess.waitForStarted();

  if(octProcess.state()!=QProcess::Running) {
    output->setText(tr("ERROR: Failed to execute \"%1\"").arg(Program));
    return false;
  }

  adjustDirectory();
  return true;
}

// ------------------------------------------------------------------------
void OctaveWindow::adjustDirectory()
{
  sendtaskElement("cd \"" + QucsSettings.QucsWorkDir.absolutePath() + "\"");
}

// ------------------------------------------------------------------------
void OctaveWindow::sendtaskElement(const QString& cmd)
{
  //int par = output->paragraphs() - 1;
  //int idx = output->paragraphLength(par);
  output->setTextColor(QColor(Qt::blue));
  output->append(cmd);
  QString cmdstr = cmd + "\n";
  //output->insertAt(cmdstr, par, idx);
  //output->scrollToBottom();
  QByteArray ba = cmdstr.toLatin1();
  const char *c_cmdstr = ba.data();
  octProcess.write(c_cmdstr);
}

// ------------------------------------------------------------------------
void OctaveWindow::runOctaveScript(const QString& name)
{
  QFileInfo info(name);
  sendtaskElement(info.completeBaseName());
}

// ------------------------------------------------------------------------
void OctaveWindow::slotSendtaskElement()
{
  sendtaskElement(input->text());
  if(!input->text().trimmed().isEmpty())
    cmdHistory.append(input->text());
  //histIterator = cmdHistory.end();
  histPosition = cmdHistory.length();
  input->clear();
  qDebug() << cmdHistory << histPosition;
}


bool OctaveWindow::eventFilter(QObject *obj, QEvent *event) {
    Q_UNUSED(obj);

    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            if (keyEvent->key() == Qt::Key_PageUp) {
                //if(histIterator == cmdHistory.begin())
                if(histPosition == 0)
                    return false;
                //histIterator--;
                histPosition--;
                input->setText(cmdHistory.at(histPosition));//*histIterator);
                return true;
            }
            else if(keyEvent->key() == Qt::Key_PageDown) {
                //if(histIterator == cmdHistory.end())
                if (histPosition >= cmdHistory.length()-1)
                    return false;
                //histIterator++;
                histPosition++;
                input->setText(cmdHistory.at(histPosition));//*histIterator);
                return true;
            }
     }
     return false;
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stdout.
void OctaveWindow::slotDisplayMsg()
{
  //int par = output->paragraphs() - 1;
  //int idx = output->paragraphLength(par);
  //output->insertAt(QString(octProcess.readAllStandardOutput()), par, idx);
  //output->scrollToBottom();
  output->setTextColor(QColor(Qt::black));
  output->append(octProcess.readAllStandardOutput());
}

// ------------------------------------------------------------------------
// Is called when the process sends an output to stderr.
void OctaveWindow::slotDisplayErr()
{
  //if(!isVisible())
  //  ((Q3DockWindow*)parent())->show();  // always show an error

  //int par = output->paragraphs() - 1;
  //int idx = output->paragraphLength(par);
  //output->insertAt(QString(octProcess.readAllStandardError()), par, idx);
  //output->scrollToBottom();
    output->setTextColor(QColor(Qt::red));
    output->append(octProcess.readAllStandardError());
}

// ------------------------------------------------------------------------
// Is called when the simulation process terminates.
void OctaveWindow::slotOctaveEnded(int status)
{
  qDebug() << "Octave ended status" << status;
  output->clear();
}
