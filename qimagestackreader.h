#ifndef QIMAGESTACKREADER_H
#define QIMAGESTACKREADER_H

#include <QImageReader>
#include <QRegExp>
#include <QFileInfo>
#include <QStringList>

class QImageStackReader : public QImageReader
{
public:
    QImageStackReader();
    QImageStackReader(QString& _fn);
    virtual ~QImageStackReader();

    QByteArray* readstack();
    bool IsOk() const { return !_errorflag; }
    int Width()  const { return _width; }
    int Height() const { return _height; }
    int Slices() const { return _slices; }

    QString Basename() const { return _basename; }
    void setFileNameRange(const QString &fileName);

private:
    QByteArray *_data;
    QString _basename;
    QString _ext;
    int _lowlimit, _highlimit;
    int _width, _height, _slices;
    bool _errorflag;
    bool _loadeddata;
    bool _hasfilenumberrange;

    void _getFileNumberRange();
};

#endif // QIMAGESTACKREADER_H
