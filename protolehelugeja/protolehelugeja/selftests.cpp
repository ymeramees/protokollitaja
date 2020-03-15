#include "selftests.h"

SelfTests::SelfTests(QObject *parent) : QObject(parent)
{

}

void SelfTests::generateShots(int noOfShots)
{
    for(int i = 0; i < noOfShots - 1; i++)
        emit shotRead(
                Lask("_SHOT;9;10;36;60;73;10:43:18.64;3;31;7;96;0;0;48;-0.00197546;-0.00262108;900;0;0;655.35;98899783;61;450;0")
                );
    emit shotRead(
            Lask("_SHOT;9;10;36;60;73;10:43:18.64;3;31;7;97;0;0;48;-0.00197546;-0.00262108;900;0;0;655.35;98899783;61;450;0")
            );
}
