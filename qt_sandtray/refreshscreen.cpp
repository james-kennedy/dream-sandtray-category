#include "refreshscreen.h"

// this is small class used as a thread to force the screen to update at times we specify
// it's lined to gameData on signals/slots to force the start and stop from around the game
RefreshScreen::RefreshScreen(QGraphicsScene &sceneIn, GameData &currData)
{
    mainScene = &sceneIn;
    mGameData = &currData;

    mbRun = false;
}

void RefreshScreen::start()
{
    mbRun = false;
}

void RefreshScreen::startUpdate()
{
    mbRun = true;

    while(mbRun)
    {
        mainScene->update();

        if (!mGameData->getAnyCategoryFeedback() && !mGameData->getAnyRobotMoving())
            mbRun = false;
    }
}

void RefreshScreen::stopUpdate()
{
    mbRun = false;
}

void RefreshScreen::killThread()
{
    mbRun = false;

    emit finished();    // kills the thread
}
