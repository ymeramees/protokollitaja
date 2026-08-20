#ifndef DUELCOMPETITORVIEW_H
#define DUELCOMPETITORVIEW_H

#include <QFrame>
#include <QLabel>
#include <optional>

#include "laskur.h"
#include "target.h"

/**
 * One competitor's target view of a duel match, for the spectator screen: the competitor's name,
 * number of shots with the last shot's value, the target picture with all the shots on it,
 * series and total, similar to the target views used in Bundesliga matches.
 *
 * The target picture itself is the same Target widget that Protofinaal's spectator view uses.
 * As all the information is shown around the target, the target's own information boxes
 * are turned off.
 */
class DuelCompetitorView : public QFrame
{
    Q_OBJECT

public:
    enum Side { Left = 0, Right = 1 };  // Which team's competitor it is, only used for colouring
    DuelCompetitorView(Laskur *competitor, Side side, int gunType, QWidget *parent = nullptr);
    ~DuelCompetitorView();
    Laskur* competitor() const;
    void setPoints(const std::optional<int> points);
    static QString shotValue(const Lask *shot, const bool withDecimals);

public slots:
    void refresh();

private:
    int m_drawnShots = 0;   //Number of competition shots already drawn onto the target
    int m_drawnSightingShots = 0;   //Number of sighting shots already drawn onto the target
    Laskur *m_competitor = nullptr;
    Target *m_target = nullptr;
    QLabel *m_nameLabel = nullptr;
    QLabel *m_pointsLabel = nullptr;
    QLabel *m_seriesLabel = nullptr;
    QLabel *m_shotLabel = nullptr;
    QLabel *m_targetNoLabel = nullptr;
    QLabel *m_totalLabel = nullptr;
    void clearTarget();
    void createLayout(Side side, int gunType);
    void drawNewShots();
    QString name() const;
    QList<Lask*> shots() const;
    bool withDecimals() const;
};

#endif // DUELCOMPETITORVIEW_H
