#include "ageclasses.h"

QMap<AgeClasses::AgeClass, AgeClasses::AgeClassDefinition>& AgeClasses::classDefinitions()
{
    static QMap<AgeClasses::AgeClass, AgeClasses::AgeClassDefinition> *definitions = new QMap<AgeClasses::AgeClass, AgeClasses::AgeClassDefinition>({
        {M, {tr("Men"), tr("M")}},
        {W, {tr("Women"), tr("W")}},
        {MJ, {tr("Men Junior"), tr("MJ")}},
        {WJ, {tr("Women Junior"), tr("WJ")}},
        {YM, {tr("Youth Men"), tr("YM")}},
        {YW, {tr("Youth Women"), tr("YW")}},
        {MS, {tr("Men Senior"), tr("MS")}},
        {WS, {tr("Women Senior"), tr("WS")}},
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
