#include <QtTest>
#include <QCoreApplication>

#include "protofinaal.h"
#include "siusshotdata.h"

bool verbose = false;

class ProtofinaalTest : public QObject
{
    Q_OBJECT

public:
    ProtofinaalTest();
    ~ProtofinaalTest();

private slots:
    void readSiusInfo();

};

ProtofinaalTest::ProtofinaalTest()
{
    QFile confFile("conf.json");

    if(confFile.open(QIODevice::WriteOnly)){
        QJsonObject jsonObj;

        jsonObj["lastFile"] = "test.fin";
        jsonObj["windowXLocation"] = 0;
        jsonObj["windowYLocation"] = 0;
        QJsonDocument jsonDoc(jsonObj);
        confFile.write(jsonDoc.toJson());
        confFile.close();
    }

//    QFile file(":/templates/spectatorView_template.html");
//    if(file.open(QIODevice::ReadOnly)) {
//        QByteArray dump = file.readAll();
//        qDebug() << "contents: " << dump;
//    } else {
//        qDebug() << "error: " << file.error();
//    }
    QFile::copy(":/templates/spectatorView_template.html", "spectatorView_template.html");
}

ProtofinaalTest::~ProtofinaalTest()
{

}

void ProtofinaalTest::readSiusInfo()
{
    Protofinaal finaal;
    SiusShotData shot1(13, 0, 1, Lask("_SHOT;17;18;13;60;28;10:02:56.30;3;1;0;10;101;0;1;0.00396;-0.00583;900;0;0;655.35;387137447;64;559;0"));

//    finaal.readSiusInfo(shot1);
}

QTEST_MAIN(ProtofinaalTest)

#include "tst_protofinaaltest.moc"
