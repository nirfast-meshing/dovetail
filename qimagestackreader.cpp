#include <QDir>

#include "qimagestackreader.h"
#include <iostream>


QImageStackReader::QImageStackReader() : _lowlimit(-1), _highlimit(-1),
    _errorflag(false), _loadeddata(false), _hasfilenumberrange(false)
{
    setFileName("");
}

QImageStackReader::QImageStackReader(QString &_fn) : QImageReader(_fn),
    _lowlimit(-1), _highlimit(-1), _errorflag(false), _loadeddata(false), _hasfilenumberrange(false)
{
    _getFileNumberRange();
}

void QImageStackReader::setFileNameRange(const QString &_fileName)
{
    QImageReader::setFileName(_fileName);
    _getFileNumberRange();
}

void QImageStackReader::_getFileNumberRange()
{
    // Get base name too
    QFileInfo fi(fileName());
    _ext = fi.suffix();
    QRegExp rx("[0-9]+\\." + _ext);
    bool ok;
    rx.setCaseSensitivity(Qt::CaseInsensitive);

    _basename = fi.fileName();

    int pos = rx.indexIn(_basename);
    if (pos != -1)
    {
        fi.setFile(QFileInfo(fileName()).dir(), _basename.mid(0,pos));
        _basename = fi.absoluteFilePath();
        int i = rx.matchedLength() - _ext.length() - 1;
        _lowlimit = rx.cap(0).mid(0,i).toInt(&ok);

        QFileInfo foo;
        if (!ok)
            _errorflag = true;
        else
        {
            int i = _lowlimit;

            while(true)
            {
                foo.setFile(_basename+QString::number(i)+"."+_ext);
                if (foo.exists() /*&& foo.size() != 0*/)
                {
                    ++i;
                    continue;
                }
                else
                    break;
            }
            _highlimit = i-1;
            QImage _tmpimg;
            this->setFileName(_basename+QString::number(_lowlimit)+"."+_ext);
            _tmpimg = this->read();
            if (_tmpimg.isNull())
            {
                _errorflag = true;
            }
            else
            {
                _width = _tmpimg.width();
                _height = _tmpimg.height();
                _slices = (_highlimit - _lowlimit + 1);
                _pixelsize  = 1 / _tmpimg.dotsPerMeterX() * 1000.;
            }
        }
//        _basename = foo.setFile(fi.dir(),_basename);
        std::cout << "basename: " << _basename.toStdString() << std::endl;
        std::cout << "ext: " << _ext.toStdString() << '\n';
        std::cout << "from " << _lowlimit << " to " << _highlimit << std::endl;
        _hasfilenumberrange = true;
    }
    else
        _errorflag = true;
}

QByteArray* QImageStackReader::readstack()
{
    // Read first image and get height/width
    QString fn = _basename + QString::number(_lowlimit) + '.' + _ext;
    this->setFileName(fn);
    QImage foo = this->read();

    if (foo.isNull())
    {
        _errorflag = true;
        return NULL;
    }
    _width = foo.width();
    _height = foo.height();

    if (_loadeddata)
        delete _data;
    _data = new QByteArray; //(, '0');
    _data->reserve(_width*_height*_slices);
    QImage _reader(_width, _height, foo.format());
    int c = 0;
    for (int i=_lowlimit; i<=_highlimit; ++i)
    {
        fn = _basename + QString::number(i) + '.' + _ext;
        this->setFileName(fn);
        if (!this->canRead())
        {
            _errorflag = true;
            _loadeddata = false;
            delete _data;
            return NULL;
        }
        this->read(&_reader);
        _reader = _reader.convertToFormat(QImage::Format_Indexed8);

        QRgb col;
        int64_t gray;
        for (int ii=0; ii<_height; ++ii)
        {
            for (int jj=0; jj<_width; ++jj)
            {
                col = _reader.pixel(jj,ii);
                gray = qGray(col);
                uint8_t p = static_cast<uint8_t>(gray);
                _data->append(reinterpret_cast<char *>(&p),1);
                if  (gray != p)
                {
                    std::cerr << "before: " << gray << ", after: " << p << '\n';
                    std::cerr.flush();
                }
                c++;
//                _data->append(p+7,1); // Assuming little-endian
            }
        }
    }

    std::cout << "c: " << c << std::endl;

    _loadeddata = true;
    return _data;
}

QImageStackReader::~QImageStackReader()
{
    if (_loadeddata)
        delete _data;
    // Delete _data
}
