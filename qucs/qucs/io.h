#ifndef QUCS_IO_H
#define QUCS_IO_H
class QFile;
class DocumentStream : public QTextStream {
public:
  explicit DocumentStream(){ incomplete(); }
  explicit DocumentStream(QFile* /* BUG const */ file);
  explicit DocumentStream(QString /* BUG const */ * filename, QIODevice::OpenModeFlag flag) :
    QTextStream(filename, flag){}

};
typedef QTextStream stream_t;
#endif
