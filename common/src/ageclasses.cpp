#include "ageclasses.h"

QMap<AgeClasses::AgeClass, AgeClasses::AgeClassDefinition>& AgeClasses::classDefinitions()
{
    static QMap<AgeClasses::AgeClass, AgeClasses::AgeClassDefinition> *definitions = new QMap<AgeClasses::AgeClass, AgeClasses::AgeClassDefinition>({
        {M, {tr("Mehed"), tr("M")}},
        {W, {tr("Naised"), tr("N")}},
        {MJ, {tr("Meesjuuniorid"), tr("MJ")}},
        {WJ, {tr("Naisjuuniorid"), tr("NJ")}},
        {YM, {tr("Poisid"), tr("P")}},
        {YW, {tr("Tüdrukud"), tr("T")}},
        {MS, {tr("Meesveteranid"), tr("MV")}},
        {WS, {tr("Naisveteranid"), tr("NV")}},
    });
    return *definitions;
}

AgeClasses::AgeClassDefinition AgeClasses::ageClassName(const AgeClass ageClass)
{
    return classDefinitions().value(ageClass);
}

QStringList AgeClasses::classAbbreviations()
{
    QStringList names;
    for (AgeClassDefinition ageClass : classDefinitions()) {
        names << ageClass.abbreviation;
    }
    return names;
}

QStringList AgeClasses::classNames()
{
    QStringList names;
    for (AgeClassDefinition ageClass : classDefinitions()) {
        names << ageClass.name;
    }
    return names;
}
