/*
 * @Description: 通讯协议相关的接口
 * @Author: Ivy
 * demo :
 * @Date: 2022-05-08 09:34:43
 * @LastEditTime: 2022-05-08 13:39:52
 * @LastEditors: XTZJ-2022OQEHLZ
 */

/** 基础帧数据结构定义
	设备通讯之间的的数据帧格式分两种，一种是有正常数据，一种是空数据域。
	正常：STX	LEN	SN	TOPCMD	SUBCMD	DATA	VERIFY	ETX
	空数据：STX	LEN	SN	TOPCMD	SUBCMD	VERIFY	ETX
*/

#pragma once 
#include <QByteArray>

#include "protocol_cmd.h"

class ProtocolParse
{
public:
	ProtocolParse();

public:
    // packet打包
    static vector<uint8> PackBytes_SiChuan(SiChuanDevCmdType actionType, char *msg, int len);

    // unpacket解包
    static vector<vector<uint8>> UnPackBytes_SiChuan(vector<uint8> &dataBuff);

    // 数据处理
    static uint8 XORVerifyArithmetic_SiChuan(vector<uint8>& Verify);    //异或校验
    static vector<uint8> unEscapeComProtocol_SiChuan(vector<uint8> &lpData, uint32 len, uint32 bSendState);   //转译，bSendState：1发送，0接收
};
