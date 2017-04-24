#include "gameengine.h"

GameEngine::GameEngine(QGraphicsScene &sceneIn)
{
    mainScene = &sceneIn;
}

GameEngine::~GameEngine()
{
    //delete music;

    emit appClosing();

    //collisionThread->quit();
    //collisionThread->wait();
    //delete collisionThread;
    //delete refreshThread;
}

void GameEngine::createScene()
{
    GameData *mainData = new GameData();
    mainData->setScreenSize(QSize(mainScene->width(), mainScene->height()));    // use this quite often throughout - done here to be thread safe

    // the library manager creates instances of the images and categories and adds them to the scene
    clsLibManager = new LibraryManager(*mainScene, *mainData);
    clsLibManager->nextLibrary();

    // check for overlaps between images and categories when no user input - game logic thread
    collisionThread = new QThread;
    collision = new CheckCategorisation(*mainScene, *mainData);
    collision->moveToThread(collisionThread);
    connect(collision, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(collisionThread, SIGNAL(started()), collision, SLOT(start()));
    connect(collision, SIGNAL(finished()), collisionThread, SLOT(quit()));
    //connect(collision, SIGNAL(finished()), collision, SLOT(deleteLater()));
    connect(collisionThread, SIGNAL(finished()), collisionThread, SLOT(deleteLater()));
    connect(collision, SIGNAL(playSound(QString)), this, SLOT(playSound(QString)));
    connect(this, SIGNAL(appClosing()), collision, SLOT(killThread()));
    collisionThread->start();

    if (mainData->getUseRobot())
    {
        // launch urbi send thread, give it access to *mainData
        QThread* urbiSendThread = new QThread;
        UrbiSend* urbiSend = new UrbiSend(*mainData);
        urbiSend->moveToThread(urbiSendThread);
        connect(urbiSend, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(urbiSendThread, SIGNAL(started()), urbiSend, SLOT(start()));
        connect(urbiSend, SIGNAL(finished()), urbiSendThread, SLOT(quit()));
        connect(urbiSend, SIGNAL(finished()), urbiSend, SLOT(deleteLater()));
        connect(urbiSendThread, SIGNAL(finished()), urbiSendThread, SLOT(deleteLater()));
        urbiSendThread->start();

        // launch urbi receive thread, give it access to *mainData
        QThread* urbiRecThread = new QThread;
        UrbiReceive* urbiRec = new UrbiReceive(*mainData, *clsLibManager);
        urbiRec->moveToThread(urbiRecThread);
        connect(urbiRec, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
        connect(urbiRecThread, SIGNAL(started()), urbiRec, SLOT(start()));
        connect(urbiRec, SIGNAL(finished()), urbiRecThread, SLOT(quit()));
        connect(urbiRec, SIGNAL(finished()), urbiRec, SLOT(deleteLater()));
        connect(urbiRecThread, SIGNAL(finished()), urbiRecThread, SLOT(deleteLater()));
        urbiRecThread->start();
    }

    // connect to the GameData class to join the forced screen update routines
    refreshThread = new QThread;
    refresh = new RefreshScreen(*mainScene, *mainData);
    refresh->moveToThread(refreshThread);
    connect(refresh, SIGNAL(error(QString)), this, SLOT(errorString(QString)));
    connect(refreshThread, SIGNAL(started()), refresh, SLOT(start()));
    connect(refresh, SIGNAL(finished()), refreshThread, SLOT(quit()));
    //connect(refresh, SIGNAL(finished()), refresh, SLOT(deleteLater()));
    connect(refreshThread, SIGNAL(finished()), this, SLOT(deleteLater()));
    connect(mainData, SIGNAL(forceUpdateScreen()), refresh, SLOT(startUpdate()));
    connect(mainData, SIGNAL(stopUpdateScreen()), refresh, SLOT(stopUpdate()));
    connect(this, SIGNAL(appClosing()), refresh, SLOT(killThread()));
    refreshThread->start();

    music = Phonon::createPlayer(Phonon::MusicCategory);
    playSound(mainData->getRightSound());
}

// handle errors - errors from threads get propagated upwards for dealing with here
void GameEngine::errorString(QString sErr)
{
    qDebug() << sErr;
}


void GameEngine::playSound(QString sFilePath)
{
    // only pass in the new source if we aren't already playing something
    if (music->state() == Phonon::StoppedState || music->state() == Phonon::LoadingState || music->state() == Phonon::PausedState)
    {
        music->setCurrentSource(Phonon::MediaSource(sFilePath));
        music->play();
    }
}
