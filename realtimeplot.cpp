#include "realtimeplot.h"

#include <QVBoxLayout>

realTimePlot::realTimePlot(QWidget *parent) :
    QWidget(parent),
    m_customPlot(new QCustomPlot(this)),
    m_time(QTime::currentTime())
{
    createLayout();
    plotSettings();
    m_time.start();
}
void realTimePlot::createLayout()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(m_customPlot);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
}

void realTimePlot::plotSettings()
{
    m_customPlot->setMinimumSize(500, 400);
    m_customPlot->setBackground(QBrush(QColor(236, 239, 241)));

    m_customPlot->addGraph(); // length
    m_customPlot->graph(0)->setPen(QPen(QPen(Qt::red)));

    m_customPlot->addGraph(); // yaw
    m_customPlot->graph(1)->setPen(QPen(QPen(Qt::cyan)));

    m_customPlot->addGraph(); // pitch
    m_customPlot->graph(2)->setPen(QPen(QPen(Qt::green)));

    m_customPlot->addGraph(); // roll
    m_customPlot->graph(3)->setPen(QPen(QPen(Qt::blue)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%m:%s");
    m_customPlot->xAxis->setTicker(timeTicker);
    m_customPlot->axisRect()->setupFullAxesBox();
    m_customPlot->yAxis->setRange(-30.0, 30.0);

    connect(m_customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(m_customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_customPlot->yAxis2, SLOT(setRange(QCPRange)));
}

void realTimePlot::realtimeDataSlot(double &value)
{
    double key = m_time.elapsed()/1000.0; //QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;

    if (key - m_lastPointKey > 0.02)
    {
        m_customPlot->graph(0)->addData(key, value);
        m_customPlot->graph(0)->rescaleValueAxis();

        m_lastPointKey = key;
    }

    m_customPlot->xAxis->setRange(key, 8, Qt::AlignRight);
    m_customPlot->replot();
}
