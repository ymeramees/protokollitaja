#ifndef TARGET_H
#define TARGET_H

#include <QColor>
#include <QLabel>
#include <QList>
#include <QPainter>
#include <QPainterPath>
#ifdef QT_DEBUG
#include <QDebug>
#endif

#include "lask.h"

class Target : public QLabel
{
    Q_OBJECT

private:
    int m_caliber;   // Diameter of shot circle in mm
    int m_farthestShot;    // Distance of the furthest shot from the centre in px, 0 if no shots have been drawn
    int m_multiplier;    // Ratio between mm and px'es, scale: 1px = m_multiplier * mm
    int m_targetRadius;
    int m_gunType;
//    int m_zoomLevel;
    bool m_active;  // Shows if the competitor is dropped out in finals or not
    bool m_infoBoxesVisible;    // Will the name, target no, shot and result boxes be drawn onto the target or not
    bool m_zoomEnabled;    // Will the target automatically zoom or not
    QImage *m_targetImage = nullptr;
    Lask m_previousShot;
    int m_shotNumber = 0;   // Number of shots drawn since the last reset, shown as the marker's label
    QList<Lask> m_seriesShots;   // Shots of the current series (at most 10), the only ones shown on the target
    QPainter *m_targetPainter = nullptr;
    QString m_name;
    QString m_targetNo;
    QString m_result;
    static const QStringList m_targetTypes;// = QStringList() << QString::fromLatin1("Air Rifle") << QString::fromLatin1("Air Pistol") << QString::fromLatin1("50m Rifle");

private slots:
//    void mouseReleaseEvent(QMouseEvent * event);
    void resizeEvent(QResizeEvent * e);
public:
    explicit Target(QWidget *parent = nullptr);
    explicit Target(int m_gunType, QString n, QString r, QWidget *parent = nullptr);
    ~Target();

signals:

public slots:
    bool infoBoxesVisible();
    bool zoomEnabled();
    QString name();
    QString targetNo();
    QStringList targetTypes();
    int gunType();
    void init(int valik);
    void drawAShot(Lask & l);    // Coordinates in millimeters
    void drawTarget();
    void reset();
    void setActive(bool a);
    void setInfoBoxesVisible(bool newInfoBoxesVisible);
    void setZoomEnabled(bool newZoomEnabled);
    void setName(QString n);
    void setTargetNo(QString r);
    void setGunType(int r);
    void setResult(QString newResult);
    int zoomedWidth();    // Width of the target's area currently shown, in the target image's px
    void zoomAndUpdate();

private:
    void drawShotMarker(const QPointF &center, double fillRadius, double edgeWidth, const QColor &fillColor, const QColor &borderColor, int shotNumber);
    void redrawSeriesShots();
};

#endif // TARGET_H
