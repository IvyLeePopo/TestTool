#include "protocol_parse.h"
#include <QByteArray>
#include <QTextCodec>
#include <QDebug>

#include "protocol_cmd.h"

ProtocolParse::ProtocolParse()
{
}

vector<uint8> ProtocolParse::PackBytes_SiChuan(SiChuanDevCmdType actionType, char *msg, int len)
{
    vector<uint8> bytes(msg, msg+len);
    vector<uint8> object;

    // Data[0]:包头
    object.push_back(SICHUAN_PROT_HEADER);
    // Data[1～2]:一级指令
    object.push_back(static_cast<uint8>(actionType));
    if(bytes.size() != 0)
    {
        object.insert(object.end(),bytes.begin(),bytes.end());
    }

    //校验码
    uint8 xorData = XORVerifyArithmetic_SiChuan(bytes);
    object.push_back(xorData);

    //包尾
    object.push_back(SICHUAN_PROT_END);

    QByteArray byte111(reinterpret_cast<const char*>(object.data()), static_cast<int>(object.size()));
    qDebug() << __LINE__ << __FUNCTION__ << "PackBytes 转义前：" << byte111.toHex().toUpper();
    vector<uint8> ObjectBytes = unEscapeComProtocol_SiChuan(object, static_cast<uint32>(object.size()), 1);

    QByteArray byte(reinterpret_cast<const char*>(ObjectBytes.data()), static_cast<int>(ObjectBytes.size()));
    qDebug() << __LINE__ << __FUNCTION__ << "PackBytes 转义后：" << byte.toHex().toUpper();

    return ObjectBytes;

}

vector<vector<uint8> > ProtocolParse::UnPackBytes_SiChuan(vector<uint8> &dataBuff)
{
    vector<vector<uint8>> completePackets;
    while(dataBuff.size() > 1)
    {
        QByteArray bytes1111(reinterpret_cast<const char*>(dataBuff.data()), static_cast<int>(dataBuff.size()));
        qDebug() << __LINE__ << __FUNCTION__ << "刚传进来的数据:" << bytes1111.toHex().toUpper();

        //1.将无法找回的数据删除
        //数据开头如果不是0xFF，则将数据FF前的全部删除，如果整段数据都没有0xFF则整段删除
        if(dataBuff[0] != SICHUAN_PROT_HEADER)
        {
            qDebug() << __LINE__ << __FUNCTION__ << "111111111:" ;

            auto iter = find(dataBuff.begin(), dataBuff.end(), SICHUAN_PROT_HEADER);
            qDebug() << __LINE__ << __FUNCTION__ << "222222222:" ;

            if (iter == dataBuff.end())
            {
                //dataBuff中没再找到0xFF,清除整个buff
                qDebug() << __LINE__ << __FUNCTION__ << "33333333:" ;
                dataBuff.clear();
            }
            else
            {
                //如果找到了，则删除0xFF前的部分
                qDebug() << __LINE__ << __FUNCTION__ << "44444444:" ;
                dataBuff.erase(dataBuff.begin(), iter);
            }

            qDebug() << __LINE__ << __FUNCTION__ << "5555555555:" ;
            continue;
        }

        //2.跳过不完整的数据（4字节为包头+指令+检验码+包尾）
        if(dataBuff.size() < 4)
        {
            break;
        }

        //3.取出一包数据
        vector<uint8> srcData;
        //3.1.查找包尾0xFF,从包头后一位开始查找
        auto end_it = find(dataBuff.begin()+1, dataBuff.end(), SICHUAN_PROT_END);
        if(end_it != dataBuff.end())
        {
            //如果找到了
            std::advance(end_it, 1);
            srcData.insert(srcData.end(), dataBuff.begin(), end_it);
            //删除包头后面到包尾0xFF的数据
            dataBuff.erase(dataBuff.begin(), end_it);
        }
        else    //数据未接收完，只有包头没有包尾
        {
            QByteArray bytes(reinterpret_cast<const char*>(dataBuff.data()), static_cast<int>(dataBuff.size()));
            qDebug() << __LINE__ << __FUNCTION__ << "surplus data:" << bytes.toHex().toUpper();
            break;
        }

        //4.数据转译
        vector<uint8> objectData = unEscapeComProtocol_SiChuan(srcData, static_cast<uint32>(srcData.size()), 0);

        if(objectData.size() < 4)
            continue;

        //取DATA段数据，用于判断校验
        vector<uint8> packetBuf(objectData.begin()+2, objectData.end()-2);

        //5.数据校验,判断这条数据是否正确
        uint8 srcXOR = objectData.at(objectData.size()-2);
        uint8 resXOR = XORVerifyArithmetic_SiChuan(packetBuf);
        if(srcXOR != resXOR)
        {
            qDebug() << __LINE__ << __FUNCTION__ << "XOR校验错误" << srcXOR << resXOR;
        }
        else
        {
            completePackets.push_back(objectData);
        }
    }
    return completePackets;
}

uint8 ProtocolParse::XORVerifyArithmetic_SiChuan(vector<uint8> &Verify)
{
    uint8 result=0;
    for (uint32 i = 0; i < Verify.size(); ++i)
    {
        result ^= Verify[i];
    }
    return result;
}

vector<uint8> ProtocolParse::unEscapeComProtocol_SiChuan(vector<uint8> &lpData, uint32 len, uint32 bSendState)
{
    QByteArray t_lpData;
    vector<uint8> verData;
    lpData.resize(len+1);

    t_lpData[0]=static_cast<uint8>(SICHUAN_PROT_HEADER); //设置帧头
    uint32 iIndex = 1;  //处理数据帧其它区域

    for(quint32 i = 1; i < (len-1); i++)
    {
        if(bSendState==0)//接收数据
        {
            if(lpData[i] == SICHUAN_PROT_CONVERT_FE  && lpData[i+1] == SICHUAN_PROT_CONVERT_01)
            {
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_HEADER);
                i++;
            }
            else if(lpData[i]==SICHUAN_PROT_CONVERT_FE && lpData[i+1]==SICHUAN_PROT_CONVERT_00)
            {
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_CONVERT_FE);
                i++;
            }
            else if(lpData[i]==SICHUAN_PROT_CONVERT_FE && lpData[i+1]==SICHUAN_PROT_CONVERT_01)
            {
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_END);
                i++;
            }
            else if(lpData[i]==SICHUAN_PROT_CONVERT_FE && lpData[i+1]==SICHUAN_PROT_CONVERT_00)
            {
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_CONVERT_FE);
                i++;
            }
            else
            {
                t_lpData[iIndex++] = lpData[i];
            }
        }
        else		//发送数据
        {
            if(SICHUAN_PROT_HEADER == lpData[i])
            {
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_CONVERT_FE);
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_CONVERT_01);
            }
            else if(SICHUAN_PROT_CONVERT_FE == lpData[i])
            {
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_CONVERT_FE);
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_CONVERT_00);
            }
            else if(SICHUAN_PROT_END == lpData[i])
            {
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_CONVERT_FE);
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_CONVERT_01);
            }
            else if(SICHUAN_PROT_CONVERT_FE == lpData[i])
            {
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_CONVERT_FE);
                t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_CONVERT_00);
            }
            else
            {
                t_lpData[iIndex++]=lpData[i];
            }
        }
    }
    //设置帧尾
    t_lpData[iIndex++] = static_cast<uint8>(SICHUAN_PROT_END);

    verData.insert(verData.end(), t_lpData.begin(), t_lpData.end());
    return verData;
}
