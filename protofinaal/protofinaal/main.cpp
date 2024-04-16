#include "protofinaal.h"
#include <QApplication>
#include <QTextStream>

bool verbose = false;

int main(int argc, char *argv[])
{
    QString fileName;
    if(argc > 1)
        for(int i = 0; i < argc; i++) {
            QString currentArg = QString("%1").arg(argv[i]);
            if (currentArg == "-v" || currentArg == "-debug" || currentArg == "--debug") {
                QTextStream(stdout) << "-v => Debug mode!" << Qt::endl;
                verbose = true;
            } else if (currentArg == "-f" || currentArg == "--file" && i + 1 < argc) {
                fileName = QString("%1").arg(argv[i+1]);
            }
        }

    QApplication a(argc, argv);
    QCoreApplication::setApplicationVersion("3.0.0");
    Protofinaal w(fileName);
    w.show();

    return a.exec();
}
