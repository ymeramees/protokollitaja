#include "protorangecontrol.h"

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

bool verbose = false;

int main(int argc, char *argv[])
{
    if(argc > 1)
        for(int i = 0; i < argc; i++){
            if(QString("%1").arg(argv[i]) == "-v"){
                QTextStream(stdout) << "-v => kuvatakse debug info" << Qt::endl;
                verbose = true;
            }else if(QString("%1").arg(argv[i]) == "-vv"){
                QTextStream(stdout) << "-vv => kuvatakse rohke debug info" << Qt::endl;
                verbose = true;
            }
        }

    QApplication a(argc, argv);
    ProtoRangeControl w;
    w.show();
    return a.exec();
}
