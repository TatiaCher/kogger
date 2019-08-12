#include "SendData.h"
#include <QDebug>

SendData::SendData()
{}

QByteArray SendData::CreateArrayMSG(uint8T &id, uint8T &mode, QByteArray &payload)
{
    ZeroOutData();

    QByteArray arrayMSG;

    arrayMSG.append(static_cast<char>(0xBB));
    arrayMSG.append(static_cast<char>(0x55));

    if (payload.size() == 0)
    {
        arrayMSG.append(static_cast<char>(0));
        CheckSumUpdate(static_cast<uint8T>(0));
    }
    else
    {
        arrayMSG.append(static_cast<char>(payload.size()));
        CheckSumUpdate(static_cast<uint8T>(payload.size()));
    }

    arrayMSG.append(static_cast<char>(mode));
    arrayMSG.append(static_cast<char>(id));

    CheckSumUpdate(mode);
    CheckSumUpdate(id);

    if (payload.size() != 0)
    {
        for (int i = 0; i < payload.size(); i++)
        {
            arrayMSG.append(payload[i]);
            CheckSumUpdate(static_cast<uint8T>(payload[i]));
        }
    }

    arrayMSG.append(static_cast<char>(CHECK1));
    arrayMSG.append(static_cast<char>(CHECK2));

    ZeroOutData();

    return arrayMSG;
}

void SendData::CheckSumUpdate(uint8T byte)
{
    CHECK1 += byte;
    CHECK2 += CHECK1;
}

void SendData::ZeroOutData()
{
    CHECK1 = 0;
    CHECK2 = 0;
}
