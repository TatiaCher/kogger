#ifndef REALTIMEPLOT_H
#define REALTIMEPLOT_H

#include <QWidget>

//#include "Parse.h"
#include "qcustomplot.h"

class realTimePlot : public QWidget
{
    Q_OBJECT
public:
    explicit realTimePlot(QWidget *parent = nullptr);

private:
    void createLayout();
    void plotSettings();

    double m_lastPointKey = 0;
    QCustomPlot *m_customPlot;
    QTime m_time;

signals:

public:
    void realtimeDataSlot(const double &r);
};

#endif // REALTIMEPLOT_H
