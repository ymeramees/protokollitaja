#include <QHBoxLayout>
#include <QVBoxLayout>

#include "duelcompetitorview.h"

namespace {

/**
 * The styles of the points label. The padding is the same in both of them, so that the number
 * does not move when the background is added or removed. A won pair is highlighted with a green
 * background, so that the spectators can see at a glance who has earned a point for their team,
 * while a lost pair is left without any background.
 */
const QString pointsStyle = "padding: 0px 4px;";
const QString wonPointsStyle = "padding: 0px 4px; background-color: #1a9c2e; color: white;";

}

DuelCompetitorView::DuelCompetitorView(Laskur *competitor, Side side, int gunType, QWidget *parent)
    : QFrame(parent)
{
    m_competitor = competitor;

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Plain);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);

    createLayout(side, gunType);
    refresh();
}

Laskur* DuelCompetitorView::competitor() const
{
    return m_competitor;
}

void DuelCompetitorView::createLayout(Side side, int gunType)
{
    QFont nameFont;
    nameFont.setPointSize(15);
    nameFont.setBold(true);
    nameFont.setItalic(true);
    QFont shotFont;
    shotFont.setPointSize(14);
    QFont seriesFont;
    seriesFont.setPointSize(13);
    QFont totalFont;
    totalFont.setPointSize(18);
    totalFont.setBold(true);
    totalFont.setItalic(true);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setFont(nameFont);
    m_nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_targetNoLabel = new QLabel(this);   //Firing point number, coloured according to the team
    m_targetNoLabel->setFont(nameFont);
    m_targetNoLabel->setAlignment(Qt::AlignCenter);
    m_targetNoLabel->setMinimumWidth(40);
    if(side == Left)
        m_targetNoLabel->setStyleSheet("background-color: #f2c200; color: black;");
    else
        m_targetNoLabel->setStyleSheet("background-color: #12408a; color: white;");

    m_shotLabel = new QLabel(this);   //Number of shots and the value of the last shot
    m_shotLabel->setFont(shotFont);
    m_shotLabel->setAlignment(Qt::AlignCenter);

    m_target = new Target(gunType, "", "", this);
    m_target->setZoomEnabled(true);
    m_target->setInfoBoxesVisible(false);   //Name, shot and result are shown around the target instead
    m_target->setMinimumSize(100, 100);

    m_seriesLabel = new QLabel(this);
    m_seriesLabel->setFont(seriesFont);
    m_seriesLabel->setAlignment(Qt::AlignCenter);

    m_totalLabel = new QLabel(this);
    m_totalLabel->setFont(totalFont);
    m_totalLabel->setAlignment(Qt::AlignCenter);

    m_pointsLabel = new QLabel(this);   //Point of the pair, 1 for the winner of the pair
    m_pointsLabel->setFont(totalFont);
    m_pointsLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_pointsLabel->setMinimumWidth(30);
    m_pointsLabel->setStyleSheet(pointsStyle);
    m_pointsLabel->setToolTip(tr("Points of the pair"));

    QHBoxLayout *nameRow = new QHBoxLayout;
    nameRow->setContentsMargins(0, 0, 0, 0);
    nameRow->addWidget(m_nameLabel, 1);
    nameRow->addWidget(m_targetNoLabel);

    QHBoxLayout *totalRow = new QHBoxLayout;
    totalRow->setContentsMargins(0, 0, 0, 0);
    totalRow->addSpacing(30);
    totalRow->addWidget(m_totalLabel, 1);
    totalRow->addWidget(m_pointsLabel);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 2, 4, 2);
    layout->setSpacing(2);
    layout->addLayout(nameRow);
    layout->addWidget(m_shotLabel);
    layout->addWidget(m_target, 1);
    layout->addWidget(m_seriesLabel);
    layout->addLayout(totalRow);
}

/**
 * Clears the target of all the shots drawn onto it, so that the next round of shots can be
 * drawn onto an empty target.
 */
void DuelCompetitorView::clearTarget()
{
    m_target->reset();
    m_drawnShots = 0;
    m_drawnSightingShots = 0;
}

/**
 * Draws the shots that have been added since the previous update, so that the target does not
 * have to be redrawn every time the view is updated. If shots have been removed or edited,
 * the target is reset and all the shots are drawn again.
 *
 * Until the competition begins, the sighting shots are shown, so that the spectators can already
 * follow the competitors. The target is cleared once the first competition shot arrives, as well
 * as when a new round of sighting shots begins, for example before the next stage of the event.
 */
void DuelCompetitorView::drawNewShots()
{
    const QList<Lask*> firedShots = shots();
    const QList<Lask> sightingShots = m_competitor->sightingShots();
    const bool shotsRemoved = firedShots.count() < m_drawnShots || sightingShots.count() < m_drawnSightingShots;

    if(sightingShots.count() > m_drawnSightingShots){   //The competition has not begun yet
        if(shotsRemoved || m_drawnShots > 0)   //The competition shots of the previous stage are not shown with the new sighting shots
            clearTarget();

        for(int i = m_drawnSightingShots; i < sightingShots.count(); i++)
            if(sightingShots[i].X() != -999 && sightingShots[i].Y() != -999)
                m_target->drawAShot(sightingShots[i]);

        m_drawnSightingShots = sightingShots.count();
    }else if(shotsRemoved || firedShots.count() > m_drawnShots){
        if(shotsRemoved || m_drawnSightingShots > 0)   //The sighting shots are wiped off the target once the competition begins
            clearTarget();

        for(int i = m_drawnShots; i < firedShots.count(); i++)
            if(firedShots[i]->X() != -999 && firedShots[i]->Y() != -999)   //Manually entered shots have no coordinates
                m_target->drawAShot(*firedShots[i]);

        m_drawnShots = firedShots.count();
    }
}

QString DuelCompetitorView::name() const
{
    const QString familyName = m_competitor->perekNimi->text().trimmed();
    const QString firstName = m_competitor->eesNimi->text().trimmed();

    if(familyName.isEmpty())
        return firstName;
    if(firstName.isEmpty())
        return familyName.toUpper();

    return QString("%1, %2").arg(familyName.toUpper(), firstName);
}

void DuelCompetitorView::refresh()
{
    m_nameLabel->setText(name());
    m_targetNoLabel->setText(m_competitor->rajaNr->text());

    QStringList series;
    foreach (QLineEdit *serie, m_competitor->seeriad)
        if(!serie->text().isEmpty())
            series << serie->text();
    m_seriesLabel->setText(series.join(' '));

    const QString total = m_competitor->getSumma();
    if(total.isEmpty() || total == "0" || total == "0,0")
        m_totalLabel->setText("-");
    else
        m_totalLabel->setText(total);

    const QList<Lask*> firedShots = shots();
    QString shotInfo;
    if(!firedShots.isEmpty())
        shotInfo = QString("%1: %2").arg(QString::number(firedShots.count()), shotValue(firedShots.last(), withDecimals()));
    const QString remark = m_competitor->markus->text().trimmed();
    if(!remark.isEmpty())
        shotInfo = shotInfo.isEmpty() ? remark : QString("%1  %2").arg(shotInfo, remark);
    m_shotLabel->setText(shotInfo);

    drawNewShots();
    updateSighterMark();
}

/**
 * All the shots the competitor has shot, in the order they were shot. The additional shot of every
 * series, meant for penalties, is left out, as it has no coordinates on the target.
 */
QList<Lask*> DuelCompetitorView::shots() const
{
    QList<Lask*> firedShots;

    for(int series = 0; series < m_competitor->lasud.count(); series++)
        for(int shot = 0; shot < m_competitor->laskudeArv && shot < m_competitor->lasud[series].count(); shot++)
            if(m_competitor->lasud[series][shot]->getILask() != -999)
                firedShots << m_competitor->lasud[series][shot];

    return firedShots;
}

/**
 * The value of a shot as it is shown next to the target. Inner tens are marked with an asterisk.
 * When the results are scored with full rings, the decimal value is added in parenthesis, so that
 * the spectators can still see how good the shot was, for example "10* (10,5)".
 */
QString DuelCompetitorView::shotValue(const Lask *shot, const bool withDecimals)
{
    if(shot == nullptr || shot->getILask() == -999)
        return QString();

    QString value = withDecimals ? shot->getSLask() : QString::number(shot->getILask());
    if(shot->isInnerTen())
        value.append('*');
    if(!withDecimals)
        value = QString("%1 (%2)").arg(value, shot->getSLask());

    return value;
}

void DuelCompetitorView::setPoints(const std::optional<int> points)
{
    if(points.has_value())
        m_pointsLabel->setText(QString("%1").arg(points.value()));
    else
        m_pointsLabel->setText("");

    m_pointsLabel->setStyleSheet(points.value_or(0) > 0 ? wonPointsStyle : pointsStyle);
}

/**
 * If the sighter mark is currently shown in the corner of the target, telling the spectators that
 * the shots they see do not count yet.
 */
bool DuelCompetitorView::showsSighterMark() const
{
    return m_target->sighterMarkVisible();
}

/**
 * Adds the sighter mark onto the target or removes it, according to the shots currently drawn onto
 * it: the mark is shown as long as there are no competition shots on the target, so either
 * the competitor has not shot at all yet or the shots drawn are sighting shots.
 */
void DuelCompetitorView::updateSighterMark()
{
    m_target->setSighterMarkVisible(m_drawnShots == 0);
}

/**
 * If the results of the competition are scored with decimals or with full rings.
 */
bool DuelCompetitorView::withDecimals() const
{
    return m_competitor->kumnendikega != nullptr && *m_competitor->kumnendikega;
}

DuelCompetitorView::~DuelCompetitorView()
{

}
