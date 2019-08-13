#include "Parse.h"

#include <QDebug>

Parse::Parse(QObject *parent) :
    QObject(parent)
{
}

uint8T Parse::ReadU1()
{
    uint8T byte = static_cast<uint8T>(payloadArray[payloadPos]);
    payloadPos++;

    return byte;
}

uint16_t Parse::ReadU2()
{
    uint8T byte1 = ReadU1();
    uint8T byte2 = ReadU1();

    return static_cast<uint16_t>((byte1) | (byte2 << 8));
}

uint32_t Parse::ReadU4()
{
    uint8T byte1 = ReadU1();
    uint8T byte2 = ReadU1();
    uint8T byte3 = ReadU1();
    uint8T byte4 = ReadU1();

    return static_cast<uint32_t>((byte1) | (byte2 << 8) | (byte3 << 16) | (byte4 << 24));
}

int8_t Parse::ReadS1()
{
    int8_t byte = static_cast<int8_t>(payloadArray[payloadPos]);
    payloadPos++;

    return byte;
}

int16_t Parse::ReadS2()
{
    int16_t byte1 = ReadS1();
    int16_t byte2 = ReadS1();

    return static_cast<int16_t>((byte1) | (byte2 << 8));
}

void Parse::ParseResp()
{
    uint8T code = ReadU1();
    uint8T lenght = ReadU1();
    uint8T mode = ReadU1();
    uint8T id = ReadU1();
    uint8T check1 = ReadU1();
    uint8T check2 = ReadU1();
}

void Parse::ParseChartDataMode()
{
    double data = 0;
    uint32_t strtPos = ReadU4();
    uint16_t itemOffset = ReadU2();
    uint16_t itemUnit = ReadU2();

    for (int i = payloadPos; i < payloadArray.size(); i++)
    {
       data = static_cast<double>(static_cast<uint8T>(payloadArray[i]) * itemUnit)/1000.0;
       emit gotDistanceData(data);
    }
}

void Parse::ParseChartRqstMode()
{
    uint32_t strtPos = ReadU4();
    uint16_t itemCount = ReadU2();
    uint16_t itemResol = ReadU2();
    uint16_t repeatPeriod = ReadU2();
    uint32_t reserved = ReadU4();
}

void Parse::ParseYPR()
{
    int16_t YAW = ReadS2();
    int16_t PITCH = ReadS2();
    int16_t ROLL = ReadS2();

    double yaw = YAW * 0.01;
    double pitch = PITCH * 0.01;
    double roll = ROLL * 0.01;

    emit gotYPRdata(yaw, pitch, roll);
}

void Parse::ParseYPRattach()
{
    uint8T ATTACH = ReadU1();
}

void Parse::ParseTemp()
{
    int16_t TEMPIMU = ReadS2();
    int16_t TEMPCPU = ReadS2();

    double tempIMU = TEMPIMU * 0.01;
    double tempCPU = TEMPCPU * 0.01;

    emit gotTempData(tempIMU, tempCPU);
}

void Parse::ParseArray()
{
    uint16_t pcktNbr = ReadU2();
    int countPckt = (length - 2)/8;

    payloadPos += 2;

    for (int i = 1; i <= countPckt; i++)
    {
        uint32_t distance = ReadU4();
        uint16_t width = ReadU2();
        uint16_t amplitude = ReadU2();
    }
}

void Parse::ParseQUAT()
{
    float W0 = ReadU4();
    float W1 = ReadU4();
    float W2 = ReadU4();
    float W3 = ReadU4();
}

void Parse::ParseDiag()
{
    uint32_t UPTIME = ReadU4(); //ms
    int16_t TEMP_IMU = ReadS2();
    int16_t TEMP_CPU = ReadS2();
    int16_t TEMP_MIN = ReadS2();
    int16_t TEMP_MAX = ReadS2();
    uint16_t SYS_VOLT = ReadU2(); //mV
    uint16_t BOOST_VOLT = ReadU2(); //mV
    uint16_t DET_VOLT = ReadU2(); //mV
    uint16_t DET_NOISE = ReadU2(); //mV
    uint16_t AGC_GATE_VOLT = ReadU2(); //mV

    double tempIMU = TEMP_IMU * 0.01;
    double tempCPU = TEMP_CPU * 0.01;
    double tempMIN = TEMP_MIN * 0.01;
    double tempMAX = TEMP_MAX * 0.01;

    ReadU4(); //Reserved
    ReadU4(); //Reserved
    ReadU4(); //Reserved
}

void Parse::ParseAGC()
{
    uint32_t STRT_POS = ReadU4();   // mm
    int16_t OFFSET = ReadS2();  // dB/100
    uint16_t SLOPE = ReadU2();  // Exp/100
    uint16_t ABSORP = ReadU2(); // dB/km

    ReadU4(); //Reserved
    ReadU4(); //Reserved
}

void Parse::ParseTransc()
{
    uint16_t FREQ = ReadU2(); //kHz
    uint8T PULSE = ReadU1();
    uint8T BOOST = ReadU1();

    ReadU4(); // Reserved
}

void Parse::ParseSpeed()
{
    uint16_t sndSpeed = ReadU2(); // m/s

    ReadU4(); // Reserved
}

void Parse::ParseUART()
{
    uint32_t UART_KEY = ReadU4();
    uint8T UART_ID = ReadU1();
    uint32_t BAUDRATE = ReadU4();

    ReadU4(); //Reserved

}

void Parse::ParseMark()
{
    uint8T MARK = ReadU1();
}

void Parse::ParseVersion()
{
    uint8T SW_MAJOR = ReadU1();
    uint8T SW_MINOR = ReadU1();
    uint8T HW_MAJOR = ReadU1();
    uint8T HW_MINOR = ReadU1();

    QByteArray SERIAL_NBR;
    for (int i = 1; i <=12; i++)
    {
        SERIAL_NBR.append(static_cast<char>(ReadU1()));
    }

    uint8T LICENSE = ReadU1();
    uint8T BOOT = ReadU1();
    uint8T DATE_Y = ReadU1();
    uint8T DATE_M = ReadU1();
    uint8T DATE_D = ReadU1();
}

void Parse::CheckSumUpdate(uint8T byte)
{
    CHECK1 += byte;
    CHECK2 += CHECK1;
}

void Parse::ZeroOutData()
{
    payloadArray.clear();

    CHECK1 = 0;
    CHECK2 = 0;

    payloadPos = 0;
}

QBitArray Parse::ParseBytesToBits(uint8T mode)
{
    QByteArray bytes;
    bytes[0] = mode;
    QBitArray bits(bytes.count()*8);

    for(int i=0; i<bytes.count(); ++i)
    {
        for(int b=0; b < 8; ++b)
        {
            bits.setBit(i*8+b, bytes.at(i)&(1<<(7-b)));
        }
    }
    return bits;
}

int Parse::GetVersion(uint8T mode)
{
    QBitArray bits = ParseBytesToBits(mode);

    return bits[3] & bits[4] & bits[5];
}

void Parse::GetPayload(uint8T id, uint8T mode)
{
    qDebug() << id;
    switch (id)
    {
    case CMD_RESP:
    {
        ParseResp();
        break;
    }
    case CMD_CHART:
    {
        switch (mode)
        {
        //Content settings of Chart
        case Reaction:
            ParseChartRqstMode();
            break;
        //Data of chart
        case Content:
            ParseChartDataMode();
            break;
        }
        break;
    }
    case CMD_ARRAY:
    {
        ParseArray();
        break;
    }
    case CMD_YPR:
    {
        qDebug() << "YPR";
        switch (mode)
        {
        //Attitude information in Euler 321 format
        case Content:
            if (GetVersion(mode) == 0)
                ParseYPR();
            else {
                ParseYPRattach();
            }
        }
        break;
    }
    case CMD_QUAT:
    {
        //Attitude information in quaternion format
        ParseQUAT();
        break;
    }
    case 0x12:
    {
        qDebug() << "0x12";

        ParseTemp();
        break;
    }
    case CMD_DIAG:
    {
        qDebug() << "0x13";

        ParseDiag();
        break;
    }
    case CMD_AGC:
    {
        qDebug() << "0x14";

        ParseAGC();
        break;
    }
    case CMD_TRANSC:
    {
        qDebug() << "0x15";

        ParseTransc();
        break;
    }
    case CMD_SND_SPD:
    {
        qDebug() << "0x16";

        ParseSpeed();
        break;
    }
    case CMD_UART:
    {
        qDebug() << "0x17";

        ParseUART();
        break;
    }
    case CMD_MARK:
    {
        qDebug() << "0x20";

        ParseMark();
        break;
    }
    case CMD_VERSION:
    {
        qDebug() << "0x21";

        ParseVersion();
        break;
    }
    }
}

void Parse::ParseData(QByteArray &dataArray)
{
    ZeroOutData();

    if (dataArray.size() > 5)
    {
        for (int x = 0; x < dataArray.size(); x++)
        {
            if (static_cast<uint8T>(dataArray[x]) == 0xBB)
            {
                x++;
                if (static_cast<uint8T>(dataArray[x]) == 0x55)
                {
                    x++;
                    length = static_cast<uint8T>(dataArray[x]);
                    CheckSumUpdate(length);

                    x++;
                    mode = static_cast<uint8T>(dataArray[x]);
                    CheckSumUpdate(static_cast<uint8T>(mode));

                    x++;
                    id = static_cast<uint8T>(dataArray[x]);
                    CheckSumUpdate(id);

                    x++;
                    if (dataArray.size() - length+7 >= 0)
                    {
                        for (int i = 0; i < length; i++)
                        {
                            payloadArray.append(dataArray[x]);
                            CheckSumUpdate(static_cast<uint8T>(payloadArray[i]));
                            x++;
                        }
                    }
                    else
                    {
                        break;
                    }

                    uint8T ch1 = static_cast<uint8T>(dataArray[x]);
                    x++;
                    uint8T ch2 = static_cast<uint8T>(dataArray[x]);

                    if (ch1 == CHECK1 && ch2 == CHECK2)
                    {
                        GetPayload(id, mode);
                    }
                }
            }
        }
    }

}
