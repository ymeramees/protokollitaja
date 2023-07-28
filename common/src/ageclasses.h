#ifndef AGECLASSES_H
#define AGECLASSES_H

#include <QObject>
#include <QString>
#include <QMap>

class AgeClasses : public QObject
{
    Q_OBJECT
public:
    enum AgeClass {
        M,
        W,
        MJ,
        WJ,
        YM,
        YW,
        MS,
        WS
    };

    struct AgeClassDefinition {
        QString name;
        QString abbreviation;
    };

    static AgeClassDefinition ageClassName(const AgeClass ageClass);
    static QStringList classNames();
    static QStringList classAbbreviations();

private:
    static QMap<AgeClass, AgeClassDefinition>& classDefinitions();

};

#endif // AGECLASSES_H
