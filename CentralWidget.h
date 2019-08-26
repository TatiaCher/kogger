#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "realtimeplot.h"
#include "KoogerParser.h"

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>


class CentralWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CentralWidget(QWidget *parent = nullptr);

private:
    void CreateInterface();
    void CreateConnections();
    void CreateLayouts();
    void SetPorts();

    void setChartData(const KoogerParser::ChartDataMode &r);
    void sendWithoutPayloudData(uint8T id, uint8T mode);
    void setYPR(const KoogerParser::YPR &y);
    void setTemp(const KoogerParser::Temp &t);

    void startTimer();

    void sendData(std::vector<uint8T> &r);
    void sendActionChartData();
    void sendActionArray();
    void sendYPRData();
    void sendAGCData();
    void sendTranscData();
    void sendSpeedData();

    //QTimer *m_yPRTimer;

    QSerialPort *m_serialPort;
    realTimePlot *m_realTimePlot;
    KoogerParser *m_parse;

    QPushButton *m_connectB;
    QPushButton *m_disconnectB;

    QLabel *m_textLable;

    QLabel *m_lbYaw;
    QLineEdit *m_teYaw;
    QLabel *m_lbPitch;
    QLineEdit *m_tePitch;
    QLabel *m_lbRoll;
    QLineEdit *m_teRoll;
    QLabel *m_lbTemp;
    QLineEdit *m_teTemp;

    QToolBox *m_toolBox;

    QWidget *charWidget;
    void createDistWidget();
    QComboBox *m_comPortCB;
    QSpinBox * m_sbStrtPosCh; //mm
    QLabel *m_lbStr;
    QSpinBox * m_sbItemCount;
    QLabel *m_lbCount;
    QSpinBox * m_sbItemResol; //mm
    QLabel *m_lbResol;
    QSpinBox * m_sbPeriod; //ms
    QLabel *m_lbPeriod;
    QPushButton *m_pbUpdateSettChar;

    QWidget *arrayWidget;
    void createArrayWidget();
    QSpinBox * m_sbStrtPosArr; //mm
    QLabel *m_lbStrArr;
    QSpinBox * m_sbEndPosArr; //mm
    QLabel *m_lbEndPosArr;
    QSpinBox * m_sbMaxCountArr;
    QLabel *m_lbMaxCountArr;
    QSpinBox * m_sbThrRiseArr;
    QLabel *m_lbThrRiseArr;
    QSpinBox * m_sbThrFallArr;
    QLabel *m_lbThrFallArr;
    QSpinBox * m_sbOverlapArr; //mm
    QLabel *m_lbOverlapArr;
    QSpinBox * m_sbMinWidthArr; //mm
    QLabel *m_lbMinWidthArr;
    QSpinBox * m_sbMinAmplArr;
    QLabel *m_lbMinAmplArr;
    QPushButton *m_pbSetArr;

    QWidget *YPRWidget;
    void createYPRWidget();
    QLabel *m_lbAttachYPR;
    QSpinBox * m_sbAttachYPR;
    QPushButton *m_pbSetYPR;

    QWidget *AGCWidget;
    void createAGCWidget();
    QSpinBox * m_sbStrPosAGC; //mm
    QLabel *m_lbStrPosAGC;
    QSpinBox * m_sbOffsetAGC; //dB/100
    QLabel *m_lbOffsetAGC;
    QSpinBox * m_sbSlopeAGC; //Exp/100
    QLabel *m_lbSlopeAGC;
    QSpinBox * m_sbAbsorpAGC; //dB/km
    QLabel *m_lbAbsorpAGC;
    QPushButton *m_pbSetAGC;

    QWidget *TtranscWidget;
    void createTranscWidget();
    QSpinBox * m_sbFreqtransc; //kHz
    QLabel *m_lbFreqTransc;
    QSpinBox * m_sbPulseTransc;
    QLabel *m_lbPulseTransc;
    QSpinBox * m_sbBoostTransc;
    QLabel *m_lbBoostTransc;
    QPushButton *m_pbSetTransc;

    QWidget *SPDWidget;
    void createSPDWidget();
    QSpinBox * m_sbSndSpd; //m/s
    QLabel *m_lbsndSpd;
    QPushButton *m_pbSetSndSpd;

    //UART
    //FLASH
    //FORMAT
    //REBOOT
    //MARK
    //VERSION

private slots:
    void onDisconnect();
    void onConnect();
    void parseData();

};

#endif // MAINWIDGET_H
