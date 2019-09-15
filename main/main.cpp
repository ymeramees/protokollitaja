#include "lehelugeja.h"
#include <QApplication>
#include "version.h"

QString programmiNimi = VER_INTERNALNAME_STR; //"Protolehelugeja 0.3 Beta"; // TODO uuendada nime
QString versioon = VER_PRODUCTVERSION_STR; //"0.3.0 Beta";
QString aasta = VER_AASTA;
QString argument;
bool verbose = false;
bool veryVerbose = false;

int main(int argc, char *argv[])
{
    if(argc > 1)
        for(int i = 0; i < argc; i++)
            if(QString("%1").arg(argv[i]) == "-v"){
                QTextStream(stdout) << "-v => kuvatakse debug info" << endl;
                verbose = true;
            }else if(QString("%1").arg(argv[i]) == "-vv"){
                QTextStream(stdout) << "-vv => kuvatakse rohke debug info" << endl;
                verbose = true;
            }
    QApplication a(argc, argv);
    Lehelugeja w;
    w.show();

    return a.exec();
}
