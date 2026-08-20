#include <QtTest>

#include "target.h"

/**
 * Tests of the target's picture: the automatic zooming, where the shown area of the target has to
 * be zoomed according to the furthest shot, so that the furthest shot is still fully visible,
 * as well as the scoring rings' numbers drawn onto the target.
 */
class TargetTest : public QObject
{
    Q_OBJECT

public:
    TargetTest();
    ~TargetTest();

private slots:
    void test_ringNumbersAreDrawnInFourDirections();
    void test_ringNumbersAreDrawnInFourDirections_data();
    void test_wholeTargetIsShownWithoutShots();
    void test_zoomingAccordingToTheFurthestShot();
    void test_zoomingAccordingToTheFurthestShot_data();
    void test_zoomIsNotChangedByNearerShots();
    void test_zoomIsNotChangedByResizing();
    void test_zoomingOfInnerTens();
    void test_zoomingOutWhenTheTargetIsReset();
    void test_zoomingTurnedOff();
};

TargetTest::TargetTest()
{

}

TargetTest::~TargetTest()
{

}

namespace {

const int airRifleWholeTarget = 856;   // (364 + 2 * 32) * 2 + 32 * 4, radius 364 px, shot radius 32 px
const int airPistolWholeTarget = 1250;   // The whole air pistol target does not fit into the image
const int smallboreWholeTarget = 1250;   // Neither does the whole 50 m rifle target

Lask shotAt(const float mmX, const float mmY, const int shotValue = 100)
{
    Lask shot;
    shot.set10Lask(shotValue);
    shot.setMmX(mmX);
    shot.setMmY(mmY);
    return shot;
}

/**
 * Tells if there is any dark ink in a small box around the given point of the target's picture.
 * Used for finding the scoring rings' numbers, which are drawn in black onto the white area
 * of the target. The box is small enough to fit between the ring lines.
 */
bool hasDarkInk(const QImage &picture, const QPoint &center, const int halfBox = 12)
{
    for(int x = center.x() - halfBox; x <= center.x() + halfBox; x++)
        for(int y = center.y() - halfBox; y <= center.y() + halfBox; y++)
            if(picture.valid(x, y) && picture.pixelColor(x, y).lightness() < 100)
                return true;

    return false;
}

/**
 * The target's picture as it is shown, in the target image's own scale: the widget is resized to
 * the width of the shown area, so that the picture is not scaled at all and the distances can
 * be checked in the target image's px. Returns the centre point of the target as well.
 */
QImage shownPicture(Target &target, QPoint &centerOfTarget)
{
    const int shownWidth = target.zoomedWidth();
    target.resize(shownWidth, shownWidth);
    target.setInfoBoxesVisible(false);   // Redraws the picture in the new size, without the info boxes

    centerOfTarget = QPoint(shownWidth / 2, shownWidth / 2);

    return target.pixmap().toImage();
}

}

/**
 * Every scoring ring's number has to be drawn at all the four cardinal positions of the ring, both
 * horizontally and vertically, the same way as on a real target sheet.
 */
void TargetTest::test_ringNumbersAreDrawnInFourDirections_data()
{
    QTest::addColumn<int>("gunType");
    QTest::addColumn<int>("distance");

    // The distances are the middles of the rings' bands, in the target image's px
    QTest::newRow("air rifle, 2") << 0 << 304;
    QTest::newRow("air rifle, 1") << 0 << 344;
    QTest::newRow("air pistol, 2") << 1 << 526;
    QTest::newRow("air pistol, 1") << 1 << 590;
    QTest::newRow("50 m rifle, 1") << 2 << 583;   // (blackRings[1] + blackRings[2]) / 2
}

void TargetTest::test_ringNumbersAreDrawnInFourDirections()
{
    QFETCH(int, gunType);
    QFETCH(int, distance);

    Target target(gunType, "", "", nullptr);
    QPoint center;
    const QImage picture = shownPicture(target, center);

    QCOMPARE(picture.width(), target.zoomedWidth());   // The picture has to be in the target image's own scale
    QVERIFY(hasDarkInk(picture, center + QPoint(distance, 0)));   // Right
    QVERIFY(hasDarkInk(picture, center + QPoint(-distance, 0)));   // Left
    QVERIFY(hasDarkInk(picture, center + QPoint(0, distance)));   // Below
    QVERIFY(hasDarkInk(picture, center + QPoint(0, -distance)));   // Above
}

void TargetTest::test_wholeTargetIsShownWithoutShots()
{
    Target airRifle(0, "", "", nullptr);
    Target airPistol(1, "", "", nullptr);
    Target smallbore(2, "", "", nullptr);

    QCOMPARE(airRifle.zoomedWidth(), airRifleWholeTarget);
    QCOMPARE(airPistol.zoomedWidth(), airPistolWholeTarget);
    QCOMPARE(smallbore.zoomedWidth(), smallboreWholeTarget);
}

void TargetTest::test_zoomingAccordingToTheFurthestShot_data()
{
    QTest::addColumn<int>("gunType");
    QTest::addColumn<float>("mmX");
    QTest::addColumn<float>("mmY");
    QTest::addColumn<int>("expectedWidth");

    // Scale of the target image: 2 * multiplier px per mm, 8 px/mm for air rifle, 4 px/mm for the others
    QTest::newRow("air rifle, 6 mm") << 0 << 6.0f << 3.0f << 448;   // (6 * 16 + 2 * 32) * 2 + 32 * 4
    QTest::newRow("air rifle, y further") << 0 << 3.0f << -6.0f << 448;   // The bigger coordinate counts
    QTest::newRow("air rifle, whole target") << 0 << 30.0f << 0.0f << airRifleWholeTarget;   // Not more than the target
    QTest::newRow("air pistol, 20 mm") << 1 << -20.0f << 5.0f << 448;   // (20 * 8 + 2 * 16) * 2 + 16 * 4
    QTest::newRow("air pistol, whole target") << 1 << 0.0f << 200.0f << airPistolWholeTarget;
    QTest::newRow("50 m rifle, 20 mm") << 2 << 12.0f << -20.0f << 480;   // (20 * 8 + 2 * 20) * 2 + 20 * 4
    QTest::newRow("50 m rifle, whole target") << 2 << 200.0f << 0.0f << smallboreWholeTarget;
}

void TargetTest::test_zoomingAccordingToTheFurthestShot()
{
    QFETCH(int, gunType);
    QFETCH(float, mmX);
    QFETCH(float, mmY);
    QFETCH(int, expectedWidth);

    Target target(gunType, "", "", nullptr);
    Lask shot = shotAt(mmX, mmY);
    target.drawAShot(shot);

    QCOMPARE(target.zoomedWidth(), expectedWidth);
}

void TargetTest::test_zoomIsNotChangedByNearerShots()
{
    Target target(0, "", "", nullptr);

    Lask nearShot = shotAt(1.0f, 1.0f, 105);
    target.drawAShot(nearShot);
    const int afterNearShot = target.zoomedWidth();

    Lask farShot = shotAt(-6.0f, 2.0f, 93);
    target.drawAShot(farShot);
    QCOMPARE(target.zoomedWidth(), 448);
    QVERIFY(target.zoomedWidth() > afterNearShot);

    Lask anotherNearShot = shotAt(0.5f, -1.5f, 104);
    target.drawAShot(anotherNearShot);
    QCOMPARE(target.zoomedWidth(), 448);   // The furthest shot has to stay visible
}

void TargetTest::test_zoomIsNotChangedByResizing()
{
    Target target(0, "", "", nullptr);
    target.resize(300, 300);

    Lask shot = shotAt(6.0f, 3.0f);
    target.drawAShot(shot);

    target.resize(150, 400);   // Resizing only rescales the picture, the zoom level stays the same
    QCOMPARE(target.zoomedWidth(), 448);
}

void TargetTest::test_zoomingOfInnerTens()
{
    Target target(0, "", "", nullptr);

    Lask innerTen = shotAt(0.2f, -0.1f, 106);
    target.drawAShot(innerTen);

    QCOMPARE(target.zoomedWidth(), 272);   // (364 / 5 + 32 * 4), zooming is limited to a fifth of the target
}

void TargetTest::test_zoomingOutWhenTheTargetIsReset()
{
    Target target(1, "", "", nullptr);

    Lask shot = shotAt(-20.0f, 5.0f);
    target.drawAShot(shot);
    QCOMPARE(target.zoomedWidth(), 448);

    target.reset();   // The shots are wiped, so the whole target is shown again
    QCOMPARE(target.zoomedWidth(), airPistolWholeTarget);
}

void TargetTest::test_zoomingTurnedOff()
{
    Target target(0, "", "", nullptr);
    target.setZoomEnabled(false);

    Lask shot = shotAt(6.0f, 3.0f);
    target.drawAShot(shot);

    QCOMPARE(target.zoomedWidth(), airRifleWholeTarget);

    target.setZoomEnabled(true);   // The shots are remembered, so the target can be zoomed later as well
    QCOMPARE(target.zoomedWidth(), 448);
}

QTEST_MAIN(TargetTest)

#include "tst_target.moc"
