#include <QtGlobal>
#if QT_VERSION >= 0x050000
#include <QApplication>
#else
#include <QtGui/QApplication>
#endif
#include "globalvariables.h"
#include "protokollitaja.h"
#include "version.h"

#ifdef ASM_CRASH_REPORT
#include "asmCrashReport.h"
#endif

#include <QLocale>

QString organization = "Ümeramees";
QString programmiNimi = VER_INTERNALNAME_STR; //"Protokollitaja 0.5 Beta"; // TODO uuendada nime
QString versioon = VER_PRODUCTVERSION_STR; //"0.5.x Beta";
QString aasta = VER_AASTA;
QString argument;
bool verbose = false;
bool veryVerbose = false;

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_Use96Dpi);    // Disable scaling as it does not work properly

    QLocale::setDefault(QLocale::Estonian);
    if(argc > 1)
        for(int i = 0; i < argc; i++){
            if(QString("%1").arg(argv[i]) == "-v"){
                QTextStream(stdout) << "-v => kuvatakse debug info" << endl;
                verbose = true;
            }else if(QString("%1").arg(argv[i]) == "-vv"){
                QTextStream(stdout) << "-vv => kuvatakse rohke debug info" << endl;
                verbose = true;
            }
        }


    QApplication a(argc, argv);

    QFont font = qApp->font();
#ifdef Q_OS_UNIX
    font.setPixelSize(14);
#else
    font.setPixelSize(11);
#endif
    qApp->setFont(font);    // Fix font size, as scaling does not work properly

    a.setApplicationName(programmiNimi);
    a.setOrganizationName(organization);
    a.setApplicationVersion(versioon);

#ifdef ASM_CRASH_REPORT
  asmCrashReport::setSignalHandler( QString(), [] (const QString &inFileName, bool inSuccess) {
      QString  message;

      if ( inSuccess ) {
          message = QStringLiteral( "Sorry, %1 has crashed. A log file was written to:\n\n%2\n\nPlease email this to ymeramees@gmail.com." ).arg( QCoreApplication::applicationName(), inFileName );
      } else {
          message = QStringLiteral( "Sorry, %1 has crashed and we could not write a log file to:\n\n%2\n\nPlease contact ymeramees@gmail.com." ).arg( QCoreApplication::applicationName(), inFileName );
      }

      QMessageBox::critical( nullptr, QObject::tr( "%1 Crashed" ).arg( QCoreApplication::applicationName() ), message );
  });
#endif

    Protokollitaja w;
    w.show();
    a.connect(&a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()));
    return a.exec();
}
