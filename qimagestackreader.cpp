#include "qimagestackreader.h"

QImageStackReader::QImageStackReader() : _lowlimit(-1), _highlimit(-1)
{

}

QImageStackReader::QImageStackReader(QString &_fn) : QImageReader(_fn), _lowlimit(-1), _highlimit(-1)
{
    _getFileNumberRange();
}

void QImageStackReader::_getFileNumberRange()
{
    // Get base name too
}

QByteArray* read()
{
    // load images one by one
    // convert them to grayscale
    // convert them to uchar
    // populate _data
}

QImageStackReader::~QImageStackReader()
{
    // Delete _data
}
