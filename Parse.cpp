#include "Parse.h"

void Parse::CheckSumUpdate(uint8T byte)
{
    m_CHECK1 += byte;
    m_CHECK2 += m_CHECK1;
}

void Parse::ResetCheck()
{
    m_CHECK1 = 0;
    m_CHECK2 = 0;
}

void Parse::ResetData()
{
    m_payloadArray.clear();
    m_countByte = 0;
    m_payloadPos = 0;

    ResetCheck();
}

int Parse::GetVersion(uint8T &mode)
{
    std::bitset<8> bits(mode);

    return bits[3] & bits[4] & bits[5];
}

uint8T Parse::ReadU1()
{
    uint8T byte = static_cast<uint8T>(m_payloadArray[m_payloadPos]);
    m_payloadPos++;

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
    int8_t byte = static_cast<int8_t>(m_payloadArray[m_payloadPos]);
    m_payloadPos++;

    return byte;
}

int16_t Parse::ReadS2()
{
    int16_t byte1 = ReadS1();
    int16_t byte2 = ReadS1();

    return static_cast<int16_t>((byte1) | (byte2 << 8));
}

/*////////////////////////Parse data//////////////////////////
////////////////////////////////////////////////////////////*/

void Parse::setRespCallback(std::function<void (const Resp &)> c)
{
    respCallback = std::move(c);
}

void Parse::setChartDataCallback(std::function<void (const ChartDataMode &)> c)
{
    chartDataCallback = std::move(c);
}

void Parse::setChartRqstCallback(std::function<void (const ChartRqstMode &)> c)
{
    chartRqstCallback = std::move(c);
}

void Parse::setYPRCallback(std::function<void (const YPR &)> c)
{
   YPRCallback = std::move(c);
}

void Parse::setYPRattchCallback(std::function<void (const YPRattch &)> c)
{
    YPRattchCallback = std::move(c);
}

void Parse::setTempCallback(std::function<void (const Temp &)> c)
{
    tempCallback = std::move(c);
}

void Parse::setArrayCallback(std::function<void (const Array &)> c)
{
    arrayCallback = std::move(c);
}

void Parse::setQUATCallback(std::function<void (const QUAT &)> c)
{
    quatCallback = std::move(c);
}

void Parse::setDiagCallback(std::function<void (const Diag &)> c)
{
    diagCallback = std::move(c);
}

void Parse::setAGCCallback(std::function<void (const AGC &)> c)
{
    agcCallback = std::move(c);
}

void Parse::setTranscCallback(std::function<void (const Transc &)> c)
{
    transcCallback = std::move(c);
}

void Parse::setSpeedCallback(std::function<void (const Speed &)> c)
{
    speedCallback = std::move(c);
}

void Parse::setUARTCallback(std::function<void (const UART &)> c)
{
    uartCallback = std::move(c);
}

void Parse::setMARKCallback(std::function<void (const MARK &)> c)
{
    markCallback = std::move(c);
}

void Parse::setVersionCallback(std::function<void (const Version &)> c)
{
    versionCallback = std::move(c);
}

void Parse::ParseResp(Resp &resp)
{
    resp.code = ReadU1();
    resp.lenght = ReadU1();
    resp.mode = ReadU1();
    resp.id = ReadU1();
    resp.check1 = ReadU1();
    resp.check2 = ReadU1();
}

void Parse::ParseChartDataMode(ChartDataMode &chartData)
{
    chartData.strtPos = ReadU4();
    chartData.itemOffset = ReadU2();
    chartData.itemUnit = ReadU2();

    for (int i = m_payloadPos; i < m_payloadArray.size(); i++)
    {
       chartData.distance.push_back((static_cast<uint8T>(m_payloadArray[i]) * chartData.itemUnit)/1000.0);
    }
}

void Parse::ParseChartRqstMode(ChartRqstMode &chartRqst)
{
    chartRqst.strtPos = ReadU4();
    chartRqst.itemCount = ReadU2();
    chartRqst.itemResol = ReadU2();
    chartRqst.repeatPeriod = ReadU2();

    ReadU4(); // Reserved
}

void Parse::ParseYPR(YPR &ypr)
{
    int16_t YAW = ReadS2();
    int16_t PITCH = ReadS2();
    int16_t ROLL = ReadS2();

    ypr.YAW = YAW * 0.01;
    ypr.PITCH = PITCH * 0.01;
    ypr.ROLL = ROLL * 0.01;
}

void Parse::ParseYPRattach(YPRattch &yprAttach)
{
    yprAttach.ATTACH = ReadU1();
}

void Parse::ParseTemp(Temp &temp)
{
    int16_t TEMPIMU = ReadS2();
    int16_t TEMPCPU = ReadS2();
    ReadU4(); //Reserved

    temp.TEMPIMU = TEMPIMU * 0.01;
    temp.TEMPCPU = TEMPCPU * 0.01;
}

void Parse::ParseArray(Array &array)
{
    array.pcktNbr = ReadU2();
    int countPckt = (m_length - 2)/8;

    m_payloadPos += 2;

    ArrayData data;
    for (int i = 1; i <= countPckt; i++)
    {
        data.distance = ReadU4();
        data.width = ReadU2();
        data.amplitude = ReadU2();

        array.dwa.push_back(data);
    }
}

void Parse::ParseQUAT(QUAT &quat)
{
    quat.W0 = ReadU4();
    quat.W1 = ReadU4();
    quat.W2 = ReadU4();
    quat.W3 = ReadU4();
}

void Parse::ParseDiag(Diag &diag)
{
    diag.UPTIME = ReadU4(); //ms
    int16_t TEMP_IMU = ReadS2();
    int16_t TEMP_CPU = ReadS2();
    int16_t TEMP_MIN = ReadS2();
    int16_t TEMP_MAX = ReadS2();
    diag.SYS_VOLT = ReadU2(); //mV
    diag.BOOST_VOLT = ReadU2(); //mV
    diag.DET_VOLT = ReadU2(); //mV
    diag.DET_NOISE = ReadU2(); //mV
    diag.AGC_GATE_VOLT = ReadU2(); //mV

    diag.TEMP_IMU = TEMP_IMU * 0.01;
    diag.TEMP_CPU = TEMP_CPU * 0.01;
    diag.TEMP_MIN = TEMP_MIN * 0.01;
    diag.TEMP_MAX = TEMP_MAX * 0.01;

    ReadU4(); //Reserved
    ReadU4(); //Reserved
    ReadU4(); //Reserved
}

void Parse::ParseAGC(AGC &agc)
{
    agc.STRT_POS = ReadU4();   // mm
    agc.OFFSET = ReadS2();  // dB/100
    agc.SLOPE = ReadU2();  // Exp/100
    agc.ABSORP = ReadU2(); // dB/km

    ReadU4(); //Reserved
    ReadU4(); //Reserved
}

void Parse::ParseTransc(Transc &transc)
{
    transc.FREQ = ReadU2(); //kHz
    transc.PULSE = ReadU1();
    transc.BOOST = ReadU1();

    transc.reserved =  ReadU4();
}

void Parse::ParseSpeed(Speed &speed)
{
    speed.sndSpeed = ReadU2(); // m/s

    speed.reserved = ReadU4();
}

void Parse::ParseUART(UART &uart)
{
    uart.UART_KEY = ReadU4();
    uart.UART_ID = ReadU1();
    uart.BAUDRATE = ReadU4();

    ReadU4(); //Reserved
}

void Parse::ParseMark(MARK &mark)
{
    mark.MARK = ReadU1();
}

void Parse::ParseVersion(Version &version)
{
    version.SW_MAJOR = ReadU1();
    version.SW_MINOR = ReadU1();
    version.HW_MAJOR = ReadU1();
    version.HW_MINOR = ReadU1();

    for (int i = 0; i < 12; i++)
    {
        version.SERIAL_NBR[i] = ReadU1();
    }

    version.LICENSE = ReadU1();
    version.BOOT = ReadU1();
    version.DATE_Y = ReadU1();
    version.DATE_M = ReadU1();
    version.DATE_D = ReadU1();
}

void Parse::GetPayload(uint8T &id, uint8T &mode)
{
    switch (id)
    {
    case CMD_RESP:
    {
        Resp resp;
        ParseResp(resp);
        respCallback(resp);
        break;
    }
    case CMD_CHART:
    {
        switch (mode)
        {
        case Reaction:
            ChartRqstMode chsrtRqst;
            ParseChartRqstMode(chsrtRqst);
            chartRqstCallback(chsrtRqst);
            break;
        case Content:
            ChartDataMode chartData;
            ParseChartDataMode(chartData);
            chartDataCallback(chartData);
            break;
        }
        break;
    }
    case CMD_ARRAY:
    {
        Array array;
        ParseArray(array);
        arrayCallback(array);
        break;
    }
    case CMD_YPR:
    {
        switch (mode)
        {
        case Content:
            if (GetVersion(mode) == 0)
            {
                YPR ypr;
                ParseYPR(ypr);
                YPRCallback(ypr);
            }
            else
            {
                YPRattch yprAttch;
                ParseYPRattach(yprAttch);
                YPRattchCallback(yprAttch);
            }
        }
        break;
    }
    case CMD_QUAT:
    {
        QUAT quat;
        ParseQUAT(quat);
        quatCallback(quat);
        break;
    }
    case CMD_TEMP:
    {
        Temp temp;
        ParseTemp(temp);
        tempCallback(temp);
        break;
    }
    case CMD_DIAG:
    {
        Diag diag;
        ParseDiag(diag);
        diagCallback(diag);
        break;
    }
    case CMD_AGC:
    {
        AGC agc;
        ParseAGC(agc);
        agcCallback(agc);
        break;
    }
    case CMD_TRANSC:
    {
        Transc transc;
        ParseTransc(transc);
        transcCallback(transc);
        break;
    }
    case CMD_SND_SPD:
    {
        Speed speed;
        ParseSpeed(speed);
        speedCallback(speed);
        break;
    }
    case CMD_UART:
    {
        UART uart;
        ParseUART(uart);
        uartCallback(uart);
        break;
    }
    case CMD_MARK:
    {
        MARK mark;
        ParseMark(mark);
        markCallback(mark);
        break;
    }
    case CMD_VERSION:
    {
        Version version;
        ParseVersion(version);
        versionCallback(version);
        break;
    }
    }
}

void Parse::ParseData(char byte)
{
    switch (m_frameStatus)
    {
    case sync1:
        if (static_cast<uint8T>(byte) == 0xBB)
        {
            m_frameStatus = sync2;
        }
        break;
    case sync2:
        if (static_cast<uint8T>(byte) == 0x55)
        {
            m_frameStatus = payloudLength;
        }
        else {
            m_frameStatus = sync1;
        }
        break;
    case payloudLength:
        m_length = static_cast<uint8T>(byte);
        CheckSumUpdate(m_length);
        m_frameStatus = frameMode;
        break;
    case frameMode:
        m_mode = static_cast<uint8T>(byte);
        CheckSumUpdate(m_mode);
        m_frameStatus = frameId;
        break;
    case frameId:
        m_id = static_cast<uint8T>(byte);
        CheckSumUpdate(m_id);
        m_frameStatus = payload;
        break;
    case payload:
        m_payloadArray.push_back(byte);
        CheckSumUpdate( static_cast<uint8T>(byte));
        m_countByte++;
        if (m_countByte >= m_length)
        {
            m_frameStatus = check1;
        }
        break;
    case check1:
        m_frameCheck1 = static_cast<uint8T>(byte);
        m_frameStatus = check2;
        break;
    case check2:
        m_frameCheck2 = static_cast<uint8T>(byte);
        if (m_frameCheck1 == m_CHECK1 && m_frameCheck2 == m_CHECK2)
        {
            GetPayload(m_id, m_mode);

            m_frameStatus = sync1;
            ResetData();
        }
        else
        {
            m_frameStatus = sync1;
            ResetData();
        }
        break;
    }
}

/*////////////////////////Send data///////////////////////////
////////////////////////////////////////////////////////////*/


std::vector<uint8T> Parse::ParseUint32ToBytes(uint32_t &b)
{
    std::vector<uint8T> bytes;
    for (int i = 0; i < 4; i++)
    {
        bytes.push_back((b >> i * 8) & 0xFF);
    }
    return bytes;
}

std::vector<uint8T> Parse::ParseUint16ToBytes(uint16_t &b)
{
    std::vector<uint8T> bytes;
    for (int i = 0; i < 3; i++)
    {
        bytes.push_back((b >> i * 8) & 0xFF);
    }
    return bytes;
}

std::vector<uint8T> Parse::CreateWithoutPayloadPacket(uint8T id, uint8T mode)
{
    std::vector<uint8T> payload;
    return  CreatePacket(id, mode, payload);
}

//Get chart with/without(?) update settings
std::vector<uint8T> Parse::CreateChartActionPacket(uint32_t STRT_POS, uint16_t ITEM_COUNT, uint16_t ITEM_RESOL,
                                            uint16_t REPEAT_PERIOD)
{
    uint8T id = CMD_CHART;
    uint8T mode = Action;

    uint32_t RESERVED = 0;

    std::vector<uint8T> payload;

    std::vector<uint8T> variable = ParseUint32ToBytes(STRT_POS);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint16ToBytes(ITEM_COUNT);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint16ToBytes(ITEM_RESOL);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint16ToBytes(REPEAT_PERIOD);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint32ToBytes(RESERVED);
    payload.insert(payload.end(), variable.begin(), variable.end());

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateArrayActionPacket(uint32_t STRT_POS, uint32_t END_POS, uint16_t MAX_COUNT,uint16_t THR_RISE,
     uint16_t THR_FALL, uint16_t OVERLAP,uint16_t MIN_WIDTH, uint16_t MIN_AMPL)
{
    uint8T id = CMD_ARRAY;
    uint8T mode = Getting; //doesn't work with Action

    std::vector<uint8T> payload;

    std::vector<uint8T> variable = ParseUint32ToBytes(STRT_POS);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint32ToBytes(END_POS);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint16ToBytes(MAX_COUNT);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint16ToBytes(THR_RISE);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint16ToBytes(THR_FALL);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint16ToBytes(OVERLAP);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint16ToBytes(MIN_WIDTH);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint16ToBytes(MIN_AMPL);
    payload.insert(payload.end(), variable.begin(), variable.end());

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateYPRGettingPacket(uint8T ATTACH) //v1
{
    uint8T id = CMD_YPR;
    uint8T mode = Getting;
    mode = mode | 1 << 3;

    std::vector<uint8T> payload;
    payload.push_back(ATTACH);

    return CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateYPRSettingPacket(uint8T ATTACH) //v1
{
    uint8T id = CMD_YPR;
    uint8T mode = Setting;
    mode = mode | 1 << 3;

    std::vector<uint8T> payload;
    payload.push_back(ATTACH);

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateAGCSettingPacket(uint32_t STRT_POS, int16_t OFFSET, uint16_t SLOPE, uint16_t ABSORP)
{
    uint8T id = CMD_AGC;
    uint8T mode = Setting;

    uint32_t RESERVED = 0;
    uint32_t RESERVED2 = 0;

    std::vector<uint8T> payload;

    std::vector<uint8T> variable = ParseUint32ToBytes(STRT_POS);
    payload.insert(payload.end(), variable.begin(), variable.end());

                        //Parse offset
    payload.push_back(  OFFSET & 0xFF);
    payload.push_back( (OFFSET >> 8) & 0xFF);
    payload.push_back( (OFFSET >> 16) & 0xFF);
    payload.push_back( (OFFSET >> 24) & 0xFF);

    variable = ParseUint16ToBytes(SLOPE);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint16ToBytes(ABSORP);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint32ToBytes(RESERVED);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint32ToBytes(RESERVED2);
    payload.insert(payload.end(), variable.begin(), variable.end());

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateTRANSCSettingPacket(uint32_t FREQ, uint8T PULSE, uint8T BOOST)
{
    uint8T id = CMD_TRANSC;
    uint8T mode = Setting;

    uint32_t RESERVED = 0;

    std::vector<uint8T> payload;

    std::vector<uint8T> variable = ParseUint32ToBytes(FREQ);
    payload.insert(payload.end(), variable.begin(), variable.end());

    payload.push_back(PULSE);
    payload.push_back(BOOST);

    variable = ParseUint32ToBytes(RESERVED);
    payload.insert(payload.end(), variable.begin(), variable.end());

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateSPDSettingPacket(uint16_t SOUND_SPEED)
{
    uint8T id = CMD_SND_SPD;
    uint8T mode = Setting;

    uint32_t RESERVED = 0;

    std::vector<uint8T> payload;

    std::vector<uint8T> variable = ParseUint16ToBytes(SOUND_SPEED);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint32ToBytes(RESERVED);
    payload.insert(payload.end(), variable.begin(), variable.end());

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateUARTSettingPacket(uint8T UART_ID, uint32_t BAUDRATE)
{
    uint8T id = CMD_UART;
    uint8T mode = Setting;

    uint32_t UART_KEY = 0xC96B5D4A;
    uint32_t RESERVED = 0;

    std::vector<uint8T> payload;

    std::vector<uint8T> variable = ParseUint32ToBytes(UART_KEY);
    payload.insert(payload.end(), variable.begin(), variable.end());

    payload.push_back(UART_ID);

    variable = ParseUint32ToBytes(BAUDRATE);
    payload.insert(payload.end(), variable.begin(), variable.end());

    variable = ParseUint32ToBytes(RESERVED);
    payload.insert(payload.end(), variable.begin(), variable.end());

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateUARTGettingPacket(uint8T UART_ID)
{
    uint8T id = CMD_UART;
    uint8T mode = Getting;

    uint32_t UART_KEY = 0xC96B5D4A;

    std::vector<uint8T> payload = ParseUint32ToBytes(UART_KEY);
    payload.push_back(UART_ID);

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateFlashActionPacket()
{
    uint8T id = CMD_FLASH;
    uint8T mode = Action;

    uint32_t KEY = 0xC96B5D4A;

    std::vector<uint8T> payload = ParseUint32ToBytes(KEY);

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateFormatSettingPacket()
{
    uint8T id = CMD_FORMAT;
    uint8T mode = Action;

    uint32_t KEY = 0xC96B5D4A;

    std::vector<uint8T> payload = ParseUint32ToBytes(KEY);

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateRebootActionPacket()
{
    uint8T id = CMD_REBOOT;
    uint8T mode = Action;

    uint32_t KEY = 0xC96B5D4A;

    std::vector<uint8T> payload = ParseUint32ToBytes(KEY);

    return CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreateMarkSettingPacket()
{
    uint8T id = CMD_MARK;
    uint8T mode = Setting;

    uint32_t KEY = 0xC96B5D4A;

    std::vector<uint8T> payload = ParseUint32ToBytes(KEY);

    return  CreatePacket(id, mode, payload);
}

std::vector<uint8T> Parse::CreatePacket(uint8T &id, uint8T &mode, std::vector<uint8T> &payload)
{
    ResetCheck();

    std::vector<uint8T> packet;

    packet.push_back(0xBB);
    packet.push_back(0x55);

    if (payload.size() == 0)
    {
        packet.push_back(0);
        CheckSumUpdate(0);
    }
    else
    {
        packet.push_back(static_cast<uint8T>(payload.size()));
        CheckSumUpdate(static_cast<uint8T>(payload.size()));
    }

    packet.push_back(mode);
    packet.push_back(id);

    CheckSumUpdate(mode);
    CheckSumUpdate(id);

    if (payload.size() != 0)
    {
        for (int i = 0; i < payload.size(); i++)
        {
            packet.push_back(payload[i]);
            CheckSumUpdate(payload[i]);
        }
    }

    packet.push_back(m_CHECK1);
    packet.push_back(m_CHECK2);

    ResetCheck();
    return packet;
}


