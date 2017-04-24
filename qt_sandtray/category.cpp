#include "category.h"

// construct the image data and add it to the gamedata currently used
Category::Category(QString sFile, GameData &currData, int idIn)
{
    mGameData = &currData;
    myListSlot = idIn;

    QImage qiImage;
    qiImage.load(sFile);
    mpxImage = QPixmap::fromImage(qiImage);
    miImageHeight = mpxImage.height();
    miImageWidth = mpxImage.width();
    miRungWidth = mGameData->getLadderWidth();
    miNumberOfRungs = mGameData->getLadderRungs();
    miRungHeight = miImageHeight / miNumberOfRungs;   // height of rung = height of im / num of rungs

    if (idIn % 2)
        msLadderSide = "R";     // id odd, so ladder on right
    else
        msLadderSide = "L";     // id even, so ladder left

    QFileInfo fiImage(sFile);
    fiImage.fileName();

    // create image struct and add it to the gamedata list of images
    GameData::CategoryDetails myDetails;
    myDetails.catId = myListSlot;
    myDetails.catName = fiImage.fileName().mid(3,1).toUpper();  // first 3 are 'cat', next letter is name
    myDetails.qsCatSize = QSize(miImageWidth, miImageHeight);
    myDetails.catProps = extractImageProps(fiImage);
    myDetails.qpfCatPosition = scenePos();
    myDetails.ladderSlots = miNumberOfRungs;
    myDetails.rungHeight = miRungHeight;
    myDetails.rungWidth = miRungWidth;
    myDetails.ladderSide = msLadderSide;
    myDetails.ladderPos = getLadderPosition();
    myDetails.currOverlap = false;
    myDetails.bShowFeedback = false;
    myDetails.bCorrectFeedback = false;
    myDetails.iFeedbackStart = 0;

    mGameData->addCatDetails(myDetails);

    setZValue(-1000);                                           // place at the back, so dragImages always go on top
}

// get the image properties from the filename - use underscore delimiter
QString Category::extractImageProps(QFileInfo fiIm)
{
    QRegExp rx("(\\_|\\.)"); //RegEx for '.' or '_'
    QStringList splitString = fiIm.fileName().split(rx);

    splitString.removeAt(0);                    // remove cat/id at start
    splitString.removeAt(splitString.count() - 1);  // remove file type at end

    return splitString.join("_");
}

// get bounding rectange for image - works from top left, so to make image centre of the box
// we need to multiply the image width and height by -0.5 to give the top left point
QRectF Category::boundingRect() const
{
    return QRectF(miImageWidth * -0.5, miImageHeight * -0.5, miImageWidth, miImageHeight);
}

// draws the category on screen - update to change appearance
void Category::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    // make slightly transparent when there is an overlap
    if (mGameData->getCurrOverlap(myListSlot))
        painter->setOpacity(0.2);
    else
        painter->setOpacity(1.0);

    painter->drawPixmap(QPointF(miImageWidth * -0.5, miImageHeight * -0.5), mpxImage);

    // draw a ladder
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(QPen(Qt::black, 2));
    painter->setOpacity(1.0);                               // no opacity on the ladder

    int iImageL = miImageWidth * -0.5;
    int iImageR = miImageWidth * 0.5;
    int iImageT = miImageHeight * -0.5;
    int iImageB = iImageT + (miNumberOfRungs * miRungHeight);
    int iLineLeft, iLineRight;

    if (msLadderSide == "L")
    {
        iLineLeft = iImageL - miRungWidth;
        iLineRight = iImageL;
    }
    else
    {
        iLineLeft = iImageR;
        iLineRight = iImageR + miRungWidth;
    }

    painter->drawLine(QPointF(iLineLeft, iImageT),QPointF(iLineLeft, iImageB));      // draw left line
    painter->drawLine(QPointF(iLineRight, iImageT), QPointF(iLineRight, iImageB));   // draw right line

    // put on the horizontal rungs
    for (int iCount = 0; iCount < miNumberOfRungs + 1; iCount++)
    {
        int iRungY = iImageT + (iCount * miRungHeight);
        painter->drawLine(QPointF(iLineLeft, iRungY), QPointF(iLineRight, iRungY));
    }

    // if something has just been categorised, then overlay the feedback if turned on
    if (mGameData->getShowFeedback())
    {
        if (mGameData->getShowCategoryFeedback(myListSlot))
        {
            // only want to show for a short time
            int iStart = mGameData->getFeedbackStart(myListSlot);
            int iTimeNow = QDateTime::currentMSecsSinceEpoch();
            int iDiff = iTimeNow - iStart;

            // the start is defaulted to 0 - ignore if this is called before the value is updated
            if (iStart != 0)
            {
                if (iDiff > 2000)
                {
                    mGameData->setShowCategoryFeedback(myListSlot, false);
                    mGameData->setForceScreenUpdate(false);
                }
                else
                {
                    QPixmap* pixOverlay;
                    if (mGameData->getIsFeedbackCorrect(myListSlot))
                        pixOverlay = new QPixmap(mGameData->getCorrectFeedback());
                    else
                        pixOverlay = new QPixmap(mGameData->getIncorrectFeedback());

                    float flHScale = pixOverlay->height() / float(miImageHeight);               // scale the image feedback so we can use any size going in
                    float flWScale = pixOverlay->width() / float(miImageWidth);

                    if (flHScale > flWScale)
                       *pixOverlay = pixOverlay->scaledToHeight(miImageHeight);
                    else
                       *pixOverlay = pixOverlay->scaledToWidth(miImageWidth);

                    float flTransparency = 1;
                    if (iDiff == 0)
                        iDiff = 1;  // prevent division by 0

                    if (iDiff <= 500)
                        flTransparency = float(iDiff) / 500;

                    if (iDiff >= 1500)
                        flTransparency = float((2000 - iDiff)) / 500;

                    painter->setOpacity(flTransparency);                                    // apply transparency to our overlay
                    painter->drawPixmap(QPointF(miImageWidth * -0.5, miImageHeight * -0.5), *pixOverlay);       // draw the overlay - use centre as category centre
                    delete pixOverlay;
                }
            }
        }
    }
}

// returns the top-left corner of the ladder
QPointF Category::getLadderPosition()
{
    // x = left side of image - width of rung, y = top of image
    if (msLadderSide == "L")
        return QPointF((miImageWidth * -0.5) - miRungWidth, miImageHeight * -0.5);
    // x = right side of image, y = top of image
    else
        return QPointF(miImageWidth * 0.5, miImageHeight * -0.5);
}
