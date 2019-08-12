#ifndef SENDDATA_H
#define SENDDATA_H

#include <QByteArray>

class SendData
{
public:
    SendData();

    QByteArray CreateArrayMSG(uint8T &id, uint8T &mode, QByteArray &payload);
    /*QByteArray GetNoPayloadArray(uint8T &id, uint8T &mode);
    QByteArray GetChartActArray(uint8T &id, uint8T &mode);
    QByteArray GetArrayActArray(uint8T &id, uint8T &mode);
    QByteArray GetYPRGettArray(uint8T &id, uint8T &mode);
    QByteArray GetYPRSettArray(uint8T &id, uint8T &mode);
    QByteArray GetAGCSettArray(uint8T &id, uint8T &mode);
    QByteArray GetTRANSCSettArray(uint8T &id, uint8T &mode);
    QByteArray GetSPDSettArray(uint8T &id, uint8T &mode);
    QByteArray GetUARTSettArray(uint8T &id, uint8T &mode);
    QByteArray GetUARTGettArray(uint8T &id, uint8T &mode);
    QByteArray GetFLASHActArray(uint8T &id, uint8T &mode);
    QByteArray GetFORMATSettArray(uint8T &id, uint8T &mode);
    QByteArray GetREBOOTActArray(uint8T &id, uint8T &mode);
    QByteArray GetMARKSettArray(uint8T &id, uint8T &mode);
    QByteArray GetMARKContArray(uint8T &id, uint8T &mode);*/
private:
        void CheckSumUpdate(uint8T byte);
    void ZeroOutData();

    uint8T CHECK1 = 0;
    uint8T CHECK2 = 0;
};

#endif // SENDDATA_H
