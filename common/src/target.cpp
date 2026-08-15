#include "target.h"

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

void Target::drawAShot(Lask& l)
{
    m_targetPainter->setBrush(Qt::red);
    int d = m_multiplier * m_caliber;
    if (!m_previousShot.isEmpty()) {
        m_targetPainter->drawEllipse(QPoint(int(m_multiplier * 2 * m_previousShot.X() / 1000),
                                         int(m_multiplier * -2 * m_previousShot.Y() / 1000)),
            d, d); // Draw previous shot with different colour (shot x, y coordinates in nanometers)
#ifdef QT_DEBUG
        qDebug() << "JoonistaLask(): eelminelask: " << m_multiplier * 2 * m_previousShot.X() << ", " << m_multiplier * -2 * m_previousShot.Y();
#endif
    }
    m_targetPainter->setBrush(Qt::green);
    m_targetPainter->drawEllipse(QPoint(int(m_multiplier * 2 * l.X() / 1000), int(m_multiplier * -2 * l.Y() / 1000)), d, d); // Draw the new shot (shot x, y coordinates in nanometers)
    // l.X() / l.Y() are in thousandths of a millimetre (see Lask::setMmX etc.),
    // the shots are drawn onto the target image with the scale of
    // m_multiplier * 2 px per millimetre.
    int x = abs(l.X()) * 2 * m_multiplier / 1000; // Distance from the centre in px, made sure it is positive
    int y = abs(l.Y()) * 2 * m_multiplier / 1000;
#ifdef QT_DEBUG
    qDebug() << "QPoint: " << QPoint(int(m_multiplier * 2 * l.X()), int(m_multiplier * -2 * l.Y())) << "/tx: " << x << ", y: " << y;
#endif
    m_previousShot.set(&l);

    if (qMax(x, y) > m_farthestShot) // The target is zoomed according to the furthest shot's bigger coordinate
        m_farthestShot = qMax(x, y);
    zoomAndUpdate();
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
        m_targetPainter->drawText(64 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "8");
        m_targetPainter->drawText(-64 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "8");
        m_targetPainter->drawText(104 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "7");
        m_targetPainter->drawText(-104 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "7");
        m_targetPainter->drawText(144 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "6");
        m_targetPainter->drawText(-144 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "6");
        m_targetPainter->drawText(184 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "5");
        m_targetPainter->drawText(-184 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "5");
        m_targetPainter->drawText(224 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "4");
        m_targetPainter->drawText(-224 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "4");

        if (m_active)
            m_targetPainter->setPen(Qt::black);
        else
            m_targetPainter->setPen(Qt::gray);
        m_targetPainter->drawText(264 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "3");
        m_targetPainter->drawText(-264 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "3");
        m_targetPainter->drawText(304 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "2");
        m_targetPainter->drawText(-304 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "2");
        m_targetPainter->drawText(344 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "1");
        m_targetPainter->drawText(-344 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "1");
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

        m_targetPainter->drawText((whiteRings[1] + whiteRings[2]) / 2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "8");
        m_targetPainter->drawText((whiteRings[1] + whiteRings[2]) / -2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "8");
        m_targetPainter->drawText((whiteRings[2] + whiteRings[3]) / 2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "7");
        m_targetPainter->drawText((whiteRings[2] + whiteRings[3]) / -2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "7");
        m_targetPainter->drawText((whiteRings[3] + whiteRings[4]) / 2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "6");
        m_targetPainter->drawText((whiteRings[3] + whiteRings[4]) / -2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "6");
        m_targetPainter->drawText((whiteRings[4] + whiteRings[5]) / 2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "5");
        m_targetPainter->drawText((whiteRings[4] + whiteRings[5]) / -2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "5");
        m_targetPainter->drawText((whiteRings[5] + whiteRings[6]) / 2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "4");
        m_targetPainter->drawText((whiteRings[5] + whiteRings[6]) / -2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "4");

        if (m_active)
            m_targetPainter->setPen(Qt::black);
        else
            m_targetPainter->setPen(Qt::gray);
        m_targetPainter->drawText((blackArea + blackRings[0]) / 2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "3");
        m_targetPainter->drawText((blackArea + blackRings[0]) / -2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "3");
        m_targetPainter->drawText((blackRings[0] + blackRings[1]) / 2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "2");
        m_targetPainter->drawText((blackRings[0] + blackRings[1]) / -2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "2");
        m_targetPainter->drawText((blackRings[1] + blackRings[2]) / 2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "1");
        m_targetPainter->drawText((blackRings[1] + blackRings[2]) / -2 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "1");
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
        //        painter->drawText(-146, 5, "8");
        m_targetPainter->drawText(142 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "8");
        m_targetPainter->drawText(-142 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "8");
        //        painter->drawText(136, 5, "8");
        m_targetPainter->drawText(206 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "7");
        m_targetPainter->drawText(-206 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "7");
        //        painter->drawText(-210, 5, "7");
        //        painter->drawText(200, 5, "7");
        if (m_active)
            m_targetPainter->setPen(Qt::black);
        else
            m_targetPainter->setPen(Qt::gray);
        m_targetPainter->drawText(270 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "6");
        m_targetPainter->drawText(-270 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "6");
        //        painter->drawText(-274, 5, "6");
        //        painter->drawText(264, 5, "6");
        m_targetPainter->drawText(334 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "5");
        m_targetPainter->drawText(-334 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "5");
        //        painter->drawText(-338, 5, "5");
        //        painter->drawText(328, 5, "5");
        m_targetPainter->drawText(398 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "4");
        m_targetPainter->drawText(-398 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "4");
        //        painter->drawText(-402, 5, "4");
        //        painter->drawText(392, 5, "4");
        m_targetPainter->drawText(462 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "3");
        m_targetPainter->drawText(-462 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "3");
        //        painter->drawText(-466, 5, "3");
        //        painter->drawText(456, 5, "3");
        m_targetPainter->drawText(526 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "2");
        m_targetPainter->drawText(-526 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "2");
        //        painter->drawText(-530, 5, "2");
        //        painter->drawText(520, 5, "2");
        m_targetPainter->drawText(590 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "1");
        m_targetPainter->drawText(-590 - 20, -20, 40, 40, Qt::AlignCenter | Qt::AlignVCenter, "1");
        //        painter->drawText(-594, 5, "1");
        //        painter->drawText(584, 5, "1");
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
