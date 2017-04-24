#ifndef CHECKCATEGORISATION_H
#define CHECKCATEGORISATION_H

#include <QObject>
#include <QtGui>
#include <QGraphicsItem>

#include "dragimage.h"
#include "gamedata.h"
#include "messages.h"

class CheckCategorisation : public QObject
{
    Q_OBJECT

public:
    CheckCategorisation(QGraphicsScene &sceneIn, GameData &dataIn);
    ~CheckCategorisation();
    
signals:
    void finished();
    void error(QString err);
    void playSound(QString sFilePath);

public slots:
    void start();
    void killThread();
    
private:
    QGraphicsScene* mainScene;
    GameData* gameData;

    QMutex mutex;                   // mutex for concurrent access
    bool mbRun;
};

#endif // CHECKCATEGORISATION_H
