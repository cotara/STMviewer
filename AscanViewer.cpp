#include "AscanViewer.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QListView>
#include <QPushButton>
#include <QVBoxLayout>

#include "ShotListModel.h"
#include "qcustomplot.h"
#include "unius/FileWriter.h"

namespace Ui {

AScanViewer::AScanViewer(std::vector<Unius::Shot> &shots, QWidget *parent)
    : QWidget(parent), m_shots(shots) {
  setupUi();
  m_list->setModel(new ShotListModel(shots, this));
  connect(m_list, &QListView::clicked, this, &AScanViewer::listClicked);
}

void AScanViewer::show(const Unius::Shot &s) {
  if (s.isEmpty()) return;
  QVector<double> x, y;
  for (unsigned i = 0; i < s.size(); i++) {
    x.append(i);
    y.append(s[i]);
  }
  m_plot->graph(0)->setData(x, y);
  if (autoRescale) m_plot->graph(0)->rescaleAxes();
  m_plot->replot();
}

int AScanViewer::channel() { return m_ch_selector->currentIndex(); }

void AScanViewer::dataUpdate() {
  reinterpret_cast<ShotListModel *>(m_list->model())->dataUpdate();
}

void AScanViewer::setupUi() {
  QHBoxLayout *root_l = new QHBoxLayout(this);
  QVBoxLayout *plot_l = new QVBoxLayout;
  QHBoxLayout *plot_title_l = new QHBoxLayout;

  m_plot = new QCustomPlot(this);
  m_plot->addGraph();
  m_plot->graph(0)->setPen(QPen(Qt::blue));
  m_plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);

  plot_title_l->addWidget(new QLabel("Изображение А-скана с канала:", this));
  m_ch_selector = new QComboBox(this);
  for (int i = 0; i < 16; i++) m_ch_selector->addItem(QString::number(i));
  plot_title_l->addWidget(m_ch_selector);
  plot_title_l->addStretch();
  plot_l->addLayout(plot_title_l);
  plot_l->addWidget(m_plot, 1);

  QHBoxLayout *buts_l = new QHBoxLayout;
  QPushButton *open_b =
      new QPushButton(QIcon(":/images/folder_open.png"), "Открыть...", this);
  connect(open_b, &QPushButton::clicked, this, &AScanViewer::openClicked);
  QPushButton *save_b =
      new QPushButton(QIcon(":/images/disk.png"), "Сохранить...", this);
  connect(save_b, &QPushButton::clicked, this, &AScanViewer::saveClicked);
  QPushButton *rescale_b =
      new QPushButton(QIcon(":/images/magnify.png"), "Перемаштабировать", this);
  connect(rescale_b, &QPushButton::clicked, this, &AScanViewer::rescaleClicked);
  QCheckBox *auto_rescale =
      new QCheckBox("Автоматическое масштабирование", this);
  connect(auto_rescale, &QCheckBox::clicked,
          [this](bool checked) { autoRescale = checked; });
  auto_rescale->setChecked(true);

  buts_l->addWidget(open_b);
  buts_l->addWidget(save_b);
  buts_l->addWidget(rescale_b);
  buts_l->addWidget(auto_rescale);
  buts_l->addStretch();

  plot_l->addLayout(buts_l);

  QVBoxLayout *list_l = new QVBoxLayout;
  m_list = new QListView(this);
  m_list->setMaximumWidth(150);
  list_l->addWidget(new QLabel("Список А-сканов", this));
  list_l->addWidget(m_list);
  QPushButton *clearList_b =
      new QPushButton(QIcon(":/images/refresh.png"), "Очистить", this);
  connect(clearList_b, &QPushButton::clicked, this,
          &AScanViewer::clearListClicked);
  QPushButton *saveList_b =
      new QPushButton(QIcon(":/images/disk.png"), "Сохранить...", this);
  connect(saveList_b, &QPushButton::clicked, this,
          &AScanViewer::saveListClicked);
  list_l->addWidget(clearList_b);
  list_l->addWidget(saveList_b);

  root_l->addLayout(plot_l);
  root_l->addLayout(list_l);
}

void AScanViewer::listClicked(const QModelIndex &index) {
  const Unius::Shot &s = m_shots[index.row()];
  show(s);
}

void AScanViewer::openClicked() {
  QString fileName = QFileDialog::getOpenFileName(this, "Open file", "");
  QFile textFile(fileName);
  if (textFile.open(QIODevice::ReadOnly)) {
    QTextStream textStream(&textFile);
    int index = 0;
    QVector<double> x, y;
    double d;
    while (!textStream.atEnd()) {
      textStream >> d;
      if (textStream.status() == QTextStream::Ok) {
        y.append(d);
        x.append(index++);
      } else
        break;
    }
    m_plot->graph(0)->setData(x, y);
    m_plot->graph(0)->rescaleAxes();
    m_plot->replot();
  }
  textFile.close();
}

void AScanViewer::saveClicked() {
  QString fileName = QFileDialog::getSaveFileName(this, "Save file", "Shot_1");

  QFile textFile(fileName);
  if (textFile.open(QIODevice::WriteOnly)) {
    auto data = m_plot->graph(0)->data();
    for (int i = 0; i < data->size(); ++i) {
      textFile.write(QByteArray::number(data->at(i)->value) + "\n");
    }
    textFile.close();
  }
}

void AScanViewer::rescaleClicked() {
  m_plot->graph(0)->rescaleAxes();
  m_plot->replot();
}

void AScanViewer::clearListClicked() {
  m_shots.clear();
  dataUpdate();
}

void AScanViewer::saveListClicked() {
  QString fileName = QFileDialog::getSaveFileName(
      this, "Save File", "untitled.usd", tr("USD File Format (*.usd)"));

  std::vector<OrientedPoint3D> coordinates;
  coordinates.resize(m_shots.size());
  std::vector<OrientedPoint3D> true_coordinates;
  true_coordinates.resize(m_shots.size());
  Unius::GridInfo grid;
  grid.shotAmount = static_cast<int>(m_shots.size());
  grid.shotLength = USHRT_MAX;
  grid.indexLength = 64;
  grid.scanLength = grid.shotAmount / 64;
  grid.AdcFreq = 200e6;
  Unius::FileWriter::writeUsd(coordinates, true_coordinates, m_shots, grid,
                              fileName);
}

}  // namespace Ui
