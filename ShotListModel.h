#pragma once

#include <QAbstractListModel>
#include <vector>

#include "unius/Shot.h"

class ShotListModel : public QAbstractListModel {
 public:
  ShotListModel(const std::vector<Unius::Shot> &shots,
                QObject *parent = nullptr);
  void dataUpdate();

 private:
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  QVariant data(const QModelIndex &index,
                int role = Qt::DisplayRole) const override;
  const std::vector<Unius::Shot> &m_shots;
};
