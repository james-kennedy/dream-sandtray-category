#ifndef CATEG_H
#define CATEG_H

#include <QGraphicsItem>
#include <QtGui>

#include "gamedata.h"

class Category : public QGraphicsItem
{
public:
    Category(QString sFile, GameData &currData, int idIn);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    QString extractImageProps(QFileInfo fiIm);
    QPointF getLadderPosition();

    GameData* mGameData;
    int myListSlot;

    QPixmap mpxImage;
    int miImageHeight;
    int miImageWidth;
    int miRungWidth;
    int miRungHeight;
    int miNumberOfRungs;
    QString msLadderSide;
};

#endif
