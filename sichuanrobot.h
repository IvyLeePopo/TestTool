#ifndef SICHUANROBOT_H
#define SICHUANROBOT_H

#include <QWidget>

#include "Common/protocol_parse.h"

namespace Ui {
class SichuanRobot;
}

class QTcpSocket;
class QTimer;

class SichuanRobot : public QWidget
{
    Q_OBJECT

public:
    explicit SichuanRobot(QWidget *parent = nullptr);
    ~SichuanRobot();

    // 通讯的槽函数
public slots:
    // 接收数据
    void slotRecv();

    // tcp 已连接信号
    void slotConnected();

private slots:
    void on_pushButton_connect_clicked();

    void on_pushButton_C1_clicked();

    void on_pushButton_C2_clicked();

    void on_pushButton_C3_clicked();

    void on_pushButton_C4_clicked();

    void on_pushButton_C5_clicked();

    void on_lineEdit_CardMachineIp_textChanged(const QString &arg1);

    void on_lineEdit_CardMachinePort_textChanged(const QString &arg1);

    void SlotSendHeartBeat();

    void on_pushButton_reserve_clicked();

    // 读卡器
    void on_lineEdit_CardReaderIp_textChanged(const QString &arg1);

    void on_lineEdit_CardReaderPort_textChanged(const QString &arg1);

    void on_pushButton_connect_CardReader_clicked();

    void on_pushButton_OpenCad_clicked();

    void on_pushButton_CloseCard_clicked();

    void on_pushButton_FindCard_clicked();

    void on_pushButton_CPCbaseInfo_clicked();

    void on_pushButton_ReadCPC_EF01_clicked();

    void on_pushButton_ReadCPC_EF02_clicked();

    void on_pushButton_ReadCPC_EF04_clicked();

    void on_pushButton_UpdateCPC_EF01_clicked();

    void on_pushButton_UpdateCPC_EF02_clicked();

    void on_pushButton_UpdateCPC_EF04_clicked();

    void on_pushButton_ReadETCinfo_clicked();

    void on_pushButton_UpdateETCinfo_clicked();

    void on_pushButton_GetTAC_clicked();

    void on_pushButton_GetdAuthorizInfo_clicked();

    void on_pushButton_ExecOnlineAuthoriz_clicked();

    // 通讯的槽函数
    // 接收数据
    void slotCardReaderRecv();

    // tcp 已连接信号
    void slotCardReaderConnected();

    void SlotCardReaderSendHeartBeat();

private:
    void sendMsg(SiChuanDevCmdType actionType, char *msg, int len);
    void sendMsgCardReader(SiChuanDevCmdType actionType, char *msg, int len);

private:
    Ui::SichuanRobot *ui;

    // 卡机
    QString m_strCardMachineIp;
    QString m_strCardMachinePort;
    QTcpSocket* m_pCardMachineSocket = nullptr;
    bool m_bConnect = false;
    QTimer* m_timer = nullptr;

    vector<uint8> m_dataBuff;

    // 读卡器
    QString m_strCardReaderIp;
    QString m_strCardReaderPort;
    QTcpSocket* m_pCardReaderSocket = nullptr;
    bool m_bConnectCardReader = false;
    QTimer* m_timerCardReader = nullptr;
    vector<uint8> m_dataBuffCardReader;

};

#endif // SICHUANROBOT_H
