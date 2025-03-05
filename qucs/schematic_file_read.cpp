#define TRACE_FUNCTION_CALLS 0

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif
#include <iostream>

#include <QtCore>
#include <QMessageBox>
#include <QDir>
#include <QStringList>
#include <QPlainTextEdit>
#include <QTextStream>
#include <QList>
#include <QProcess>
#include <QDebug>

#include "qucs.h"
#include "node.h"
#include "schematic.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"
#include "components/spicefile.h"
#include "components/vhdlfile.h"
#include "components/verilogfile.h"
#include "components/libcomp.h"
#include "module.h"
#include "misc.h"

#if TRACE_FUNCTION_CALLS
#define trace_method_calls() qInfo()<<__FILE__ <<":"<<__func__
#else
#define trace_method_calls() {}
#endif

/*!
 * \brief Schematic::loadDocument tries to load a schematic document.
 * \return true/false in case of success/failure
 */
bool Schematic::loadVerilogDocument()
{
  return true;
}
