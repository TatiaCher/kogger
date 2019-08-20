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

#include <vector>
#include <bitset>
#include <functional>

struct Resp
{
    uint8T code;
    uint8T lenght;
    uint8T mode;
    uint8T id;
    uint8T check1;
    uint8T check2;
};

struct ChartDataMode
{
    uint32_t strtPos;
    uint16_t itemOffset;
    uint16_t itemUnit;

    std::vector<double> distance;
};

struct ChartRqstMode
{
    uint32_t strtPos;
    uint16_t itemCount;
    uint16_t itemResol;
    uint16_t repeatPeriod;
};

struct YPR
{
    double YAW;
    double PITCH;
    double ROLL;
};

struct YPRattch
{
    uint8T ATTACH;
};

struct Temp
{
    double TEMPIMU;
    double TEMPCPU;
};

struct ArrayData
{
    uint32_t distance;
    uint16_t width;
    uint16_t amplitude;
};

struct Array
{
    uint16_t pcktNbr;
    std::vector<ArrayData> dwa;
};

struct QUAT
{
    float W0;
    float W1;
    float W2;
    float W3;
};

struct Diag
{
    uint32_t UPTIME;
    double TEMP_IMU;
    double TEMP_CPU;
    double TEMP_MIN;
    double TEMP_MAX;
    uint16_t SYS_VOLT;
    uint16_t BOOST_VOLT;
    uint16_t DET_VOLT;
    uint16_t DET_NOISE;
    uint16_t AGC_GATE_VOLT;
};

struct AGC
{
    uint32_t STRT_POS;
    int16_t OFFSET;
    uint16_t SLOPE;
    uint16_t ABSORP;
};

struct Transc
{
    uint16_t FREQ;
    uint8T PULSE;
    uint8T BOOST;

    uint32_t reserved;
};

struct Speed
{
    uint16_t sndSpeed;

    uint32_t reserved;
};

struct UART
{
    uint32_t UART_KEY = 0xC96B5D4A;
    uint8T UART_ID;
    uint32_t BAUDRATE;

    uint32_t reserved;
};

struct MARK
{
    uint8T MARK;
};

struct Version
{
    uint8T SW_MAJOR;
    uint8T SW_MINOR;
    uint8T HW_MAJOR;
    uint8T HW_MINOR;

    uint8T SERIAL_NBR[12];

    uint8T LICENSE;
    uint8T BOOT;
    uint8T DATE_Y;
    uint8T DATE_M;
    uint8T DATE_D;
};

class Parse
{

public:
    void ParseData(char byte);

    std::vector<uint8T> CreateWithoutPayloadPacket(uint8T m_id, uint8T m_mode);
    std::vector<uint8T> CreateChartActionPacket(uint32_t STRT_POS, uint16_t ITEM_COUNT, uint16_t ITEM_RESOL, uint16_t REPEAT_PERIOD);
    std::vector<uint8T> CreateArrayActionPacket(uint32_t STRT_POS, uint32_t END_POS, uint16_t MAX_COUNT,uint16_t THR_RISE,
                                         uint16_t THR_FALL, uint16_t OVERLAP,uint16_t MIN_WIDTH, uint16_t MIN_AMPL);
    std::vector<uint8T> CreateYPRGettingPacket(uint8T ATTACH);
    std::vector<uint8T> CreateYPRSettingPacket(uint8T ATTACH);
    std::vector<uint8T> CreateAGCSettingPacket(uint32_t STRT_POS, int16_t OFFSET, uint16_t SLOPE, uint16_t ABSORP);
    std::vector<uint8T> CreateTRANSCSettingPacket(uint32_t FREQ, uint8T PULSE, uint8T BOOST);
    std::vector<uint8T> CreateSPDSettingPacket(uint16_t SOUND_SPEED);
    std::vector<uint8T> CreateUARTSettingPacket(uint8T UART_ID, uint32_t BAUDRATE);
    std::vector<uint8T> CreateUARTGettingPacket(uint8T UART_ID);
    std::vector<uint8T> CreateFlashActionPacket();
    std::vector<uint8T> CreateFormatSettingPacket();
    std::vector<uint8T> CreateRebootActionPacket();
    std::vector<uint8T> CreateMarkSettingPacket();

    void setRespCallback(std::function<void (const Resp &)> c);
    void setChartDataCallback(std::function<void (const ChartDataMode &)> c);
    void setChartRqstCallback(std::function<void (const ChartRqstMode &)> c);
    void setYPRCallback(std::function<void (const YPR &)> c);
    void setYPRattchCallback(std::function<void (const YPRattch &)> c);
    void setTempCallback(std::function<void (const Temp &)> c);
    void setArrayCallback(std::function<void (const Array &)> c);
    void setQUATCallback(std::function<void (const QUAT &)> c);
    void setDiagCallback(std::function<void (const Diag &)> c);
    void setAGCCallback(std::function<void (const AGC &)> c);
    void setTranscCallback(std::function<void (const Transc &)> c);
    void setSpeedCallback(std::function<void (const Speed &)> c);
    void setUARTCallback(std::function<void (const UART &)> c);
    void setMARKCallback(std::function<void (const MARK &)> c);
    void setVersionCallback(std::function<void (const Version &)> c);

private:
    void CheckSumUpdate(uint8T byte);
    void GetPayload( uint8T &m_id, uint8T &m_mode);
    void ResetCheck();
    void ResetData();
    int GetVersion(uint8T &m_mode);

    uint8T ReadU1();
    uint16_t ReadU2();
    uint32_t ReadU4();
    int8_t ReadS1();
    int16_t ReadS2();

    void ParseResp(Resp &resp);
    void ParseChartDataMode(ChartDataMode &chartData);
    void ParseChartRqstMode(ChartRqstMode &chartRqst);
    void ParseYPR(YPR &ypr);
    void ParseYPRattach(YPRattch &yprAttach);
    void ParseTemp(Temp &temp);
    void ParseArray(Array &array);
    void ParseQUAT(QUAT &quat);
    void ParseDiag(Diag &diag);
    void ParseAGC(AGC &agc);
    void ParseTransc(Transc &transc);
    void ParseSpeed(Speed &speed);
    void ParseUART(UART &uart);
    void ParseMark(MARK &mark);
    void ParseVersion(Version &version);

    std::vector<uint8T> ParseUint32ToBytes(uint32_t &b);
    std::vector<uint8T> ParseUint16ToBytes(uint16_t &b);
    std::vector<uint8T> CreatePacket(uint8T &id, uint8T &mode, std::vector<uint8T> &payload);

    int m_payloadPos = 0;
    int m_countByte = 0;
    std::vector<char> m_payloadArray;

    uint8T m_length = 0;
    uint8T m_mode = 0;
    uint8T m_id = 0;
    uint8T m_frameCheck1 = 0;
    uint8T m_frameCheck2 = 0;

    uint8T m_CHECK1 = 0;
    uint8T m_CHECK2 = 0;

    enum frameStructure
    {
        sync1,
        sync2,
        payloudLength,
        frameMode,
        frameId,
        payload,
        check1,
        check2
    };

    frameStructure m_frameStatus = sync1;

    std::function<void (Resp &resp)> respCallback;
    std::function<void (ChartDataMode &chartData)> chartDataCallback;
    std::function<void (ChartRqstMode &rqstData)> chartRqstCallback;
    std::function<void (YPR &ypr)> YPRCallback;
    std::function<void (YPRattch &yprAttch)> YPRattchCallback;
    std::function<void (Temp &temp)> tempCallback;
    std::function<void (Array &array)> arrayCallback;
    std::function<void (QUAT &quat)> quatCallback;
    std::function<void (Diag &diag)> diagCallback;
    std::function<void (AGC &agc)> agcCallback;
    std::function<void (Transc &transc)> transcCallback;
    std::function<void (Speed &speed)> speedCallback;
    std::function<void (UART &uart)> uartCallback;
    std::function<void (MARK &mark)> markCallback;
    std::function<void (Version &version)> versionCallback;
};

#endif // PARSEARR_H
