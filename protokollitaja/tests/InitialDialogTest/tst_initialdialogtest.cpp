#include <QtTest>
#include <QDate>

#include "initialdialog.h"

class InitialDialogTest : public QObject
{
    Q_OBJECT

public:
    InitialDialogTest();
    ~InitialDialogTest();

private slots:
    void test_case();

};

InitialDialogTest::InitialDialogTest()
{

}

InitialDialogTest::~InitialDialogTest()
{

}

void InitialDialogTest::test_case()
{
}

QTEST_MAIN(InitialDialogTest)

#include "tst_initialdialogtest.moc"
