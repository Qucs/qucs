#ifndef SPICECOMPAT_H
#define SPICECOMPAT_H

#include <QtCore>

/*!
  \brief spicecompat namespace contains definitions responsible
         for conversion of variables and node names from Qucs
         notation to Spice notation
 *
 */
namespace spicecompat {
     QString check_refdes(QString &Name, QString &Model);
     QString normalize_value(QString Value);
     QString convert_functions(QString tok, bool isXyce);
     void splitEqn(QString &eqn, QStringList &tokens);
     bool containNodes(QStringList &tokens, QStringList &vars);
     void convertNodeNames(QStringList &tokens, QString &sim);
}

#endif // SPICECOMPAT_H
