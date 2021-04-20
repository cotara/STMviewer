#pragma once

#include <QWidget>

#include "unius/Shot.h"

class QCustomPlot;
class QListView;
class QComboBox;

namespace Ui {

class AScanViewer : public QWidget {
  Q_OBJECT
 public:
  explicit AScanViewer(std::vector<Unius::Shot> &shots,
                       QWidget *parent = nullptr);
  void show(const Unius::Shot &s);
  int channel();

 public slots:
  void dataUpdate();

 private:
  void setupUi();
  void listClicked(const QModelIndex &index);
  void openClicked();
  void saveClicked();
  void rescaleClicked();
  void clearListClicked();
  void saveListClicked();

  QCustomPlot *m_plot;
  QListView *m_list;
  QComboBox *m_ch_selector;
  bool autoRescale = true;
  std::vector<Unius::Shot> &m_shots;
};

}  // namespace Ui
