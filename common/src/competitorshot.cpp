#include "competitorshot.h"

CompetitorShot::CompetitorShot()
{
    clear();
}

void CompetitorShot::clear()
{
    Lask::clear();
    m_isIgnored = false;
//    m_innerTen = false;
//    m_lask = -999;
//    m_shotTime = QTime();
//    m_x = -999;
//    m_y = -999;
//    m_competitionShot = false;
//    m_shotOrigin = OriginType::Manual;
}

bool CompetitorShot::isIgnored() const
{
    return m_isIgnored;
}

void CompetitorShot::setIgnored(bool ignored)
{
    m_isIgnored = ignored;
}
