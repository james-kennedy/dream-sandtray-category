#include "librarymanager.h"

LibraryManager::LibraryManager(QGraphicsScene &sceneIn, GameData &dataIn)
{
    mMainScene = &sceneIn;               // access to the scene to directly add images
    mGameData = &dataIn;                 // take the gameData structure to populate/pass on
    msLibRoot = mGameData->getLibraryPath();
    mGameData->setLibraryId(-1);        // init at -1 as we call nextLibrary() to bring up a new lib, making start lib0

    if (!mGameData->getLibraryTestReserved())
        mGameData->setLibraryLimit(QDir(msLibRoot).entryList(QDir::Dirs | QDir::NoDotAndDotDot).count());

    QObject::connect(mGameData, SIGNAL(newGame()), this, SLOT(nextLibrary()));
    QObject::connect(mGameData, SIGNAL(resetGame()), this, SLOT(reloadCurrentLibrary()));
}

LibraryManager::~LibraryManager()
{
}

// go to next library and loop round if we reach library limit
void LibraryManager::nextLibrary()
{
    int iLibrary = mGameData->getLibraryId();
    iLibrary++;

    if (iLibrary >= mGameData->getLibraryLimit())
        iLibrary = 0;

    mGameData->setLibraryId(iLibrary);

    loadLibrary();
}

void LibraryManager::reloadCurrentLibrary()
{
    loadLibrary();
}

// loads the library specified in iCurrLib - creates the category and image objects and adds them to the scene
void LibraryManager::loadLibrary()
{
    int iCurrLib = mGameData->getLibraryId();
    mGameData->setCollisionCheck(false);     // turn off collision checking - causes a crash if on whilst library changing

    QStringList libFolders = QDir(msLibRoot).entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    if (libFolders.count() < 1)
    {
        qDebug() << "No library folders found.";
    }
    else
    {
        QDir libDir(msLibRoot + libFolders.at(iCurrLib));

        // pull in the images from the correct folder - only get png or jpg files
        if (libDir.exists())
        {          
            // get number of categories - we need to have at least 1
            int iTotalCats = getNumberOfCategoriesInLibrary(libDir);

            // for the DREAM game, we often intentionally don't want categories, so remove check
            //if (iTotalCats < 1)
            //{
            //    qDebug() << "No categories found in library. Please add at least 1 image file starting 'cat'.";
            //}
            //else
            //{
                mMainScene->clear();             // clear scene, also destroys objects
                mGameData->clearImageDetails();  // clear out our data structures
                mGameData->clearCatDetails();

                QString sLibProps = extractLibraryProps(libFolders.at(iCurrLib));
                mGameData->setLibraryProperties(sLibProps);

                // read from the library name which mode we are in
                bool bTurnTake = extractLibraryTurnTakeMode(libFolders.at(iCurrLib));
                mGameData->setTurnTakeMode(bTurnTake);
                if (bTurnTake)
                    mGameData->setOneAtATime(false);

                if (mGameData->getShowButtons())
                {
                    // add new library button
                    QFileInfo fiNewButton(mGameData->getNewLibButton());
                    if (fiNewButton.isFile())
                    {
                        LibraryButton *newButton = new LibraryButton(fiNewButton.absoluteFilePath(), *mGameData, true);
                        QPointF qpfNewLib = getButtonPosition(fiNewButton.absoluteFilePath(), iTotalCats, true);
                        newButton->setPos(qpfNewLib);
                        mMainScene->addItem(newButton);
                    }

                    // add reset library button
                    QFileInfo fiResetButton(mGameData->getResetLibButton());
                    if (fiResetButton.isFile())
                    {
                        LibraryButton *resetButton = new LibraryButton(fiResetButton.absoluteFilePath(), *mGameData, false);
                        QPointF qpfResetLib = getButtonPosition(fiResetButton.absoluteFilePath(), iTotalCats, false);
                        resetButton->setPos(qpfResetLib);
                        mMainScene->addItem(resetButton);
                    }
                }

                QStringList nameFilter;
                nameFilter << "*.png" << "*.jpg";

                int iImgCounter = 0, iCatCounter = 0, iTotalImgs = 0;

                // load all categories first so when we add the images, we know where they are and don't overlap
                foreach(QFileInfo fiImage, libDir.entryInfoList(nameFilter, QDir::Files))
                {
                    if (fiImage.isFile())
                    {
                        if (fiImage.fileName().mid(0,3).toUpper() == "CAT")
                        {
                            // new category - adds itself to the gameData structure, but we get it's position from here
                            Category *newCat = new Category(fiImage.absoluteFilePath(), *mGameData, iCatCounter);
                            GameData::CategoryDetails thisCat = mGameData->getCatDetails()[iCatCounter];
                            QPointF qpfPos = getCategoryPosition(iCatCounter, iTotalCats, thisCat.qsCatSize.width(), thisCat.qsCatSize.height(), thisCat.rungWidth);
                            newCat->setPos(qpfPos);
                            mMainScene->addItem(newCat);
                            mGameData->setCatPos(iCatCounter, qpfPos);

                            iCatCounter++;
                        }
                        else
                            iTotalImgs++;
                    }
                }

                foreach(QFileInfo fiImage, libDir.entryInfoList(nameFilter, QDir::Files))
                {
                    if (fiImage.isFile())
                    {
                        if (fiImage.fileName().mid(0,3).toUpper() != "CAT")
                        {
                            // new image for categorising - adds itself to the gameData structure
                            DragImage *newImage = new DragImage(fiImage.absoluteFilePath(), *mGameData, iImgCounter);
                            GameData::ImageDetails thisIm = mGameData->getImageDetails()[iImgCounter];
                            // calculate image position (depends on mode and intelligent to avoiding categories)
                            QPointF qpfPos = getImagePosition(thisIm.qsImageSize.width(), thisIm.qsImageSize.height(), bTurnTake, iImgCounter, iTotalImgs);
                            newImage->setPos(qpfPos);
                            mMainScene->addItem(newImage);
                            mGameData->setImagePositionById(iImgCounter, qpfPos);

                            iImgCounter++;
                        }
                    }
                }

                // if we are showing 1 at a time then give it index 0
                if (mGameData->getOneAtATime())
                {
                    mGameData->setShuffleOrder();
                    mGameData->setCurrOneToShow(0);
                }

                emit libraryLoaded();
                // now all images are in, restart the collision detection
                mGameData->setCollisionCheck(true);
            //}
        }
        else
        {
            qDebug() << "Lib folders found, but index given was wrong.";
        }
    }
}

// get the library properties from the foldername - use underscore delimiter
QString LibraryManager::extractLibraryProps(QString sLibDirectory)
{
    QStringList splitString = sLibDirectory.split("_");
    splitString.removeAt(0);        // remove lib UID from start
    splitString.removeAt(1);        // remove lib mode
    return splitString.join("_");   // rejoin props to be unpacked by robot
}

bool LibraryManager::extractLibraryTurnTakeMode(QString sLibDirectory)
{
    QStringList splitString = sLibDirectory.split("_");
    if (splitString.at(1) == "mode0")
        return false;
    else
        return true;
}

// loops through the library path and counts the number of categories are present (file must start 'cat...')
int LibraryManager::getNumberOfCategoriesInLibrary(QDir libDir)
{
    int iNum = 0;

    QStringList nameFilter;
    nameFilter << "*.png" << "*.jpg";

    foreach(QFileInfo fiImage, libDir.entryInfoList(nameFilter, QDir::Files))
    {
        if (fiImage.isFile())
        {
            if (fiImage.fileName().mid(0,3).toUpper() == "CAT")
            {
                iNum++;
            }
        }
    }

    return iNum;
}

// return X and Y position for a category based on how many total and which one this is; implemented for 1, 2 or 4 cats
QPointF LibraryManager::getCategoryPosition(int iThisCat, int iTotalCats, int iImWidth, int iImHeight, int iLadderWidth)
{
    // 1 cat = middle of screen - centred height + width
    if (iTotalCats == 1)
    {
        return QPointF(0,0);
    }
    // 2 cats = two sides, centred height, factor in ladder widths
    else if (iTotalCats == 2)
    {
        int iScreenWidth = mGameData->getScreenSize().width();

        if (iThisCat == 0)
            return QPointF((-iScreenWidth / 2) + (iImWidth / 2) + iLadderWidth + 4, 0);
        else if (iThisCat == 1)
            return QPointF((iScreenWidth / 2) - (iImWidth / 2) - iLadderWidth - 4, 0);
        else
        {
            qDebug() << "A category higher than the total has been found.";
            return QPointF(0, 0);
        }
    }
    // 4 cats = corners
    else if (iTotalCats == 4)
    {
        int iScreenWidth = mGameData->getScreenSize().width();
        int iScreenHeight = mGameData->getScreenSize().height();
        int iXPos = 0, iYPos = 0;

        // calc X - left for 0 and 2, right for 1 and 3
        if (iThisCat == 0 || iThisCat == 2)
            iXPos = (-iScreenWidth / 2) + (iImWidth / 2) + iLadderWidth + 4;
        else if (iThisCat == 1 || iThisCat == 3)
            iXPos = (iScreenWidth / 2) - (iImWidth / 2) - iLadderWidth - 4;
        else
            qDebug() << "A category higher than the total has been found.";

        // now Y - top of screen for 0 and 1, bottom for 2 and 3
        if (iThisCat == 0 || iThisCat == 1)
            iYPos = (-iScreenHeight / 2) + (iImHeight / 2);
        else if (iThisCat == 2 || iThisCat == 3)
            iYPos = (iScreenHeight / 2) - (iImHeight / 2);
        else
            qDebug() << "A category higher than the total has been found.";

        return QPointF(iXPos, iYPos);
    }
    // error - not 1, 2 or 4 categories
    else
    {
        qDebug() << "Too many categories! Cannot work out positions.";
        return QPointF(0, 0);
    }
}

// pick a random position within the screen bounds taking into account the image size
// see if it overlaps with any category image - loop again if it does
QPointF LibraryManager::getImagePosition(int imWidth, int imHeight, bool bTurnTake, int iImgPos, int iTotalImgs)
{
    int iScreenWidth = mGameData->getScreenSize().width();
    int iScreenHeight = mGameData->getScreenSize().height();
    int iScreenL = - iScreenWidth / 2;
    int iScreenR = iScreenWidth / 2;
    int iScreenT = - iScreenHeight / 2;
    int iScreenB = iScreenHeight / 2;
    int iXPos, iYPos;
    bool bGoodPosition = false;

    if (bTurnTake)
    {
       // we know for turn taking that we have images to go across the centre of the screen
       int iInterval = iScreenWidth / (3*(iTotalImgs)-1); // split screen into equal chunks
       iXPos = iScreenL + (iInterval * (3*iImgPos + 1));  // horizontal position determined by ID
       iYPos = 0;                                       // centre vertically
    }
    else
    {
        while(!bGoodPosition)
        {
            // centre images if we are showing one-at-a-time and the option is on, otherwise random placement
            if (mGameData->getCentreImages() && mGameData->getOneAtATime() && (mGameData->getNumberOfCats() != 1))
            {
                iXPos = 0;
                iYPos = 0;
            }
            else
            {
                iXPos = getRandomNumber(iScreenL + (imWidth / 2), iScreenR - (imWidth / 2));
                iYPos = getRandomNumber(iScreenT + (imHeight / 2), iScreenB - (imHeight / 2));
            }

            int imageL = iXPos - (imWidth / 2);
            int imageR = iXPos + (imWidth / 2);
            int imageT = iYPos - (imHeight / 2);
            int imageB = iYPos + (imHeight / 2);
            bool bCollision = false;

            QList<GameData::CategoryDetails> allCats = mGameData->getCatDetails();

            foreach (GameData::CategoryDetails thisCat, allCats)
            {
                int iCatL = thisCat.qpfCatPosition.x() - (thisCat.qsCatSize.width() / 2);
                int iCatR = thisCat.qpfCatPosition.x() + (thisCat.qsCatSize.width() / 2);
                int iCatT = thisCat.qpfCatPosition.y() - (thisCat.qsCatSize.height() / 2);
                int iCatB = thisCat.qpfCatPosition.y() + (thisCat.qsCatSize.height() / 2);

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
    }

    return QPointF(iXPos, iYPos);
}

// return the position for the new and reset buttons based on the number of categories
QPointF LibraryManager::getButtonPosition(QString sFilePath, int iTotalCats, bool bNewLibrary)
{
    int iScreenWidth = mGameData->getScreenSize().width();
    int iScreenHeight = mGameData->getScreenSize().height();
    int iScreenL = - iScreenWidth / 2;
    int iScreenT = - iScreenHeight / 2;
    int iScreenB = iScreenHeight / 2;
    int iXPos, iYPos;

    QImage qiImage;
    qiImage.load(sFilePath);
    int iImageH = qiImage.height();
    int iImageW = qiImage.width();

    if (bNewLibrary)
        iYPos = iScreenB - (iImageH / 2);       // bottom of screen
    else
        iYPos = iScreenT + (iImageH / 2);       // top of screen

    if (iTotalCats <= 2)
        iXPos = iScreenL + (iImageW / 2);       // left edge for 2 or 1 cat
    else
        iXPos = 0;                              // centre of screen for 4 cats (corners taken)

    return QPointF(iXPos, iYPos);
}

int LibraryManager::getRandomNumber(int min, int max)
{
   return (qrand() % ((max + 1) - min)) + min;
}
