#include <algorithm>

#include "duelpair.h"

DuelPair::DuelPair(Laskur *leftCompetitor, Laskur *rightCompetitor, int pairNumber, QWidget *parent)
    : QFrame(parent)
{
    m_left = leftCompetitor;
    m_right = rightCompetitor;

    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);

    m_numberLabel = new QLabel(this);
    m_numberLabel->setToolTip(tr("Pair number"));
    m_numberLabel->setMinimumWidth(25);
    m_numberLabel->setMaximumWidth(25);
    setPairNumber(pairNumber);

    m_leftPointsLabel = new QLabel(this);
    m_leftPointsLabel->setToolTip(tr("Points of the pair"));
    m_leftPointsLabel->setMinimumWidth(20);
    m_leftPointsLabel->setMaximumWidth(20);
    m_leftPointsLabel->setAlignment(Qt::AlignCenter);
    m_rightPointsLabel = new QLabel(this);
    m_rightPointsLabel->setToolTip(tr("Points of the pair"));
    m_rightPointsLabel->setMinimumWidth(20);
    m_rightPointsLabel->setMaximumWidth(20);
    m_rightPointsLabel->setAlignment(Qt::AlignCenter);

    QGridLayout *layout = new QGridLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);
    layout->addWidget(m_numberLabel, 0, 0, 2, 1);
    m_left->setParent(this);
    m_right->setParent(this);
    layout->addWidget(m_left, 0, 1);
    layout->addWidget(m_right, 1, 1);
    layout->addWidget(m_leftPointsLabel, 0, 2);
    layout->addWidget(m_rightPointsLabel, 1, 2);

    showPoints();
}

bool DuelPair::hasResult(Laskur *competitor) const
{
    const QString total = competitor->getSumma();
    return total != "0" && total != "0,0" && !total.isEmpty();
}

Laskur* DuelPair::left() const
{
    return m_left;
}

std::optional<int> DuelPair::leftPoints() const
{
    return m_leftPoints;
}

Laskur* DuelPair::right() const
{
    return m_right;
}

std::optional<int> DuelPair::rightPoints() const
{
    return m_rightPoints;
}

/**
 * Tells if the competitor has been excluded from the competition, so that they cannot win
 * their pair, regardless of the result they have shot.
 */
bool DuelPair::isExcluded(Laskur *competitor) const
{
    const QStringList statuses = {"DSQ", "DNF", "V.A"};
    return std::any_of(statuses.cbegin(), statuses.cend(), [competitor](const QString &status){
        return competitor->markus->text().contains(status, Qt::CaseInsensitive);
    });
}

void DuelPair::setPairNumber(int newNumber)
{
    m_numberLabel->setText(QString("%1.").arg(newNumber));
}

void DuelPair::showPoints()
{
    if(m_leftPoints.has_value())
        m_leftPointsLabel->setText(QString("<b>%1</b>").arg(m_leftPoints.value()));
    else
        m_leftPointsLabel->setText("-");

    if(m_rightPoints.has_value())
        m_rightPointsLabel->setText(QString("<b>%1</b>").arg(m_rightPoints.value()));
    else
        m_rightPointsLabel->setText("-");
}

/**
 * The total result of the competitor in tenths of a ring, so that the totals can be compared
 * as exact integers, no matter if decimals are in use or not.
 */
int DuelPair::totalTenths(Laskur *competitor) const
{
    QString total = competitor->getSumma();
    total.replace(',', '.');
    return qRound(total.toDouble() * 10);
}

QJsonObject DuelPair::toExportJson(int localId) const
{
    QJsonObject json;
    json["id"] = localId;
    // The two sides of a pair carry the whole competitor, not just a reference to one, matching the
    // backend's Pair entity.
    json["left"] = m_left->toExportJson();
    json["right"] = m_right->toExportJson();
    if(m_leftPoints.has_value())
        json["leftPoints"] = m_leftPoints.value();
    if(m_rightPoints.has_value())
        json["rightPoints"] = m_rightPoints.value();
    return json;
}

/**
 * The full internal representation of a pair, for saving to and loading from the .kll file. Unlike
 * toExportJson(), the competitors are serialized with Laskur::toJson(), which keeps all the details
 * (shots, target numbers, ciphers, ...) needed to keep editing the competitors after loading.
 * Points are not included, as they are always recalculated from the loaded results by updatePoints().
 */
QJsonObject DuelPair::toFileJson() const
{
    QJsonObject json;
    json["left"] = m_left->toJson();
    json["right"] = m_right->toJson();
    return json;
}

/**
 * Only the competitor with the higher total result wins the pair and gets 1 point. Equal totals
 * are not separated by the tie-breaking rules of an individual event, both competitors get
 * 0 points instead.
 */
void DuelPair::updatePoints()
{
    m_left->liida();
    m_right->liida();

    if(!hasResult(m_left) && !hasResult(m_right)){  // Nobody has shot yet
        m_leftPoints.reset();
        m_rightPoints.reset();
        showPoints();
        return;
    }

    const bool leftExcluded = isExcluded(m_left);
    const bool rightExcluded = isExcluded(m_right);

    if(leftExcluded != rightExcluded){  // An excluded competitor cannot win their pair
        m_leftPoints = leftExcluded ? 0 : 1;
        m_rightPoints = leftExcluded ? 1 : 0;
    }else{
        const int leftTotal = totalTenths(m_left);
        const int rightTotal = totalTenths(m_right);
        m_leftPoints = leftTotal > rightTotal ? 1 : 0;
        m_rightPoints = rightTotal > leftTotal ? 1 : 0;
    }

    showPoints();
}

DuelPair::~DuelPair()
{

}
