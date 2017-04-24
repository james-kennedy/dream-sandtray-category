#ifndef LIBRARYBUTTON_H
#define LIBRARYBUTTON_H

#include <QGraphicsItem>
#include <QtGui>

#include "gamedata.h"

class LibraryButton : public QGraphicsItem
{

public:
    LibraryButton(QString sFile, GameData &currData, bool bNewLibrary);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    GameData* mGameData;
    bool bType;

    int miImageHeight;
    int miImageWidth;
    QPixmap mpxImage;
    bool mbNewLibrary;
    bool mbBeingPressed;
};

#endif // LIBRARYBUTTON_H
