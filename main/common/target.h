#ifndef TARGET_H
#define TARGET_H

#include <QLabel>
#include <QPainter>
#ifdef PROOV
#include <QDebug>
#endif

#include "lask.h"

class Target : public QLabel
{
    Q_OBJECT

private:
    int m_caliber;   // Diameter of shot circle in mm
    int m_farthestX;
    int m_farthestY;
    int m_multiplier;    // Ratio between mm and px'es, scale: 1px = m_multiplier * mm
    int m_targetRadius;
    int m_gunType;
//    int m_zoomLevel;
    bool m_active;  // Shows if the competitor is dropped out in finals or not
    bool m_zoomEnabled;    // Will the target automatically zoom or not
    QImage *m_targetImage;
    Lask m_previousShot;
    QPainter *m_targetPainter;
    QString m_name;
    QString m_targetNo;
    QString m_result;
    static const QStringList m_targetTypes;// = QStringList() << QString::fromLatin1("Õhupüss") << QString::fromLatin1("Õhupüstol") << QString::fromLatin1("Sportpüss");

private slots:
//    void mouseReleaseEvent(QMouseEvent * event);
    void resizeEvent(QResizeEvent * e);
public:
    explicit Target(QWidget *parent = nullptr);
    explicit Target(int m_gunType, QString n, QString r, QWidget *parent = nullptr);

signals:

public slots:
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
    void setZoomEnabled(bool newZoomEnabled);
    void setName(QString n);
    void setTargetNo(QString r);
    void setGunType(int r);
    void setResult(QString newResult);
    void zoomAndUpdate();

};

#endif // TARGET_H
