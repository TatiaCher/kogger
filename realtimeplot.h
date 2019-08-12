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

    double m_yaw = 0;
    double m_pitch = 0;
    double m_roll = 0;

    bool isYaw = false;
    bool isPitch = false;
    bool isRoll = false;

signals:

public:
    void realtimeDataSlot(double &value);
    void YPRDataSlot(double &m_yaw, double &m_pitch, double &m_roll);
};

#endif // REALTIMEPLOT_H
