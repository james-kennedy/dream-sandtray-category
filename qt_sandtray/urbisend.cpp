#include "urbisend.h"

UrbiSend::UrbiSend(GameData &dataIn)
{
    socket = new QTcpSocket(this);
    gameData = &dataIn;
    connectSignalsToSlots();
    clsBezier = new BezierClass(*gameData);
    isRunning = false;
    miPort = 86200;

    qint64 iTimeNow = QDateTime::currentMSecsSinceEpoch();
    QString sFilename = "Data-" + QString::number(iTimeNow) + ".txt";
    logFile = new QFile(sFilename);
    logFile->open(QIODevice::WriteOnly | QIODevice::Text);
}

UrbiSend::~UrbiSend()
{
    if (isRunning)
        disconnectFromServer();

    logFile->close();
}

void UrbiSend::connectSignalsToSlots()
{
    QObject::connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                     this,SLOT(socketStatusChanged(QAbstractSocket::SocketState)));

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(dataForReading()));

    QObject::connect(gameData, SIGNAL(readyWrite(QString)), this, SLOT(dataForWriting(QString)));
}

void UrbiSend::start()
{
    std::string sServer = gameData->getServerIP().toStdString();

    // try to connect to server
    while(!isRunning)
        connectToServer(sServer, miPort);

    // send confirmation to Urbi
    int iImagesLeft = clsBezier->getNumberOfImagesRemaining();
    sendMessage(_GREET_ + "," + QString::number(iImagesLeft));

    // now rely on signals and slots to go to DataForReading and DataForWriting
}

void UrbiSend::connectToServer(std::string sHostIP, int iHostPort)
{
    if (!isRunning)
    {
        socket->connectToHost(sHostIP.c_str(), iHostPort);
        socket->waitForConnected(200);
    }
}

void UrbiSend::socketStatusChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::ConnectedState)
    {
        qDebug() << "Send socket connected";
        isRunning = true;
    }
    else if (state == QAbstractSocket::UnconnectedState)
    {
        //qDebug() << "Send socket disconnected";
        isRunning = false;
    }
    else if (state == QAbstractSocket::ConnectingState)
    {
        //qDebug() << "Send socket connecting";
        isRunning = false;
    }
}

void UrbiSend::dataForWriting(QString sMsgIn)
{
    // check that we have a socket connected and send out on it
    if (isRunning)
    {
        QString sMsgToSend = constructResponse(sMsgIn);
        sendMessage(sMsgToSend);
    }
}

// data in comes here from socket - connected using signal/slot
void UrbiSend::dataForReading()
{
    QString data = socket->readAll();
    QList<QString> dataList = data.split(",");
    QString sResponse = generateResponse(dataList);
    sendMessage(sResponse);
}

QString UrbiSend::generateResponse(QList<QString> sDataIn)
{
    if (sDataIn.count() < 2)
        return _FAILURE_;
    else
    {
        QString sFirstSlot = sDataIn[0];

        if (sFirstSlot == _VERIFY_)
            return _CONFIRM_;
        else if (sFirstSlot == _SHUTDOWN_)
            return _EXIT_;
        else
            return "";
    }
}

// a big switch on the Msg in which then gets the bits from gameData and puts together an actual string to send
QString UrbiSend::constructResponse(QString sMsgIn)
{
    QString sResponse = "";
    int iImagesLeft = clsBezier->getNumberOfImagesRemaining();

    if (sMsgIn == _PLAYER_MOVE_)
    {
        int iImageId = gameData->getLastImageCategorised();
        QString sProps = gameData->getImagePropertiesById(iImageId);
        float flDelay = gameData->getLastDelay();
        float flSpeed = gameData->getLastSpeed();
        int iCorrectCat = gameData->getCatPlaced(iImageId);
        QString sCatProps = gameData->getCategoryPropertiesById(iCorrectCat);

        QString sEnd = "," + QString::number(iImagesLeft);
        sEnd += "," + QString::number(flDelay);
        sEnd += "," + QString::number(flSpeed);
        sEnd += "," + sProps;
        sEnd += "," + sCatProps;

        if (gameData->getLastCategorisationCorrect())
            sResponse = _PLAYER_DONE_GOOD_MOVE_ + sEnd;
        else
            sResponse = _PLAYER_DONE_BAD_MOVE_ + sEnd;

        QTextStream outStream(logFile);
        QString sTimestamp = QString::number(QDateTime::currentMSecsSinceEpoch());
        outStream << sTimestamp << "-" << sResponse + "\n";
        outStream.flush();
    }
    else if (sMsgIn == _ROBOT_MOVE_)
    {
        int iImageId = gameData->getLastRobotMoveId();
        QString sProps = gameData->getImagePropertiesById(iImageId);

        QString sEnd = "," + QString::number(iImagesLeft);
        sEnd += "," + sProps;

        if (gameData->getLastRobotMoveCorrect())
            sResponse = _ROBOT_DONE_GOOD_MOVE_ + sEnd;
        else
            sResponse = _ROBOT_DONE_BAD_MOVE_ + sEnd;
    }
    else if (sMsgIn == _PLAYER_NEW_GAME_)
    {
        sResponse = _NEW_GAME_ + ",";
        sResponse += QString::number(iImagesLeft) + ",";
        sResponse += gameData->getLibraryProperties() + ",";
        sResponse += QString::number(gameData->getNumberOfCats()) + ",";
        sResponse += gameData->getAllCategoryProperties();
    }
    else if (sMsgIn == _PLAYER_RESET_GAME_)
    {
        sResponse = _RESET_BOARD_ + ",";
        sResponse += QString::number(iImagesLeft) + ",";
        sResponse += gameData->getLibraryProperties() + ",";
        sResponse += QString::number(gameData->getNumberOfCats()) + ",";
        sResponse += gameData->getAllCategoryProperties();
    }
    else if (sMsgIn.contains(_PLAYER_TOUCH_IMAGE_))
    {
        sResponse = sMsgIn;     // here we have cheated and already attached the payload (image ID) before it arrives here
    }
    else if (sMsgIn.contains(_PLAYER_RELEASE_IMAGE_))
    {
        sResponse = sMsgIn;     // here we have cheated and already attached the payload (image ID) before it arrives here
    }

    return sResponse;
}

void UrbiSend::sendMessage(QString sMsg)
{
    if (isRunning)
    {
        if (sMsg != "")
        {
            qDebug() << "Event Sending: " << sMsg.toStdString().c_str();
            socket->write(sMsg.toStdString().c_str());
            socket->flush();
        }
    }
}

void UrbiSend::disconnectFromServer()
{
    qDebug() << "Event disconnected";
    sendMessage(_EXIT_);
    socket->disconnectFromHost();
    socket->waitForDisconnected();
    isRunning = false;
    emit finished();
}
