#include "bezierclass.h"

#define PI 3.14159265

BezierClass::BezierClass(GameData &currData)
{
    mGameData = &currData;
}

int BezierClass::getImageIdToMove(bool bCorrectMove, bool bToCategory)
{
    QList<GameData::CategoryDetails> allCats = mGameData->getCatDetails();

    // for one-at-a-time, we only want to return the one being shown if not owned and move possible
    if (mGameData->getOneAtATime())
    {
        int iImageId = mGameData->getCurrOneToShow();
        QString sCatBelonged = mGameData->getCatBelonged(iImageId);

        if (!mGameData->getImageOwned(iImageId) && mGameData->getImageActive(iImageId))
        {
            foreach (GameData::CategoryDetails thisCat, allCats)
            {
                if (bCorrectMove)
                {
                    if (sCatBelonged == thisCat.catName)
                    {
                        return iImageId;
                    }
                }
                else
                {
                    if (sCatBelonged != thisCat.catName)
                    {
                        return iImageId;
                    }
                }
            }
        }
    }
    else
    {
        // showing all images, so go through them all until we find a suitable one
        QList<GameData::ImageDetails> allImages = mGameData->getImageDetails();

        foreach (GameData::ImageDetails thisImage, allImages)
        {
            // can't use owned or inactive
            if (!thisImage.imageOwned && thisImage.imageActive)
            {
                if (bToCategory)
                {
                    // make sure there is a category for it to go into
                    foreach (GameData::CategoryDetails thisCat, allCats)
                    {
                        if (bCorrectMove)
                        {
                            if (thisImage.catBelonged == thisCat.catName)
                            {
                                return thisImage.imageId;
                            }
                        }
                        else
                        {
                            if (thisImage.catBelonged != thisCat.catName)
                            {
                                return thisImage.imageId;
                            }
                        }
                    }
                }
                else
                {
                    return thisImage.imageId;
                }
            }
        }
    }

    return -1;  // if we made it here, we found no suitable images, so return something we can interpret as a FAIL MOVE
}

QPainterPath BezierClass::calculateBezierPath(int iImageId, bool bCorrectMove, bool bToCategory)
{
    QPointF qpfStart = mGameData->getImagePositionById(iImageId);
    QPointF qpfTarget;

    if (bToCategory)
    {
        QString sCategory = mGameData->getCatBelonged(iImageId);
        qpfTarget = getPositionOfCategory(bCorrectMove, sCategory);
    }
    else
    {
        qpfTarget = getPositionOfSpace(iImageId);
    }

    int iControlRadius = mGameData->getScreenSize().height() / 3;
    QPointF qpfControl1 = getControlPointOne(qpfStart, qpfTarget, iControlRadius);
    QPointF qpfControl2 = getControlPointTwo(qpfStart, qpfTarget, iControlRadius);

    QPainterPath ppBezier(qpfStart);
    ppBezier.cubicTo(qpfControl1, qpfControl2, qpfTarget);

    QList<QPointF> qpflBezierPoints;
    qpflBezierPoints << qpfStart << qpfControl1 << qpfControl2 << qpfTarget;
    mGameData->setMovePoints(iImageId, qpflBezierPoints);

    return ppBezier;
}

QPoint BezierClass::getPositionOfCategory(bool bCorrectMove, QString sCorrectCat)
{
    QList<GameData::CategoryDetails> allCats = mGameData->getCatDetails();
    int iNumberOfCats = allCats.length();
    int iTryCategory;
    bool bGoodCat = false;

    // loop and pick at random until we get one - essential for 4 category games
    while(!bGoodCat)
    {
        iTryCategory = getRandomNumber(0, iNumberOfCats - 1);

        if (bCorrectMove)
        {
            if (allCats[iTryCategory].catName == sCorrectCat)
                bGoodCat = true;
        }
        else
        {
            if (allCats[iTryCategory].catName != sCorrectCat)
                bGoodCat = true;
        }
    }

    mGameData->setBezierTargetCat(iTryCategory);
    return QPoint(allCats[iTryCategory].qpfCatPosition.x(), allCats[iTryCategory].qpfCatPosition.y());
}

// this is the same as the library manager code for placing images on load - if error here, likely one there too
QPointF BezierClass::getPositionOfSpace(int iImageId)
{
    QSize iImageSize = mGameData->getImageSizeById(iImageId);
    int iScreenWidth = mGameData->getScreenSize().width();
    int iScreenHeight = mGameData->getScreenSize().height();
    int iScreenL = - iScreenWidth / 2;
    int iScreenR = iScreenWidth / 2;
    int iScreenT = - iScreenHeight / 2;
    int iScreenB = iScreenHeight / 2;
    int iXPos, iYPos;
    bool bGoodPosition = false;

    while(!bGoodPosition)
    {
        iXPos = getRandomNumber(iScreenL + (iImageSize.width() / 2), iScreenR - (iImageSize.width() / 2));
        iYPos = getRandomNumber(iScreenT + (iImageSize.height() / 2), iScreenB - (iImageSize.height() / 2));

        int imageL = iXPos - (iImageSize.width() / 2);
        int imageR = iXPos + (iImageSize.width() / 2);
        int imageT = iYPos - (iImageSize.height() / 2);
        int imageB = iYPos + (iImageSize.height() / 2);
        bool bCollision = false;

        QList<GameData::CategoryDetails> allCats = mGameData->getCatDetails();

        foreach (GameData::CategoryDetails thisCat, allCats)
        {
            // +/- 50 so that there is space around the category as well - prevents accidental categorisations
            // TODO: this needs to be tested!
            int iCatL = thisCat.qpfCatPosition.x() - (thisCat.qsCatSize.width() / 2) - 50;
            int iCatR = thisCat.qpfCatPosition.x() + (thisCat.qsCatSize.width() / 2) + 50;
            int iCatT = thisCat.qpfCatPosition.y() - (thisCat.qsCatSize.height() / 2) - 50;
            int iCatB = thisCat.qpfCatPosition.y() + (thisCat.qsCatSize.height() / 2) + 50;

            if (imageL < iCatR && imageR > iCatL && imageT < iCatB && imageB > iCatT)
            {
                bCollision = true;
                break;
            }
        }

        // if we have no collision then we are happy, so stop this loop
        if (!bCollision)
            bGoodPosition = true;
    }

    return QPointF(iXPos, iYPos);
}

// this point needs to be in the quadrant facing from the start to the end
QPointF BezierClass::getControlPointOne(QPointF qpfStart, QPointF qpfEnd, int iRadius)
{
    int iTheta = getRandomNumber(0, 90);
    QPointF qpfReturn = QPointF(0, 0);

    if (qpfEnd.x() < qpfStart.x())
        qpfReturn.setX(qpfStart.x() - (iRadius * cos(double(iTheta * PI / 180.0))));     // left of start
    else
        qpfReturn.setX(qpfStart.x() + (iRadius * cos(double(iTheta * PI / 180.0))));     // right of start

    if (qpfEnd.y() < qpfStart.y())
        qpfReturn.setY(qpfStart.y() + (iRadius * sin(double(iTheta * PI / 180.0))));     // below start
    else
        qpfReturn.setY(qpfStart.y() - (iRadius * sin(double(iTheta * PI / 180.0))));     // above start

    return qpfReturn;
}

// this point needs to be in the quadrant facing from the end to the start
QPointF BezierClass::getControlPointTwo(QPointF qpfStart, QPointF qpfEnd, int iRadius)
{
    int iTheta = getRandomNumber(0, 90);
    QPointF qpfReturn = QPointF(0, 0);

    if (qpfEnd.x() < qpfStart.x())
        qpfReturn.setX(qpfEnd.x() + (iRadius * cos(double(iTheta * PI / 180.0))));       // right of end
    else
        qpfReturn.setX(qpfEnd.x() - (iRadius * cos(double(iTheta * PI / 180.0))));       // left of end

    if (qpfEnd.y() < qpfStart.y())
        qpfReturn.setY(qpfEnd.y() + (iRadius * sin(double(iTheta * PI / 180.0))));       // above end
    else
        qpfReturn.setY(qpfEnd.y() - (iRadius * sin(double(iTheta * PI / 180.0))));       // below end

    return qpfReturn;
}

int BezierClass::getRandomNumber(int min, int max)
{
    // prevent division by 0
    if ((max - min) <= 0)
        return 0;
    else
        return (qrand() % ((max + 1) - min)) + min;
}

// helper functions for urbi send/receive ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int BezierClass::getNumberOfUncategorisedImages()
{
    QList<GameData::ImageDetails> allImages = mGameData->getImageDetails();
    int iNumberLeft = 0;

    foreach (GameData::ImageDetails thisImage, allImages)
    {
        // can't use owned or inactive
        if (!thisImage.imageOwned && thisImage.imageActive)
            iNumberLeft++;
    }

    return iNumberLeft;
}

int BezierClass::getNumberOfImagesRemaining()
{
    QList<GameData::ImageDetails> allImages = mGameData->getImageDetails();
    int iNumberLeft = 0;

    foreach (GameData::ImageDetails thisImage, allImages)
    {
        // can't use owned or inactive
        if (thisImage.imageActive)
            iNumberLeft++;
    }

    return iNumberLeft;
}

QList<int> BezierClass::getActiveImageList()
{
    QList<int> iReturnList;
    QList<GameData::ImageDetails> allImages = mGameData->getImageDetails();

    foreach (GameData::ImageDetails thisImage, allImages)
    {
        if (!thisImage.imageOwned && thisImage.imageActive)
            iReturnList.append(thisImage.imageId);
    }

    return iReturnList;
}
