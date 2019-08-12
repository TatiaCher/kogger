#include "CentralWidget.h"

#include <QDebug>

CentralWidget::CentralWidget(QWidget *parent) :
    QWidget(parent),
    m_yPRTimer(new QTimer(this)),
    m_serialPort(new QSerialPort(this)),
    m_realTimePlot(new realTimePlot(this)),
    m_parse(new Parse(this)),
    m_sendData(new SendData)
{
    createDistWidget();
    createArrayWidget();
    createYPRWidget();
    createAGCWidget();
    createTranscWidget();
    createSPDWidget();

    CreateInterface();
    CreateLayouts();
    CreateConnections();
    SetPorts();

    m_yPRTimer->setInterval(1000);
}

void CentralWidget::CreateConnections()
{
     connect(m_connectB, &QPushButton::clicked, this, &CentralWidget::onConnect);
     connect(m_disconnectB, &QPushButton::clicked,  this, &CentralWidget::onDisconnect);
     connect(m_serialPort, &QSerialPort::readyRead, this, &CentralWidget::parseData);

     connect(m_parse, &Parse::gotDistanceData, m_realTimePlot, &realTimePlot::realtimeDataSlot);

     connect(m_pbUpdateSettChar, &QPushButton::clicked, [=](){SendMSG(CMD_CHART, Action);}); //this, &CentralWidget::GetChartActArray);
     connect(m_pbSetArr, &QPushButton::clicked, [=](){SendMSG(CMD_ARRAY , Getting);}); //doesn't work with "Action"  //this, &CentralWidget::GetArrayActArray);
     connect(m_pbSetYPR, &QPushButton::clicked, [=](){SendMSG(CMD_YPR, Setting);}); //this, &CentralWidget::GetYPRSettArray);
     connect(m_pbSetAGC, &QPushButton::clicked, [=](){SendMSG(CMD_AGC, Setting);}); //this, &CentralWidget::GetAGCSettArray);
     connect(m_pbSetTransc, &QPushButton::clicked, [=](){SendMSG(CMD_TRANSC, Setting);}); //this, &CentralWidget::GetTRANSCSettArray);
     connect(m_pbSetSndSpd, &QPushButton::clicked, [=](){SendMSG(CMD_SND_SPD, Setting);}); //this, &CentralWidget::GetSPDSettArray);
     connect(m_parse, &Parse::gotYPRdata, this, &CentralWidget::setYPR);
     connect(m_parse, &Parse::gotTempData, this, &CentralWidget::setTemp);

     connect(m_yPRTimer, &QTimer::timeout, this, &CentralWidget::setdata); //[=](){SendMSG(CMD_YPR, Getting);});
}

void CentralWidget::setdata()
{
    SendMSG(CMD_TEMP, Getting);
    SendMSG(CMD_YPR, Getting);

}

void CentralWidget::startTimer()
{
    if (!m_yPRTimer->isActive())
    {
        m_yPRTimer->start();
    }
}

void CentralWidget::setYPR(double &yaw, double &pitch, double &roll)
{
    m_teYaw->setText(QString("%1").arg(yaw));
    m_tePitch->setText(QString("%1").arg(pitch));
    m_teRoll->setText(QString("%1").arg(roll));
}

void CentralWidget::setTemp(double &tempIMU, double &tempCPU)
{
    m_teRoll->setText(QString("%1").arg(tempIMU));
}

void CentralWidget::createDistWidget()
{
    m_comPortCB = new QComboBox(this);
    m_comPortCB->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ComboBox));

    m_baudRateCB = new QComboBox(this);

    m_sbStrtPosCh = new QSpinBox(this);
    m_sbStrtPosCh->setMinimum(0); //mm
    m_sbItemCount = new QSpinBox(this);
    m_sbItemCount->setRange(1, 5000);
    m_sbItemResol = new QSpinBox(this);
    m_sbItemResol->setRange(10, 1000); //mm
    m_sbPeriod = new QSpinBox(this);
    m_sbPeriod->setRange(0, 65535); //ms

    m_lbStr = new QLabel(this);
    m_lbStr->setText("Start pos, mm, 0 - ");
    m_lbCount = new QLabel(this);
    m_lbCount->setText("Count, 1 - 5000");
    m_lbResol = new QLabel(this);
    m_lbResol->setText("Resolution, mm, 10 - 1000");
    m_lbPeriod = new QLabel(this);
    m_lbPeriod->setText("Period, ms, 0 - 65535");

    m_pbUpdateSettChar = new QPushButton(tr("Update Settings"), this);
    m_pbUpdateSettChar->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    charWidget = new QWidget;
    QHBoxLayout *hLayoutCh= new QHBoxLayout;
    hLayoutCh->addWidget(m_comPortCB);
    hLayoutCh->addWidget(m_baudRateCB);

    QVBoxLayout *vLayoutCh = new QVBoxLayout;
    vLayoutCh->addLayout(hLayoutCh);
    vLayoutCh->addWidget(m_lbStr);
    vLayoutCh->addWidget(m_sbStrtPosCh);
    vLayoutCh->addWidget(m_lbCount);
    vLayoutCh->addWidget(m_sbItemCount);
    vLayoutCh->addWidget(m_lbResol);
    vLayoutCh->addWidget(m_sbItemResol);
    vLayoutCh->addWidget(m_lbPeriod);
    vLayoutCh->addWidget(m_sbPeriod);
    vLayoutCh->addWidget(m_pbUpdateSettChar);

    charWidget->setLayout(vLayoutCh);
}

void CentralWidget::createArrayWidget()
{
    m_sbStrtPosArr = new QSpinBox(this);
    m_sbStrtPosArr->setRange(0, 1000); //mm
    m_sbEndPosArr = new QSpinBox(this);
    m_sbEndPosArr->setMinimum(1000); //mm
    m_sbMaxCountArr = new QSpinBox(this);
    m_sbMaxCountArr->setRange(1, 200);
    m_sbThrRiseArr = new QSpinBox(this);
    m_sbThrRiseArr->setMinimum(0);
    m_sbThrFallArr = new QSpinBox(this);
    m_sbThrFallArr->setMinimum(0);
    m_sbOverlapArr = new QSpinBox(this);
    m_sbOverlapArr->setMinimum(0); //mm
    m_sbMinWidthArr = new QSpinBox(this);
    m_sbMinWidthArr->setMinimum(0); //mm
    m_sbMinAmplArr = new QSpinBox(this);
    m_sbMinAmplArr->setMinimum(0);

    m_lbStrArr = new QLabel(this);
    m_lbStrArr->setText("Start pos, mm, 0 - 1000");
    m_lbEndPosArr = new QLabel(this);
    m_lbEndPosArr->setText("End pos, mm, 1000 - ");
    m_lbMaxCountArr = new QLabel(this);
    m_lbMaxCountArr->setText("Mac Count, 1 - 200");
    m_lbThrRiseArr = new QLabel(this);
    m_lbThrRiseArr->setText("Thr rise, 0 - ");
    m_lbThrFallArr = new QLabel(this);
    m_lbThrFallArr->setText("Thr fall, 0 - ");
    m_lbOverlapArr = new QLabel(this);
    m_lbOverlapArr->setText("Overlap, mm, 0 - ");
    m_lbMinWidthArr = new QLabel(this);
    m_lbMinWidthArr->setText("Resolution, mm, 0 - ");
    m_lbMinAmplArr = new QLabel(this);
    m_lbMinAmplArr->setText("Period, 0 - ");

    m_pbSetArr = new QPushButton(tr("Get array"), this);
    m_pbSetArr->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    arrayWidget = new QWidget;

    QVBoxLayout *vLayoutCh = new QVBoxLayout;
    vLayoutCh->addWidget(m_lbStrArr);
    vLayoutCh->addWidget(m_sbStrtPosArr);
    vLayoutCh->addWidget(m_lbEndPosArr);
    vLayoutCh->addWidget(m_sbEndPosArr);
    vLayoutCh->addWidget(m_lbMaxCountArr);
    vLayoutCh->addWidget(m_sbMaxCountArr);
    vLayoutCh->addWidget(m_lbThrRiseArr);
    vLayoutCh->addWidget(m_sbThrRiseArr);
    vLayoutCh->addWidget(m_lbThrFallArr);
    vLayoutCh->addWidget(m_sbThrFallArr);
    vLayoutCh->addWidget(m_lbOverlapArr);
    vLayoutCh->addWidget(m_sbOverlapArr);
    vLayoutCh->addWidget(m_lbMinWidthArr);
    vLayoutCh->addWidget(m_sbMinWidthArr);
    vLayoutCh->addWidget(m_lbMinAmplArr);
    vLayoutCh->addWidget(m_sbMinAmplArr);
    vLayoutCh->addWidget(m_pbSetArr);

    arrayWidget->setLayout(vLayoutCh);
}

void CentralWidget::createYPRWidget()
{
    m_lbAttachYPR = new QLabel(this);
    m_lbAttachYPR->setText("Attach,0 - 1");

    m_sbAttachYPR = new QSpinBox(this);
    m_sbAttachYPR->setRange(0, 1);

    m_pbSetYPR = new QPushButton(tr("Set"), this);
    m_pbSetYPR->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    YPRWidget = new QWidget;

    QVBoxLayout *vLayoutCh = new QVBoxLayout;
    vLayoutCh->addWidget(m_lbAttachYPR);
    vLayoutCh->addWidget(m_sbAttachYPR);
    vLayoutCh->addWidget(m_pbSetYPR);

    YPRWidget->setLayout(vLayoutCh);
}

void CentralWidget::createAGCWidget()
{
    m_lbStrPosAGC = new QLabel(this);
    m_lbStrPosAGC->setText("Str pos, mm, 0 -");
    m_lbOffsetAGC = new QLabel(this);
    m_lbOffsetAGC->setText("Offset, dB/100, -48 - +48");
    m_lbSlopeAGC = new QLabel(this);
    m_lbSlopeAGC->setText("Slope, Exp/100, 100 - 300");
    m_lbAbsorpAGC = new QLabel(this);
    m_lbAbsorpAGC->setText("Absorp, dB/km, 0 - ");

    m_sbStrPosAGC = new QSpinBox(this);
    m_sbStrPosAGC->setMinimum(0); //mm
    m_sbStrPosAGC->setValue(1000);
    m_sbOffsetAGC = new QSpinBox(this);
    m_sbOffsetAGC->setRange(-48, +48); //(dB)/100
    m_sbOffsetAGC->setValue(0);
    m_sbSlopeAGC = new QSpinBox(this);
    m_sbSlopeAGC->setRange(100, 300); // Exp/100
    m_sbSlopeAGC->setValue(200);
    m_sbAbsorpAGC = new QSpinBox(this);
    m_sbAbsorpAGC->setMinimum(0);

    m_pbSetAGC = new QPushButton(tr("Set"), this);
    m_pbSetAGC->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    AGCWidget = new QWidget;

    QVBoxLayout *vLayoutCh = new QVBoxLayout;
    vLayoutCh->addWidget(m_lbStrPosAGC);
    vLayoutCh->addWidget(m_sbStrPosAGC);
    vLayoutCh->addWidget(m_lbOffsetAGC);
    vLayoutCh->addWidget(m_sbOffsetAGC);
    vLayoutCh->addWidget(m_lbSlopeAGC);
    vLayoutCh->addWidget(m_sbSlopeAGC);
    vLayoutCh->addWidget(m_lbAbsorpAGC);
    vLayoutCh->addWidget(m_sbAbsorpAGC);
    vLayoutCh->addWidget(m_pbSetAGC);

    AGCWidget->setLayout(vLayoutCh);
}

void CentralWidget::createTranscWidget()
{
    m_lbFreqTransc = new QLabel(this);
    m_lbFreqTransc->setText("Freq, kHz, 600 - 720");
    m_lbPulseTransc = new QLabel(this);
    m_lbPulseTransc->setText("Pulse, 0 - 30");
    m_lbBoostTransc = new QLabel(this);
    m_lbBoostTransc->setText("Boost, 0 - 1");

    m_sbFreqtransc = new QSpinBox(this);
    m_sbFreqtransc->setRange(600, 720); //kHz
    m_sbFreqtransc->setValue(675);
    m_sbPulseTransc = new QSpinBox(this);
    m_sbPulseTransc->setRange(0, 30);
    m_sbPulseTransc->setValue(10);
    m_sbBoostTransc = new QSpinBox(this);
    m_sbBoostTransc->setRange(0, 1);
    m_sbBoostTransc->setValue(1);

    m_pbSetTransc = new QPushButton(tr("Set"), this);
    m_pbSetTransc->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    TtranscWidget = new QWidget;

    QVBoxLayout *vLayoutCh = new QVBoxLayout;
    vLayoutCh->addWidget(m_lbFreqTransc);
    vLayoutCh->addWidget(m_sbFreqtransc);
    vLayoutCh->addWidget(m_lbPulseTransc);
    vLayoutCh->addWidget(m_sbPulseTransc);
    vLayoutCh->addWidget(m_lbBoostTransc);
    vLayoutCh->addWidget(m_sbBoostTransc);
    vLayoutCh->addWidget(m_pbSetTransc);

    TtranscWidget->setLayout(vLayoutCh);
}

void CentralWidget::createSPDWidget()
{
    m_lbsndSpd = new QLabel(this);
    m_lbsndSpd->setText("Speed, m/s, 1 - 6000");

    m_sbSndSpd = new QSpinBox(this); //m/s
    m_sbSndSpd->setRange(1, 6000);
    m_sbSndSpd->setValue(1500);

    m_pbSetSndSpd = new QPushButton(tr("Set"), this);
    m_pbSetSndSpd->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    SPDWidget = new QWidget;

    QVBoxLayout *vLayoutCh = new QVBoxLayout;
    vLayoutCh->addWidget(m_lbsndSpd);
    vLayoutCh->addWidget(m_sbSndSpd);
    vLayoutCh->addWidget(m_pbSetSndSpd);

    SPDWidget->setLayout(vLayoutCh);
}

void CentralWidget::CreateInterface()
{
    m_connectB = new QPushButton(tr("Connect"), this);
    m_connectB->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));
    m_disconnectB = new QPushButton(tr("Disconnect"), this);
    m_disconnectB->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed, QSizePolicy::ToolButton));

    m_textLable = new QLabel(this);

    m_lbYaw = new QLabel("Yaw", this);
    m_teYaw = new QLineEdit(this);
    m_teYaw->setReadOnly(true);
    m_lbPitch = new QLabel("Pitch", this);
    m_tePitch = new QLineEdit(this);
    m_tePitch->setReadOnly(true);
    m_lbRoll = new QLabel("Roll", this);
    m_teRoll = new QLineEdit(this);
    m_teRoll->setReadOnly(true);
    m_lbTemp = new QLabel("Temp", this);
    m_teTemp = new QLineEdit(this);
    m_teTemp->setReadOnly(true);

    createDistWidget();

    m_toolBox = new QToolBox(this);
    m_toolBox->setFixedWidth(250);
    m_toolBox->addItem(charWidget, "Distance");
    m_toolBox->addItem(arrayWidget, "Array");
    m_toolBox->addItem(YPRWidget, "YPR");
    m_toolBox->addItem(AGCWidget, "AGC");
    m_toolBox->addItem(TtranscWidget, "Transc");
    m_toolBox->addItem(SPDWidget, "Sound speed");
}

void CentralWidget::CreateLayouts()
{
    QHBoxLayout *hLayout2 = new QHBoxLayout;
    hLayout2->addWidget(m_connectB);
    hLayout2->addWidget(m_disconnectB);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(m_toolBox);
    vLayout->addLayout(hLayout2);
    vLayout->addWidget(m_textLable);

    QHBoxLayout * hLayout3 = new QHBoxLayout;
    hLayout3->addWidget(m_lbYaw);
    hLayout3->addWidget(m_teYaw);
    hLayout3->addWidget(m_lbPitch);
    hLayout3->addWidget(m_tePitch);
    hLayout3->addWidget(m_lbRoll);
    hLayout3->addWidget(m_teRoll);
    hLayout3->addWidget(m_lbTemp);
    hLayout3->addWidget(m_teTemp);

    QVBoxLayout *vLayout2 = new QVBoxLayout;
    vLayout2->addLayout(hLayout3);
    vLayout2->addWidget(m_realTimePlot);

    QHBoxLayout *hLayout4 = new QHBoxLayout;
    hLayout4->addLayout(vLayout);
    hLayout4->addLayout(vLayout2);

    setContentsMargins(0, 0, 0, 0);
    setLayout(hLayout4);
}

void CentralWidget::onDisconnect()
{
    m_serialPort->close();
    m_yPRTimer->stop();
    m_textLable->setText("Disconnected");
}

void CentralWidget::onConnect()
{
    m_serialPort->setPortName(m_comPortCB->currentText());
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    if (m_serialPort->open(QIODevice::ReadWrite))
    {
        m_textLable->setText("Connected");
        startTimer();
    }
    else {
        m_textLable->setText("Can't connect");
    }
}

void CentralWidget::parseData()
{
    QByteArray dataArray = m_serialPort->readAll();
    m_parse->ParseData(dataArray);
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
    if (m_serialPort->write(GetArrayMSG(id, mode)) == -1)
    {
        m_textLable->setText(m_serialPort->errorString());
    }
}

QByteArray CentralWidget::GetArrayMSG(uint8T &id, uint8T &mode)
{
    switch(id)
    {
    case CMD_CHART:
        switch (mode)
        {
            case Getting:   //Get setting of Chart
                return GetNoPayloadArray(id, mode);
            case Action:    //Get chart with/without(?) update settings
                return GetChartActArray(id, mode);
        }
    break;
    //Get array
    case CMD_ARRAY:
        return GetArrayActArray(id, mode);
    case CMD_YPR:
        switch (mode)
        {
        //Request attitude information in Euler 321 format
            case Getting:
                switch (m_parse->GetVersion(mode))
                {
                case 0:
                    return GetNoPayloadArray(id, mode);
                case 1:
                    return GetYPRGettArray(id, mode);
                }
            break;
            case Setting:
                return GetYPRSettArray(id, mode);
        }
        break;
    case CMD_QUAT:
        //Request attitude information in quaternion format
        switch (mode)
        {
        case Getting:
            return GetNoPayloadArray(id, mode);
        }
    break;
    case CMD_TEMP:
        switch (mode)
        {
        case Getting:
            return GetNoPayloadArray(id, mode);
        }
    break;
    case CMD_DIAG:
        switch (mode)
        {
        case Getting:
            return GetNoPayloadArray(id, mode);
        }
    break;
    case CMD_AGC:
        switch (mode)
        {
            case Getting:
                return GetNoPayloadArray(id, mode);
            case Setting:
                return GetAGCSettArray(id, mode);
        }
    break;
    case CMD_TRANSC:
        switch (mode)
        {
            case Getting:
            {
                return GetNoPayloadArray(id, mode);
            }
            case Setting:
                return GetTRANSCSettArray(id, mode);
        }
    break;
    case CMD_SND_SPD:
        switch (mode)
        {
            case Getting:
                return GetNoPayloadArray(id, mode);
            case Setting:
                return GetSPDSettArray(id, mode);
        }
    break;
    case CMD_UART:
        switch (mode)
        {
            case Getting:
            {
                return GetUARTGettArray(id, mode);
            }
            case Setting:
            {
                return GetUARTSettArray(id, mode);
            }
        }
    break;
    case CMD_FLASH:
        return GetFLASHActArray(id, mode);
    case CMD_FORMAT:
        return GetFORMATSettArray(id, mode);
    case CMD_REBOOT:
        return GetREBOOTActArray(id, mode);
    case CMD_MARK:
        switch (mode)
        {
            case Getting:
            {
                return GetNoPayloadArray(id, mode);
            }
            case Setting:
            {
                return GetMARKSettArray(id, mode);
            }
            case Content:
            {
                return GetMARKContArray(id, mode);
            }
        }
    break;
    case CMD_VERSION :
        return GetNoPayloadArray(id, mode);
    default: break;
    }
    return nullptr;
}

QByteArray CentralWidget::GetNoPayloadArray(uint8T &id, uint8T &mode)
{
    QByteArray arrayPayload;
    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetChartActArray(uint8T &id, uint8T &mode)
{
    uint32_t STRT_POS = m_sbStrtPosCh->value();
    uint16_t ITEM_COUNT = m_sbItemCount->value();
    uint16_t ITEM_RESOL = m_sbItemResol->value();
    uint16_t REPEAT_PERIOD = m_sbPeriod->value();
    uint32_t RESERVED = 0;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&STRT_POS, sizeof(STRT_POS));
    arrayPayload.append((const char*)&ITEM_COUNT, sizeof(ITEM_COUNT));
    arrayPayload.append((const char*)&ITEM_RESOL, sizeof(ITEM_RESOL));
    arrayPayload.append((const char*)&REPEAT_PERIOD, sizeof(REPEAT_PERIOD));
    arrayPayload.append((const char*)&RESERVED, sizeof(RESERVED));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetArrayActArray(uint8T &id, uint8T &mode)
{
    uint32_t STRT_POS = m_sbStrtPosArr->value();
    uint32_t END_POS = m_sbEndPosArr->value();
    uint16_t MAX_COUNT = m_sbMaxCountArr->value();
    uint16_t THR_RISE = m_sbThrRiseArr->value();
    uint16_t THR_FALL = m_sbThrFallArr->value();
    uint16_t OVERLAP = m_sbOverlapArr->value();
    uint16_t MIN_WIDTH = m_sbMinWidthArr->value();
    uint16_t MIN_AMPL = m_sbMinAmplArr->value();

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&STRT_POS, sizeof(STRT_POS));
    arrayPayload.append((const char*)&END_POS, sizeof(END_POS));
    arrayPayload.append((const char*)&MAX_COUNT, sizeof(MAX_COUNT));
    arrayPayload.append((const char*)&THR_RISE, sizeof(THR_RISE));
    arrayPayload.append((const char*)&THR_FALL, sizeof(THR_FALL));
    arrayPayload.append((const char*)&OVERLAP, sizeof(OVERLAP));
    arrayPayload.append((const char*)&MIN_WIDTH, sizeof(MIN_WIDTH));
    arrayPayload.append((const char*)&MIN_AMPL, sizeof(MIN_AMPL));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetYPRGettArray(uint8T &id, uint8T &mode)
{
    uint8T ATTACH = m_sbAttachYPR->value();

    QByteArray arrayPayload;
    arrayPayload.append((const char*)&ATTACH, sizeof(ATTACH));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetYPRSettArray(uint8T &id, uint8T &mode)
{
    uint8T ATTACH  = m_sbAttachYPR->value();

    QByteArray arrayPayload;
    arrayPayload.append((const char*)&ATTACH, sizeof(ATTACH));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetAGCSettArray(uint8T &id, uint8T &mode)
{
    uint32_t STRT_POS = m_sbStrPosAGC->value();
    int16_t OFFSET = m_sbOffsetAGC->value();
    uint16_t SLOPE = m_sbSlopeAGC->value();
    uint16_t ABSORP = m_sbAbsorpAGC->value();
    uint32_t RESERVED = 0;
    uint32_t RESERVED2 = 0;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&STRT_POS, sizeof(STRT_POS));
    arrayPayload.append((const char*)&OFFSET, sizeof(OFFSET));
    arrayPayload.append((const char*)&SLOPE, sizeof(SLOPE));
    arrayPayload.append((const char*)&ABSORP, sizeof(ABSORP));
    arrayPayload.append((const char*)&RESERVED, sizeof(RESERVED));
    arrayPayload.append((const char*)&RESERVED2, sizeof(RESERVED2));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetTRANSCSettArray(uint8T &id, uint8T &mode)
{
    uint32_t FREQ = m_sbFreqtransc->value();
    uint8T PULSE = m_sbPulseTransc->value();
    uint8T BOOST = m_sbBoostTransc->value();
    uint32_t RESERVED = 0;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&FREQ, sizeof(FREQ));
    arrayPayload.append((const char*)&PULSE, sizeof(PULSE));
    arrayPayload.append((const char*)&BOOST, sizeof(BOOST));
    arrayPayload.append((const char*)&RESERVED, sizeof(RESERVED));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetSPDSettArray(uint8T &id, uint8T &mode)
{
    uint16_t SOUND_SPEED = m_sbSndSpd->value();
    uint32_t RESERVED = 0;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&SOUND_SPEED, sizeof(SOUND_SPEED));
    arrayPayload.append((const char*)&RESERVED, sizeof(RESERVED));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

//is not using now
QByteArray CentralWidget::GetUARTSettArray(uint8T &id, uint8T &mode)
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

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetUARTGettArray(uint8T &id, uint8T &mode)
{
    uint32_t UART_KEY = 0xC96B5D4A;
    uint8T UART_ID;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&UART_KEY, sizeof(UART_KEY));
    arrayPayload.append((const char*)&UART_ID, sizeof(UART_ID));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetFLASHActArray(uint8T &id, uint8T &mode)
{
    uint32_t KEY = 0xC96B5D4A;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&KEY, sizeof(KEY));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetFORMATSettArray(uint8T &id, uint8T &mode)
{
    uint32_t KEY = 0xC96B5D4A;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&KEY, sizeof(KEY));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetREBOOTActArray(uint8T &id, uint8T &mode)
{
    uint32_t KEY = 0xC96B5D4A;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&KEY, sizeof(KEY));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetMARKSettArray(uint8T &id, uint8T &mode)
{
    uint32_t KEY = 0xC96B5D4A;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&KEY, sizeof(KEY));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}

QByteArray CentralWidget::GetMARKContArray(uint8T &id, uint8T &mode)
{
    uint32_t MARK;

    QByteArray arrayPayload;

    arrayPayload.append((const char*)&MARK, sizeof(MARK));

    return  m_sendData->CreateArrayMSG(id, mode, arrayPayload);
}
