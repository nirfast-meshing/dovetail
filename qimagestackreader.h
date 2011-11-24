#ifndef QIMAGESTACKREADER_H
#define QIMAGESTACKREADER_H

#include <QImageReader>

class QImageStackReader : public QImageReader
{
public:
    QImageStackReader();
    QImageStackReader(QString& _fn);
    virtual ~QImageStackReader();

    QByteArray* read();

private:
    QByteArray *_data;
    int _lowlimit, _hightlimit;
    QString _basename;

    void _getFileNumberRange();
};

#endif // QIMAGESTACKREADER_H
