#ifndef __HDF_H
#define __HDF_H
#include <qstringlist.h>
#include <qmap.h>
#include <qfile.h>

class HtmlDataFetcher
{
  public:
    HtmlDataFetcher();
    ~HtmlDataFetcher() {}
    QStringList fetchChapterTexts(const QString &indexFile);
    QStringList fetchLinksToFiles(const QString &indexFile);
  private:
    void initMap();
    QChar unicodeFor(QString placeholder);
    void formatAndReplace(QString &txt);

    QMap<QString, QChar> entityMap;
};

#endif
