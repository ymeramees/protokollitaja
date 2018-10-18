#include "lehelugeja.h"
#include <QApplication>
#include "version.h"

QString programmiNimi = VER_INTERNALNAME_STR; //"Protolehelugeja 0.3 Beta"; // TODO uuendada nime
QString versioon = VER_PRODUCTVERSION_STR; //"0.3.0 Beta";
QString aasta = VER_AASTA;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Lehelugeja w;
    w.show();

    return a.exec();
}
