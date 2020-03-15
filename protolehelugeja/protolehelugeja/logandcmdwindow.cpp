#include "logandcmdwindow.h"
#include "ui_logandcmdwindow.h"

#include <QTextStream>

LogAndCmdWindow::LogAndCmdWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogAndCmdWindow)
{
    ui->setupUi(this);

    connect(ui->sendButton, &QPushButton::pressed, this, &LogAndCmdWindow::sendText);
    connect(ui->cmdEdit, &QLineEdit::returnPressed, this, &LogAndCmdWindow::sendText);
}

LogAndCmdWindow::~LogAndCmdWindow()
{
    delete ui;
}

void LogAndCmdWindow::append(QString textToAppend, bool received)
{
    if(!received){  // Sent text with blue colour, received one black
        textToAppend.prepend("<font color=blue>");
        textToAppend.append("</font>");
    }

    ui->logView->append(textToAppend);
}

void LogAndCmdWindow::sendText()
{
    if(ui->cmdEdit->text().isEmpty())
        return;
    else if(ui->cmdEdit->text().contains("uuenda porte", Qt::CaseInsensitive)){
        ui->logView->append("Uuenda porte:");
        emit(receivedCommand("uuenda porte"));
    }else if(ui->cmdEdit->text().left(3) == "COM" && ui->cmdEdit->text().length() <= 5 && ui->cmdEdit->text().length() >= 4){
        ui->logView->append("Lisatud: " + ui->cmdEdit->text());
        emit(changeVariable("comPort;" + ui->cmdEdit->text()));
    }else if(ui->cmdEdit->text().startsWith("scoringMachineType=") && ui->cmdEdit->text().length() > 19){
        emit(changeVariable("scoringMachineType;" + ui->cmdEdit->text().mid(19)));
    }else if(ui->cmdEdit->text() == "ACK"){
        emit(receivedCommand("ACK"));
    }else{
        emit(receivedCommand(ui->cmdEdit->text()));
    }
    ui->cmdEdit->clear();
}
