#include <QImageIOPlugin>
#include <QDebug>

#include "fontimageiohandler.h"

class FontImagePlugin : public QImageIOPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QImageIOHandlerFactoryInterface_iid FILE "font2image.json")

public:
    Capabilities capabilities(QIODevice *device, const QByteArray &format) const Q_DECL_OVERRIDE
    {
        Q_UNUSED(device)
        Q_UNUSED(format)

        return QImageIOPlugin::CanRead;
    }

    QImageIOHandler *create(QIODevice *device, const QByteArray &format = QByteArray()) const Q_DECL_OVERRIDE
    {
        if (format != "ttf" && format != "ttc" && format != "otf")
            return 0;

        FontImageIOHandler *handler = new FontImageIOHandler(device);

        if (!format.isEmpty())
            handler->setFormat(format);

        return handler;
    }
};

#include "main.moc"
