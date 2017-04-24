#include "gamedata.h"

GameData::GameData()
{
    QString sFilePath = "../settings.ini";          // Linux/Windows
    //QString sFilePath = "../../../settings.ini";    // Mac

    retrieveSettingsFromFile(sFilePath);

    bCollisionCheck = false;                        // prevent collision checks until library is loaded
    iRobotSpeed = _DEFAULT_SPEED_;
    iLastMoveEnd = 0;                               // so we can ignore first delay
    iCurrOneAtATime = -1;
    sLibProps = "";
    bCollisionCheck = false;
    bRobotReadyToMove = false;
    iRobotMoveStart = 0;
    iCurrentLibrary = -1;
    iBezierTargetCat = 0;
    bButtonsActive = true;
    bRobotLocked = false;
    iMaxLibrary = 0;
    bTurnTakeMode = false;
    bUseSound = true;
}

// settings - used internally ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void GameData::retrieveSettingsFromFile(QString sFile)
{
    QSettings appSettings(sFile, QSettings::IniFormat);

    // paths   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    sLibPath = appSettings.value("paths/Library").toString();
    sRightSound = appSettings.value("paths/RightSound").toString();
    sWrongSound = appSettings.value("paths/WrongSound").toString();
    sNewLibButton = appSettings.value("paths/NewLibraryButton").toString();
    sResetLibButton = appSettings.value("paths/ResetLibraryButton").toString();
    sCorrectFeedback = appSettings.value("paths/CorrectImage").toString();
    sIncorrectFeedback = appSettings.value("paths/IncorrectImage").toString();

    QImage qiTemp;
    qiTemp.load(sCorrectFeedback);  // fix an error on this line!?
    pmCorrectFeedback = QPixmap::fromImage(qiTemp);
    qiTemp.load(sIncorrectFeedback);
    pmIncorrectFeedback = QPixmap::fromImage(qiTemp);

    // robot   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    sServerIP = appSettings.value("robot/ServerIP").toString();
    bUseRobot = appSettings.value("robot/UseRobot").toBool();

    // game    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    iLadderSlots = appSettings.value("game/LadderRungs").toInt();
    iLadderWidth = appSettings.value("game/LadderWidth").toInt();
    bReserveTestLibs = appSettings.value("game/ReserveTests").toBool();
    iMaxLibrary = appSettings.value("game/TestLibStart").toInt();
    bShowButtons = appSettings.value("game/UseButtons").toBool();
    bShowFeedback = appSettings.value("game/ShowFeedback").toBool();
    bOneAtATime = appSettings.value("game/OneAtATime").toBool();
    bCentreImages = appSettings.value("game/CentreImages").toBool();
}

QString GameData::getServerIP()
{
    QMutexLocker locker(&mutex);
    return sServerIP;
}
QString GameData::getLibraryPath()
{
    QMutexLocker locker(&mutex);
    return sLibPath;
}
bool GameData::getUseRobot()
{
    QMutexLocker locker(&mutex);
    return bUseRobot;
}
int GameData::getLadderWidth()
{
    QMutexLocker locker(&mutex);
    return iLadderWidth;
}
int GameData::getLadderRungs()
{
    QMutexLocker locker(&mutex);
    return iLadderSlots;
}
QString GameData::getRightSound()
{
    QMutexLocker locker(&mutex);
    return sRightSound;
}
QString GameData::getWrongSound()
{
    QMutexLocker locker(&mutex);
    return sWrongSound;
}
QString GameData::getNewLibButton()
{
    QMutexLocker locker(&mutex);
    return sNewLibButton;
}
QString GameData::getResetLibButton()
{
    QMutexLocker locker(&mutex);
    return sResetLibButton;
}
QPixmap GameData::getCorrectFeedback()
{
    QMutexLocker locker(&mutex);
    return pmCorrectFeedback;
}
QPixmap GameData::getIncorrectFeedback()
{
    QMutexLocker locker(&mutex);
    return pmIncorrectFeedback;
}
bool GameData::getShowFeedback()
{
    QMutexLocker locker(&mutex);
    return bShowFeedback;
}
void GameData::setShowFeedback(bool bFeedback)
{
    QMutexLocker locker(&mutex);
    bShowFeedback = bFeedback;
}
bool GameData::getOneAtATime()
{
    QMutexLocker locker(&mutex);
    return bOneAtATime;
}
void GameData::setOneAtATime(bool bBoolIn)
{
    QMutexLocker locker(&mutex);
    bOneAtATime = bBoolIn;
}
bool GameData::getCentreImages()
{
    QMutexLocker locker(&mutex);
    return bCentreImages;
}

QSize GameData::getScreenSize()
{
    QMutexLocker locker(&mutex);
    return iScreenSize;
}
void GameData::setScreenSize(QSize iScreenSizeIn)
{
    QMutexLocker locker(&mutex);
    iScreenSize = iScreenSizeIn;
}

bool GameData::getCollisionCheck()
{
    QMutexLocker locker(&mutex);
    return bCollisionCheck;
}
void GameData::setCollisionCheck(bool bCheck)
{
    QMutexLocker locker(&mutex);
    bCollisionCheck = bCheck;
}

int GameData::getLibraryId()
{
    QMutexLocker locker(&mutex);
    return iCurrentLibrary;
}
void GameData::setLibraryId(int iLibraryId)
{
    QMutexLocker locker(&mutex);
    iCurrentLibrary = iLibraryId;
}

// player/game info  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QString GameData::getLibraryProperties()
{
    QMutexLocker locker(&mutex);
    return sLibProps;
}
void GameData::setLibraryProperties(QString sLibPropsIn)
{
    QMutexLocker locker(&mutex);
    sLibProps = sLibPropsIn;
}

bool GameData::getLibraryTestReserved()
{
    QMutexLocker locker(&mutex);
    return bReserveTestLibs;
}

int GameData::getLibraryLimit()
{
    QMutexLocker locker(&mutex);
    return iMaxLibrary;
}
void GameData::setLibraryLimit(int iLimit)
{
    QMutexLocker locker(&mutex);
    iMaxLibrary = iLimit;
}

bool GameData::getButtonsActive()
{
    QMutexLocker locker(&mutex);
    return bButtonsActive;
}
void GameData::setButtonsActive(bool bActive)
{
    QMutexLocker locker(&mutex);
    bButtonsActive = bActive;
}

bool GameData::getShowButtons()
{
    QMutexLocker locker(&mutex);
    return bShowButtons;
}
void GameData::setShowButtons(bool bShow)
{
    QMutexLocker locker(&mutex);
    bShowButtons = bShow;
}

QList<float> GameData::getFullTimeList()
{
    QMutexLocker locker(&mutex);
    return flListTimes;
}
void GameData::setNewTime(float flTime)
{
    QMutexLocker locker(&mutex);
    flListTimes.prepend(flTime);
}
float GameData::getAverageTime()
{
    QList<float> flTimeListCopy;
    float flTotalTime = 0;
    {
        QMutexLocker locker(&mutex);
        flTimeListCopy = flListTimes;
    }

    foreach (float flTime, flTimeListCopy)
        flTotalTime += flTime;

    if (flTimeListCopy.length() > 0)
        return flTotalTime / flTimeListCopy.length();
    else
        return 0;
}
float GameData::getLastTime()
{
    QMutexLocker locker(&mutex);
    if (flListTimes.length() > 0)
        return flListTimes[0];
    else
        return -1;
}
void GameData::clearTimeList()
{
    QMutexLocker locker(&mutex);
    flListTimes.clear();
}

QList<float> GameData::getFullDelayList()
{
    QMutexLocker locker(&mutex);
    return flListDelays;
}
void GameData::setNewDelay(float flDelay)
{
    QMutexLocker locker(&mutex);
    flListDelays.prepend(flDelay);
}
float GameData::getAverageDelay()
{
    QList<float> flDelayListCopy;
    float flTotalDelay = 0;
    {
        QMutexLocker locker(&mutex);
        flDelayListCopy = flListDelays;
    }

    foreach (float flDelay, flDelayListCopy)
        flTotalDelay += flDelay;

    if (flDelayListCopy.length() > 0)
        return flTotalDelay / flDelayListCopy.length();
    else
        return 0;
}
float GameData::getLastDelay()
{
    QMutexLocker locker(&mutex);
    if (flListDelays.length() > 0)
        return flListDelays[0];
    else
        return 0;
}
void GameData::clearDelayList()
{
    QMutexLocker locker(&mutex);
    flListDelays.clear();
}

QList<float> GameData::getFullDistanceList()
{
    QMutexLocker locker(&mutex);
    return flListDists;
}
void GameData::setNewDistance(float flDistance)
{
    QMutexLocker locker(&mutex);
    flListDists.prepend(flDistance);
}
float GameData::getAverageDistance()
{
    QList<float> flDistanceListCopy;
    float flTotalDistance = 0;
    {
        QMutexLocker locker(&mutex);
        flDistanceListCopy = flListDists;
    }

    foreach (float flDist, flDistanceListCopy)
        flTotalDistance += flDist;

    if (flDistanceListCopy.length() > 0)
        return flTotalDistance / flDistanceListCopy.length();
    else
        return 0;
}
float GameData::getLastDistance()
{
    QMutexLocker locker(&mutex);
    if (flListDists.length() > 0)
        return flListDists[0];
    else
        return -1;
}
void GameData::clearDistanceList()
{
    QMutexLocker locker(&mutex);
    flListDists.clear();
}

QList<float> GameData::getFullSpeedList()
{
    QMutexLocker locker(&mutex);
    return flListSpeeds;
}
void GameData::setNewSpeed(float flSpeed)
{
    QMutexLocker locker(&mutex);
    flListSpeeds.prepend(flSpeed);
}
float GameData::getAverageSpeed()
{
    QList<float> flSpeedListCopy;
    float flTotalSpeed = 0;
    {
        QMutexLocker locker(&mutex);
        flSpeedListCopy = flListSpeeds;
    }

    foreach (float flSpeed, flSpeedListCopy)
        flTotalSpeed += flSpeed;

    if (flSpeedListCopy.length() > 0)
        return flTotalSpeed / flSpeedListCopy.length();
    else
        return 0;
}
float GameData::getLastSpeed()
{
    QMutexLocker locker(&mutex);
    if (flListSpeeds.length() > 0)
        return flListSpeeds[0];
    else
        return -1;
}
void GameData::clearSpeedList()
{
    QMutexLocker locker(&mutex);
    flListSpeeds.clear();
}

qint64 GameData::getLastMoveEnd()
{
    QMutexLocker locker(&mutex);
    return iLastMoveEnd;
}
void GameData::setLastMoveEnd(qint64 iTime)
{
    QMutexLocker locker(&mutex);
    iLastMoveEnd = iTime;
}

int GameData::getNumberPlayerRightMoves()
{
    QList<int> iCorrectList;
    {
        QMutexLocker locker(&mutex);            // tie up variable for short time, so make copy
        iCorrectList = iListCorrectCats;
    }

    int iCorrect = 0;

    foreach (int i, iCorrectList)
        iCorrect += i;

    return iCorrect;
}
void GameData::setPlayerRightMove(int iImageId, int iCategoryId)
{
    QMutexLocker locker(&mutex);
    iListCorrectCats.prepend(1);
    iListCategorised.prepend(iImageId);
    iListInCategory.prepend(iCategoryId);
}
int GameData::getNumberPlayerWrongMoves()
{
    QList<int> iCorrectList;
    {
        QMutexLocker locker(&mutex);            // tie up variable for short time, so make copy
        iCorrectList = iListCorrectCats;
    }

    int iCorrect = 0;

    foreach (int i, iCorrectList)
        iCorrect += i;

    return iCorrectList.length() - iCorrect;    // list length = Correct + Incorrect, so length - Correct = Incorrect
}
void GameData::setPlayerWrongMove(int iImageId, int iCategoryId)
{
    QMutexLocker locker(&mutex);
    iListCorrectCats.prepend(0);
    iListCategorised.prepend(iImageId);
    iListInCategory.prepend(iCategoryId);
}
void GameData::resetPlayerScore()
{
    QMutexLocker locker(&mutex);
    iListCorrectCats.clear();
}

void GameData::setRobotMove(int iImageId, bool bCorrect, int iCategoryId)
{
    QMutexLocker locker(&mutex);
    bRobotCorrect.prepend(bCorrect);
    iRobotCategorised.prepend(iImageId);
    iRobotCategory.prepend(iCategoryId);
}
int GameData::getLastRobotMoveId()
{
   QMutexLocker locker(&mutex);
   if (iRobotCategorised.length() > 0)
       return iRobotCategorised[0];
   else
       return -1;
}
bool GameData::getLastRobotMoveCorrect()
{
    QMutexLocker locker(&mutex);
    if (bRobotCorrect.length() > 0)
        return bRobotCorrect[0];
    else
        return false;
}

bool GameData::getLastCategorisationCorrect()
{
    QMutexLocker locker(&mutex);
    if (iListCorrectCats.length() > 0)
    {
        if (iListCorrectCats[0] == 1)
            return true;
        else
            return false;
    }
    else
        return false;
}
int GameData::getLastImageCategorised()
{
    QMutexLocker locker(&mutex);
    if (iListCategorised.length() > 0)
        return iListCategorised[0];
    else
        return -1;
}

QString GameData::getLastPlayerCatProps()
{
    QMutexLocker locker(&mutex);
    if (iListInCategory.length() > 0 && categories.length() > 0)
        return categories[iListInCategory[0]].catProps;
    else
        return "";
}

void GameData::setMsgToSend(QString sMsg)
{
    if (sMsg != "")
    {
        emit readyWrite(sMsg);
    }
}

bool GameData::getAnyImagesOwned()
{
    QList<GameData::ImageDetails> allImages;
    {
        QMutexLocker locker(&mutex);
        allImages = imageLibrary;
    }
    bool bImagesOwned = false;

    foreach (GameData::ImageDetails thisImage, allImages)
    {
        if (thisImage.imageOwned && thisImage.imageActive)
        {
            bImagesOwned = true;
            break;
        }
    }

    return bImagesOwned;
}

void GameData::setNewGame()
{
    emit newGame();
}
void GameData::setResetGame()
{
    emit resetGame();
}

void GameData::setForceScreenUpdate(bool bUpdate)
{
    if (bUpdate)
        emit forceUpdateScreen();
    else
        emit stopUpdateScreen();
}

int GameData::getRobotSpeed()
{
    QMutexLocker locker(&mutex);
    return iRobotSpeed;
}
void GameData::setRobotSpeed(int iSpeed)
{
    QMutexLocker locker(&mutex);
    iRobotSpeed = iSpeed;
}

bool GameData::getRobotReadyToMove()
{
    QMutexLocker locker(&mutex);
    return bRobotReadyToMove;
}
void GameData::setRobotReadyToMove(bool bReady)
{
    QMutexLocker locker(&mutex);
    bRobotReadyToMove = bReady;
}

qint64 GameData::getRobotMoveStart()
{
    QMutexLocker locker(&mutex);
    return iRobotMoveStart;
}
void GameData::setRobotMoveStart(qint64 iStartTime)
{
    QMutexLocker locker(&mutex);
    iRobotMoveStart = iStartTime;
}

void GameData::setCurrOneToShow(int iImageId)
{
    QMutexLocker locker(&mutex);
    iCurrOneAtATime = iImageId;
}
int GameData::getCurrOneToShow()
{
    QMutexLocker locker(&mutex);

    if (iOneToShowShuffled.length() > 0 && iCurrOneAtATime != -1)
        return iOneToShowShuffled[iCurrOneAtATime];
    else
        return -1;
}
void GameData::setNewOneToShow()
{
    QMutexLocker locker(&mutex);

    int iLength = iOneToShowShuffled.length();
    iCurrOneAtATime++;

    if (iCurrOneAtATime > iLength - 1)
        iCurrOneAtATime = -1;
}
void GameData::setShuffleOrder()
{
    QMutexLocker locker(&mutex);
    iOneToShowShuffled = getRandomShuffleLibrary(imageLibrary.length());
}

QString GameData::getCurrOneToShowProps()
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iOneToShowShuffled[iCurrOneAtATime]].imageProps;
}

int GameData::getBezierTargetCat()
{
    QMutexLocker locker(&mutex);
    return iBezierTargetCat;
}
void GameData::setBezierTargetCat(int iCategory)
{
    QMutexLocker locker(&mutex);
    iBezierTargetCat = iCategory;
}

bool GameData::getUseSound()
{
    QMutexLocker locker(&mutex);
    return bUseSound;
}
void GameData::setUseSound(bool bSound)
{
    QMutexLocker locker(&mutex);
    bUseSound = bSound;
}

// category info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QList<GameData::CategoryDetails> GameData::getCatDetails()
{
    QMutexLocker locker(&mutex);
    return categories;
}

void GameData::addCatDetails(GameData::CategoryDetails catDetails)
{
    QMutexLocker locker(&mutex);
    categories.append(catDetails);
}
void GameData::clearCatDetails()
{
    QMutexLocker locker(&mutex);
    categories.clear();
}

int GameData::getNumberOfCats()
{
    QMutexLocker locker(&mutex);
    return categories.length();
}

bool GameData::getCurrOverlap(int iCatNo)
{
    QMutexLocker locker(&mutex);
    return categories[iCatNo].currOverlap;
}
void GameData::setCurrOverlap(int iCatNo, bool bOverlap)
{
    QMutexLocker locker(&mutex);
    categories[iCatNo].currOverlap = bOverlap;
}

QList<int> GameData::getInsideCategory(int iCatNo)
{
    QMutexLocker locker(&mutex);
    return categories[iCatNo].insideMe;
}
void GameData::addInsideCategory(int iCatNo, int iImgId)
{
    QMutexLocker locker(&mutex);
    categories[iCatNo].insideMe.prepend(iImgId);
}

bool GameData::getAnyCategoryFeedback()
{
    QList<GameData::CategoryDetails> allCats;
    {
        QMutexLocker locker(&mutex);
        allCats = categories;
    }
    bool bFeedback = false;

    foreach (GameData::CategoryDetails thisCat, allCats)
    {
        if (thisCat.bShowFeedback)
        {
            bFeedback = true;
            break;
        }
    }

    return bFeedback;
}
bool GameData::getShowCategoryFeedback(int iCatNo)
{
    QMutexLocker locker(&mutex);
    return categories[iCatNo].bShowFeedback;
}
void GameData::setShowCategoryFeedback(int iCatNo, bool bShow)
{
    QMutexLocker locker(&mutex);
    categories[iCatNo].bShowFeedback = bShow;
}

bool GameData::getIsFeedbackCorrect(int iCatNo)
{
    QMutexLocker locker(&mutex);
    return categories[iCatNo].bCorrectFeedback;
}
void GameData::setIsFeedbackCorrect(int iCatNo, bool bCorrect)
{
    QMutexLocker locker(&mutex);
    categories[iCatNo].bCorrectFeedback = bCorrect;
}

void GameData::delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

qint64 GameData::getFeedbackStart(int iCatNo)
{
    QMutexLocker locker(&mutex);
    return categories[iCatNo].iFeedbackStart;
}
void GameData::setFeedbackStart(int iCatNo, qint64 iStart)
{
    QMutexLocker locker(&mutex);
    categories[iCatNo].iFeedbackStart = iStart;
}

void GameData::setCatPos(int iCatId, QPointF qpfMyCentre)
{
    QMutexLocker locker(&mutex);
    categories[iCatId].qpfCatPosition = qpfMyCentre;
}

QString GameData::getCategoryPropertiesById(int iCategoryId)
{
    QMutexLocker locker(&mutex);
    if (categories.length() >= iCategoryId)
        return categories[iCategoryId].catProps;
    else
        return "";
}

QString GameData::getAllCategoryProperties()
{
    QMutexLocker locker(&mutex);
    if (categories.length() > 0)
    {
        QStringList sCatProps;
        foreach (GameData::CategoryDetails thisCat, categories)
        {
            sCatProps.append(thisCat.catProps);
        }

        return sCatProps.join(",");
    }
    else
        return "";
}

// image set info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
QList<GameData::ImageDetails> GameData::getImageDetails()
{
    QMutexLocker locker(&mutex);
    return imageLibrary;
}
void GameData::addImageDetails(GameData::ImageDetails imDetails)
{
    QMutexLocker locker(&mutex);
    imageLibrary.append(imDetails);
}
void GameData::clearImageDetails()
{
    QMutexLocker locker(&mutex);
    imageLibrary.clear();
}

QPointF GameData::getImagePositionById(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].qpfImagePosition;
}
void GameData::setImagePositionById(int iImageId, QPointF qpfPosition)
{
    QMutexLocker locker(&mutex);
    imageLibrary[iImageId].qpfImagePosition = qpfPosition;
}

QSize GameData::getImageSizeById(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].qsImageSize;
}

bool GameData::getImageOwned(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].imageOwned;
}
void GameData::setImageOwned(int iImageId, bool bOwned)
{
    {
        QMutexLocker locker(&mutex);
        imageLibrary[iImageId].imageOwned = bOwned;
    }

    if (bOwned)
    {
        setMsgToSend(_PLAYER_TOUCH_IMAGE_ + "," + QString::number(iImageId));
    }
    else
    {
        setMsgToSend(_PLAYER_RELEASE_IMAGE_ + "," + QString::number(iImageId));
    }
}

bool GameData::getImageActive(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].imageActive;
}
void GameData::setImageActive(int iImageId, bool bActive)
{
    QMutexLocker locker(&mutex);
    imageLibrary[iImageId].imageActive = bActive;
}

int GameData::getCatPlaced(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].catPlaced;
}
void GameData::setCatPlaced(int iImageId, int iCat)
{
    QMutexLocker locker(&mutex);
    imageLibrary[iImageId].catPlaced = iCat;
}

QString GameData::getCatBelonged(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].catBelonged;
}

QString GameData::getImagePropertiesById(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].imageProps;
}

bool GameData::getAnyRobotMoving()
{
    QList<GameData::ImageDetails> allImages;
    {
        QMutexLocker locker(&mutex);
        allImages = imageLibrary;
    }
    bool bImagesOwned = false;

    foreach (GameData::ImageDetails thisImage, allImages)
    {
        if (thisImage.bRobotMoving)
        {
            bImagesOwned = true;
            break;
        }
    }

    return bImagesOwned;
}

// robot move info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool GameData::getRobotMoving(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].bRobotMoving;
}
void GameData::setRobotMoving(int iImageId, bool bRobotMoving)
{
    QMutexLocker locker(&mutex);
    imageLibrary[iImageId].bRobotMoving = bRobotMoving;
}

int GameData::getRobotMoveTimeMs(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].iRobotMoveTime;
}
void GameData::setRobotMoveTimeMs(int iImageId, int iTimeLength)
{
    QMutexLocker locker(&mutex);
    imageLibrary[iImageId].iRobotMoveTime = iTimeLength;
}

QPainterPath GameData::getRobotMovePath(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].ppBezier;
}
void GameData::setRobotMovePath(int iImageId, QPainterPath ppBezierIn)
{
    QMutexLocker locker(&mutex);
    imageLibrary[iImageId].ppBezier = ppBezierIn;
}

QPointF GameData::getPointOnRobotMove(int iImageId, float flTParamater)
{
    QMutexLocker locker(&mutex);

    if (imageLibrary[iImageId].bRobotMoving)
        return imageLibrary[iImageId].ppBezier.pointAtPercent(flTParamater);
    else
        return imageLibrary[iImageId].qpfImagePosition;
}

void GameData::setRobotOwned(int iImageId, bool bOwned)
{
    QMutexLocker locker(&mutex);
    imageLibrary[iImageId].bRobotLastOwner = bOwned;
}
bool GameData::getRobotOwned(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].bRobotLastOwner;
}

void GameData::setRobotLocked(bool bLock)
{
    QMutexLocker locker(&mutex);
    bCollisionCheck = false;
    bRobotLocked = bLock;
    bCollisionCheck = true;
}
bool GameData::getRobotLocked()
{
    QMutexLocker locker(&mutex);
    return bRobotLocked;
}

QList<QPointF> GameData::getMovePoints(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].qpflBezPoints;
}
void GameData::setMovePoints(int iImageId, QList<QPointF> qpflBezier)
{
    QMutexLocker locker(&mutex);
    imageLibrary[iImageId].qpflBezPoints = qpflBezier;
}

QList<int> GameData::getRandomShuffleLibrary(int iImagesInLib)
{
    QList<int> iNumbersUnused;
    QList<int> iReturn;
    int iRandom = 0;

    // create list of indexes available
    for (int iCount = 0; iCount < iImagesInLib; iCount++)
        iNumbersUnused.append(iCount);

    // loop and randomly re-assign the indexes until they're all used
    while (iNumbersUnused.length() > 0)
    {
        iRandom = getRandomNumber(0, iNumbersUnused.length() - 1);

        iReturn.append(iNumbersUnused[iRandom]);
        iNumbersUnused.removeAt(iRandom);
    }

    return iReturn;
}
int GameData::getRandomNumber(int min, int max)
{
    // prevent division by 0
    if ((max - min) <= 0)
        return 0;
    else
        return (qrand() % ((max + 1) - min)) + min;
}

void GameData::setTurnTakeMode(bool bTurnTake)
{
    QMutexLocker locker(&mutex);
    bTurnTakeMode = bTurnTake;
}

bool GameData::getTurnTakeMode()
{
    QMutexLocker locker(&mutex);
    return bTurnTakeMode;
}

void GameData::setImageGreenBorder(int iImageId, bool bBorder)
{
    QMutexLocker locker(&mutex);
    imageLibrary[iImageId].bGreenBorder = bBorder;
}

bool GameData::getImageGreenBorder(int iImageId)
{
    QMutexLocker locker(&mutex);
    return imageLibrary[iImageId].bGreenBorder;
}
