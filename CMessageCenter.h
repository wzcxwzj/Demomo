#ifndef CMESSAGECENTER_H
#define CMESSAGECENTER_H
#include <QObject>
#include <qjsondocument.h>
#include <QJsonArray>
#include <QJsonObject>
#include "CNetWorkForServer.h"
#include "CNetWorkForClients.h"
#include "CNetWorkInterface.h"
#include "CNetWorkForClientsUdp.h"
#include "CNetWorkForUdpServer.h"
#include "SocketIOClient.h"
#include "Rpc_Proxy.h"
#include "notifymanager.h"

class CMessageCenter :public QObject, public CNetWorkInterface
{
    Q_OBJECT
public:
    static CMessageCenter *getInstance();
    void send(QString strMessage);
    void sendPermissionServer(QString strMessage);

private:
     CMessageCenter(QObject *parent = 0);
public:
    void dataArrived(QByteArray arrMessage,QString strIp,int nPort);
    void connectStateChanged(bool bState, QString strIp, int nPort);
    void dataArrived(QByteArray arrMessage);
signals:
    void sigMsgRetInNodeList(QJsonObject obj);//信号
    void sigMsgRetUserList(QJsonObject obj);//用户
    void sigMsgRetRooms(QJsonObject obj);//群组
    void sigMsgScreenShowConf(QJsonObject obj);//窗口
    void sigMsgUpdateVideo(QByteArray arraryData);//图片
    void sigMsgRetLogin(QJsonObject obj);//登陆
    void sigMsgRetScenes(QJsonObject obj);//场景
    void sigMsgRecvCustomMsg(QJsonObject obj);//对话消息
    void sigMsgRecvSceneLinkage(QJsonObject obj);//收到场景联动消息
    void sigDisconnect(bool);
    void sigMsgPlantsInit(QJsonObject obj);//预案
    void sigMsgPlantManagersInit(QJsonObject obj);//预案管理
public slots:
private:
    QString m_strServerIp;
    int m_nServerPort;
    int m_nLocalPort;
    int m_nChangeServerPort;
    CNetWorkForClientsUdp *m_pDataNetWrok;
    SocketIOClient *m_pDataSocketIOClient;
public:
    Rpc_Proxy *m_Rpc;
    NotifyManager *m_NotifyManager;

};

#endif // CMESSAGECENTER_H
