#include "librarybutton.h"

LibraryButton::LibraryButton(QString sFile, GameData &currData, bool bNewLibrary)
{
    mGameData = &currData;

    QImage qiImage;
    qiImage.load(sFile);
    mpxImage = QPixmap::fromImage(qiImage);
    miImageHeight = mpxImage.height();
    miImageWidth = mpxImage.width();

    mbNewLibrary = bNewLibrary;         // flag for whether new or reset library
    mbBeingPressed = false;
}

QRectF LibraryButton::boundingRect() const
{
    return QRectF(miImageWidth * -0.5, miImageHeight * -0.5, miImageWidth, miImageHeight);
}

// draws the image on screen - update mimImage to change appearance
void LibraryButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawPixmap(QPointF(miImageWidth * -0.5, miImageHeight * -0.5), mpxImage);

    if (mbBeingPressed)
    {
        QPixmap* pixOverlay = new QPixmap(miImageWidth, miImageHeight);                             // create pixmap overlay the size of the image
        pixOverlay->fill(Qt::red);                                                                  // set the overlay colour
        pixOverlay->setMask(mpxImage.createMaskFromColor(Qt::transparent));                         // don't colour parts of the image which are transparent
        painter->setOpacity(0.4);                                                                   // apply transparency to our overlay
        painter->drawPixmap(QPointF(miImageWidth * -0.5, miImageHeight * -0.5), *pixOverlay);       // draw the overlay
        delete pixOverlay;                                                                          // destroy the item to prevent mem leak
    }
}

// when pressed set the flag which changes the paint appearance
void LibraryButton::mousePressEvent(QGraphicsSceneMouseEvent * event)
{
    Q_UNUSED(event);
    mbBeingPressed = true;
}

// on release reverse flag and signal for new/reset library
void LibraryButton::mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
    Q_UNUSED(event);
    mbBeingPressed = false;

    if (mGameData->getButtonsActive())
    {
        if (mbNewLibrary)
        {
            mGameData->setMsgToSend(_PLAYER_NEW_GAME_);
            mGameData->setNewGame();
        }
        else
        {
            mGameData->setMsgToSend(_PLAYER_RESET_GAME_);
            mGameData->setResetGame();
        }
    }
}
