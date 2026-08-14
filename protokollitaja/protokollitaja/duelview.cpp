#include <QHBoxLayout>
#include <QVBoxLayout>

#include "duelview.h"

DuelView::DuelView(QWidget *parent) : QWidget(parent)
{
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);

    m_pairsLayout = new QGridLayout;
    m_pairsLayout->setContentsMargins(0, 0, 0, 0);
    m_pairsLayout->setSpacing(4);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(4);
    createHeading(layout);
    layout->addLayout(m_pairsLayout, 1);
}

/**
 * Creates a target view for both competitors of every pair. The pairs are shown as columns, so that
 * the competitor of the first team is above the competitor of the second team. If there are more
 * pairs than fits into one row of columns, the next pairs are shown below the previous ones.
 */
void DuelView::createCompetitorViews(const QList<DuelPair*> &pairs)
{
    removeCompetitorViews();

    m_pairs = pairs;
    const int columns = qMin(qMax(pairs.count(), 1), m_maxColumns);

    for(int i = 0; i < pairs.count(); i++){
        DuelCompetitorView *leftView = new DuelCompetitorView(pairs[i]->left(), DuelCompetitorView::Left,
                                                              gunType(m_targetType), this);
        DuelCompetitorView *rightView = new DuelCompetitorView(pairs[i]->right(), DuelCompetitorView::Right,
                                                               gunType(m_targetType), this);
        const int row = (i / columns) * 2;   // Every pair takes two rows
        const int column = i % columns;
        m_pairsLayout->addWidget(leftView, row, column);
        m_pairsLayout->addWidget(rightView, row + 1, column);
        m_competitorViews << leftView << rightView;
        leftView->show();
        rightView->show();
    }
}

void DuelView::createHeading(QVBoxLayout *layout)
{
    QFont competitionFont;
    competitionFont.setPointSize(22);
    competitionFont.setBold(true);
    QFont matchFont;
    matchFont.setPointSize(20);
    matchFont.setBold(true);
    QFont scoreFont;
    scoreFont.setPointSize(28);
    scoreFont.setBold(true);
    QFont timePlaceFont;
    timePlaceFont.setPointSize(14);

    m_competitionNameLabel = new QLabel(this);
    m_competitionNameLabel->setFont(competitionFont);
    m_competitionNameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_matchNameLabel = new QLabel(this);
    m_matchNameLabel->setFont(matchFont);
    m_matchNameLabel->setAlignment(Qt::AlignCenter);

    m_scoreLabel = new QLabel(this);   //Points of the match
    m_scoreLabel->setFont(scoreFont);
    m_scoreLabel->setAlignment(Qt::AlignCenter);

    m_timePlaceLabel = new QLabel(this);
    m_timePlaceLabel->setFont(timePlaceFont);
    m_timePlaceLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QVBoxLayout *matchBox = new QVBoxLayout;
    matchBox->setContentsMargins(0, 0, 0, 0);
    matchBox->setSpacing(0);
    matchBox->addWidget(m_matchNameLabel);
    matchBox->addWidget(m_scoreLabel);

    QHBoxLayout *headingLayout = new QHBoxLayout;
    headingLayout->setContentsMargins(0, 0, 0, 0);
    headingLayout->addWidget(m_competitionNameLabel, 1);
    headingLayout->addLayout(matchBox);
    headingLayout->addWidget(m_timePlaceLabel, 1);

    layout->addLayout(headingLayout);
}

/**
 * The type of the target picture Target draws: 0 - air rifle, 1 - air pistol, 2 - 50m rifle.
 * Same conversion as in Protofinaal's spectator view.
 */
int DuelView::gunType(const TargetTypes::TargetType targetType)
{
    switch(targetType){
    case TargetTypes::AirPistol :
    case TargetTypes::FreePistol :
    case TargetTypes::RapidPistol :
    case TargetTypes::OtherPistol :
        return 1;
    case TargetTypes::SmallboreRifle :
    case TargetTypes::Rifle300m :
    case TargetTypes::RunningTarget50m :
    case TargetTypes::OtherRifle :
        return 2;
    default :
        return 0;   //Air rifle target is used also for all the other kinds of events
    }
}

void DuelView::removeCompetitorViews()
{
    foreach (DuelCompetitorView *view, m_competitorViews) {
        m_pairsLayout->removeWidget(view);
        view->deleteLater();
    }
    m_competitorViews.clear();
    m_pairs.clear();
}

void DuelView::setHeading(const QString &competitionName, const QString &matchName, const QString &timePlace)
{
    m_competitionNameLabel->setText(competitionName);
    m_matchNameLabel->setText(matchName);
    m_timePlaceLabel->setText(timePlace);
}

void DuelView::showMatch(const QList<DuelPair*> &pairs, const TargetTypes::TargetType targetType)
{
    if(pairs != m_pairs || targetType != m_targetType){   //Only another match needs new target views
        m_targetType = targetType;
        createCompetitorViews(pairs);
    }

    int leftPoints = 0;
    int rightPoints = 0;

    for(int i = 0; i < m_pairs.count(); i++){
        leftPoints += m_pairs[i]->leftPoints().value_or(0);
        rightPoints += m_pairs[i]->rightPoints().value_or(0);
        m_competitorViews[i * 2]->setPoints(m_pairs[i]->leftPoints());
        m_competitorViews[i * 2 + 1]->setPoints(m_pairs[i]->rightPoints());
    }

    foreach (DuelCompetitorView *view, m_competitorViews)
        view->refresh();

    m_scoreLabel->setText(QString("%1 : %2").arg(leftPoints).arg(rightPoints));
}

DuelView::~DuelView()
{

}
