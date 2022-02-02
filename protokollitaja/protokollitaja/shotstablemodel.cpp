#include "shotstablemodel.h"

ShotsTableModel::ShotsTableModel(QVector<CompetitorShot> *shots, QObject *parent) : QAbstractTableModel(parent)
{
    m_shots = shots;
}

int ShotsTableModel::columnCount(const QModelIndex &) const
{
    return 7;   // TODO Connect it somehow to number of Lask's fields
}

QVariant ShotsTableModel::data(const QModelIndex &index, int role) const
{
//    qDebug() << "ShotsTableModel::data: index.row() = " << index.row() << ", role: " << role;
    if (role == Qt::DisplayRole && index.row() < m_shots->size())
        // Competition/sighting/ignored shot, shot value, x, y, isInnerTen, time, origin
       switch (index.column()) {
//       case 0:
//           return m_shots->at(index.row()).isCompetitionShot();
//           return QVariant(QPixmap::fromImage(QImage(":/images/new.png")));
       case 1:
           return m_shots->at(index.row()).getSLask();
       case 2:
           return m_shots->at(index.row()).stringX();
       case 3:
           return m_shots->at(index.row()).stringY();
       case 4:
           return m_shots->at(index.row()).isInnerTen();
       case 5:
           return m_shots->at(index.row()).shotTime().toString();
       case 6:
           return m_shots->at(index.row()).shotOrigin();
       }
    else if (role == Qt::DecorationRole && index.row() < m_shots->size())
        if (index.column() == 0) {
 //           return m_shots->at(index.row()).isCompetitionShot();
            if (m_shots->at(index.row()).isIgnored())
                return QVariant(QIcon(QPixmap::fromImage(QImage("/home/lauri/Downloads/ikoonid/Võistlus lõppenud.svg"))));
            else if (m_shots->at(index.row()).isCompetitionShot())
                return QVariant(QIcon(QPixmap::fromImage(QImage("/home/lauri/Downloads/ikoonid/Võistlusleht.svg"))));
            else
                return QVariant(QIcon(QPixmap::fromImage(QImage("/home/lauri/Downloads/ikoonid/Proovileht 2.svg"))));
        }
    return QVariant();
}

bool ShotsTableModel::deleteShot(int row)
{
    if (m_shots->at(row).shotOrigin() == Lask::Manual && row < m_shots->size()) {
        beginRemoveRows(QModelIndex(), row, row);
        m_shots->remove(row);
        removeRows(row, 1);
        endRemoveRows();
        return true;
    } else {
        QMessageBox::critical(nullptr, tr("Viga!"), tr("Kustutada saab ainult käsitsi sisestatud laske. Teisi laske saab ainult ignoreerida."), QMessageBox::Ok);
        return false;
    }
}

Qt::ItemFlags ShotsTableModel::flags(const QModelIndex &index) const
{
    if (m_shots->size() > index.row() &&
            m_shots->at(index.row()).shotOrigin() == Lask::Manual && index.column() != 6)
        return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
    else
        return QAbstractTableModel::flags(index);
}

QVariant ShotsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("");
        case 1:
            return tr("Väärtus");
        case 2:
            return tr("X");
        case 3:
            return tr("Y");
        case 4:
            return tr("Sisekümme");
        case 5:
            return tr("Aeg");
        case 6:
            return tr("Päritolu");
        }
    } else if (role == Qt::DisplayRole && orientation == Qt::Vertical) {
        return QString("%1").arg(section + 1);
    }
    return QVariant();
}

bool ShotsTableModel::ignoreShot(int row)
{
    if (row < m_shots->size()) {
        m_shots[0][row].setIgnored(!m_shots->at(row).isIgnored());
        QModelIndex index = createIndex(row, 0);
        emit dataChanged(index, index, {Qt::DisplayRole});
        return true;
    }
    return false;
}

bool ShotsTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count);
    CompetitorShot newShot;
    newShot.setShotTime(QTime::currentTime());
    newShot.setCompetitionShot(true);
    m_shots->insert(row, newShot);
    endInsertRows();
    return true;
}

bool ShotsTableModel::insertShot(int row, int value, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row);
    CompetitorShot newShot;
    newShot.set10Lask(value);
    newShot.setShotTime(QTime::currentTime());
    newShot.setCompetitionShot(true);
    m_shots->insert(row, newShot);
    endInsertRows();
    return true;
}

void ShotsTableModel::markCompetition(int row, bool isCompetition)
{
    if (row < m_shots->size()) {
        m_shots[0][row].setCompetitionShot(isCompetition);
        QModelIndex index = createIndex(row, 0);
        emit dataChanged(index, index, {Qt::DisplayRole});
    }
}

QPair<int, int> ShotsTableModel::moveDown(QModelIndexList selection)    // TODO move selection also
{
    qDebug() << "ShotsTableModel::moveDown";
//    beginMoveRows(selection.first().parent(), selection.first().row(), selection.last().row(), selection.last().parent(), selection.last().row() + 1);
//    moveRows(selection.first().parent(), selection.first().row(), selection.size(), selection.last().parent(), selection.last().row() + 1);
    int minRow = m_shots->size();
    int maxRow = 0;
    foreach(QModelIndex index, selection) {
        if (index.row() < m_shots->size() - 1) {
            if (minRow > index.row())
                minRow = index.row();
            if (maxRow < index.row() + 1)
                maxRow = index.row() + 1;
            m_shots->move(index.row(), index.row() + 1);
        }
    }
//    endMoveRows();
    emit dataChanged(createIndex(minRow, columnCount()), createIndex(maxRow, columnCount()), {Qt::DisplayRole});
    return QPair<int, int>(minRow, maxRow);
}

QItemSelection ShotsTableModel::moveUp(QModelIndexList selection)
{
    qDebug() << "ShotsTableModel::moveUp";
    int minRow = m_shots->size();
    int maxRow = 0;
    foreach(QModelIndex index, selection) {
        if (index.row() > 0) {
            if (minRow > index.row() - 1)
                minRow = index.row() - 1;
            if (maxRow < index.row())
                maxRow = index.row();
            m_shots->move(index.row(), index.row() -1);
        }
    }
    QModelIndex top = createIndex(0, columnCount());
    QModelIndex bottom = createIndex(maxRow - 1, columnCount());
    emit dataChanged(top, bottom, {Qt::DisplayRole});
    return QItemSelection(top, bottom);
}

int ShotsTableModel::rowCount(const QModelIndex & /*parent*/) const
{
//    qDebug() << "ShotsTableModel::rowCount: m_shots.size() = " << m_shots->size();
    return m_shots->size();
}

bool ShotsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (role == Qt::EditRole && index.row() < m_shots->size()) {
        qDebug() << "ShotsTableModel::setData: index.row() = " << index.row() << ", role: " << role;
        if (!checkIndex(index) || value.isNull() || value.toString().isEmpty())
            return false;
        switch (index.column()) {
        case 0: {
            CompetitorShot modified = m_shots->at(index.row());
            modified.setCompetitionShot(value.toBool());
            m_shots[0][index.row()] = modified;
            return true;
        }
        case 1: {
            CompetitorShot modified = m_shots->at(index.row());
            modified.setLask(value.toString());
            m_shots[0][index.row()] = modified;
            return true;
        }
        case 2: {
            CompetitorShot modified = m_shots->at(index.row());
            modified.setMmX(value.toString());
            m_shots[0][index.row()] = modified;
            return true;
        }
        case 3: {
            CompetitorShot modified = m_shots->at(index.row());
            modified.setMmY(value.toString());
            m_shots[0][index.row()] = modified;
            return true;
        }
        case 4: {
            CompetitorShot modified = m_shots->at(index.row());
            modified.setInnerTen(value.toBool());
            m_shots[0][index.row()] = modified;
            return true;
        }
        case 5: {
            CompetitorShot modified = m_shots->at(index.row());
            modified.setShotTime(value.toTime());
            m_shots[0][index.row()] = modified;
            return true;
        }
        case 6: {
            bool success = false;
            CompetitorShot modified = m_shots->at(index.row());
            modified.setShotOrigin( static_cast<Lask::OriginType>(value.toInt(&success)));
            m_shots[0][index.row()] = modified;
            return success;
        }
        }
        return true;
    }
    return false;
}

int ShotsTableModel::size()
{
    return m_shots->size();
}
