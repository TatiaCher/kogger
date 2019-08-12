#ifndef REALTIMEPLOT_H
#define REALTIMEPLOT_H

#include <QWidget>

#include "qcustomplot.h"

class realTimePlot : public QWidget
{
    Q_OBJECT
public:
    explicit realTimePlot(QWidget *parent = nullptr);

    void changeYaw();
    void changePitch();
    void changeRoll();

private:
    void createLayout();
    void plotSettings();

    double m_lastPointKey = 0;
    QCustomPlot *m_customPlot;
    QTime m_time;

signals:

public:
    void realtimeDataSlot(double &value);
};

#endif // REALTIMEPLOT_H
