#ifndef REALTIMEPLOT_H
#define REALTIMEPLOT_H

#include <QWidget>

#include "qcustomplot.h"

class realTimePlot : public QWidget
{
    Q_OBJECT
public:
    explicit realTimePlot(QWidget *parent = nullptr);
    QCustomPlot *m_customPlot;

private:
    void createLayout();
    void plotSettings();
    QTime time;
signals:

public:
    void realtimeDataSlot(double value);
};

#endif // REALTIMEPLOT_H
