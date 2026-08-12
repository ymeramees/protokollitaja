#ifndef DUELPAIR_H
#define DUELPAIR_H

#include <QFrame>
#include <QGridLayout>
#include <QJsonObject>
#include <QLabel>
#include <optional>

#include "laskur.h"

/**
 * One pair of a duel match: the competitor of the left team against the competitor
 * of the right team, holding the same position in both teams.
 *
 * The competitors themselves are ordinary Laskur rows, created and wired up by Leht,
 * so all the usual functionality (autocomplete, shots window, SiusData) is available.
 * DuelPair only groups them visually and calculates the points of the pair.
 */
class DuelPair : public QFrame
{
    Q_OBJECT

public:
    DuelPair(Laskur *leftCompetitor, Laskur *rightCompetitor, int pairNumber, QWidget *parent = nullptr);
    ~DuelPair();
    Laskur* left() const;
    std::optional<int> leftPoints() const;
    Laskur* right() const;
    std::optional<int> rightPoints() const;
    void setPairNumber(int newNumber);
    QJsonObject toExportJson(int localId) const;
    QJsonObject toFileJson() const;

public slots:
    void updatePoints();

private:
    Laskur *m_left = nullptr;
    Laskur *m_right = nullptr;
    QLabel *m_leftPointsLabel = nullptr;
    QLabel *m_rightPointsLabel = nullptr;
    QLabel *m_numberLabel = nullptr;
    std::optional<int> m_leftPoints;
    std::optional<int> m_rightPoints;
    bool hasResult(Laskur *competitor) const;
    bool isExcluded(Laskur *competitor) const;
    void showPoints();
    int totalTenths(Laskur *competitor) const;
};

#endif // DUELPAIR_H
