#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QApplication>
#else
#include <QtGui/QApplication>
#endif
#include "protokollitaja.h"
#include "version.h"

#include <QLocale>

QString programmiNimi = VER_INTERNALNAME_STR; //"Protokollitaja 0.5 Beta"; // TODO uuendada nime
QString versioon = VER_PRODUCTVERSION_STR; //"0.5.x Beta";
QString aasta = VER_AASTA;
QString argument;
bool verbose = false;
bool veryVerbose = false;
//QDir asukoht;

int main(int argc, char *argv[])
{
    QLocale::setDefault(QLocale::Estonian);
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
//    asukoht = QDir(argv[0]);
//    asukoht.cdUp();
    a.setApplicationName(programmiNimi);
    a.setOrganizationName("Ümeramees");
    Protokollitaja w;
    w.show();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
