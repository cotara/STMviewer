#include "diameterviewer.h"

#include <complex>
#include <fftw3.h>

DiameterViewer::DiameterViewer(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *diameterPlotLayout = new QVBoxLayout(this);
    diameterPlot = new QCustomPlot(this);
    spectrePlot = new QCustomPlot(this);
    diameterPlotLayout->addWidget(diameterPlot);
    diameterPlotLayout->addWidget(spectrePlot);

    diameterPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    diameterPlot->axisRect()->setupFullAxesBox();
    spectrePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    spectrePlot->axisRect()->setupFullAxesBox();

    QFont legendFont = font();
    legendFont.setPointSize(10);
    diameterPlot->xAxis->setRangeLower(0);
    diameterPlot->xAxis->setRangeUpper(5000);
    diameterPlot->yAxis->setRangeLower(-2);
    diameterPlot->yAxis->setRangeUpper(2);
    diameterPlot->legend->setVisible(true);
    diameterPlot->legend->setFont(legendFont);
    diameterPlot->legend->setSelectedFont(legendFont);
    diameterPlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    spectrePlot->legend->setVisible(true);
    spectrePlot->legend->setFont(legendFont);
    spectrePlot->legend->setSelectedFont(legendFont);
    spectrePlot->legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

    connect(diameterPlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel1()));
    connect(spectrePlot, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel2()));

    QPen m_pen;
    m_pen.setWidth(2);
    r1 = diameterPlot->addGraph();
    r2 = diameterPlot->addGraph();
    c1 = diameterPlot->addGraph();
    c2 = diameterPlot->addGraph();
    m1 = diameterPlot->addGraph();
    m2 = diameterPlot->addGraph();
    f1 = diameterPlot->addGraph();
    f2 = diameterPlot->addGraph();
    spec1 = spectrePlot->addGraph();
    spec2 = spectrePlot->addGraph();
    r1->setName("Диаметр по оси Х");
    r2->setName("Диаметр по оси Y");
    c1->setName("Отклонение от центра по оси Х");
    c2->setName("Отклонение от центра по оси Y");
    m1->setName("Фильтрованный диаметр по оси X");
    m2->setName("Фильтрованный диаметр по оси Y");
    f1->setName("Фурье диаметр по оси X");
    f2->setName("Фурье диаметр по оси Y");
    spec1->setName("Спектр диаметра Х");
    spec2->setName("Спектр диаметра Y");
    m_pen.setColor("#ff5c5c");
    r1->setPen(m_pen);
    m_pen.setColor("#ad0000");
    r2->setPen(m_pen);

    m_pen.setColor("#fffd61");
    c1->setPen(m_pen);
    m_pen.setColor("#c7c500");
    c2->setPen(m_pen);

    m_pen.setColor("#6bff61");
    m1->setPen(m_pen);
    m_pen.setColor("#0bb300");
    m2->setPen(m_pen);

    m_pen.setColor("#66aeff");
    f1->setPen(m_pen);
    m_pen.setColor("#0054b3");
    f2->setPen(m_pen);

    m_pen.setColor("#bf80ff");
    spec1->setPen(m_pen);
    m_pen.setColor("#7c00fa");
    spec2->setPen(m_pen);
}

void DiameterViewer::addDataToGraph(const QVector<double>& d1, const QVector<double>& d2,const QVector<double>& c1, const QVector<double>& c2,const QVector<double>& m1, const QVector<double>& m2){
    int size = d1.size();//Размер данных, которые надо добавить на график
    for(int i = 0;i<size; i++){
        xDiameter.append(lastIndex++);
    }
    yr1.append(d1);
    yr2.append(d2);
    yc1.append(c1);
    yc2.append(c2);
    ym1.append(m1);
    ym2.append(m2);

    //Фурье
    xFurie.clear();
    ySpectr1.clear();
    ySpectr2.clear();
    yFurieFiltered1.clear();
    yFurieFiltered2.clear();
    yf1.clear();
    yf2.clear();
    furie(&yc1,&ySpectr1,&yFurieFiltered1,m_furieLimit);
    furie(&yc2,&ySpectr2,&yFurieFiltered2,m_furieLimit);

    double freqX = 0;
    double delta = 926.0/yr1.size(); //926 - частота генерации диаметров

    for (int i=0;i <ySpectr1.size();i++){
        freqX+=delta;
        xFurie.append(freqX);
    }

    yf1.append(yFurieFiltered1);
    yf2.append(yFurieFiltered2);



    filled+=size;
    if(!m_diamPlotMode)
        realTimeDiameter();
    else
        collectDiameter();
    plotDiameter();
}

void DiameterViewer::clearGraphs(){
    lastIndex=0;
    clearDiameterVectors();
}

void DiameterViewer::autoScale()
{
    diameterPlot->yAxis->rescale();
    diameterPlot->yAxis->setRangeLower(diameterPlot->yAxis->range().lower-10);
    diameterPlot->yAxis->setRangeUpper(diameterPlot->yAxis->range().upper+10);
    diameterPlot->replot();

    spectrePlot->yAxis->rescale();
    spectrePlot->xAxis->rescale();
    spectrePlot->yAxis->setRangeLower(spectrePlot->yAxis->range().lower-5);
    spectrePlot->yAxis->setRangeUpper(spectrePlot->yAxis->range().upper+10);
    spectrePlot->replot();
}

void DiameterViewer::plotDiameter()
{
    r1->setData(xDiameter,yr1);
    r2->setData(xDiameter,yr2);
    c1->setData(xDiameter,yc1);
    c2->setData(xDiameter,yc2);
    m1->setData(xDiameter,ym1);
    m2->setData(xDiameter,ym2);
    f1->setData(xDiameter,yf1);
    f2->setData(xDiameter,yf2);
    spec1->setData(xFurie,ySpectr1);
    spec2->setData(xFurie,ySpectr2);

    if(m_diamEnPlot){
        r1->setVisible(true);
        r2->setVisible(true);
    }
    else{
        r1->setVisible(false);
        r2->setVisible(false);
    }

    if(m_offsetsEnPlot){
        c1->setVisible(true);
        c2->setVisible(true);
    }
    else{
        c1->setVisible(false);
        c2->setVisible(false);
    }
    if(m_filteredEnPlot){
        m1->setVisible(true);
        m2->setVisible(true);
    }
    else{
        m1->setVisible(false);
        m2->setVisible(false);
    }

    if(m_furieEnPlot){
        f1->setVisible(true);
        f2->setVisible(true);
        spec1->setVisible(true);
        spec2->setVisible(true);
    }
    else{
        f1->setVisible(false);
        f2->setVisible(false);
        spec1->setVisible(false);
        spec2->setVisible(false);
    }

    diameterPlot->xAxis->rescale();
    diameterPlot->yAxis->rescale(true);
    if(!m_diamPlotMode){
        diameterPlot->xAxis->setRange(diameterPlot->xAxis->range().upper, m_window, Qt::AlignRight);
    }
    diameterPlot->replot();

    spectrePlot->replot();
}

void DiameterViewer::realTimeDiameter(){
    int overload = filled-m_window;         //Если данных больше, чем окно, отрезаем хвост
    if(overload>=0){
        if(yr1.size()>overload){            //Если то, сколько надо отрезать с конца больше размера вектора, режем
            xDiameter.remove(0,overload);
            yr1.remove(0,overload);
            yr2.remove(0,overload);
            yc1.remove(0,overload);
            yc2.remove(0,overload);
            ym1.remove(0,overload);
            ym2.remove(0,overload);
            yf1.remove(0,overload);
            yf2.remove(0,overload);
            filled-=overload;
        }
        else{//Иначе, режем все
            clearDiameterVectors();
            filled=0;
        }
    }
}

void DiameterViewer::collectDiameter(){
    //m_ManagementWidget->m_DiameterTransmition->collectCountLabel->setNum(filled);   //Выводим, сколько точек уже приянто
}

void DiameterViewer::furie(QVector<double> *in, QVector<double> *spectr, QVector<double> *out, double  cutOfFreq){

    int size = in->size();
    double decr;//на сколько Дб надо опустить
    double re,im;
    QVector<std::complex<double> > dataIn, dataSpectr(size,0),dataOut(size,0);

    for (int i =0;i<size;i++)
        dataIn.append(in->at(i));

    // создаем план прямого преобазования фурье
    fftw_plan plan=fftw_plan_dft_1d(size, (fftw_complex*) &dataIn[0], (fftw_complex*) &dataSpectr[0], FFTW_FORWARD, FFTW_ESTIMATE);
    fftw_execute(plan);
    fftw_destroy_plan(plan);

    //Преобразуем выход фурья в децибелы
    //Фильтрация
    for (int i=0;i<size/2+1;i++){
        spectr->append(10*std::log10(sqrt(dataSpectr.at(i).real()*dataSpectr.at(i).real() + dataSpectr.at(i).imag()*dataSpectr.at(i).imag())));
        if(spectr->at(i)>cutOfFreq && i>0){
            decr = (spectr->at(i)-cutOfFreq)/10;
            re = dataSpectr.at(i).real()/(pow(10,decr));
            im = dataSpectr.at(i).real()/(pow(10,decr));
            dataSpectr[i].real(re);
            dataSpectr[i].imag(im);
            dataSpectr[size-i-1].real(re);
            dataSpectr[size-i-1].imag(im);
            (*spectr)[i] = 10*std::log10(sqrt(dataSpectr.at(i).real()*dataSpectr.at(i).real() + dataSpectr.at(i).imag()*dataSpectr.at(i).imag()));
        }
        //y2Temp_.append(10*std::log10(sqrt(dataOut.at(i).real()*dataOut.at(i).real() + dataOut.at(i).imag()*dataOut.at(i).imag())));
    }

    // создаем план обратного преобазования фурье
    fftw_plan plan2=fftw_plan_dft_1d(dataOut.size(), (fftw_complex*) &dataSpectr[0], (fftw_complex*) &dataOut[0], FFTW_BACKWARD , FFTW_ESTIMATE);
    fftw_execute(plan2);
    fftw_destroy_plan(plan2);

    for (int i=0;i<size;i++)
        out->append(dataOut.at(i).real()/size);
}

void DiameterViewer::clearDiameterVectors(){
    yr1.clear();
    yr2.clear();
    yc1.clear();
    yc2.clear();
    ym1.clear();
    ym2.clear();
    yf1.clear();
    yf2.clear();
    ySpectr1.clear();
    ySpectr2.clear();
    xDiameter.clear();
    xFurie.clear();
}


void DiameterViewer::mouseWheel1(){
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (diameterPlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        diameterPlot->axisRect()->setRangeZoom(diameterPlot->xAxis->orientation());

    else if (diameterPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        diameterPlot->axisRect()->setRangeZoom(diameterPlot->yAxis->orientation());

    else
        diameterPlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
void DiameterViewer::mouseWheel2(){

    if (spectrePlot->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        spectrePlot->axisRect()->setRangeZoom(spectrePlot->xAxis->orientation());

    else if (spectrePlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        spectrePlot->axisRect()->setRangeZoom(spectrePlot->yAxis->orientation());

    else
        spectrePlot->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}
