#include "competitor.h"

Competitor::Competitor(QJsonArray configJson, QWidget *parent) : QWidget(parent)
{
    QHBoxLayout *hBox = new QHBoxLayout;

    nameEdit = new QLineEdit();
    nameEdit->setMinimumWidth(75);
    nameEdit->setMinimumHeight(22);
    nameEdit->setToolTip(tr("Võistleja nimi"));
    hBox->addWidget(nameEdit);

    if(verbose)
        QTextStream(stdout) << "Competitor::configJson.size(): " << configJson.size() << endl;

    for(int i = 0; i < configJson.size(); i++){
        QVector<QLineEdit*> *thisSeries = new QVector<QLineEdit*>;
        for(int j = 0; j < configJson.at(i).toInt(); j++){
//            if(verbose)
//                QTextStream(stdout) << "Competitor::configJson.at(i).toInt(): " << configJson.at(i).toInt() << endl;
            QLineEdit *shotEdit = new QLineEdit;
            shotEdit->setToolTip(tr("Lask"));
            shotEdit->setMinimumWidth(30);
            shotEdit->setMaximumWidth(40);
            shotEdit->setMinimumHeight(22);
            connect(shotEdit, &QLineEdit::editingFinished, this, &Competitor::sum);

            shots.append(shotEdit);
            thisSeries->append(shotEdit);
            hBox->addWidget(shotEdit);
        }
        series.append(thisSeries);

        QLabel *sumLabel = new QLabel(tr("0,0"));
        sumLabels.append(sumLabel);
        hBox->addWidget(sumLabel);
        hBox->setContentsMargins(0, 2, 0, 2);
    }

    if(verbose)
        QTextStream(stdout) << "Competitor::shots: " << shots.size() << " , series: " << series.size() << " ,sumLabels: " << sumLabels.size() << endl;
    setLayout(hBox);
}

Competitor::~Competitor()
{

}

int Competitor::current10Sum()
{
    return sumLabels.at(sumLabels.size()-1)->text().remove(',').toInt();
}

void Competitor::sum()
{
    int totalSum = 0;
    for(int i = 0; i < series.size(); i++){
        int seriesSum = 0;
        for(int j = 0; j < series.at(i)->size(); j++){
            seriesSum += series.at(i)->at(j)->text().remove(',').toInt();
        }
        totalSum += seriesSum;
        double dSeriesSum = seriesSum;
        dSeriesSum /= 10;
        sumLabels.at(i)->setText(QString("%1").arg(dSeriesSum).replace('.', ','));
        if(!sumLabels.at(i)->text().contains(','))
            sumLabels.at(i)->setText(sumLabels.at(i)->text() + ",0");
    }
    double dTotalSum = totalSum;
    dTotalSum /= 10;
    sumLabels.at(sumLabels.size() - 1)->setText(QString("%1").arg(dTotalSum).replace('.', ','));
    if(!sumLabels.at(sumLabels.size() - 1)->text().contains(','))
        sumLabels.at(sumLabels.size() - 1)->setText(sumLabels.at(sumLabels.size() - 1)->text() + ",0");
    emit newShot();
//    return dTotalSum;
}
