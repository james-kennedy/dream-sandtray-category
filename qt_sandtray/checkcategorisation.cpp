#include "checkcategorisation.h"
#include <QThread>

class SleepThread : public QThread {
public:
   static inline void usleep(unsigned long usecs) {
       QThread::usleep(usecs);
   }
};

CheckCategorisation::CheckCategorisation(QGraphicsScene &sceneIn, GameData &dataIn)
{
    mainScene = &sceneIn;
    gameData = &dataIn;

    mbRun = true;
}

CheckCategorisation::~CheckCategorisation()
{
    mbRun = false;
}

void CheckCategorisation::start()
{
    int imageL = 0, imageR = 0, imageT = 0, imageB = 0;           // dummy values so no overlaps can occur on first loop
    int catL = 2000, catR = 2000, catT = 2000, catB = 2000;
    SleepThread sl;
    while(mbRun){
        sl.usleep(1);
        if (gameData->getCollisionCheck())
        {
            // get lists of images and categories from gameData
            QList<GameData::ImageDetails> allImages = gameData->getImageDetails();
            QList<GameData::CategoryDetails> allCats = gameData->getCatDetails();

            foreach (GameData::CategoryDetails thisCat, allCats)
            {
                // get bounds of categories
                catL = thisCat.qpfCatPosition.x() - (thisCat.qsCatSize.width() / 2);
                catR = thisCat.qpfCatPosition.x() + (thisCat.qsCatSize.width() / 2);
                catT = thisCat.qpfCatPosition.y() - (thisCat.qsCatSize.height() / 2);
                catB = thisCat.qpfCatPosition.y() + (thisCat.qsCatSize.height() / 2);

                bool bCollisionThisCat = false;

                // loop through each image and see if it has collided with a category
                foreach (GameData::ImageDetails thisImage, allImages)
                {
                    if(thisImage.imageActive)
                    {
                        // get bounds of image (xleft, xright, ytop, ybottom)
                        imageL = int(thisImage.qpfImagePosition.x()) - (thisImage.qsImageSize.width() / 2);
                        imageR = int(thisImage.qpfImagePosition.x()) + (thisImage.qsImageSize.width() / 2);
                        imageT = int(thisImage.qpfImagePosition.y()) - (thisImage.qsImageSize.height() / 2);
                        imageB = int(thisImage.qpfImagePosition.y()) + (thisImage.qsImageSize.height() / 2);

                        // check for overlap (proof by contradiction)
                        if(imageL < catR && imageR > catL && imageT < catB && imageB > catT)
                        {
                            if (thisImage.imageOwned)
                            {
                                bCollisionThisCat = true;
                            }
                            else if (!thisImage.imageOwned)
                            {
                                int iThisImageId = thisImage.imageId;
                                int iThisCatId = thisCat.catId;

                                // not owned so it's been categorised; do something with the image
                                gameData->setImageActive(iThisImageId, false);
                                gameData->setCatPlaced(iThisImageId, iThisCatId);
                                gameData->addInsideCategory(iThisCatId, iThisImageId);

                                if (thisCat.catName == thisImage.catBelonged)
                                {
                                    if (gameData->getUseSound() && gameData->getShowFeedback())
                                        emit playSound(gameData->getRightSound());

                                    gameData->setIsFeedbackCorrect(iThisCatId, true);

                                    if (thisImage.bRobotLastOwner)
                                    {
                                        gameData->setRobotMove(iThisImageId, true, iThisCatId);
                                        gameData->setMsgToSend(_ROBOT_MOVE_);
                                    }
                                    else
                                    {
                                        gameData->setPlayerRightMove(iThisImageId, iThisCatId);
                                        gameData->setMsgToSend(_PLAYER_MOVE_);
                                    }
                                }
                                else
                                {
                                    if (gameData->getUseSound() && gameData->getShowFeedback())
                                        emit playSound(gameData->getWrongSound());

                                    gameData->setIsFeedbackCorrect(iThisCatId, false);

                                    if (thisImage.bRobotLastOwner)
                                    {
                                        gameData->setRobotMove(iThisImageId, false, iThisCatId);
                                        gameData->setMsgToSend(_ROBOT_MOVE_);
                                    }
                                    else
                                    {
                                        gameData->setPlayerWrongMove(thisImage.imageId, iThisCatId);
                                        gameData->setMsgToSend(_PLAYER_MOVE_);
                                    }
                                }

                                if (gameData->getShowFeedback())
                                {
                                    gameData->setFeedbackStart(iThisCatId, QDateTime::currentMSecsSinceEpoch());
                                    gameData->setShowCategoryFeedback(iThisCatId, true);
                                    gameData->setForceScreenUpdate(true);
                                }

                                if (gameData->getOneAtATime())
                                    gameData->setNewOneToShow();
                            }
                        }
                    }
                }

                // if we have collided and don't already know, then update the overlap status
                if (bCollisionThisCat && !gameData->getCurrOverlap(thisCat.catId))
                    gameData->setCurrOverlap(thisCat.catId, true);
                else if (!bCollisionThisCat)
                    gameData->setCurrOverlap(thisCat.catId, false);
            }

            //mainScene->update();      // if there are any issues with painting, then put this back in, but should be avoided due to inefficiency
        }
    }
}

void CheckCategorisation::killThread()
{
    QMutexLocker locker(&mutex);
    {
        mbRun = false;
    }

    emit finished();    // kills the thread
    this->deleteLater();
}
