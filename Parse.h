#ifndef PARSEARR_H
#define PARSEARR_H

#define Error 0
#define Confirm 1
#define Setting 2
#define Getting 3 //aka Request
#define Content 4 //aka Data
#define Action 5 //aka Reserved
#define Reaction 6 //aka Reserved

#define CMD_RESP 0x01
#define CMD_CHART 0x0A
#define CMD_ARRAY 0x0B
#define CMD_YPR 0x0C
#define CMD_QUAT 0x0D
#define CMD_TEMP 0x12
#define CMD_DIAG 0x13
#define CMD_AGC 0x14
#define CMD_TRANSC 0x15
#define CMD_SND_SPD 0x16
#define CMD_UART 0x17
#define CMD_FLASH 0x1D
#define CMD_FORMAT 0x1E
#define CMD_REBOOT 0x1F
#define CMD_MARK 0x20
#define CMD_VERSION 0x21

#include <QCoreApplication>
#include <QSerialPort>
#include <QObject>
#include <QBitArray>

class Parse : public QObject
{
    Q_OBJECT
public:
    explicit Parse(QObject *parent = nullptr);

    void ParseData(QByteArray &dataArray);
    int GetVersion(uint8T mode);
private:

    void CheckSumUpdate(uint8T byte);
    void GetPayload( uint8T id, uint8T mode);
    void ZeroOutData();

    QBitArray ParseBytesToBits(uint8T mode);

    uint8T ReadU1();
    uint16_t ReadU2();
    uint32_t ReadU4();
    int8_t ReadS1();
    int16_t ReadS2();

    void ParseResp();
    void ParseChartDataMode();
    void ParseChartRqstMode();
    void ParseYPR();
    void ParseYPRattach();
    void ParseTemp();
    void ParseArray();
    void ParseQUAT();
    void ParseDiag();
    void ParseAGC();
    void ParseTransc();
    void ParseSpeed();
    void ParseUART();
    void ParseMark();
    void ParseVersion();

    int payloadPos = 0;
    QByteArray payloadArray;

    uint8T length = 0;
    uint8T mode = 0;
    uint8T id = 0;

    uint8T CHECK1 = 0;
    uint8T CHECK2 = 0;

    int yprt = 0;

signals:
    void gotDistanceData(double&);
    void gotYPRdata(double&, double&, double&);
    void gotTempData(double&, double&);
};

#endif // PARSEARR_H
