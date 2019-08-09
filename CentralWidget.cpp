#include "CentralWidget.h"

#include <QDebug>

CentralWidget::CentralWidget(QWidget *parent) :
    QWidget(parent),
    m_serialPort(new QSerialPort(this)),
    m_realTimePlot(new realTimePlot(this)),
    m_parse(new Parse(this))
{
    CreateInterface();
    CreateLayouts();
    CreateConnections();
    SetPorts();
}

void CentralWidget::CreateConnections()
{
     connect(m_connectB, &QPushButton::clicked, this, &CentralWidget::onConnect);
     connect(m_disconnectB, &QPushButton::clicked, this, &CentralWidget::onDisconnect); //[=](){SendMSG(CMD_YPR, Getting);});
     connect(m_serialPort, &QSerialPort::readyRead, this, &CentralWidget::parseData);
     connect(m_parse, &Parse::setData, m_realTimePlot, &realTimePlot::realtimeDataSlot);
}

void CentralWidget::CreateInterface()
{
    m_connectB = new QPushButton(tr("Connect"), this);
    m_connectB->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));
    m_disconnectB = new QPushButton(tr("Disconnect"), this);
    m_disconnectB->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    m_comPortCB = new QComboBox;
    m_comPortCB->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ComboBox));
    //m_baudRateCB = new QComboBox;
}

void CentralWidget::CreateLayouts()
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->setStretch(0,5);
    vLayout->addWidget(m_connectB);
    vLayout->addWidget(m_disconnectB);
    vLayout->addWidget(m_comPortCB);
    //vLayout->addWidget(m_baudRateCB);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addLayout(vLayout);
    hLayout->addWidget(m_realTimePlot->m_customPlot);

    setContentsMargins(0, 0, 0, 0);
    setLayout(hLayout);
}

void CentralWidget::onDisconnect()
{
    m_serialPort->close();
}

void CentralWidget::onConnect()
{
    m_serialPort->setPortName(m_comPortCB->currentText());
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    if (m_serialPort->open(QIODevice::ReadWrite))
    {
        qDebug() << "yes";
    }
    else {
        qDebug() << "noup";
    }
}

void CentralWidget::parseData()
{
    m_parse->ParseData(*m_serialPort);
}

void CentralWidget::SetPorts()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for(QSerialPortInfo &port: ports)
    {
        m_comPortCB->addItem(port.portName());
    }
}

void CentralWidget::SendMSG(uint8T id, uint8T mode)
{
    m_serialPort->write(GetArrayMSG(id, mode));
}

QByteArray CentralWidget::GetArrayMSG(uint8T &id, uint8T &mode)
{
    QByteArray arr;

    switch(id)
    {
    case CMD_CHART:
        switch (mode)
        {
            case Getting:   //Get setting of Chart
                arr = "0";
                return m_parse->CreateArrayMSG(id, mode, arr);
            case Action:    //Get chart with/without(?) update settings
                arr = CreateArrChartAct();
                return m_parse->CreateArrayMSG(id, mode, arr);
        }
    break;
    //Get array
    case CMD_ARRAY:
        arr = CreateArrArrayAct();
        return m_parse->CreateArrayMSG(id, mode, arr);
    case CMD_YPR:
        switch (mode)
        {
        //Request attitude information in Euler 321 format
            case Getting:
                switch (m_parse->GetVersion(mode))
                {
                case 0:
                    arr = "0";
                    return m_parse->CreateArrayMSG(id, mode, arr);
                case 1:
                    arr = CreateArrayYPRGett();
                    return m_parse->CreateArrayMSG(id, mode, arr);
                }
            break;
            case Setting:
                arr = CreateArrayYPRSett();
                return m_parse->CreateArrayMSG(id, mode, arr);
        }
        break;
    case CMD_QUAT:
        //Request attitude information in quaternion format
        switch (mode)
        {
        case Getting:
            arr = "0";
            return m_parse->CreateArrayMSG(id, mode, arr);
        }
    break;
    case CMD_TEMP:
        switch (mode)
        {
        case Getting:
            arr = "0";
            return m_parse->CreateArrayMSG(id, mode, arr);
        }
    break;
    case CMD_DIAG:
        switch (mode)
        {
        case Getting:
            arr = "0";
            return m_parse->CreateArrayMSG(id, mode, arr);
        }
    break;
    case CMD_AGC:
        switch (mode)
        {
            case Getting:
                arr = "0";
                return m_parse->CreateArrayMSG(id, mode, arr);
            case Setting:
                arr = CreateArrayAGCSett();
                return m_parse->CreateArrayMSG(id, mode, arr);
        }
    break;
    case CMD_TRANSC:
        switch (mode)
        {
            case Getting:
            {
                arr = "0";
                return m_parse->CreateArrayMSG(id, mode, arr);
            }
            case Setting:
                arr = CreateArrayTRANSCSett();
                return m_parse->CreateArrayMSG(id, mode, arr);
        }
    break;
    case CMD_SND_SPD:
        switch (mode)
        {
            case Getting:
                arr = "0";
                return m_parse->CreateArrayMSG(id, mode, arr);
            case Setting:
                arr = CreateArraySPDSett();
                return m_parse->CreateArrayMSG(id, mode, arr);
        }
    break;
    case CMD_UART:
        switch (mode)
        {
            case Getting:
            {
                arr = CreateArrayUARTGett();
                return m_parse->CreateArrayMSG(id, mode, arr);
            }
            case Setting:
            {
                arr = CreateArrayUARTSett();
                return m_parse->CreateArrayMSG(id, mode, arr);
            }
        }
    break;
    case CMD_FLASH:
        arr = CreateArrayFLASHAct();
        return m_parse->CreateArrayMSG(id, mode, arr);
    case CMD_FORMAT:
        arr = CreateArrayFLASHAct();
        return m_parse->CreateArrayMSG(id, mode, arr);
    case CMD_REBOOT:
        arr = CreateArrayREBOOTAct();
        return m_parse->CreateArrayMSG(id, mode, arr);
    case CMD_MARK:
        switch (mode)
        {
            case Getting:
            {
                arr = "0";
                return m_parse->CreateArrayMSG(id, mode, arr);
            }
            case Setting:
            {
                arr = CreateArrayMARKSett();
                return m_parse->CreateArrayMSG(id, mode, arr);
            }
            case Content:
            {
                arr = CreateArrayMARKCont();
                return m_parse->CreateArrayMSG(id, mode, arr);
            }
        }
    break;
    case CMD_VERSION :
        arr = "0";
        return m_parse->CreateArrayMSG(id, mode, arr);
    default: break;
    }
    return nullptr;
}

QByteArray CentralWidget::CreateArrChartAct()
{
    uint32_t STRT_POS;
    uint16_t ITEM_COUNT;
    uint16_t ITEM_RESOL;
    uint16_t REPEAT_PERIOD;
    uint32_t RESERVED = 0;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&STRT_POS, sizeof(STRT_POS));
    arrayPayload.append((const char*)&ITEM_COUNT, sizeof(ITEM_COUNT));
    arrayPayload.append((const char*)&ITEM_RESOL, sizeof(ITEM_RESOL));
    arrayPayload.append((const char*)&REPEAT_PERIOD, sizeof(REPEAT_PERIOD));
    arrayPayload.append((const char*)&RESERVED, sizeof(RESERVED));

    return  arrayPayload;

}

QByteArray CentralWidget::CreateArrArrayAct()
{
    uint32_t STRT_POS;
    uint32_t END_POS;
    uint16_t MAX_COUNT;
    uint16_t THR_RISE;
    uint16_t THR_FALL;
    uint16_t OVERLAP;
    uint16_t MIN_WIDTH;
    uint16_t MIN_AMPL;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&STRT_POS, sizeof(STRT_POS));
    arrayPayload.append((const char*)&END_POS, sizeof(END_POS));
    arrayPayload.append((const char*)&MAX_COUNT, sizeof(MAX_COUNT));
    arrayPayload.append((const char*)&THR_RISE, sizeof(THR_RISE));
    arrayPayload.append((const char*)&THR_FALL, sizeof(THR_FALL));
    arrayPayload.append((const char*)&OVERLAP, sizeof(OVERLAP));
    arrayPayload.append((const char*)&MIN_WIDTH, sizeof(MIN_WIDTH));
    arrayPayload.append((const char*)&MIN_AMPL, sizeof(MIN_AMPL));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayYPRGett()
{
    uint8T ATTACH;

    QByteArray arrayPayload;
    arrayPayload.append((const char*)&ATTACH, sizeof(ATTACH));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayYPRSett()
{
    uint8T ATTACH;

    QByteArray arrayPayload;
    arrayPayload.append((const char*)&ATTACH, sizeof(ATTACH));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayAGCSett()
{
    uint32_t STRT_POS;
    int16_t OFFSET;
    uint16_t SLOPE;
    uint16_t ABSORP;
    uint32_t RESERVED = 0;
    uint32_t RESERVED2 = 0;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&STRT_POS, sizeof(STRT_POS));
    arrayPayload.append((const char*)&OFFSET, sizeof(OFFSET));
    arrayPayload.append((const char*)&SLOPE, sizeof(SLOPE));
    arrayPayload.append((const char*)&ABSORP, sizeof(ABSORP));
    arrayPayload.append((const char*)&RESERVED, sizeof(RESERVED));
    arrayPayload.append((const char*)&RESERVED2, sizeof(RESERVED2));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayTRANSCSett()
{
    uint32_t FREQ;
    uint8T PULSE;
    uint8T BOOST;
    uint32_t RESERVED = 0;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&FREQ, sizeof(FREQ));
    arrayPayload.append((const char*)&PULSE, sizeof(PULSE));
    arrayPayload.append((const char*)&BOOST, sizeof(BOOST));
    arrayPayload.append((const char*)&RESERVED, sizeof(RESERVED));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArraySPDSett()
{
    uint16_t SOUND_SPEED;
    uint32_t RESERVED = 0;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&SOUND_SPEED, sizeof(SOUND_SPEED));
    arrayPayload.append((const char*)&RESERVED, sizeof(RESERVED));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayUARTSett()
{
    uint32_t UART_KEY = 0xC96B5D4A;
    uint8T UART_ID;
    uint32_t BAUDRATE;
    uint32_t RESERVED = 0;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&UART_KEY, sizeof(UART_KEY));
    arrayPayload.append((const char*)&UART_ID, sizeof(UART_ID));
    arrayPayload.append((const char*)&BAUDRATE, sizeof(BAUDRATE));
    arrayPayload.append((const char*)&RESERVED, sizeof(RESERVED));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayUARTGett()
{
    uint32_t UART_KEY = 0xC96B5D4A;
    uint8T UART_ID;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&UART_KEY, sizeof(UART_KEY));
    arrayPayload.append((const char*)&UART_ID, sizeof(UART_ID));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayFLASHAct()
{
    uint32_t KEY = 0xC96B5D4A;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&KEY, sizeof(KEY));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayFORMATSett()
{
    uint32_t KEY = 0xC96B5D4A;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&KEY, sizeof(KEY));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayREBOOTAct()
{
    uint32_t KEY = 0xC96B5D4A;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&KEY, sizeof(KEY));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayMARKSett()
{
    uint32_t KEY = 0xC96B5D4A;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&KEY, sizeof(KEY));

    return  arrayPayload;
}

QByteArray CentralWidget::CreateArrayMARKCont()
{
    uint32_t MARK;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&MARK, sizeof(MARK));

    return  arrayPayload;
}
