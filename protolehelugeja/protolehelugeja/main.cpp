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
    QApplication::setAttribute(Qt::AA_Use96Dpi);    // Disable scaling as it does not work properly

    if(argc > 1)
        for(int i = 0; i < argc; i++)
            if(QString("%1").arg(argv[i]) == "-v"){
                QTextStream(stdout) << "-v => kuvatakse debug info" << Qt::endl;
                verbose = true;
            }else if(QString("%1").arg(argv[i]) == "-vv"){
                QTextStream(stdout) << "-vv => kuvatakse rohke debug info" << Qt::endl;
                verbose = true;
            }
    QApplication a(argc, argv);

    QFont font = qApp->font();
#ifdef Q_OS_UNIX
    font.setPixelSize(18);
#else
    font.setPixelSize(12);
#endif
    qApp->setFont(font);    // Fix font size, as scaling does not work properly

    Lehelugeja w;
    w.show();

    return a.exec();
}
