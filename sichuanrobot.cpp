#include "sichuanrobot.h"
#include "ui_sichuanrobot.h"

#include <QDebug>
#include <QTcpSocket>
#include <QNetworkProxy>
#include <QTimer>

#define CheckResult(a)  if(!(a)) \
  return;

SichuanRobot::SichuanRobot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SichuanRobot)
{
    ui->setupUi(this);

    ui->lineEdit_CardMachineIp->setText("192.168.101.98");
    ui->lineEdit_CardMachinePort->setText("50001");
    m_timer = new QTimer();

    ui->lineEdit_CardReaderIp->setText("192.168.1.136");
    ui->lineEdit_CardReaderPort->setText("21003");
    m_timerCardReader = new QTimer();
}

SichuanRobot::~SichuanRobot()
{
    delete m_pCardMachineSocket;
    m_pCardMachineSocket = nullptr;
    delete m_timer;
    m_timer = nullptr;

    delete m_pCardReaderSocket;
    m_pCardReaderSocket = nullptr;
    delete m_timerCardReader;
    m_timerCardReader = nullptr;

    delete ui;
}

void SichuanRobot::on_pushButton_connect_clicked()
{
    if(m_strCardMachineIp.isEmpty() || m_strCardMachinePort.isEmpty())
    {
        qDebug() << __LINE__ << __FUNCTION__ << "卡机 Ip or Port is empty!";
        return;
    }

    if(m_pCardMachineSocket == nullptr)
    {
        m_pCardMachineSocket = new QTcpSocket(this);
        connect(m_pCardMachineSocket, &QTcpSocket::readyRead, this, &SichuanRobot::slotRecv);
        connect(m_pCardMachineSocket, &QTcpSocket::connected, this, &SichuanRobot::slotConnected);
        connect(m_timer, &QTimer::timeout, this, &SichuanRobot::SlotSendHeartBeat);
        m_timer->start(10000);
    }

    if(!m_bConnect)
    {
        m_pCardMachineSocket->setProxy(QNetworkProxy::NoProxy);
        m_pCardMachineSocket->connectToHost(QHostAddress(m_strCardMachineIp), quint16(m_strCardMachinePort.toInt()));
    }
}

void SichuanRobot::on_pushButton_C1_clicked()
{
    QByteArray bytes = "20230523101400";
    sendMsg(SiChuanDevCmdType::CMD_TYPE_CardMachine_C1, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_C2_clicked()
{
    QByteArray bytes = "1";
    sendMsg(SiChuanDevCmdType::CMD_TYPE_CardMachine_C2, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_C3_clicked()
{
    QByteArray bytes = "11";
    sendMsg(SiChuanDevCmdType::CMD_TYPE_CardMachine_C3, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_C4_clicked()
{
    QByteArray bytes = "11";
    sendMsg(SiChuanDevCmdType::CMD_TYPE_CardMachine_C4, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_C5_clicked()
{
    QByteArray bytes = "11";
    sendMsg(SiChuanDevCmdType::CMD_TYPE_CardMachine_C5, bytes.data(), bytes.size());
}

void SichuanRobot::slotRecv()
{
    CheckResult(m_pCardMachineSocket);

    QByteArray byteArray = m_pCardMachineSocket->readAll();
    qDebug() << __LINE__ << __FUNCTION__ << "aaaaaaaaa" << byteArray;

    m_dataBuff.insert(m_dataBuff.end(), byteArray.begin(), byteArray.end());
    vector<vector<uint8>> completePackets = ProtocolParse::UnPackBytes_SiChuan(m_dataBuff);

    if(completePackets.size() < 1)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "completePackets count < 1";
        return;
    }

    for(auto &packet : completePackets)
    {
        vector<uint8> packageData(packet);
        //uint8 Cmd = packet.at(1);   //获取指令
        //只保留实际数据
        packet.erase(packet.begin(), packet.begin() + 2);
        packet.erase(packet.end()-2, packet.end());
    }

    ui->textEdit->append("[接收] " + byteArray.toHex(' ').toUpper());
}

void SichuanRobot::slotConnected()
{
    ui->pushButton_connect->setText("已连接");

    QByteArray byteArray;
    byteArray.append("connected");

    m_pCardMachineSocket->write(byteArray);
    m_bConnect = true;
}

void SichuanRobot::on_lineEdit_CardMachineIp_textChanged(const QString &arg1)
{
    m_strCardMachineIp = arg1;
}

void SichuanRobot::on_lineEdit_CardMachinePort_textChanged(const QString &arg1)
{
    m_strCardMachinePort = arg1;
}

void SichuanRobot::SlotSendHeartBeat()
{
    if(m_bConnect)
    {
        QByteArray bytes = "0x01";
        sendMsg(CMD_TYPE_HeartBeat_00, bytes.data(), bytes.size());
    }
}

void SichuanRobot::sendMsg(SiChuanDevCmdType actionType, char *msg, int len)
{
    vector<uint8> vec_data = ProtocolParse::PackBytes_SiChuan(actionType, msg, len);
    QByteArray bytes(reinterpret_cast<const char*>(vec_data.data()), static_cast<int>(vec_data.size()));

    m_pCardMachineSocket->write(bytes);
}

void SichuanRobot::sendMsgCardReader(SiChuanDevCmdType actionType, char *msg, int len)
{
    CheckResult(m_pCardReaderSocket);
    vector<uint8> vec_data = ProtocolParse::PackBytes_SiChuan(actionType, msg, len);
    QByteArray bytes(reinterpret_cast<const char*>(vec_data.data()), static_cast<int>(vec_data.size()));

    m_pCardReaderSocket->write(bytes);
}

void SichuanRobot::on_pushButton_reserve_clicked()
{
    vector<uint8> verData = {0x01,0x02};
    sendMsg(SiChuanDevCmdType::CMD_TYPE_CardMachine_B1, reinterpret_cast<char*>(verData.data()), static_cast<int>(verData.size()));
}

/// 读卡器 ////////////////////////////////////////////
void SichuanRobot::on_lineEdit_CardReaderIp_textChanged(const QString &arg1)
{
    m_strCardReaderIp = arg1;
}

void SichuanRobot::on_lineEdit_CardReaderPort_textChanged(const QString &arg1)
{
    m_strCardReaderPort = arg1;
}

void SichuanRobot::on_pushButton_connect_CardReader_clicked()
{
    if(m_strCardReaderIp.isEmpty() || m_strCardReaderPort.isEmpty())
    {
        qDebug() << __LINE__ << __FUNCTION__ << "读卡器 Ip or Port is empty!";
        return;
    }

    if(m_pCardReaderSocket == nullptr)
    {
        m_pCardReaderSocket = new QTcpSocket(this);
        connect(m_pCardReaderSocket, &QTcpSocket::readyRead, this, &SichuanRobot::slotCardReaderRecv);
        connect(m_pCardReaderSocket, &QTcpSocket::connected, this, &SichuanRobot::slotCardReaderConnected);
        connect(m_timerCardReader, &QTimer::timeout, this, &SichuanRobot::SlotCardReaderSendHeartBeat);
        m_timerCardReader->start(10000);
    }

    if(!m_bConnectCardReader)
    {
        m_pCardReaderSocket->setProxy(QNetworkProxy::NoProxy);
        m_pCardReaderSocket->connectToHost(QHostAddress(m_strCardReaderIp), quint16(m_strCardReaderPort.toInt()));
    }
}

void SichuanRobot::on_pushButton_OpenCad_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_C1, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_CloseCard_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_C2, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_FindCard_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_C3, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_CPCbaseInfo_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_C4, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_ReadCPC_EF01_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_C5, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_ReadCPC_EF02_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_C6, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_ReadCPC_EF04_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_C7, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_UpdateCPC_EF01_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_C8, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_UpdateCPC_EF02_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_C9, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_UpdateCPC_EF04_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_CA, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_ReadETCinfo_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_CB, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_UpdateETCinfo_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_CC, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_GetTAC_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_CD, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_GetdAuthorizInfo_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_CE, bytes.data(), bytes.size());
}

void SichuanRobot::on_pushButton_ExecOnlineAuthoriz_clicked()
{
    QByteArray bytes = "11";
    sendMsgCardReader(SiChuanDevCmdType::CMD_TYPE_CardMachine_CF, bytes.data(), bytes.size());
}

void SichuanRobot::slotCardReaderRecv()
{
    CheckResult(m_pCardReaderSocket);

    QByteArray byteArray = m_pCardReaderSocket->readAll();
    m_dataBuff.insert(m_dataBuff.end(), byteArray.begin(), byteArray.end());
    vector<vector<uint8>> completePackets = ProtocolParse::UnPackBytes_SiChuan(m_dataBuff);

    if(completePackets.size() < 1)
    {
        qDebug() << __LINE__ << __FUNCTION__ << "completePackets count < 1";
        return;
    }

    for(auto &packet : completePackets)
    {
        vector<uint8> packageData(packet);
        //uint8 Cmd = packet.at(1);   //获取指令
        //只保留实际数据
        packet.erase(packet.begin(), packet.begin() + 2);
        packet.erase(packet.end()-2, packet.end());
    }

    ui->textEdit_CardReader->append("[接收] " + byteArray.toHex(' ').toUpper());
}

void SichuanRobot::slotCardReaderConnected()
{
    CheckResult(m_pCardReaderSocket);

    ui->pushButton_connect_CardReader->setText("已连接");

    QByteArray byteArray;
    byteArray.append("connected");
    m_pCardReaderSocket->write(byteArray);

    m_bConnectCardReader = true;
}

void SichuanRobot::SlotCardReaderSendHeartBeat()
{
    if(m_bConnectCardReader)
    {
        QByteArray bytes = "0x00";
        sendMsgCardReader(CMD_TYPE_HeartBeat_00, bytes.data(), bytes.size());
    }
}
