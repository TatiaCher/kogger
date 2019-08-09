#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "realtimeplot.h"
#include "Parse.h"

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QList>

class CentralWidget : public QWidget {
    Q_OBJECT

public:
    explicit CentralWidget(QWidget *parent = nullptr);

    void onConnect();

private:
    void CreateInterface();
    void CreateConnections();
    void CreateLayouts();
    void SetPorts();
    void SendMSG(uint8T id, uint8T mode);
    QByteArray GetArrayMSG(uint8T &id, uint8T &mode);

    QByteArray CreateArrChartAct();
    QByteArray CreateArrArrayAct();
    QByteArray CreateArrayYPRGett();
    QByteArray CreateArrayYPRSett();
    QByteArray CreateArrayAGCSett();
    QByteArray CreateArrayTRANSCSett();
    QByteArray CreateArraySPDSett();
    QByteArray CreateArrayUARTSett();
    QByteArray CreateArrayUARTGett();
    QByteArray CreateArrayFLASHAct();
    QByteArray CreateArrayFORMATSett();
    QByteArray CreateArrayREBOOTAct();
    QByteArray CreateArrayMARKSett();
    QByteArray CreateArrayMARKCont();

    QSerialPort *m_serialPort;
    realTimePlot *m_realTimePlot;
    Parse *m_parse;

    QPushButton *m_connectB;
    QPushButton *m_disconnectB;

    //QComboBox *m_baudRateCB;
    QComboBox *m_comPortCB;

signals:

private slots:
    void parseData();
    void onDisconnect();
};

#endif // MAINWIDGET_H
