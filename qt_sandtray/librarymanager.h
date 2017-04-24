#ifndef LIBRARY_MANAGER_H
#define LIBRARY_MANAGER_H

#include <QtGui>
#include <QObject>

#include "dragimage.h"
#include "category.h"
#include "librarybutton.h"

class LibraryManager : public QObject
{
    Q_OBJECT

public:
    LibraryManager(QGraphicsScene &sceneIn, GameData &dataIn);
   ~LibraryManager();

signals:
    void libraryLoaded();

public slots:
    void nextLibrary();
    void reloadCurrentLibrary();

private:
    void loadLibrary();
    QString extractLibraryProps(QString sLibDirectory);
    bool extractLibraryTurnTakeMode(QString sLibDirectory);
    int getNumberOfCategoriesInLibrary(QDir libDir);
    QPointF getCategoryPosition(int iThisCat, int iTotalCats, int iImWidth, int iImHeight, int iLadderWidth);
    QPointF getImagePosition(int imWidth, int imHeight, bool bTurnTake, int iImgPos, int iTotalImgs);
    QPointF getButtonPosition(QString sFilePath, int iTotalCats, bool bNewLibrary);
    int getRandomNumber(int min, int max);

    QGraphicsScene* mMainScene;
    GameData* mGameData;

    QString msLibRoot;
};

#endif // LIBRARY_MANAGER_H
