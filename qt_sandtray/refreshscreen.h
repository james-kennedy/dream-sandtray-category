#ifndef REFRESHSCREEN_H
#define REFRESHSCREEN_H

#include <QObject>
#include <QtGui>
#include <QGraphicsItem>

#include "gamedata.h"

class RefreshScreen : public QObject
{
    Q_OBJECT

public:
    RefreshScreen(QGraphicsScene &sceneIn, GameData &currData);

signals:
    void finished();
    void error(QString err);

public slots:
    void start();
    void startUpdate();
    void stopUpdate();
    void killThread();

private:
    QGraphicsScene* mainScene;
    GameData* mGameData;

    QMutex mutex;                   // mutex for concurrent access
    bool mbRun;
};

#endif // REFRESHSCREEN_H
