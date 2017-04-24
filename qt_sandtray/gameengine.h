#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <QtGui>
#include <phonon/AudioOutput>
#include <phonon/MediaObject>
#include <phonon/MediaSource>

#include "librarymanager.h"
#include "category.h"
#include "checkcategorisation.h"
#include "urbisend.h"
#include "urbireceive.h"
#include "refreshscreen.h"

class GameEngine : public QObject
{
    Q_OBJECT

public:
    explicit GameEngine(QGraphicsScene &sceneIn);
    ~GameEngine();

    void createScene();

public slots:
     void errorString(QString sErr);
     void playSound(QString sFilePath);

signals:
     void appClosing();

private:
    QGraphicsScene* mainScene;
    LibraryManager* clsLibManager;

    CheckCategorisation* collision;
    QThread* collisionThread;

    RefreshScreen* refresh;
    QThread* refreshThread;

    QFile fiSoundFile;
    bool bUpdateScreen;
    Phonon::MediaObject *music;
};

#endif // GAMEENGINE_H
