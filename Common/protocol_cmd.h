
#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <QString>
#include <string>
#include <string.h>

using namespace std;

typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef unsigned long long      uint64;

//四川用(TCP)
enum SICHUAN_PROT_CONST
{
    SICHUAN_PROT_HEADER	= 0xFF,         //协议起始符
    SICHUAN_PROT_END = 0xFF,            //协议结束符
    SICHUAN_PROT_CONVERT_00 = 0x00,     //协议转译公共字符
    SICHUAN_PROT_CONVERT_FE	= 0xFE,     //协议起始符转译字符1
    SICHUAN_PROT_CONVERT_01	= 0x01,     //协议起始符转译字符2
};

enum SiChuanDevCmdType
{
    //心跳
    CMD_TYPE_HeartBeat_00 = 0x00,
    //回复指令（卡机）
    CMD_TYPE_CardMachine_B1 = 0xB1,     //控制指令执行结果
    CMD_TYPE_CardMachine_B2 = 0xB2,     //入口卡机状态
    CMD_TYPE_CardMachine_B3 = 0xB3,     //入口卡机事件
    CMD_TYPE_CardMachine_B4 = 0xB4,     //出口卡机状态
    CMD_TYPE_CardMachine_B5 = 0xB5,     //出口卡机事件
    CMD_TYPE_CardMachine_B6 = 0xB6,     //出票状态
    CMD_TYPE_CardMachine_B7 = 0xB7,     //扫码状态
    CMD_TYPE_CardMachine_B8 = 0xB8,     //语音求助对讲

    //请求指令（卡机）
    CMD_TYPE_CardMachine_C1 = 0xC1,     //时间同步
    CMD_TYPE_CardMachine_C2 = 0xC2,     //设置黄闪报警
    CMD_TYPE_CardMachine_C3 = 0xC3,     //入口卡机控制
    CMD_TYPE_CardMachine_C4 = 0xC4,     //出口卡机控制
    CMD_TYPE_CardMachine_C5 = 0xC5,     //出票控制

    //请求指令（读卡器）
    CMD_TYPE_CardReader_C1 = 0xC1,     //打开读卡器
    CMD_TYPE_CardReader_C2 = 0xC2,     //关闭读卡器
    CMD_TYPE_CardReader_C3 = 0xC3,     //开始寻卡
    CMD_TYPE_CardReader_C4 = 0xC4,     //读CPC基础信息
    CMD_TYPE_CardReader_C5 = 0xC5,     //读CPC EF01信息
    CMD_TYPE_CardReader_C6 = 0xC6,     //读CPC EF02信息
    CMD_TYPE_CardReader_C7 = 0xC7,     //读CPC EF04信息
    CMD_TYPE_CardReader_C8 = 0xC8,     //更新CPC EF01信息
    CMD_TYPE_CardReader_C9 = 0xC9,     //更新CPC EF02信息
    CMD_TYPE_CardReader_CA = 0xCA,     //更新CPC EF04信息
    CMD_TYPE_CardReader_CB = 0xCB,     //读ETC信息
    CMD_TYPE_CardReader_CC = 0xCC,     //更新ETC信息
    CMD_TYPE_CardReader_CD = 0xCD,     //重取TAC
    CMD_TYPE_CardReader_CF = 0xCF,     //取授权信息
    CMD_TYPE_CardReader_CE = 0xCE,     //执行在线授权
};

