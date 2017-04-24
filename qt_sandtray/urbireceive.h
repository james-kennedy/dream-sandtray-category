#ifndef URBIRECEIVE_H
#define URBIRECEIVE_H

#include <QObject>
#include <QTcpSocket>
#include <iostream>
#include <sstream>
#include <string>

#include "gamedata.h"
#include "librarymanager.h"
#include "messages.h"
#include "bezierclass.h"

class UrbiReceive : public QObject
{
    Q_OBJECT

public:
    UrbiReceive(GameData &dataIn, LibraryManager &libIn);
    ~UrbiReceive();

signals:
    void finished();
    void error(QString err);

public slots:
    void start();
    void socketStatusChanged(QAbstractSocket::SocketState state);
    void dataForReading();

private:
    void connectSignalsToSlots();
    void disconnectFromServer();
    void connectToServer(std::string sHostIP, int iHostPort);
    QString generateResponse(QList<QString> sDataIn);
    void sendMessage(QString sMsg);

    GameData* gameData;
    LibraryManager* libManager;
    QTcpSocket* socket;
    BezierClass* clsBezier;
    bool isRunning;
    int miPort;
};

#endif // URBIRECEIVE_H
