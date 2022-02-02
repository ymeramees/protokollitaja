#ifndef COMPETITORSHOT_H
#define COMPETITORSHOT_H

#include "lask.h"

class CompetitorShot : public Lask
{
public:
    CompetitorShot();
    void clear();
    bool isIgnored() const;
    void setIgnored(bool ignored);

private:
    bool m_isIgnored;
};

#endif // COMPETITORSHOT_H
