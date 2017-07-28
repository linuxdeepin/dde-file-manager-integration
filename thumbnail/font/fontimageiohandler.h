#ifndef FONTIMAGEIOHANDLER_H
#define FONTIMAGEIOHANDLER_H

#include <QImageIOHandler>
#include <QSize>
#include <QColor>

class FontImageIOHandler : public QImageIOHandler
{
public:
    FontImageIOHandler(QIODevice *device);

    bool canRead() const Q_DECL_OVERRIDE;
    bool read(QImage *image) Q_DECL_OVERRIDE;

    QVariant option(ImageOption option) const Q_DECL_OVERRIDE;
    void setOption(ImageOption option, const QVariant &value) Q_DECL_OVERRIDE;
    bool supportsOption(ImageOption option) const Q_DECL_OVERRIDE;

private:
    mutable QString m_fontFamily;
    mutable int m_fontWeight;
    QSize m_size;
    QImageIOHandler::Transformations m_transformation;
    QColor m_backgroundColor = Qt::transparent;
};

#endif // FONTIMAGEIOHANDLER_H
