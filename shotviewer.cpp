#include "shotviewer.h"
#include "qcustomplot/qcustomplot.h"

ShotViewer::ShotViewer(QWidget *parent) : QWidget(parent)
{
    setObjectName("shotviewer");
    //Графики
    customPlot1 = new QCustomPlot();
    customPlot2 = new QCustomPlot();

    layoutV = new QVBoxLayout;

    setLayout(layoutV);
    layoutV->addWidget(customPlot1);
    layoutV->addWidget(customPlot2);


    customPlot1->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    customPlot2->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    //Настройка CustomPlot
    customPlot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    customPlot1->axisRect()->setupFullAxesBox();
    customPlot2->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    customPlot2->axisRect()->setupFullAxesBox();
    customPlot1->plotLayout()->insertRow(0);
    QCPTextElement *title1 = new QCPTextElement(customPlot1, "Канал 1", QFont("sans", 14, QFont::Bold));
    customPlot1->plotLayout()->addElement(0, 0, title1);
    customPlot2->plotLayout()->insertRow(0);
    QCPTextElement *title2 = new QCPTextElement(customPlot2, "Канал 2", QFont("sans", 14, QFont::Bold));
    customPlot2->plotLayout()->addElement(0, 0, title2);
    //customPlot->xAxis->setLabel("X");
    //customPlot->yAxis->setLabel("Y");
    QFont legendFont = font();
    legendFont.setPointSize(10);

    customPlot1->xAxis->setRangeLower(0);
    customPlot1->xAxis->setRangeUpper(11000);
    customPlot1->yAxis->setRangeLower(-5);
    customPlot1->yAxis->setRangeUpper(260);
    customPlot1->legend->setVisible(true);
    customPlot1->legend->setFont(legendFont);
    customPlot1->legend->setSelectedFont(legendFont);
    customPlot1->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    customPlot2->xAxis->setRangeLower(0);
    customPlot2->xAxis->setRangeUpper(11000);
    customPlot2->yAxis->setRangeLower(-5);
    customPlot2->yAxis->setRangeUpper(260);
    customPlot2->legend->setVisible(true);
    customPlot2->legend->setFont(legendFont);
    customPlot2->legend->setSelectedFont(legendFont);
    customPlot2->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    tracer1 = new QCPItemTracer(customPlot1);
    tracer1->setStyle(QCPItemTracer::tsCircle);
    tracer1->setPen(QPen(Qt::black));
    tracer1->setBrush(Qt::black);
    tracer1->setSize(7);

    tracer2 = new QCPItemTracer(customPlot2);
    tracer2->setStyle(QCPItemTracer::tsCircle);
    tracer2->setPen(QPen(Qt::black));
    tracer2->setBrush(Qt::black);
    tracer2->setSize(7);


    // Выделение одной оси, ведет к выделению противоположной
    connect(customPlot1, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged1()));
    connect(customPlot2, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged2()));
    // зумируется только выделенная ось
    connect(customPlot1, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress1(QMouseEvent*)));
    connect(customPlot1, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel1()));
    connect(customPlot2, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress2(QMouseEvent*)));
    connect(customPlot2, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel2()));

    // При зумировании одной оси зизменяется диапазон противоположной
    connect(customPlot1->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot1->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot1->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot1->yAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot2->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot2->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot2->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot2->yAxis2, SLOT(setRange(QCPRange)));

    //Изменение заголовка
    connect(title1, SIGNAL(clicked(QMouseEvent*)), this, SLOT(titleDoubleClick1(QMouseEvent*)));
    connect(title2, SIGNAL(clicked(QMouseEvent*)), this, SLOT(titleDoubleClick2(QMouseEvent*)));

    //Отображение текущей точки в Тултипе
    connect(customPlot1, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked1(QCPAbstractPlottable*,int,QMouseEvent*)));
    connect(customPlot2, SIGNAL(plottableClick(QCPAbstractPlottable*,int,QMouseEvent*)), this, SLOT(graphClicked2(QCPAbstractPlottable*,int,QMouseEvent*)));

    //Переключение между 1 и 2 графиками
    connect(customPlot1,SIGNAL(mouseDoubleClick(QMouseEvent*)),this,SLOT(graphDoubleClicked1()));
    connect(customPlot2,SIGNAL(mouseDoubleClick(QMouseEvent*)),this,SLOT(graphDoubleClicked2()));

    //запоминание текущей позиции мыши для выведения радом с ней тултипа
    connect(customPlot1, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(showPointToolTip(QMouseEvent*)));
    connect(customPlot2, SIGNAL(mouseMove(QMouseEvent*)), this,SLOT(showPointToolTip(QMouseEvent*)));

    //Отображаемый диапазон изменился
    connect(customPlot1->xAxis, SIGNAL(rangeChanged(const QCPRange &)), this,SLOT(graphRangeChanged1()));
    connect(customPlot2->xAxis, SIGNAL(rangeChanged(const QCPRange &)), this,SLOT(graphRangeChanged2()));


    textLabel1 = new QCPItemText(customPlot1);
    textLabel2 = new QCPItemText(customPlot2);

}

void ShotViewer::showGraphs(int state){
    if(state & CH1)
        customPlot1->show();
    else
        customPlot1->hide();

    if(state & CH2)
        customPlot2->show();
    else
        customPlot2->hide();
}

void ShotViewer::clearGraphs(int state){
    if(state & CH1){
            customPlot1->clearGraphs();
            customPlot1->replot();

    }
    if(state & CH2){
            customPlot2->clearGraphs();
            customPlot2->replot();
    }
}

/**************************************************/
/*                  customPlot                    */
/**************************************************/
void ShotViewer::addUserGraph(QByteArray &buf, int len, int ch){
    QVector<double> x(len), y(len);
    QPen graphPen;
    QColor color;

    for (int i=0; i<len; i++){
      x[i] = i;
      y[i] = (unsigned char)buf.at(i);
    }

    if (ch==1 || ch==2){
        customPlot1->addGraph();
        customPlot1->graph()->setData(x, y);


        if (ch==1){
            customPlot1->graph()->setName(QString("Канал 1. Нефильтрованный"));
            color =  Qt::black;

        }
        else{
            customPlot1->graph()->setName(QString("Канал 1. Фильтрованный"));
            color =  Qt::darkGreen;
        }
        graphPen.setColor(color);
        customPlot1->graph()->setPen(graphPen);
        customPlot1->replot();
    }
    else if(ch==3 || ch==4){
        customPlot2->addGraph();
        customPlot2->graph()->setData(x, y);
        if (ch==3){
            customPlot2->graph()->setName(QString("Канал 2. Нефильтрованный"));
            color =  Qt::darkMagenta;
        }
        else{
            customPlot2->graph()->setName(QString("Канал 2. Фильтрованный"));
            color =  Qt::red;
        }
        graphPen.setColor(color);
        customPlot2->graph()->setPen(graphPen);
        customPlot2->replot();
    }
}
void ShotViewer::addDots(QVector<QVector<double> > dots, int ch){
    int dotsSize = dots.size();
    QVector<double> x(dotsSize), y(dotsSize);
    QColor color = Qt::red;
    QFont font;
    for(int i=0;i<dotsSize;i++){
        x[i] = dots.at(i).at(0);
        y[i] = dots.at(i).at(1);
    }
    if(!dots.isEmpty()){
        font.setPointSize(16);
        if(ch==1){
           customPlot1->addGraph();
           customPlot1->graph()->setData(x,y);
           customPlot1->graph()->setLineStyle((QCPGraph::LineStyle::lsNone));
           customPlot1->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,color, 8));
           customPlot1->graph()->setName(QString("Канал 1. Экстремумы"));
           customPlot1->replot();
        }
        else if(ch==2){
           customPlot2->addGraph();
           customPlot2->graph()->setData(x,y);
           customPlot2->graph()->setLineStyle((QCPGraph::LineStyle)0);
           customPlot2->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,color, 8));
           customPlot2->graph()->setName(QString("Канал 2. Экстремумы"));
           customPlot2->replot();
       }
    }
}


void ShotViewer::addDots(QVector<QVector<unsigned int> > dots, int ch){
    int dotsSize = dots.size();
    QVector<double> x(dotsSize), y(dotsSize);
    QColor color = Qt::red;
    QFont font;
    for(int i=0;i<dotsSize;i++){
        x[i] = dots.at(i).at(0);
        y[i] = dots.at(i).at(1);
    }
    if(!dots.isEmpty()){
        font.setPointSize(16);
        if(ch==1){
           customPlot1->addGraph();
           customPlot1->graph()->setData(x,y);
           customPlot1->graph()->setLineStyle((QCPGraph::LineStyle::lsNone));
           customPlot1->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,color, 8));
           customPlot1->graph()->setName(QString("Канал 1. Экстремумы"));
           customPlot1->replot();
        }
        else if(ch==2){
           customPlot2->addGraph();
           customPlot2->graph()->setData(x,y);
           customPlot2->graph()->setLineStyle((QCPGraph::LineStyle)0);
           customPlot2->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,color, 8));
           customPlot2->graph()->setName(QString("Канал 2. Экстремумы"));
           customPlot2->replot();
       }
    }
}



void ShotViewer::addLines(QVector<double> dots, int ch,int w){
    QFont font;
    QPen graphPen;
    QColor color(Qt::darkRed);
    QVector<double> x(2), y(2);

    font.setPointSize(16);
    graphPen.setColor(color);
    graphPen.setWidth(w);
    if(ch==1){
//           textLabel1->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
//           textLabel1->position->setType(QCPItemPosition::ptAxisRectRatio);
//           textLabel1->position->setCoords(0.5, 0.05); // place position at center/top of axis rect
//           textLabel1->setFont(font);
//           textLabel1->setText(QString::number(dots.at(1)-dots.at(0)));
        for(int i = 0; i<dots.size();i++){
            x[0]=dots.at(i);
            y[0]=0;
            x[1]=dots.at(i);
            y[1]=255;
           customPlot1->addGraph();
           customPlot1->graph()->setData(x,y);
           customPlot1->graph()->setLineStyle(QCPGraph::LineStyle::lsLine);
           customPlot1->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
           customPlot1->graph()->setName("Тень №" + QString::number(i));
           customPlot1->legend->removeItem(customPlot1->legend->itemCount()-1);
           customPlot1->graph()->setPen(graphPen);
        }
           customPlot1->replot();
      }
    else if(ch==2){
//           textLabel2->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
//           textLabel2->position->setType(QCPItemPosition::ptAxisRectRatio);
//           textLabel2->position->setCoords(0.5, 0.05); // place position at center/top of axis rect
//           textLabel2->setFont(font);
//           textLabel2->setText(QString::number(dots.at(1)-dots.at(0)));
        for(int i = 0; i<dots.size();i++){
            x[0]=dots.at(i);
            y[0]=0;
            x[1]=dots.at(i);
            y[1]=255;
           customPlot2->addGraph();
           customPlot2->graph()->setData(x,y);
           customPlot2->graph()->setLineStyle(QCPGraph::LineStyle::lsLine);
           customPlot2->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
           customPlot2->graph()->setName("Тень №" + QString::number(i));
           customPlot2->legend->removeItem(customPlot2->legend->itemCount()-1);
           customPlot2->graph()->setPen(graphPen);
        }
           customPlot2->replot();
      }

}
//горизонтальные линии
void ShotViewer::addLines2(QVector<double> dots, int ch, int w){
    QFont font;
    QPen graphPen;
    QColor color(Qt::darkRed);
    QVector<double> x(2), y(2);

    font.setPointSize(16);
    graphPen.setColor(color);
    graphPen.setWidth(w);
    if(ch==1){
        for(int i = 0; i<dots.size();i++){
            y[0]=dots.at(i);
            x[0]=0;
            y[1]=dots.at(i);

            x[1]=customPlot1->graph(0)->data()->at(customPlot1->graph(0)->data()->size()-1)->key+1;
           customPlot1->addGraph();
           customPlot1->graph()->setData(x,y);
           customPlot1->graph()->setLineStyle(QCPGraph::LineStyle::lsLine);
           customPlot1->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
           customPlot1->graph()->setName("Граница компарирования");
           customPlot1->legend->removeItem(customPlot1->legend->itemCount()-1);
           customPlot1->graph()->setPen(graphPen);
        }
           customPlot1->replot();
      }
    else if(ch==2){
        for(int i = 0; i<dots.size();i++){
            y[0]=dots.at(i);
            x[0]=0;
            y[1]=dots.at(i);
            x[1]=customPlot2->graph(0)->data()->at(customPlot2->graph(0)->data()->size()-1)->key+1;

           customPlot2->addGraph();
           customPlot2->graph()->setData(x,y);
           customPlot2->graph()->setLineStyle(QCPGraph::LineStyle::lsLine);
           customPlot2->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
           customPlot2->graph()->setName("Граница компарирования");
           customPlot2->legend->removeItem(customPlot2->legend->itemCount()-1);
           customPlot2->graph()->setPen(graphPen);
        }
           customPlot2->replot();
      }

}
void ShotViewer::titleDoubleClick1(QMouseEvent* event)
{
  Q_UNUSED(event)
  if (QCPTextElement *title = qobject_cast<QCPTextElement*>(sender()))
  {
    // Set the plot title by double clicking on it
    bool ok;
    QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
    if (ok)
    {
      title->setText(newTitle);
      //customPlot1->replot();
    }
  }
}
void ShotViewer::titleDoubleClick2(QMouseEvent* event)
{
  Q_UNUSED(event)
  if (QCPTextElement *title = qobject_cast<QCPTextElement*>(sender()))
  {
    // Set the plot title by double clicking on it
    bool ok;
    QString newTitle = QInputDialog::getText(this, "QCustomPlot example", "New plot title:", QLineEdit::Normal, title->text(), &ok);
    if (ok)
    {
      title->setText(newTitle);
      //customPlot1->replot();
    }
  }
}
void ShotViewer::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item){
  // Rename a graph by double clicking on its legend item
  Q_UNUSED(legend)
  if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
  {
    QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
    bool ok;
    QString newName = QInputDialog::getText(this, "QCustomPlot example", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
    if (ok)
    {
      plItem->plottable()->setName(newName);
      //customPlot->replot();
    }
  }
}
void ShotViewer::selectionChanged1(){
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customPlot1->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot1->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot1->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot1->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customPlot1->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot1->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot1->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot1->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<customPlot1->graphCount(); ++i){
    QCPGraph *graph = customPlot1->graph(i);
    QCPPlottableLegendItem *item = customPlot1->legend->itemWithPlottable(graph);
    if (item!=0){
    if (item->selected() || graph->selected())    {
      item->setSelected(true);
      graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
    }
    }
  }
}
void ShotViewer::selectionChanged2(){
  // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot2->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot2->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customPlot2->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot2->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot2->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot2->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
  if (customPlot2->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || customPlot2->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
      customPlot2->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || customPlot2->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
  {
    customPlot2->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    customPlot2->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
  }
  // synchronize selection of graphs with selection of corresponding legend items:
  for (int i=0; i<customPlot2->graphCount(); ++i){
    QCPGraph *graph = customPlot2->graph(i);
    QCPPlottableLegendItem *item = customPlot2->legend->itemWithPlottable(graph);
    if(item!=0)  {
        if (item->selected() || graph->selected())    {
          item->setSelected(true);
          graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
        }
    }
  }
}

void ShotViewer::mousePress1(QMouseEvent* event){
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged
   double coordX = customPlot1->xAxis->pixelToCoord(event->pos().x());

  if (customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    customPlot1->axisRect()->setRangeDrag(customPlot1->xAxis->orientation());
    if(customPlot1->xAxis->range().lower <0)
         customPlot1->xAxis->setRangeLower(0);
    if(customPlot1->xAxis->range().upper > 11000)
         customPlot1->xAxis->setRangeUpper(11000);
  }
  else if (customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
   customPlot1->axisRect()->setRangeDrag(customPlot1->yAxis->orientation());
   if(customPlot1->yAxis->range().lower <-5)
        customPlot1->yAxis->setRangeLower(-5);
   if(customPlot1->yAxis->range().upper >260)
        customPlot1->yAxis->setRangeUpper(260);
  }
  else{
    customPlot1->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    if(customPlot1->xAxis->range().lower <0)
         customPlot1->xAxis->setRangeLower(0);
    if(customPlot1->xAxis->range().upper > 11000)
         customPlot1->xAxis->setRangeUpper(11000);
    if(customPlot1->yAxis->range().lower <-5)
         customPlot1->yAxis->setRangeLower(-5);
    if(customPlot1->yAxis->range().upper >260)
         customPlot1->yAxis->setRangeUpper(260);
  }

}
void ShotViewer::mousePress2(QMouseEvent* event){
  // if an axis is selected, only allow the direction of that axis to be dragged
  // if no axis is selected, both directions may be dragged

  if (customPlot2->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    customPlot2->axisRect()->setRangeDrag(customPlot2->xAxis->orientation());
    if(customPlot2->xAxis->range().lower <0)
         customPlot2->xAxis->setRangeLower(0);
    if(customPlot2->xAxis->range().upper > 11000)
         customPlot2->xAxis->setRangeUpper(11000);
  }
  else if (customPlot2->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
   customPlot2->axisRect()->setRangeDrag(customPlot2->yAxis->orientation());
   if(customPlot2->yAxis->range().lower <-5)
        customPlot2->yAxis->setRangeLower(-5);
   if(customPlot2->yAxis->range().upper >260)
        customPlot2->yAxis->setRangeUpper(260);
  }
  else{
    customPlot2->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
    if(customPlot2->xAxis->range().lower <0)
         customPlot2->xAxis->setRangeLower(0);
    if(customPlot2->xAxis->range().upper > 11000)
         customPlot2->xAxis->setRangeUpper(11000);
    if(customPlot2->yAxis->range().lower <-5)
         customPlot2->yAxis->setRangeLower(-5);
    if(customPlot2->yAxis->range().upper >260)
         customPlot2->yAxis->setRangeUpper(260);
  }
}
void ShotViewer::mouseWheel1(){
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (customPlot1->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
      customPlot1->axisRect()->setRangeZoom(customPlot1->xAxis->orientation());
      if(customPlot1->xAxis->range().lower <0)
           customPlot1->xAxis->setRangeLower(0);
      if(customPlot1->xAxis->range().upper > 11000)
           customPlot1->xAxis->setRangeUpper(11000);
   }

  else if (customPlot1->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    customPlot1->axisRect()->setRangeZoom(customPlot1->yAxis->orientation());
    if(customPlot1->yAxis->range().lower <-1)
         customPlot1->yAxis->setRangeLower(-1);
    if(customPlot1->yAxis->range().upper >260)
         customPlot1->yAxis->setRangeUpper(260);
  }
  else{
    customPlot1->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    if(customPlot1->xAxis->range().lower <0)
         customPlot1->xAxis->setRangeLower(0);
    if(customPlot1->xAxis->range().upper > 11000)
         customPlot1->xAxis->setRangeUpper(11000);
    if(customPlot1->yAxis->range().lower <-1)
         customPlot1->yAxis->setRangeLower(-1);
    if(customPlot1->yAxis->range().upper >260)
         customPlot1->yAxis->setRangeUpper(260);
   }

}
void ShotViewer::mouseWheel2(){
  // if an axis is selected, only allow the direction of that axis to be zoomed
  // if no axis is selected, both directions may be zoomed

  if (customPlot2->xAxis->selectedParts().testFlag(QCPAxis::spAxis)){
      customPlot2->axisRect()->setRangeZoom(customPlot2->xAxis->orientation());
      if(customPlot2->xAxis->range().lower <0)
           customPlot2->xAxis->setRangeLower(0);
      if(customPlot2->xAxis->range().upper > 11000)
           customPlot2->xAxis->setRangeUpper(11000);
   }

  else if (customPlot2->yAxis->selectedParts().testFlag(QCPAxis::spAxis)){
    customPlot2->axisRect()->setRangeZoom(customPlot2->yAxis->orientation());
    if(customPlot2->yAxis->range().lower <-1)
         customPlot2->yAxis->setRangeLower(-1);
    if(customPlot2->yAxis->range().upper >260)
         customPlot2->yAxis->setRangeUpper(260);
  }
  else{
    customPlot2->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
    if(customPlot2->xAxis->range().lower <0)
         customPlot2->xAxis->setRangeLower(0);
    if(customPlot2->xAxis->range().upper > 11000)
         customPlot2->xAxis->setRangeUpper(11000);
    if(customPlot2->yAxis->range().lower <-1)
         customPlot2->yAxis->setRangeLower(-1);
    if(customPlot2->yAxis->range().upper >260)
         customPlot2->yAxis->setRangeUpper(260);
  }
}

void ShotViewer::graphDoubleClicked1(){
    if(!customPlot2->isHidden())
        showGraphs(CH1);
    else
        showGraphs(CH1|CH2);
}
void ShotViewer::graphDoubleClicked2(){
    if(!customPlot1->isHidden())
        showGraphs(CH2);
    else
        showGraphs(CH1 | CH2);

}

void ShotViewer::graphClicked1(QCPAbstractPlottable *plottable, int dataIndex,QMouseEvent* event){
  // since we know we only have QCPGraphs in the plot, we can immediately access interface1D()
  // usually it's better to first check whether interface1D() returns non-zero, and only then use it.
  /*double dataValue = plottable->interface1D()->dataMainValue(dataIndex);
  QString message = QString("Clicked on graph '%1' at data point #%2 with value %3.").arg(plottable->name()).arg(dataIndex).arg(dataValue);
  //ui->statusBar->showMessage(message, 2500);
  mTag1->updatePosition(dataValue);
  mTag1->setText(QString::number(dataValue));*/

        tracer1->setGraph(customPlot1->selectedGraphs().first());

        //int y = plottable->interface1D()->dataMainValue(dataIndex);
        //QToolTip::showText(currentMousePosition,QString("%1 , %2").arg(dataIndex).arg(y));
        int coordX = customPlot1->xAxis->pixelToCoord(event->pos().x());
        tracer1->setGraphKey(coordX);
        tracer1->updatePosition();
        //int coordY = customPlot1->yAxis->pixelToCoord(event->pos().y());
        QToolTip::showText(currentMousePosition,QString::number(tracer1->position->key()) + "," + QString::number(tracer1->position->value()));
        //QToolTip::showText(currentMousePosition,QString::number(coordX) + "," + QString::number(coordY));

        //QString name = plottable->name();
        //QSharedPointer<QCPGraphDataContainer> dataMap = customPlot1->selectedGraphs().first()->data();
        QCPGraphDataContainer dataMap2 = *(customPlot1->selectedGraphs().first()->data());
        //const QCPRange range = customPlot1->xAxis->range();

        //QCPDataContainer <QCPGraphData> *graphData = dataMap.data();


        //QCPGraph *mGraph = customPlot1->selectedGraphs().first();
        //QCPGraphDataContainer::const_iterator begin = dataMap->at(customPlot1->xAxis->range().lower);
        //QCPGraphDataContainer::const_iterator end = dataMap->at(customPlot1->xAxis->range().upper);
        dataMap2.removeBefore(customPlot1->xAxis->range().lower);
        dataMap2.removeAfter(customPlot1->xAxis->range().upper);
        //QCPGraphDataContainer::const_iterator begin = dataMap->begin();
        //QCPGraphDataContainer::const_iterator end = dataMap->end();
        //for (QCPGraphDataContainer::const_iterator it=begin; it!=end; ++it)
        //{
          // iterator "it" will go through all selected data points, as an example, we calculate the value average
           //val =  it->value;
        //}

        emit graph_selected(dataMap2);
//Использование трейсера
//        QCPItemTracer *tracer = new QCPItemTracer;
//        tracer->setGraph(graph);

//        double getValueByKey(double key)
//        {
//            tracer->setGraphKey(key);
//            return tracer->position->value();
//        }
}
void ShotViewer::graphClicked2(QCPAbstractPlottable *plottable, int dataIndex,QMouseEvent* event){
        //int y = plottable->interface1D()->dataMainValue(dataIndex);
        //QToolTip::showText(currentMousePosition,QString("%1 , %2").arg(dataIndex).arg(y));
        tracer2->setGraph(customPlot2->selectedGraphs().first());

        int coordX = customPlot2->xAxis->pixelToCoord(event->pos().x());
        tracer2->setGraphKey(coordX);
        QToolTip::showText(currentMousePosition,QString::number(tracer2->position->key()) + "," + QString::number(tracer2->position->value()));

        //int coordY = customPlot2->yAxis->pixelToCoord(event->pos().y());
        //QToolTip::showText(currentMousePosition,QString::number(coordX) + "," + QString::number(coordY));

        QCPGraphDataContainer dataMap2 = *(customPlot2->selectedGraphs().first()->data());
        dataMap2.removeBefore(customPlot2->xAxis->range().lower);
        dataMap2.removeAfter(customPlot2->xAxis->range().upper);
        emit graph_selected(dataMap2);
}
void ShotViewer::autoScale(){
        customPlot1->rescaleAxes();
        customPlot1->yAxis->setRange(customPlot1->yAxis->range().lower-1,customPlot1->yAxis->range().upper+5);
        customPlot1->xAxis->setRange(customPlot1->xAxis->range().lower-20,customPlot1->xAxis->range().upper+100 );
        customPlot1->replot();
        customPlot2->rescaleAxes();
        customPlot2->yAxis->setRange(customPlot2->yAxis->range().lower-1,customPlot2->yAxis->range().upper+5);
        customPlot2->xAxis->setRange(customPlot2->xAxis->range().lower-20,customPlot2->xAxis->range().upper+100);
        customPlot2->replot();
}

void ShotViewer::showPointToolTip(QMouseEvent *event){
    currentMousePosition = event->globalPos();
}

void ShotViewer::graphRangeChanged1()
{
    if(!customPlot1->selectedGraphs().isEmpty()){
         QCPGraphDataContainer dataMap2 = *(customPlot1->selectedGraphs().first()->data());
         dataMap2.removeBefore(customPlot1->xAxis->range().lower);
         dataMap2.removeAfter(customPlot1->xAxis->range().upper);
         emit graph_selected(dataMap2);
    }
}

void ShotViewer::graphRangeChanged2()
{
    if(!customPlot2->selectedGraphs().isEmpty()){
         QCPGraphDataContainer dataMap2 = *(customPlot2->selectedGraphs().first()->data());
         dataMap2.removeBefore(customPlot2->xAxis->range().lower);
         dataMap2.removeAfter(customPlot2->xAxis->range().upper);
         emit graph_selected(dataMap2);
    }
}

