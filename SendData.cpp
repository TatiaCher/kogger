#include "SendData.h"
#include <QDebug>

SendData::SendData()
{}

std::vector<char> SendData::CreateArrayMSG(uint8T &id, uint8T &mode, QByteArray &payload)
{
    ZeroOutData();

    std::vector<char> arrayMSG;

    arrayMSG.push_back(static_cast<char>(0xBB));
    arrayMSG.push_back(static_cast<char>(0x55));

    if (payload.size() == 0)
    {
        arrayMSG.push_back(static_cast<char>(0));
        CheckSumUpdate(static_cast<uint8T>(0));
    }
    else
    {
        arrayMSG.push_back(static_cast<char>(payload.size()));
        CheckSumUpdate(static_cast<uint8T>(payload.size()));
    }

    arrayMSG.push_back(static_cast<char>(mode));
    arrayMSG.push_back(static_cast<char>(id));

    CheckSumUpdate(mode);
    CheckSumUpdate(id);

    if (payload.size() != 0)
    {
        for (int i = 0; i < payload.size(); i++)
        {
            arrayMSG.push_back(payload[i]);
            CheckSumUpdate(static_cast<uint8T>(payload[i]));
        }
    }

    arrayMSG.push_back(static_cast<char>(CHECK1));
    arrayMSG.push_back(static_cast<char>(CHECK2));

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
