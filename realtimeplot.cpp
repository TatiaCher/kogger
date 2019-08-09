#include "realtimeplot.h"

#include <QVBoxLayout>


realTimePlot::realTimePlot(QWidget *parent) :
    QWidget(parent),
    m_customPlot(new QCustomPlot(this))
{
    plotSettings();

    time.currentTime();
    time.start();
}

void realTimePlot::plotSettings()
{
    QColor color(2, 119, 189);

    m_customPlot->addGraph();
    m_customPlot->setBackground(QBrush(QColor(236, 239, 241)));
    m_customPlot->graph()->setLineStyle(QCPGraph::lsLine);
    m_customPlot->graph()->setPen(QPen(color.lighter(200)));
    m_customPlot->graph()->setBrush(QBrush(color));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    m_customPlot->xAxis->setTicker(timeTicker);
    m_customPlot->axisRect()->setupFullAxesBox();
    m_customPlot->yAxis->setRange(0, 30.0);

    connect(m_customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

void realTimePlot::realtimeDataSlot(double value)
{
    double key =  time.elapsed()/1000.0; // QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    m_customPlot->graph(0)->addData(key, value);
    //m_customPlot->graph(0)->rescaleValueAxis();

    m_customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    m_customPlot->replot();
}

