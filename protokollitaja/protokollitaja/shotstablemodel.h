#ifndef SHOTSTABLEMODEL_H
#define SHOTSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QItemSelection>
#include <QMessageBox>
#include <QDebug>
#include <QIcon>
#include "competitorshot.h"

class ShotsTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ShotsTableModel(QVector<CompetitorShot> *shots, QObject *parent = nullptr);
    bool deleteShot(int row);
    bool ignoreShot(int row);
    bool insertShot(int row, int value, const QModelIndex &parent = QModelIndex());
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void markCompetition(int row, bool isCompetition);
    QPair<int, int> moveDown(QModelIndexList selection);
    QItemSelection moveUp(QModelIndexList selection);
    int size();

//signals:
//    void editCompleted(const QString &);

private:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    QVector<CompetitorShot> *m_shots = nullptr;
};

#endif // SHOTSTABLEMODEL_H
