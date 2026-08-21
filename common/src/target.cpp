#include "target.h"

namespace {

// Style of the shot markers, a semi-transparent
// fill colour with a darker, more opaque edge, and the shot number shown in white at the centre.
// The latest shot is red, all the other shots turn green once a newer shot has been drawn.
const double kShotEdgeWidthRatio = 0.15;   // Edge width as a fraction of the marker's outer radius
const QColor kLatestShotFillColor(0xF4, 0x43, 0x36, 204);     // Material red, ~80% opacity
const QColor kLatestShotBorderColor(0x8B, 0x00, 0x00, 200);   // Dark red, ~78% opacity
const QColor kPreviousShotFillColor(0x4C, 0xAF, 0x50, 204);   // Material green, ~80% opacity
const QColor kPreviousShotBorderColor(0x00, 0x64, 0x00, 200); // Dark green, ~78% opacity

// Only one series' worth of shots is ever shown on the target at once; once it is full,
// the target is cleared and the next series starts fresh, same as on a real target.
const int kShotsPerSeries = 10;

// Size of the box a scoring ring's number is centred in, in the target image's px. Big enough
// for the biggest font used for the ring numbers.
const int kRingNumberBoxSize = 40;

// Length of the sighter mark's legs, as a fraction of the width of the shown area of the target.
// The mark is drawn into the corner of the picture, so it scales together with the target.
const double kSighterMarkSizeRatio = 0.2;
}

// old
const QStringList Target::m_targetTypes = QStringList() << QString::fromLatin1("Air Rifle") << QString::fromLatin1("Air Pistol") << QString::fromLatin1("50m Rifle");

Target::Target(QWidget* parent)
    : QLabel(parent)
{
    m_targetImage = nullptr;
    m_targetPainter = nullptr;
    m_active = true;
    m_farthestShot = 0;
    m_infoBoxesVisible = true;
    m_sighterMarkVisible = false;
    m_zoomEnabled = true;
    //    lehetuubid << QString::fromLatin1("Air Rifle") << QString::fromLatin1("Air Pistol") << QString::fromLatin1("50m Rifle");
}

Target::Target(int relv, QString n, QString r, QWidget* parent)
    : QLabel(parent)
{
    m_targetImage = nullptr;
    m_targetPainter = nullptr;
    m_active = true;
    m_farthestShot = 0;
    m_infoBoxesVisible = true;
    m_sighterMarkVisible = false;
    m_zoomEnabled = true;
    setName(n);
    setTargetNo(r);
    init(relv);
}

Target::~Target()
{
    delete m_targetPainter;  // The painter has to be deleted before the image it is painting on
    delete m_targetImage;
}

bool Target::infoBoxesVisible()
{
    return m_infoBoxesVisible;
}

bool Target::sighterMarkVisible()
{
    return m_sighterMarkVisible;
}

bool Target::zoomEnabled()
{
    return m_zoomEnabled;
}

QStringList Target::targetTypes()
{
    return m_targetTypes;
}

QString Target::name()
{
    return m_name;
}

QString Target::targetNo()
{
    return m_targetNo;
}

int Target::gunType()
{
    return m_gunType;
}

void Target::init(int valik)
{
    //    eelmineLask.setX(10);   //Testimiseks
    //    eelmineLask.setY(15);
    //    tulemus = "1099";
    m_gunType = valik; // 0 - õhupüss; 1 - õhupüstol; 2 - sportpüss
    m_active = true;

    if (m_targetImage == nullptr)
        m_targetImage = new QImage(1250, 1250, QImage::Format_ARGB32);
    if (m_targetPainter == nullptr) {
        m_targetPainter = new QPainter(m_targetImage);
        m_targetPainter->translate(m_targetImage->width() / 2, m_targetImage->height() / 2); // 0-point to the center of image, needs to be done only once
    }
    //    painter->scale(1, -1);  //Y-koordinaat suunaga üles

    this->setBackgroundRole(QPalette::Base);
    this->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //    this->setScaledContents(true);
    reset();
}

void Target::drawAShot(const Lask& l)
{
    if (m_seriesShots.size() >= kShotsPerSeries) {
        // The current series is already full: clear the target for the next series of shots,
        // so that only the current series' shots (at most 10, i.e. one series) are ever shown
        m_seriesShots.clear();
        drawTarget();
    }

    m_seriesShots.append(l);
    m_shotNumber++;
    redrawSeriesShots();

    // l.X() / l.Y() are in thousandths of a millimetre (see Lask::setMmX etc.)
    int x = abs(l.X()) * 2 * m_multiplier / 1000; // Distance from the centre in px, made sure it is positive
    int y = abs(l.Y()) * 2 * m_multiplier / 1000;
#ifdef QT_DEBUG
    qDebug() << "QPoint: " << QPoint(int(m_multiplier * 2 * l.X()), int(m_multiplier * -2 * l.Y())) << "/tx: " << x << ", y: " << y;
#endif
    m_previousShot.set(&l);   // Kept for the last shot's value shown in the info box, regardless of the series

    if (qMax(x, y) > m_farthestShot) // The target is zoomed according to the furthest shot's bigger coordinate
        m_farthestShot = qMax(x, y);
    zoomAndUpdate();
}

/**
 * Redraws every shot of the current series onto the (already cleared) target image, so that only
 * the current series' shots are ever visible, up to a maximum of one series (10 shots). The most
 * recently drawn shot is shown in its "latest" colour, all the others in their final colour.
 */
void Target::redrawSeriesShots()
{
    // Outer radius of the marker, matching one caliber in diameter (shot x, y coordinates are in nanometers,
    // the shots are drawn onto the target image with the scale of m_multiplier * 2 px per millimetre).
    const double outerRadius = m_multiplier * m_caliber;
    const double edgeWidth = qMax(1.0, outerRadius * kShotEdgeWidthRatio);
    // The coloured fill is one edge width smaller than the caliber, so that, together with the edge
    // drawn around it, the marker's outer diameter still matches the caliber exactly.
    const double fillRadius = outerRadius - edgeWidth;

    m_targetPainter->save();

    const int count = m_seriesShots.size();
    for (int i = 0; i < count; i++) {
        const Lask &shot = m_seriesShots.at(i);
        const QPointF pos(m_multiplier * 2 * shot.X() / 1000.0, m_multiplier * -2 * shot.Y() / 1000.0);
        const bool isLatest = (i == count - 1);
        // The series' shots are numbered consecutively, ending with the overall shot number
        const int shotNumber = m_shotNumber - (count - 1 - i);
        drawShotMarker(pos, fillRadius, edgeWidth,
                       isLatest ? kLatestShotFillColor : kPreviousShotFillColor,
                       isLatest ? kLatestShotBorderColor : kPreviousShotBorderColor,
                       shotNumber);
    }

    m_targetPainter->restore();
}

/**
 * Draws one scoring ring's number at all the four cardinal positions of the ring: to the left and
 * to the right of the centre, as well as above and below it, the same way as on a real target
 * sheet. The distance is the number's distance from the centre, in the target image's px,
 * and should be the middle of the ring's band, so that the number fits inside it.
 */
void Target::drawRingNumber(int distance, const QString &number)
{
    const int offset = kRingNumberBoxSize / 2;   // The number is centred in its box

    m_targetPainter->drawText(distance - offset, -offset, kRingNumberBoxSize, kRingNumberBoxSize, Qt::AlignCenter, number);
    m_targetPainter->drawText(-distance - offset, -offset, kRingNumberBoxSize, kRingNumberBoxSize, Qt::AlignCenter, number);
    m_targetPainter->drawText(-offset, distance - offset, kRingNumberBoxSize, kRingNumberBoxSize, Qt::AlignCenter, number);
    m_targetPainter->drawText(-offset, -distance - offset, kRingNumberBoxSize, kRingNumberBoxSize, Qt::AlignCenter, number);
}

/**
 * Draws one shot marker: a semi-transparent, filled circle with a darker, more opaque edge around it,
 * and the shot's number in white at the centre. The edge is drawn just outside the fill, so that the
 * outer diameter of the whole marker (fill and edge together) is exactly 2 * (fillRadius + edgeWidth).
 */
void Target::drawShotMarker(const QPointF &center, double fillRadius, double edgeWidth, const QColor &fillColor, const QColor &borderColor, int shotNumber)
{
    QPen borderPen(borderColor);
    borderPen.setWidthF(edgeWidth);
    m_targetPainter->setPen(borderPen);
    m_targetPainter->setBrush(Qt::NoBrush);
    m_targetPainter->drawEllipse(center, fillRadius + edgeWidth / 2.0, fillRadius + edgeWidth / 2.0);

    m_targetPainter->setPen(Qt::NoPen);
    m_targetPainter->setBrush(fillColor);
    m_targetPainter->drawEllipse(center, fillRadius, fillRadius);

    QFont numberFont = m_targetPainter->font();
    numberFont.setPointSize(qMax(6, qRound(fillRadius)));
    m_targetPainter->setFont(numberFont);
    m_targetPainter->setPen(Qt::white);
    const QRectF numberBox(center.x() - fillRadius, center.y() - fillRadius, fillRadius * 2, fillRadius * 2);
    m_targetPainter->drawText(numberBox, Qt::AlignCenter, QString::number(shotNumber));
}

/**
 * Tells if the area the sighter mark is about to cover is dark, so that the mark can be drawn in
 * the colour that stands out on it. The brightness is averaged over a grid of points inside
 * the triangle, so that a single ring line drawn across the corner does not decide the colour.
 */
bool Target::isDarkCorner(const QImage &picture, const double markSize)
{
    const int step = qMax(1, qRound(markSize / 16));   // About a hundred points inside the triangle
    int lightnessSum = 0;
    int points = 0;

    for(int x = 0; x < markSize; x += step)
        for(int y = 0; x + y < markSize; y += step)
            if(picture.valid(x, y)){
                lightnessSum += picture.pixelColor(x, y).lightness();
                points++;
            }

    return points > 0 && lightnessSum / points < 128;
}

/**
 * Draws the sighter mark into the upper left corner of the target's picture: a triangle with its
 * right angle in the corner, telling the spectators that the shots on the target are sighting shots
 * and do not count yet. The mark is drawn onto the already zoomed picture, so that it keeps its
 * size and place in the corner no matter how much the target is zoomed. A zoomed target may show
 * nothing but its black area, so the mark is drawn in white there and in black on the white area,
 * to stay visible on both.
 */
void Target::drawSighterMark(QImage &picture)
{
    const double size = picture.width() * kSighterMarkSizeRatio;   // Length of the triangle's legs

    QPainterPath triangle;
    triangle.moveTo(0, 0);
    triangle.lineTo(size, 0);
    triangle.lineTo(0, size);
    triangle.closeSubpath();

    QPainter markPainter(&picture);
    markPainter.setRenderHint(QPainter::Antialiasing);
    markPainter.setPen(Qt::NoPen);
    markPainter.setBrush(isDarkCorner(picture, size) ? Qt::white : Qt::black);
    markPainter.drawPath(triangle);
}

void Target::drawTarget()
{
    // Scale: 1px = m_multiplier * mm
    if (m_gunType == 0) { // Air Rife
        m_multiplier = 8;
        m_caliber = 4.5;
        m_targetRadius = 364;
        m_farthestShot = 0; // The target is drawn without shots, so there is nothing to zoom to
        QFont font;
        font.setPointSize(24);
        m_targetPainter->setFont(font);
        m_targetImage->fill(Qt::white);
        QPen pliiats;
        if (m_active)
            pliiats.setColor(Qt::black);
        else
            pliiats.setColor(Qt::gray);
        pliiats.setWidth(4);
        m_targetPainter->setBrush(Qt::white);
        m_targetPainter->setPen(pliiats);

        m_targetPainter->drawEllipse(QPoint(0, 0), 364, 364);
        m_targetPainter->drawEllipse(QPoint(0, 0), 324, 324);
        m_targetPainter->drawEllipse(QPoint(0, 0), 284, 284);
        if (m_active)
            m_targetPainter->setBrush(Qt::black);
        else
            m_targetPainter->setBrush(Qt::gray);
        m_targetPainter->drawEllipse(QPoint(0, 0), 244, 244);
        pliiats.setColor(Qt::white);
        m_targetPainter->setPen(pliiats);
        m_targetPainter->drawEllipse(QPoint(0, 0), 204, 204);
        m_targetPainter->drawEllipse(QPoint(0, 0), 164, 164);
        m_targetPainter->drawEllipse(QPoint(0, 0), 124, 124);
        m_targetPainter->drawEllipse(QPoint(0, 0), 84, 84);
        m_targetPainter->drawEllipse(QPoint(0, 0), 44, 44);
        m_targetPainter->drawEllipse(QPoint(0, 0), 4, 4);
        drawRingNumber(64, "8");   // The numbers on the black area are drawn in white
        drawRingNumber(104, "7");
        drawRingNumber(144, "6");
        drawRingNumber(184, "5");
        drawRingNumber(224, "4");

        if (m_active)
            m_targetPainter->setPen(Qt::black);
        else
            m_targetPainter->setPen(Qt::gray);
        drawRingNumber(264, "3");   // The rest of the numbers are on the white background
        drawRingNumber(304, "2");
        drawRingNumber(344, "1");
    } else if (m_gunType == 2) { // 50m rifle target
        float fBlackRings[] = { 122.4f, 138.4f, 154.4f }; // black rings on white background, in mm's
        int blackRings[(sizeof(fBlackRings) / sizeof(*fBlackRings))];

        float fWhiteRings[] = { 10.4f, 26.4f, 42.4f, 58.4f, 74.4f, 90.4f, 106.4f }; // white rings on black background, in mm's
        int whiteRings[(sizeof(fWhiteRings) / sizeof(*fWhiteRings))];

        m_multiplier = 4;

        int blackArea = qRound(112.4 * m_multiplier);
        int innerTen = 5 * m_multiplier;
        for (ulong i = 0; i < (sizeof(fBlackRings) / sizeof(*fBlackRings)); i++) {
            blackRings[i] = qRound(fBlackRings[i] * m_multiplier);
        }
        for (ulong i = 0; i < (sizeof(fWhiteRings) / sizeof(*fWhiteRings)); i++) {
            whiteRings[i] = qRound(fWhiteRings[i] * m_multiplier);
        }

        m_caliber = 5.6;
        m_targetRadius = blackRings[(sizeof(blackRings) / sizeof(*blackRings)) - 1];
        m_farthestShot = 0; // The target is drawn without shots, so there is nothing to zoom to

        QFont font;
        font.setPointSize(30);
        m_targetPainter->setFont(font);
        m_targetImage->fill(Qt::white);
        QPen pliiats;
        if (m_active)
            pliiats.setColor(Qt::black);
        else
            pliiats.setColor(Qt::gray);
        pliiats.setWidth(4);
        m_targetPainter->setBrush(Qt::white);
        m_targetPainter->setPen(pliiats);

        m_targetPainter->drawEllipse(QPoint(0, 0), blackRings[2], blackRings[2]); // 1
        m_targetPainter->drawEllipse(QPoint(0, 0), blackRings[1], blackRings[1]);
        m_targetPainter->drawEllipse(QPoint(0, 0), blackRings[0], blackRings[0]);
        if (m_active)
            m_targetPainter->setBrush(Qt::black);
        else
            m_targetPainter->setBrush(Qt::gray);
        m_targetPainter->drawEllipse(QPoint(0, 0), blackArea, blackArea);
        pliiats.setColor(Qt::white);
        m_targetPainter->setPen(pliiats);
        m_targetPainter->drawEllipse(QPoint(0, 0), whiteRings[6], whiteRings[6]);
        m_targetPainter->drawEllipse(QPoint(0, 0), whiteRings[5], whiteRings[5]); // 5
        m_targetPainter->drawEllipse(QPoint(0, 0), whiteRings[4], whiteRings[4]);
        m_targetPainter->drawEllipse(QPoint(0, 0), whiteRings[3], whiteRings[3]);
        m_targetPainter->drawEllipse(QPoint(0, 0), whiteRings[2], whiteRings[2]);
        m_targetPainter->drawEllipse(QPoint(0, 0), whiteRings[1], whiteRings[1]);
        m_targetPainter->drawEllipse(QPoint(0, 0), whiteRings[0], whiteRings[0]); // 10
        m_targetPainter->drawEllipse(QPoint(0, 0), innerTen, innerTen);

        drawRingNumber((whiteRings[1] + whiteRings[2]) / 2, "8");   // The numbers on the black area are drawn in white
        drawRingNumber((whiteRings[2] + whiteRings[3]) / 2, "7");
        drawRingNumber((whiteRings[3] + whiteRings[4]) / 2, "6");
        drawRingNumber((whiteRings[4] + whiteRings[5]) / 2, "5");
        drawRingNumber((whiteRings[5] + whiteRings[6]) / 2, "4");

        if (m_active)
            m_targetPainter->setPen(Qt::black);
        else
            m_targetPainter->setPen(Qt::gray);
        drawRingNumber((blackArea + blackRings[0]) / 2, "3");   // The rest of the numbers are on the white background
        drawRingNumber((blackRings[0] + blackRings[1]) / 2, "2");
        drawRingNumber((blackRings[1] + blackRings[2]) / 2, "1");
    } else { // Air Pistol
        m_multiplier = 4;
        m_caliber = 4.5;
        m_targetRadius = 622;
        m_farthestShot = 0; // The target is drawn without shots, so there is nothing to zoom to
        QFont font;
        font.setPointSize(30);
        m_targetPainter->setFont(font);
        m_targetImage->fill(Qt::white);
        QPen pliiats;
        if (m_active)
            pliiats.setColor(Qt::black);
        else
            pliiats.setColor(Qt::gray);
        pliiats.setWidth(5);
        m_targetPainter->setBrush(Qt::white);
        m_targetPainter->setPen(pliiats);
        // painter->rotate(180);
        m_targetPainter->drawEllipse(QPoint(0, 0), 622, 622);
        m_targetPainter->drawEllipse(QPoint(0, 0), 558, 558);
        m_targetPainter->drawEllipse(QPoint(0, 0), 494, 494);
        m_targetPainter->drawEllipse(QPoint(0, 0), 430, 430);
        m_targetPainter->drawEllipse(QPoint(0, 0), 366, 366);
        m_targetPainter->drawEllipse(QPoint(0, 0), 302, 302);
        if (m_active)
            m_targetPainter->setBrush(Qt::black);
        else
            m_targetPainter->setBrush(Qt::gray);
        m_targetPainter->drawEllipse(QPoint(0, 0), 238, 238);
        pliiats.setColor(Qt::white);
        m_targetPainter->setPen(pliiats);
        m_targetPainter->drawEllipse(QPoint(0, 0), 174, 174);
        m_targetPainter->drawEllipse(QPoint(0, 0), 110, 110);
        m_targetPainter->drawEllipse(QPoint(0, 0), 46, 46);
        m_targetPainter->drawEllipse(QPoint(0, 0), 20, 20);
        drawRingNumber(142, "8");   // The numbers on the black area are drawn in white
        drawRingNumber(206, "7");
        if (m_active)
            m_targetPainter->setPen(Qt::black);
        else
            m_targetPainter->setPen(Qt::gray);
        drawRingNumber(270, "6");   // The rest of the numbers are on the white background
        drawRingNumber(334, "5");
        drawRingNumber(398, "4");
        drawRingNumber(462, "3");
        drawRingNumber(526, "2");
        drawRingNumber(590, "1");
    }
    zoomAndUpdate();
    //    this->setPixmap(QPixmap::fromImage(*pilt));
}

// void Target::mouseReleaseEvent(QMouseEvent *event)
//{
//     kaugeimX /= 1.5;
////    Lask l;
////    l.set10Lask(84);
////    l.setXY(QPoint(4, 8));
////    joonistaLask(l);
//}

void Target::reset()
{
    m_farthestShot = m_targetRadius = m_multiplier = 0;
    //    m_zoomLevel = 0;
    m_previousShot.clear();
    m_seriesShots.clear();
    m_shotNumber = 0;

    //    this->setGeometry(0, 0, this->width(), this->height());
    drawTarget();
}

void Target::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    zoomAndUpdate();
}

void Target::setZoomEnabled(bool newZoomEnabled)
{
    m_zoomEnabled = newZoomEnabled;
}

void Target::setActive(bool a)
{
    m_active = a;
}

/**
 * Tells if the competitor's name, target number, last shot's value and result are drawn onto
 * the target's picture or not. Should be turned off, if that information is shown around
 * the target instead, like in a duel match's view.
 */
void Target::setInfoBoxesVisible(bool newInfoBoxesVisible)
{
    m_infoBoxesVisible = newInfoBoxesVisible;
    if (m_targetImage != nullptr)  // If the target has not been initialized yet, it is drawn later anyway
        zoomAndUpdate();
}

/**
 * Shows or hides the sighter mark, which tells the spectators that the shots on the target are
 * sighting shots and do not count yet. As the mark is drawn onto the target itself, it is shown
 * whether the target's own information boxes are used or not.
 */
void Target::setSighterMarkVisible(bool newSighterMarkVisible)
{
    if (newSighterMarkVisible == m_sighterMarkVisible)
        return;   // No need to redraw the picture if the mark is already in the right state

    m_sighterMarkVisible = newSighterMarkVisible;
    if (m_targetImage != nullptr)  // If the target has not been initialized yet, it is drawn later anyway
        zoomAndUpdate();
}

void Target::setName(QString n)
{
    m_name = n;
}

void Target::setTargetNo(QString r)
{
    m_targetNo = r;
}

void Target::setGunType(int r)
{
    m_gunType = r;
}

void Target::setResult(QString newResult)
{
    m_result = newResult;
}

/**
 * The width of the target's area currently shown, in the target image's pixels. The area is a
 * square around the centre of the target, zoomed according to the furthest shot, so that
 * the furthest shot is always fully visible. If there are no shots yet or zooming is
 * turned off, the whole target is shown.
 */
int Target::zoomedWidth()
{
    const int shotRadius = m_multiplier * m_caliber; // As the shots are drawn onto the target
    int halfWidth = m_targetRadius; // Without shots there is nothing to zoom to

    if (m_zoomEnabled && m_farthestShot > 0) {
        halfWidth = m_farthestShot + 2 * shotRadius; // Make sure the furthest shot is fully visible
        if (halfWidth < m_targetRadius / 5)
            halfWidth = m_targetRadius / 5; // Avoid high zooming in case of inner tens
        if (halfWidth > m_targetRadius)
            halfWidth = m_targetRadius; // No point of showing more than the whole target
    }

    // Shot diameter is added to both sides, to make sure the shot is not behind the name box
    int width = (halfWidth * 2) + (shotRadius * 4);
    if (m_targetImage != nullptr && width > m_targetImage->width()) // The zoomed area has to fit into the target's image
        width = m_targetImage->width();
    if (width < 1)
        width = 1;

    return width;
}

void Target::zoomAndUpdate()
{
    int w = zoomedWidth(); // Area of the target to be zoomed, according to the furthest shot
    int h = w;
    int x = m_targetImage->width() / 2 - w / 2;
    int y = m_targetImage->height() / 2 - h / 2;
    if (x < 0) x = 0;
    if (y < 0) y = 0;

    QImage copy = m_targetImage->copy(x, y, w, h);

    if (m_sighterMarkVisible)
        drawSighterMark(copy);

    if (!m_infoBoxesVisible) {  // The information is shown around the target instead, for example in a duel match's view
        this->setPixmap(QPixmap::fromImage(copy.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
        return;
    }

    QPainter painter2(&copy);
    painter2.setBrush(Qt::white);
    QFont font;
    font.setPointSize(h / 15);
    painter2.setFont(font);
    QRect shotBox(w - w / 4 - 1, h - h / 8 - 1, w / 4, h / 8);
    QRect nameBox(0, 0, font.pointSize() * m_name.length(), h / 8);
    QRect resultBox(w - (font.pointSize() * m_result.length() / 2) - 56, 0, font.pointSize() * m_result.length() / 2 + 55, h / 8);
    QRect targetNoBox(0, h - h / 8 - 1, w / 5, h / 8);

    if (!m_name.isEmpty()) {
        painter2.drawRect(nameBox);
        nameBox.setLeft(5);
        painter2.drawText(nameBox, Qt::AlignVCenter, m_name);
    }
    if (m_previousShot.get10Lask() != -999) {
        painter2.drawRect(shotBox);
        //        lasuKast.setLeft(lasuKast.left() + 5);
        //        lasuKast.setRight(lasuKast.right() - 5);
        painter2.drawText(shotBox, Qt::AlignVCenter | Qt::AlignHCenter, m_previousShot.getSLask());
    }
    if (!m_result.isEmpty()) {
        if (m_result != "0" && m_result != "0,0" && m_result != "0.0") {
            painter2.drawRect(resultBox);
            painter2.drawText(resultBox, Qt::AlignVCenter | Qt::AlignHCenter, m_result);
        } else { // If m_result is 0, then probably it is a sighting shot, although not always
            QRectF rect = QRectF(w - w / 4, 0, w / 4, w / 4);

            QPainterPath path;
            path.moveTo(rect.right(), rect.top());
            path.lineTo(rect.topLeft());
            path.lineTo(rect.bottomRight());
            path.lineTo(rect.right(), rect.top());

            // painter2.fillPath(path, QBrush(QColor ("black")));
            painter2.setBrush(Qt::black); // Black triangle with white border
            QPen oldPen = painter2.pen();
            QPen pen;
            pen.setColor(Qt::white);
            pen.setWidth(3);
            painter2.setPen(pen);
            //        painter2.setPen(Qt::white);
            painter2.drawPath(path);

            painter2.setBrush(Qt::white); // Set to original values
            painter2.setPen(oldPen);
        }
    }
    if (!m_targetNo.isEmpty()) {
        painter2.drawRect(targetNoBox);
        painter2.drawText(targetNoBox, Qt::AlignVCenter | Qt::AlignHCenter, m_targetNo);
    }
    this->setPixmap(QPixmap::fromImage(copy.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation) /**pilt).copy(x, y, w, h*/));
}
