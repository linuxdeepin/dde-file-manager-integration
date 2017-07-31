#include "fontimageiohandler.h"

#include <fontconfig/fontconfig.h>

#include <QPainter>
#include <QVariant>
#include <QFile>
#include <QFontDatabase>
#include <QDebug>

#define DEFAULT_SIZE 256

static const int maxWeight = 99;

static inline int mapToQtWeightForRange(int fcweight, int fcLower, int fcUpper, int qtLower, int qtUpper)
{
    return qtLower + ((fcweight - fcLower) * (qtUpper - qtLower)) / (fcUpper - fcLower);
}

static inline int weightFromFcWeight(int fcweight)
{
    // Font Config uses weights from 0 to 215 (the highest enum value) while QFont ranges from
    // 0 to 99. The spacing between the values for the enums are uneven so a linear mapping from
    // Font Config values to Qt would give surprising results.  So, we do a piecewise linear
    // mapping.  This ensures that where there is a corresponding enum on both sides (for example
    // FC_WEIGHT_DEMIBOLD and QFont::DemiBold) we map one to the other but other values map
    // to intermediate Qt weights.

    if (fcweight <= FC_WEIGHT_THIN)
        return QFont::Thin;
    if (fcweight <= FC_WEIGHT_ULTRALIGHT)
        return mapToQtWeightForRange(fcweight, FC_WEIGHT_THIN, FC_WEIGHT_ULTRALIGHT, QFont::Thin, QFont::ExtraLight);
    if (fcweight <= FC_WEIGHT_LIGHT)
        return mapToQtWeightForRange(fcweight, FC_WEIGHT_ULTRALIGHT, FC_WEIGHT_LIGHT, QFont::ExtraLight, QFont::Light);
    if (fcweight <= FC_WEIGHT_NORMAL)
        return mapToQtWeightForRange(fcweight, FC_WEIGHT_LIGHT, FC_WEIGHT_NORMAL, QFont::Light, QFont::Normal);
    if (fcweight <= FC_WEIGHT_MEDIUM)
        return mapToQtWeightForRange(fcweight, FC_WEIGHT_NORMAL, FC_WEIGHT_MEDIUM, QFont::Normal, QFont::Medium);
    if (fcweight <= FC_WEIGHT_DEMIBOLD)
        return mapToQtWeightForRange(fcweight, FC_WEIGHT_MEDIUM, FC_WEIGHT_DEMIBOLD, QFont::Medium, QFont::DemiBold);
    if (fcweight <= FC_WEIGHT_BOLD)
        return mapToQtWeightForRange(fcweight, FC_WEIGHT_DEMIBOLD, FC_WEIGHT_BOLD, QFont::DemiBold, QFont::Bold);
    if (fcweight <= FC_WEIGHT_ULTRABOLD)
        return mapToQtWeightForRange(fcweight, FC_WEIGHT_BOLD, FC_WEIGHT_ULTRABOLD, QFont::Bold, QFont::ExtraBold);
    if (fcweight <= FC_WEIGHT_BLACK)
        return mapToQtWeightForRange(fcweight, FC_WEIGHT_ULTRABOLD, FC_WEIGHT_BLACK, QFont::ExtraBold, QFont::Black);
    if (fcweight <= FC_WEIGHT_ULTRABLACK)
        return mapToQtWeightForRange(fcweight, FC_WEIGHT_BLACK, FC_WEIGHT_ULTRABLACK, QFont::Black, maxWeight);
    return maxWeight;
}

FontImageIOHandler::FontImageIOHandler(QIODevice *device)
{
    setDevice(device);
}

bool FontImageIOHandler::canRead() const
{
    if (m_fontFamily.isEmpty()) {
        return device()->size() > 0;
    }

    return true;
}

bool FontImageIOHandler::read(QImage *image)
{
    Q_ASSERT(image);

    if (!canRead())
        return false;

    if (device()->atEnd())
        return false;

    QFile *file = qobject_cast<QFile*>(device());

    if (!file)
        return false;

    int count;
    FcPattern *pattern = FcFreeTypeQuery((const FcChar8 *)QFile::encodeName(file->fileName()).constData(), 0, FcConfigGetBlanks(0), &count);

    if (!pattern)
        return false;

    FcChar8 *fam = 0;
    if (FcPatternGetString(pattern, FC_FAMILY, 0, &fam) == FcResultMatch) {
        m_fontFamily = QString::fromUtf8(reinterpret_cast<const char *>(fam));
    }

    if (m_fontFamily.isEmpty())
        return false;

    int weight_value;
    if (FcPatternGetInteger(pattern, FC_WEIGHT, 0, &weight_value) != FcResultMatch)
        weight_value = FC_WEIGHT_REGULAR;

    m_fontWeight = weightFromFcWeight(weight_value);

    QFontDatabase fdb;

    if (!fdb.hasFamily(m_fontFamily)) {
        fdb.addApplicationFont(file->fileName());
    }

    QFont font = fdb.font(m_fontFamily, QString(), 12);

    if (font.family() != m_fontFamily) {
        return false;
    }

    font.setPixelSize(image->width() / 8);
    font.setWeight(m_fontWeight);

    QSize size(DEFAULT_SIZE, DEFAULT_SIZE);

    if (!m_size.isEmpty()) {
        size = m_size;
    }

    *image = QImage(size, QImage::Format_ARGB32_Premultiplied);
    image->fill(m_backgroundColor);

    QPainter pa(image);

    pa.setFont(font);
    pa.drawText(QRect(QPoint(0, 0), image->size()), Qt::AlignCenter,
                QString::fromUtf8("deepin\nDEEPIN\n深度操作系统\n\\\\\\zccrs///"));
    pa.end();

    return true;
}

QVariant FontImageIOHandler::option(QImageIOHandler::ImageOption option) const
{
    switch (option) {
    case Size:
        // default size
        return QSize(DEFAULT_SIZE, DEFAULT_SIZE);
    case ScaledSize:
        return m_size;
    case ImageTransformation:
        return int(m_transformation);
    case BackgroundColor:
        return m_backgroundColor;
    default:
        break;
    }

    return QImageIOHandler::option(option);
}

void FontImageIOHandler::setOption(QImageIOHandler::ImageOption option, const QVariant &value)
{
    switch (option) {
    case ScaledSize:
        m_size = value.toSize();
        return;
    case ImageTransformation: {
        int transformation = value.toInt();
        if (transformation > 0 && transformation < 8)
            m_transformation = QImageIOHandler::Transformations(transformation);
    }
    case BackgroundColor:
        m_backgroundColor = qvariant_cast<QColor>(value);
    default:
        break;
    }

    QImageIOHandler::setOption(option, value);
}

bool FontImageIOHandler::supportsOption(QImageIOHandler::ImageOption option) const
{
    switch (option) {
    case Size:
    case ScaledSize:
    case ImageTransformation:
    case BackgroundColor:
        return true;
    default:
        break;
    }

    return false;
}
