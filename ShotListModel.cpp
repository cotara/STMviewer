#include "ShotListModel.h"

ShotListModel::ShotListModel(const std::vector<Unius::Shot> &shots,
                             QObject *parent)
    : QAbstractListModel(parent), m_shots(shots) {}

void ShotListModel::dataUpdate() {
  emit dataChanged(createIndex(0, 0),
                   createIndex(static_cast<int>(m_shots.size()), 0));
}

int ShotListModel::rowCount(const QModelIndex &) const {
  return static_cast<int>(m_shots.size());
}

QVariant ShotListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) {
    return QVariant();
  }
  const Unius::Shot &s = m_shots.at(index.row());
  if (role == Qt::DisplayRole) {
    return QString::number(s.channel()) + " : " + QString::number(s.number()) +
           " (" + QString::number(s.size() / 1000) + "k)";
  }
  return QVariant();
}
