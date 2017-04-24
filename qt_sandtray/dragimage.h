#ifndef DRAGIMAGE_H
#define DRAGIMAGE_H

#include <QGraphicsItem>
#include <QtGui>
#include <qmath.h>

#include "gamedata.h"

class DragImage : public QGraphicsItem
{
public:
    DragImage(QString sFile, GameData &currData, int idIn);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    QString extractImageProps(QFileInfo fiIm);
    void updatePositionOfImage(QPointF qpfPosition, float flScale = -1);
    void setLadderPositionAndScale();
    int getMyPositionInCategory(int iCatPlaced);
    void setRobotMovePosition();

    GameData* mGameData;
    int myListSlot;

    int miImageHeight;
    int miImageWidth;
    QPixmap mpxImage;
    QPointF qpfPreviousPosition;
    int miLastLadderRung;
    QSize mScaledSize;
    qint64 mqiStartImageMove;
    float mflDistanceMoved;
};

#endif
