#ifndef URBISEND_H
#define URBISEND_H

#include <QObject>
#include <QTcpSocket>
#include <iostream>
#include <sstream>
#include <string>

#include "gamedata.h"
#include "messages.h"
#include "bezierclass.h"

class UrbiSend : public QObject
{
    Q_OBJECT

public:
    UrbiSend(GameData &dataIn);
    ~UrbiSend();

signals:
    void finished();
    void error(QString err);

public slots:
    void start();
    void socketStatusChanged(QAbstractSocket::SocketState state);
    void dataForReading();
    void dataForWriting(QString sMsgIn);

private:
    void connectSignalsToSlots();
    void disconnectFromServer();
    void connectToServer(std::string sHostIP, int iHostPort);
    QString generateResponse(QList<QString> sDataIn);
    void sendMessage(QString sMsg);
    QString constructResponse(QString sMsgIn);

    GameData* gameData;
    QTcpSocket *socket;
    BezierClass* clsBezier;
    bool isRunning;
    int miPort;
    QFile* logFile;
};

#endif // URBISEND_H
