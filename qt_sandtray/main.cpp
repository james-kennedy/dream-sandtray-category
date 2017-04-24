#include <QtGui>
#include <QKeyEvent>

#include "gameengine.h"

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView(QGraphicsScene *scene) : QGraphicsView(scene)
    {
    }

protected:
    // event handler to catch keypresses in the graphicsview and quit the app
    void keyPressEvent(QKeyEvent *keyevent)
    {
        if (keyevent->key()==Qt::Key_Escape) {
               // TODO: emit signal here to kill threads?
            qApp->quit();
        }
    }
};

int main(int argc, char **argv)
{
    try
    {
        QApplication app(argc, argv);
        //app.setOverrideCursor(QCursor(Qt::BlankCursor));  // useful to turn off for debugging on non-touch screen

        // make seed for random number gen - only need to do once at the start, covers all classes
        QTime time = QTime::currentTime();
        qsrand((uint)time.msec());

        // get the window size to use and set the scene to this size
        QDesktopWidget* desk = app.desktop();
        int iScreenWidth, iScreenHeight;

        iScreenWidth = desk->screenGeometry(0).width();
        iScreenHeight = desk->screenGeometry(0).height();

        // if the ratio is large, then assume it's 2 screens and qt hasn't resolved the individual settings for each
        if ((iScreenWidth / iScreenHeight) > 2)
            iScreenWidth = desk->screenGeometry(0).width() / 2;

        QGraphicsScene scene(iScreenWidth * -0.5, iScreenHeight * -0.5, iScreenWidth, iScreenHeight);

        // load the first library and start threads for the game logic/URBI comms
        GameEngine game(scene);
        game.createScene();

        // send the graphics view to the window and display it
        GraphicsView view(&scene);
        view.setRenderHint(QPainter::Antialiasing);                     // nice and sharp for our lines
        view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);  // must be this to redraw our collisions properly
        view.setBackgroundBrush(QColor(255, 255, 255));                 // white background for now
        view.setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);      // prevent scrollbars showing - needed for Win8.1
        view.setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);        // as above
        view.setWindowTitle("Qt Sandtray");
        view.showFullScreen();                                          // covers complete screen including menus etc
        view.setFocus();

        return app.exec();
    }
    catch (std::exception &e)
    {
        qDebug() << "An unhandled error has occurred:" << e.what();
    }
    catch (...)
    {
        qDebug() << "An unhandled error has occurred";
    }
}
