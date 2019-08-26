#include "CentralWidget.h"

#include <QDebug>

CentralWidget::CentralWidget(QWidget *parent) :
    QWidget(parent),
    //m_yPRTimer(new QTimer(this)),
    m_serialPort(new QSerialPort(this)),
    m_realTimePlot(new realTimePlot(this)),
    m_parse(new KoogerParser)
{
    CreateInterface();
    CreateLayouts();
    CreateConnections();
    SetPorts();

    //m_yPRTimer->setInterval(2000);
}

void CentralWidget::CreateConnections()
{
    connect(m_connectB, &QPushButton::clicked, this, &CentralWidget::onConnect);
    connect(m_disconnectB, &QPushButton::clicked,  this, &CentralWidget::onDisconnect);

    connect(m_serialPort, &QSerialPort::readyRead, this, &CentralWidget::parseData);

    connect(m_pbUpdateSettChar, &QPushButton::clicked, this, &CentralWidget::sendActionChartData);
    connect(m_pbSetArr, &QPushButton::clicked, this, &CentralWidget::sendActionArray);
    connect(m_pbSetYPR, &QPushButton::clicked, this, &CentralWidget::sendYPRData);
    connect(m_pbSetAGC, &QPushButton::clicked, this, &CentralWidget::sendAGCData);
    connect(m_pbSetTransc, &QPushButton::clicked, this, &CentralWidget::sendTranscData);
    connect(m_pbSetSndSpd, &QPushButton::clicked, this, &CentralWidget::sendSpeedData);

    //connect(m_yPRTimer, &QTimer::timeout, [=](){sendWithoutPayloudData(m_parse->CMD_YPR, m_parse->Getting);});

    m_parse->setChartDataCallback(std::bind(&CentralWidget::setChartData, this, std::placeholders::_1));
    m_parse->setYPRCallback(std::bind(&CentralWidget::setYPR, this, std::placeholders::_1));
    m_parse->setTempCallback(std::bind(&CentralWidget::setTemp, this, std::placeholders::_1));
}

void CentralWidget::sendYPRData()
{
    std::vector<uint8T> r = m_parse->CreateYPRGettingPacket(m_sbAttachYPR->value());
    sendData(r);
}

void CentralWidget::sendAGCData()
{
    std::vector<uint8T> r = m_parse->CreateAGCSettingPacket(m_sbStrPosAGC->value(), m_sbOffsetAGC->value(),
                                                            m_sbSlopeAGC->value(), m_sbAbsorpAGC->value());
    sendData(r);
}

void CentralWidget::sendTranscData()
{
    std::vector<uint8T> r = m_parse->CreateTRANSCSettingPacket(m_sbFreqtransc->value(), m_sbPulseTransc->value(),
                                                               m_sbBoostTransc->value());
    sendData(r);
}

void CentralWidget::sendSpeedData()
{
    std::vector<uint8T> r = m_parse->CreateSPDSettingPacket(m_sbSndSpd->value());
    sendData(r);
}

void CentralWidget::sendActionChartData()
{
     std::vector<uint8T> r = m_parse->CreateChartActionPacket(m_sbStrtPosCh->value(), m_sbItemCount->value(),
                                     m_sbItemResol->value(), m_sbPeriod->value());
     sendData(r);
}

void CentralWidget::sendActionArray()
{
     std::vector<uint8T> r = m_parse->CreateArrayActionPacket(m_sbStrtPosArr->value(), m_sbEndPosArr->value(),
                                     m_sbMaxCountArr->value(), m_sbThrRiseArr->value(),
                                     m_sbThrFallArr->value(), m_sbOverlapArr->value(),
                                     m_sbMinWidthArr->value(), m_sbMinAmplArr->value());
     sendData(r);
}

void CentralWidget::sendWithoutPayloudData(uint8T id, uint8T mode)
{
    std::vector<uint8T> r = m_parse->CreateWithoutPayloadPacket(id, mode);
    sendData(r);
}

void CentralWidget::sendData(std::vector<uint8T> &r)
{
    QByteArray arr;
    for (int i = 0; i < r.size(); i++)
    {
        arr[i] = r[i];
    }
    m_serialPort->write(arr);
}

void CentralWidget::setChartData(const KoogerParser::ChartDataMode &r)
{
    for (int i = 0; i < r.distance.size(); i++)
    {
        m_realTimePlot->realtimeDataSlot(r.distance[i]);
    }
}

void CentralWidget::setYPR(const KoogerParser::YPR &y)
{
    m_teYaw->setText(QString("%1").arg(y.YAW));
    m_tePitch->setText(QString("%1").arg(y.PITCH));
    m_teRoll->setText(QString("%1").arg(y.ROLL));
}

void CentralWidget::setTemp(const KoogerParser::Temp &t)
{
    m_teTemp->setText(QString("%1").arg(t.TEMPIMU));
}

void CentralWidget::onDisconnect()
{
    m_serialPort->close();
    //m_yPRTimer->stop();
    m_textLable->setText("Disconnected");
    m_connectB->setEnabled(true);
}

void CentralWidget::onConnect()
{
    m_serialPort->setPortName(m_comPortCB->currentText());
    m_serialPort->setBaudRate(QSerialPort::Baud115200);
    if (m_serialPort->open(QIODevice::ReadWrite))
    {
        m_connectB->setEnabled(false);
        m_textLable->setText("Connected");

        //sendWithoutPayloudData(m_parse->CMD_YPR, m_parse->Getting);
        //startTimer();
    }
    else {
        m_textLable->setText("Can't connect");
    }

}

void CentralWidget::parseData()
{
    QByteArray arr = m_serialPort->readAll();
    for (int i = 0; i < arr.size(); i++)
    {
         m_parse->ParseData(arr[i]);
    }
    arr.clear();
}

void CentralWidget::SetPorts()
{
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for(QSerialPortInfo &port: ports)
    {
        m_comPortCB->addItem(port.portName());
    }
}

void CentralWidget::startTimer()
{
    /*if (!m_yPRTimer->isActive())
    {
        m_yPRTimer->start();
    }*/
}

void CentralWidget::CreateInterface()
{
    m_comPortCB = new QComboBox(this);

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
    createArrayWidget();
    createYPRWidget();
    createAGCWidget();
    createTranscWidget();
    createSPDWidget();

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
    vLayout->addWidget(m_comPortCB);
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

void CentralWidget::createDistWidget()
{

    m_sbStrtPosCh = new QSpinBox(this);
    m_sbStrtPosCh->setRange(0, 5000); //mm
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

    QVBoxLayout *vLayoutCh = new QVBoxLayout;
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
    m_sbEndPosArr->setRange(1000, 10000); //mm
    m_sbMaxCountArr = new QSpinBox(this);
    m_sbMaxCountArr->setRange(1, 200);
    m_sbThrRiseArr = new QSpinBox(this);
    m_sbThrRiseArr->setRange(0, 1000);
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

