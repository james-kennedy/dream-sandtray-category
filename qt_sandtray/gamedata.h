#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <QObject>
#include <QtGui>

#include "messages.h"

class GameData : public QObject
{
    Q_OBJECT

public:
    explicit GameData();

    // general library/player info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void retrieveSettingsFromFile(QString sFile);
    QString getServerIP();
    QString getLibraryPath();
    bool getUseRobot();
    int getLadderWidth();
    int getLadderRungs();
    QString getRightSound();
    QString getWrongSound();
    QString getNewLibButton();
    QString getResetLibButton();
    QPixmap getCorrectFeedback();
    QPixmap getIncorrectFeedback();
    bool getShowFeedback();
    void setShowFeedback(bool bFeedback);
    bool getOneAtATime();
    void setOneAtATime(bool bBoolIn);
    bool getCentreImages();

    QSize getScreenSize();
    void setScreenSize(QSize iScreenSizeIn);

    bool getCollisionCheck();
    void setCollisionCheck(bool bCheck);

    int getLibraryId();
    void setLibraryId(int iLibraryId);

    QString getLibraryProperties();
    void setLibraryProperties(QString sLibPropsIn);

    bool getLibraryTestReserved();

    int getLibraryLimit();
    void setLibraryLimit(int iLimit);

    bool getButtonsActive();
    void setButtonsActive(bool bActive);

    bool getShowButtons();
    void setShowButtons(bool bShow);

    QList<float> getFullTimeList();
    void setNewTime(float flTime);
    float getAverageTime();
    float getLastTime();
    void clearTimeList();

    QList<float> getFullDelayList();
    void setNewDelay(float flDelay);
    float getAverageDelay();
    float getLastDelay();
    void clearDelayList();

    QList<float> getFullDistanceList();
    void setNewDistance(float flDistance);
    float getAverageDistance();
    float getLastDistance();
    void clearDistanceList();

    QList<float> getFullSpeedList();
    void setNewSpeed(float flSpeed);
    float getAverageSpeed();
    float getLastSpeed();
    void clearSpeedList();

    qint64 getLastMoveEnd();
    void setLastMoveEnd(qint64 iTime);

    int getNumberPlayerRightMoves();
    void setPlayerRightMove(int iImageId, int iCategoryId);
    int getNumberPlayerWrongMoves();
    void setPlayerWrongMove(int iImageId, int iCategoryId);
    void resetPlayerScore();

    void setRobotMove(int iImageId, bool bCorrect, int iCategoryId);
    int getLastRobotMoveId();
    bool getLastRobotMoveCorrect();

    bool getLastCategorisationCorrect();
    int getLastImageCategorised();

    QString getLastPlayerCatProps();

    void setMsgToSend(QString sMsg);

    bool getAnyImagesOwned();

    void setNewGame();
    void setResetGame();

    void setForceScreenUpdate(bool bUpdate);

    int getRobotSpeed();
    void setRobotSpeed(int iSpeed);

    bool getRobotReadyToMove();
    void setRobotReadyToMove(bool bReady);

    qint64 getRobotMoveStart();
    void setRobotMoveStart(qint64 iStartTime);

    void setCurrOneToShow(int iImageId);
    int getCurrOneToShow();
    void setNewOneToShow();
    void setShuffleOrder();

    QString getCurrOneToShowProps();

    int getBezierTargetCat();
    void setBezierTargetCat(int iCategory);

    bool getUseSound();
    void setUseSound(bool bSound);

    // category info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    struct CategoryDetails
    {
        int catId;                      // unique id - the position in the gameData list for this category
        QString catName;                // used as a string comparison to the images to see if right/wrong
        QString catProps;               // properties of this category
        QPointF qpfCatPosition;         // centre point on screen of this category in pixels
        QString ladderSide;             // the side on which the category is placed relative to this category (L or R)
        QPointF ladderPos;              // top left of the ladder for this category
        QList<int> insideMe;            // images categorised in this cat already (id of dragimage)
        QSize qsCatSize;                // size of the category image in pixels
        bool currOverlap;               // a current, owned, overlap
        bool bShowFeedback;             // show feedback at the current time
        bool bCorrectFeedback;          // should the feedback be right or wrong
        qint64 iFeedbackStart;          // the time when feedback started - used to calc transparency and finish
        int ladderSlots;                // total number of ladder slots
        int rungHeight;                 // height of a ladder rung (cat height / INUMRUNGS)
        int rungWidth;                  // width of a ladder rung
    };

    QList<CategoryDetails> getCatDetails();
    void addCatDetails(CategoryDetails catDetails);
    void clearCatDetails();

    int getNumberOfCats();

    bool getCurrOverlap(int iCatNo);
    void setCurrOverlap(int iCatNo, bool bOverlap);

    QList<int> getInsideCategory(int iCatNo);
    void addInsideCategory(int iCatNo, int iImgId);

    bool getAnyCategoryFeedback();
    bool getShowCategoryFeedback(int iCatNo);
    void setShowCategoryFeedback(int iCatNo, bool bShow);

    bool getIsFeedbackCorrect(int iCatNo);
    void setIsFeedbackCorrect(int iCatNo, bool bCorrect);

    qint64 getFeedbackStart(int iCatNo);
    void setFeedbackStart(int iCatNo, qint64 iStart);

    void setCatPos(int iCatId, QPointF qpfMyCentre);

    QString getCategoryPropertiesById(int iCategoryId);
    QString getAllCategoryProperties();

    // image set info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    struct ImageDetails
    {
        int imageId;                    // ID - also refers to position in list
        QString imageProps;             // properties for this image from the filename
        QString catBelonged;            // the correct category for this image
        QPointF qpfImagePosition;       // position of the image (x,y)
        QPainterPath ppBezier;          // the bezier path for a robot move
        QList<QPointF> qpflBezPoints;   // points of the bezier to give to the robot
        QSize qsImageSize;              // image size in pixels
        bool imageOwned;                // owned by user or not - used for drawing and physics
        bool imageActive;               // whether the img is categorised or not
        bool bRobotLastOwner;           // to differentiate for scoring purposes between robot/user
        bool bRobotMoving;              // whether the robot is in the process of moving the image
        int iRobotMoveTime;             // length of time for the whole robot move to take - used for bez calcs
        int catPlaced;                  // stores where the user has categorised the image (if they have)
        bool bGreenBorder;              // whether the img has green border
    };

    QList<ImageDetails> getImageDetails();
    void addImageDetails(ImageDetails imDetails);
    void clearImageDetails();

    QPointF getImagePositionById(int iImageId);
    void setImagePositionById(int iImageId, QPointF qpfPosition);

    QSize getImageSizeById(int iImageId);

    bool getImageOwned(int iImageId);
    void setImageOwned(int iImageId, bool bOwned);

    bool getImageActive(int iImageId);
    void setImageActive(int iImageId, bool bActive);

    int getCatPlaced(int iImageId);
    void setCatPlaced(int iImageId, int iCat);

    QString getCatBelonged(int iImageId);

    QString getImagePropertiesById(int iImageId);

    bool getAnyRobotMoving();
    bool getRobotMoving(int iImageId);
    void setRobotMoving(int iImageId, bool bRobotMoving);

    int getRobotMoveTimeMs(int iImageId);
    void setRobotMoveTimeMs(int iImageId, int flTimeLength);

    QPainterPath getRobotMovePath(int iImageId);
    void setRobotMovePath(int iImageId, QPainterPath ppBezierIn);

    QPointF getPointOnRobotMove(int iImageId, float flTParamater);

    void setRobotOwned(int iImageId, bool bOwned);
    bool getRobotOwned(int iImageId);

    void setRobotLocked(bool bLock);
    bool getRobotLocked();

    QList<QPointF> getMovePoints(int iImageId);
    void setMovePoints(int iImageId, QList<QPointF> qpflBezier);

    QList<int> getRandomShuffleLibrary(int iImagesInLib);
    int getRandomNumber(int min, int max);

    void setImageGreenBorder(int iImageId, bool bBorder);
    bool getImageGreenBorder(int iImageId);

    void setTurnTakeMode(bool bTurnTake);
    bool getTurnTakeMode();

signals:
    void readyWrite(QString sMessage);
    void newGame();
    void resetGame();
    void forceUpdateScreen();
    void stopUpdateScreen();

private:
    QPainterPath calculateBezierPath(int iImageId);

    QMutex mutex;                   // mutex for concurrent access

    // general library/player info
    QString sLibPath;               // root library directory - set in settings.txt
    QString sServerIP;              // IP of the server - set in settings.txt
    QString sRightSound;            // path for the sound of a correct categorisation
    QString sWrongSound;            // as above for incorrect
    QString sNewLibButton;          // path to the new library button image
    QString sResetLibButton;        // path to the reset library button image
    QString sCorrectFeedback;       // path to the correct feedback image
    QString sIncorrectFeedback;     // path to the incorrect feedback image
    QPixmap pmCorrectFeedback;      // pixmap copy of the correct feedback image
    QPixmap pmIncorrectFeedback;    // pixmap copy of the incorrect feedback image
    QSize iScreenSize;              // screen size in pixels
    QString sLibProps;              // library properties from directory name
    QList<int> iListCategorised;    // list of id's of categorised images - insert to front, so first is newest
    QList<int> iListCorrectCats;    // 0 = wrong, 1 = correct - insert to front - can tie in with above
    QList<int> iListInCategory;     // category the image was put in - to return properties to urbi; same order as above
    QList<float> flListTimes;       // list of move times
    QList<float> flListDelays;      // list of move delays
    QList<float> flListDists;       // list of move distances
    QList<float> flListSpeeds;      // list of move speeds (pixels per second)
    QList<int> iRobotCategorised;   // list of robot categorised images
    QList<bool> bRobotCorrect;      // true for correct, false incorrect; same order as above
    QList<int> iRobotCategory;      // category the robot put image in; same order as above
    QList<int> iOneToShowShuffled;  // shuffled order for the showing one at a time
    bool bUseRobot;                 // flag for standalone, or with robot game engine
    bool bCollisionCheck;           // flag to turn on/off collision checking
    bool bRobotReadyToMove;         // flag to start moving images by robot
    int iRobotSpeed;                // speed the robot moves at - used for bezier calcs
    qint64 iRobotMoveStart;         // start in msecs at start of move (use QDateTime::currMSecsSinceEpoch)
    qint64 iLastMoveEnd;            // end of last move in msecs since epoch
    int iLadderWidth;               // for storing the width of ladder rungs in pix from settings file
    int iLadderSlots;               // as above, for the slots in ladder
    int iCurrentLibrary;            // id of the current image library used
    bool bReserveTestLibs;          // keep back certain libraries for tests
    int iMaxLibrary;                // the max library before the normal buttons loop around (used in conjunction with above)
    bool bButtonsActive;            // whether the user can change the library or not
    bool bShowButtons;              // whether to show the new/reset library buttons
    bool bShowFeedback;             // whether to show visual feedback on categorisations
    bool bRobotLocked;              // whether drag images should be locked to the user
    bool bOneAtATime;               // whether we are showing one-at-a-time or not
    int iCurrOneAtATime;            // variable storing the current image to show if we are doing it one at a time
    int iBezierTargetCat;           // target category for the robot move
    bool bCentreImages;             // centre images when showing one-at-a-time
    bool bTurnTakeMode;             // switch program operation based on game mode
    bool bUseSound;                 // switch sound on/off

    QList<CategoryDetails> categories;      // category info
    QList<ImageDetails> imageLibrary;       // image set info
    void delay();
};

#endif // GAMEDATA_H
