#ifndef DUELVIEW_H
#define DUELVIEW_H

#include <QGridLayout>
#include <QLabel>
#include <QList>
#include <QVBoxLayout>
#include <QWidget>

#include "duelcompetitorview.h"
#include "duelpair.h"
#include "targettypes.h"

/**
 * The spectator view of a duel match: one target view per competitor, the pairs shown as columns,
 * with the competitor of the first team above the competitor of the second team, similar to
 * the views used in Bundesliga matches.
 *
 * The view is kept between the updates, so the targets keep their shots and zoom level. The target
 * views are recreated only when another match or another kind of target is shown.
 */
class DuelView : public QWidget
{
    Q_OBJECT

public:
    explicit DuelView(QWidget *parent = nullptr);
    ~DuelView();
    static int gunType(const TargetTypes::TargetType targetType);   // Type of the target picture drawn by Target
    void setHeading(const QString &competitionName, const QString &matchName, const QString &timePlace);
    void showMatch(const QList<DuelPair*> &pairs, const TargetTypes::TargetType targetType);

private:
    static constexpr int m_maxColumns = 5;   // Number of pairs shown side by side
    TargetTypes::TargetType m_targetType = TargetTypes::Other;
    QGridLayout *m_pairsLayout = nullptr;
    QLabel *m_competitionNameLabel = nullptr;
    QLabel *m_matchNameLabel = nullptr;
    QLabel *m_scoreLabel = nullptr;
    QLabel *m_timePlaceLabel = nullptr;
    QList<DuelPair*> m_pairs;
    QList<DuelCompetitorView*> m_competitorViews;   // Two views for every pair: the left one first
    void createCompetitorViews(const QList<DuelPair*> &pairs);
    void createHeading(QVBoxLayout *layout);
    void removeCompetitorViews();
};

#endif // DUELVIEW_H
