#include "urbireceive.h"

UrbiReceive::UrbiReceive(GameData &dataIn, LibraryManager &libIn)
{
    socket = new QTcpSocket(this);
    gameData = &dataIn;
    libManager = &libIn;
    connectSignalsToSlots();
    clsBezier = new BezierClass(*gameData);
    isRunning = false;
    miPort = 86000;
}

UrbiReceive::~UrbiReceive()
{
    if (isRunning)
        disconnectFromServer();
}

void UrbiReceive::connectSignalsToSlots()
{
    QObject::connect(socket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),
                     this,SLOT(socketStatusChanged(QAbstractSocket::SocketState)));

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(dataForReading()));
}

void UrbiReceive::start()
{
    std::string sServer = gameData->getServerIP().toStdString();

    // loop here until we connect to server
    while(!isRunning)
        connectToServer(sServer, miPort);

    // confirm to Urbi, which is waiting for this
    sendMessage(_GREET_);

    // now we are connected and have confirmed, we wait and see what we get in - done by signals/slots to DataForReading()
}

void UrbiReceive::connectToServer(std::string sHostIP, int iHostPort)
{
    if (!isRunning)
    {
        socket->connectToHost(sHostIP.c_str(), iHostPort);
        socket->waitForConnected(200);
    }
}

void UrbiReceive::socketStatusChanged(QAbstractSocket::SocketState state)
{
    if (state == QAbstractSocket::ConnectedState)
    {
        qDebug() << "Receive socket connected";
        isRunning = true;
    }
    else if (state == QAbstractSocket::UnconnectedState)
    {
        isRunning = false;
    }
    else if (state == QAbstractSocket::ConnectingState)
    {
        isRunning = false;
    }
}

// data in comes here from socket - connected using signal/slot
void UrbiReceive::dataForReading()
{
    QString data = socket->readAll();
    data = data.simplified();               // get the string and convert all whitespace to single spaces
    data = data.replace(" ","");            // strip any spaces
    data = data.remove(QRegExp("\""));       // strip any quotes
    qDebug() << "Data received: " << data;

    QList<QString> dataList = data.split(",");
    QString sResponse = generateResponse(dataList);
    sendMessage(sResponse);
}

QString UrbiReceive::generateResponse(QList<QString> sDataIn)
{
    if (sDataIn.count() < 2)
    {
        return _FAILURE_;
    }
    else
    {
        QString sFirstSlot = sDataIn[0];

        if (sFirstSlot == _VERIFY_)
        {
            return _CONFIRM_;
        }
        else if (sFirstSlot == _SHUTDOWN_)
        {
            qApp->exit(50307);    // exit with robot
            return _EXIT_;
        }
        else if (sFirstSlot == _FAILURE_)
        {
            return _FAIL_;
        }
        else if (sFirstSlot == _NEW_GAME_)
        {
            if (gameData->getAnyImagesOwned())
                return _FAIL_;
            else
            {
                gameData->setNewGame();

                // this is designed so that we don't progress until the new library is loaded
                QEventLoop libWaitLoop;
                connect(libManager, SIGNAL(libraryLoaded()), &libWaitLoop, SLOT(quit()));
                libWaitLoop.exec();

                int iImagesLeft = clsBezier->getNumberOfImagesRemaining();

                // return library properties, number of categories and properties for all categories
                QString sResponse = "";
                sResponse = _NEW_GAME_ + ",";
                sResponse += QString::number(iImagesLeft) + ",";
                sResponse += gameData->getLibraryProperties() + ",";
                sResponse += QString::number(gameData->getNumberOfCats()) + ",";
                sResponse += gameData->getAllCategoryProperties();

                return sResponse;
            }
        }
        else if (sFirstSlot == _RESET_BOARD_)
        {
            if (gameData->getAnyImagesOwned())
                return _FAIL_;
            else
            {
                gameData->setResetGame();

                // this is designed so that we don't progress until the new library is loaded
                QEventLoop libWaitLoop;
                connect(libManager, SIGNAL(libraryLoaded()), &libWaitLoop, SLOT(quit()));
                libWaitLoop.exec();

                int iImagesLeft = clsBezier->getNumberOfImagesRemaining();

                // return library properties, number of categories and properties for all categories
                QString sResponse = "";
                sResponse = _RESET_BOARD_ + ",";
                sResponse += QString::number(iImagesLeft) + ",";
                sResponse += gameData->getLibraryProperties() + ",";
                sResponse += QString::number(gameData->getNumberOfCats()) + ",";
                sResponse += gameData->getAllCategoryProperties();

                return sResponse;
            }
        }
        else if (sFirstSlot == _SPECIFIED_LEVEL_)
        {
            if (gameData->getAnyImagesOwned())
                return _FAIL_;
            else
            {
                int iLib = sDataIn[1].toInt();

                if (iLib >= 0)
                {
                    // prevent from specifying library outside range - game will crash!
                    if (!gameData->getLibraryTestReserved() && iLib > gameData->getLibraryLimit())
                        return _FAIL_;
                    else
                    {
                        gameData->setLibraryId(iLib);
                        gameData->setResetGame();

                        // this is designed so that we don't progress until the new library is loaded
                        QEventLoop libWaitLoop;
                        connect(libManager, SIGNAL(libraryLoaded()), &libWaitLoop, SLOT(quit()));
                        libWaitLoop.exec();

                        int iImagesLeft = clsBezier->getNumberOfImagesRemaining();

                        // return library properties, number of categories and properties for all categories
                        QString sResponse = "";
                        sResponse = _RESET_BOARD_ + ",";
                        sResponse += QString::number(iImagesLeft) + ",";
                        sResponse += gameData->getLibraryProperties() + ",";
                        sResponse += QString::number(gameData->getNumberOfCats()) + ",";
                        sResponse += gameData->getAllCategoryProperties();

                        return sResponse;
                    }
                }
                else
                    return _FAIL_;
            }
        }
        else if (sFirstSlot == _SET_BUTTONS_)
        {
            if (sDataIn[1].toInt() == 0)
                gameData->setButtonsActive(false);
            else
                gameData->setButtonsActive(true);

            return _CONFIRM_;
        }
        else if (sFirstSlot == _READY_)
        {
            gameData->setForceScreenUpdate(true);
            gameData->setRobotReadyToMove(true);    // should start the move
            gameData->setRobotMoveStart(QDateTime::currentMSecsSinceEpoch());
            return _CONFIRM_;
        }
        else if (sFirstSlot == _MOVE_FINISHED_)
        {
            return _CONFIRM_;
        }
        else if (sFirstSlot == _GET_USER_DATA_)
        {
            // p1 score, p1 wrong moves, time between events, time of events, speed of moves, dist of moves
            QString sResponse = _USER_DATA_ + ",";
            sResponse += QString::number(gameData->getNumberPlayerRightMoves()) + ",";
            sResponse += QString::number(gameData->getNumberPlayerWrongMoves()) + ",";
            sResponse += QString::number(gameData->getAverageDelay()) + ",";
            sResponse += QString::number(gameData->getAverageTime()) + ",";
            sResponse += QString::number(gameData->getAverageSpeed()) + ",";
            sResponse += QString::number(gameData->getAverageDistance());

            return sResponse;
        }
        else if (sFirstSlot == _GET_SCREEN_DATA_)
        {
            // speed AI move, current image lib, current scene name (?)
            QString sResponse = _SCREEN_DATA_ + ",";
            sResponse += QString::number(gameData->getRobotSpeed()) + ",";
            sResponse += QString::number(gameData->getLibraryId()) + ",";
            sResponse += gameData->getLibraryProperties();

            return "";
        }
        else if (sFirstSlot == _GET_IMAGE_COORDONATES_)
        {
            int iIdIn = sDataIn[1].toInt();
            QPointF qpfImagePos = gameData->getImagePositionById(iIdIn);

            return _COORDS_ + "," + QString::number(qpfImagePos.x()) + "," + QString::number(qpfImagePos.y());
        }
        else if (sFirstSlot == _GET_BEZIER_DATA_)
        {
            // this is now no longer possible - need to know the type of move - maybe reimplement if needed
            return _FAIL_;
        }
        else if (sFirstSlot == _GET_ID_IMAGE_WILL_MOVE_)
        {
            // this is now no longer possible - need to know the type of move
            return _FAIL_;
        }
        else if (sFirstSlot == _GET_IMAGES_LEFT_)
        {
            int iImagesLeft = clsBezier->getNumberOfUncategorisedImages();
            return _IMAGES_LEFT_ + "," + QString::number(iImagesLeft);
        }
        else if (sFirstSlot == _GET_ID_IMAGES_CAN_MOVE)
        {
            QList<int> iImageList = clsBezier->getActiveImageList();
            QString sResponse = _IMAGES_IDS_ + ",";

            foreach(int iThisId, iImageList)
                sResponse += QString::number(iThisId) + "_";

            sResponse.replace(sResponse.length() - 1, 1, "");       // remove final _

            return sResponse;
        }
        else if (sFirstSlot == _GET_LAST_IMAGE_PROPS_)
        {
            int iLastCatId = gameData->getLastImageCategorised();

            if (iLastCatId >= 0)
            {
                QString sProps = gameData->getImagePropertiesById(iLastCatId);
                return _IMAGE_PROPS_ + "," + sProps;
            }
            else
                return _FAIL_;
        }
        else if (sFirstSlot == _PREPARE_MOVE_)
        {
            int iMoveType = sDataIn[1].toInt();
            bool bCorrect = false;

            if (iMoveType == 20)
                bCorrect = true;

            int iImageToMove = clsBezier->getImageIdToMove(bCorrect, true);

            if (iImageToMove < 0)
                return _MOVE_FAIL_;     // no free image to move

            gameData->setImageOwned(iImageToMove, true);            // lock the image for the robot
            gameData->setRobotOwned(iImageToMove, true);
            QPainterPath ppBezierPath = clsBezier->calculateBezierPath(iImageToMove, bCorrect, true);
            int iTargetCat = gameData->getBezierTargetCat();
            gameData->setRobotMovePath(iImageToMove, ppBezierPath);
            gameData->setRobotReadyToMove(false);                   // hold move until robot is ready
            gameData->setRobotMoving(iImageToMove, true);           // prepare to move

            int iRobotSpeed = gameData->getRobotSpeed();
            int iMoveTimeMs = (ppBezierPath.length() / iRobotSpeed) * 1000;
            gameData->setRobotMoveTimeMs(iImageToMove, iMoveTimeMs);

            // [MESSAGE, image_id, start X, start Y, Speed, bez A X, bez A Y, bez B X, bez B Y, bez C X, bez C Y, bez D X, bez D Y, movetype, movetime, props of image, props of target category]
            QList<QPointF> qpflBezierPoints = gameData->getMovePoints(iImageToMove);
            QSize qpScreen = gameData->getScreenSize();

            // the old game engine used to have origin at top left, we have it at centre, so transform our numbers to match (don't have to change the robot code)
            QString sResponse = "";
            sResponse = _MOVE_DATA_ + "," + QString::number(iImageToMove) + ",";
            sResponse += QString::number(qpflBezierPoints[0].x() + (qpScreen.width() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[0].y() + (qpScreen.height() / 2)) + ",";
            sResponse += QString::number(iRobotSpeed) + ",";
            sResponse += QString::number(qpflBezierPoints[0].x() + (qpScreen.width() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[0].y() + (qpScreen.height() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[1].x() + (qpScreen.width() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[1].y() + (qpScreen.height() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[2].x() + (qpScreen.width() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[2].y() + (qpScreen.height() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[3].x() + (qpScreen.width() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[3].y() + (qpScreen.height() / 2)) + ",";
            sResponse += QString::number(iMoveType) + ",";
            sResponse += QString::number((double)((double)iMoveTimeMs / (double)1000)) + ",";   // convert to secs for robot
            sResponse += gameData->getImagePropertiesById(iImageToMove) + ",";
            sResponse += gameData->getCategoryPropertiesById(iTargetCat);

            return sResponse;
        }
        else if (sFirstSlot == _MOVE_TO_SPACE_)
        {
            int iImageToMove = clsBezier->getImageIdToMove(false, false);

            if (iImageToMove < 0)
                return _MOVE_FAIL_;     // no free image to move

            gameData->setImageOwned(iImageToMove, true);            // lock the image for the robot
            gameData->setRobotOwned(iImageToMove, true);
            QPainterPath ppBezierPath = clsBezier->calculateBezierPath(iImageToMove, false, false);
            gameData->setRobotMovePath(iImageToMove, ppBezierPath);
            gameData->setRobotReadyToMove(false);                   // hold move until robot is ready
            gameData->setRobotMoving(iImageToMove, true);           // prepare to move

            int iRobotSpeed = gameData->getRobotSpeed();
            int iMoveTimeMs = (ppBezierPath.length() / iRobotSpeed) * 1000;
            gameData->setRobotMoveTimeMs(iImageToMove, iMoveTimeMs);

            // [MESSAGE, image_id, start X, start Y, Speed, bez A X, bez A Y, bez B X, bez B Y, bez C X, bez C Y, bez D X, bez D Y, movetype, movetime, props of image]
            QList<QPointF> qpflBezierPoints = gameData->getMovePoints(iImageToMove);
            QSize qpScreen = gameData->getScreenSize();

            // the old game engine used to have origin at top left, we have it at centre, so transform our numbers to match (don't have to change the robot code)
            QString sResponse = "";
            sResponse = _MOVE_DATA_ + "," + QString::number(iImageToMove) + ",";
            sResponse += QString::number(qpflBezierPoints[0].x() + (qpScreen.width() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[0].y() + (qpScreen.height() / 2)) + ",";
            sResponse += QString::number(iRobotSpeed) + ",";
            sResponse += QString::number(qpflBezierPoints[0].x() + (qpScreen.width() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[0].y() + (qpScreen.height() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[1].x() + (qpScreen.width() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[1].y() + (qpScreen.height() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[2].x() + (qpScreen.width() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[2].y() + (qpScreen.height() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[3].x() + (qpScreen.width() / 2)) + ",";
            sResponse += QString::number(qpflBezierPoints[3].y() + (qpScreen.height() / 2)) + ",";
            sResponse += "TOSPACE,";
            sResponse += QString::number(iMoveTimeMs / 1000) + ",";     // convert to secs for robot
            sResponse += gameData->getImagePropertiesById(iImageToMove);

            return sResponse;
        }
        else if (sFirstSlot == _GET_LIBRARY_PROPS_)
        {
            // return library properties, number of categories and properties for all categories
            QString sResponse = "";
            sResponse = _LIBRARY_DATA_ + ",";
            sResponse += gameData->getLibraryProperties() + ",";
            sResponse += QString::number(gameData->getNumberOfCats()) + ",";
            sResponse += gameData->getAllCategoryProperties();

            return sResponse;
        }
        else if (sFirstSlot == _GET_CATEGORY_PROPS_)
        {
            // return properties for the last category an image was put in
            QString sResponse = "";
            sResponse = _CATEGORY_PROPS_ + ",";
            sResponse += gameData->getLastPlayerCatProps();

            return sResponse;
        }
        else if (sFirstSlot == _LOCK_ALL_IMAGES_)
        {
            gameData->setRobotLocked(true);
            return _CONFIRM_;
        }
        else if (sFirstSlot == _UNLOCK_ALL_IMAGES_)
        {
            gameData->setRobotLocked(false);
            return _CONFIRM_;
        }
        else if (sFirstSlot == _SET_FEEDBACK_ON_)
        {
            gameData->setShowFeedback(true);
            return _CONFIRM_;
        }
        else if (sFirstSlot == _SET_FEEDBACK_OFF_)
        {
            gameData->setShowFeedback(false);
            return _CONFIRM_;
        }
        else if (sFirstSlot == _SET_ONE_AT_TIME_)
        {
            bool bBoolIn;

            if (sDataIn[1] == "true") { bBoolIn = true; }
            else bBoolIn = false;

            gameData->setOneAtATime(bBoolIn);
            return _CONFIRM_;
        }
        else if (sFirstSlot == _ROBOT_TURN_SELECTION_)
        {
            int iIdIn = sDataIn[1].toInt();

            if (gameData->getImageOwned(iIdIn))
                return _MOVE_FAIL_;     // image not free

            gameData->setImageOwned(iIdIn, true);            // lock the image for the robot
            gameData->setRobotOwned(iIdIn, true);
            gameData->setRobotReadyToMove(false);            // hold move until robot is ready
            gameData->setRobotMoving(iIdIn, true);           // prepare to move

            int iMoveTimeMs = 2000;
            gameData->setRobotMoveTimeMs(iIdIn, iMoveTimeMs);

            QPointF qpfImagePos = gameData->getImagePositionById(iIdIn);

            return _ROBOT_TURN_LOCATION_ + "," + QString::number(qpfImagePos.x()) + "," + QString::number(qpfImagePos.y()) + "," + gameData->getImagePropertiesById(iIdIn);
        }
        else if (sFirstSlot == _GET_SHOWN_IM_PROPS_)
        {
            if (gameData->getOneAtATime())
            {
                QString sResponse = "";
                sResponse = _ONE_SHOWN_PROPS_ + ",";
                sResponse += gameData->getCurrOneToShowProps();

                return sResponse;
            }
            else
                return _ONE_SHOWN_PROPS_ + "," + _FAIL_;
        }
        else if (sFirstSlot == _SET_SPEED_)
        {
            int iSpeed = sDataIn[1].toInt();

            if (iSpeed > 0)
            {
                gameData->setRobotSpeed(iSpeed);
                return _CONFIRM_;
            }
            else
                return _FAIL_;
        }
        else
            return "";
    }
}

void UrbiReceive::sendMessage(QString sMsg)
{
    if (isRunning)
    {
        if (sMsg != "")
        {
            qDebug() << "Receive Sending: " << sMsg.toStdString().c_str();
            socket->write(sMsg.toStdString().c_str());
            socket->flush();
        }
    }
}

void UrbiReceive::disconnectFromServer()
{
    qDebug() << "Server disconnected";
    sendMessage(_EXIT_);
    socket->disconnectFromHost();
    socket->waitForDisconnected();
    isRunning = false;

    emit finished();    // kill thread when we disconnect
}
