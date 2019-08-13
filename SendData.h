#ifndef SENDDATA_H
#define SENDDATA_H

#include <QByteArray>

class SendData
{
public:
    SendData();

    QByteArray CreateArrayMSG(uint8T &id, uint8T &mode, QByteArray &payload);
private:
    void CheckSumUpdate(uint8T byte);
    void ZeroOutData();

    uint8T CHECK1 = 0;
    uint8T CHECK2 = 0;
};

#endif // SENDDATA_H
