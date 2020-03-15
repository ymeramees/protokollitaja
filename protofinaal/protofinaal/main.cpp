#include "protofinaal.h"
#include <QApplication>
#include <QTextStream>

bool verbose = false;

int main(int argc, char *argv[])
{
    if(argc > 1)
        for(int i = 0; i < argc; i++)
            if(QString("%1").arg(argv[i]) == "-v" || QString("%1").arg(argv[i]) == "-debug"){
                QTextStream(stdout) << "-v => Debug mode!" << endl;
                verbose = true;
            }

    QApplication a(argc, argv);
    QCoreApplication::setApplicationVersion("3.0.0");
    Protofinaal w;
    w.show();

    return a.exec();
}
