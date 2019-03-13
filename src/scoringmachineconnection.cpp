#include "scoringmachineconnection.h"

ScoringMachineConnection::ScoringMachineConnection(QObject *parent) : QObject(parent)
{

}

int ScoringMachineConnection::testFunction(QString input)
{
    return input.toInt() * 10;
}

ScoringMachineConnection::~ScoringMachineConnection()
{

}
