#ifndef BEZIERCLASS_H
#define BEZIERCLASS_H

#include <QObject>
#include <QtGui>

#include "gamedata.h"

class BezierClass
{
public:
    BezierClass(GameData &currData);

    int getImageIdToMove(bool bCorrectMove, bool bToCategory);
    QPainterPath calculateBezierPath(int iImageId, bool bCorrectMove, bool bToCategory);

    int getNumberOfUncategorisedImages();
    int getNumberOfImagesRemaining();
    QList<int> getActiveImageList();

private:
    GameData* mGameData;

    QPoint getPositionOfCategory(bool bCorrectMove, QString sCorrectCat);
    QPointF getPositionOfSpace(int iImageId);
    QPointF getControlPointOne(QPointF qpfStart, QPointF qpfEnd, int iRadius);
    QPointF getControlPointTwo(QPointF qpfStart, QPointF qpfEnd, int iRadius);
    int getRandomNumber(int min, int max);
};

#endif // BEZIERCLASS_H
