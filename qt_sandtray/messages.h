#ifndef MESSAGES_H
#define MESSAGES_H

#include <QObject>

// COMMANDS FROM SERVER
const QString _VERIFY_ = "10";                      // 1..: setup and shutdown
const QString _SHUTDOWN_ = "11";
const QString _FAILURE_ = "12";
const QString _NEW_GAME_ = "22";                    // 2..: game state
const QString _RESET_BOARD_ = "23";
const QString _READY_ = "25";
const QString _MOVE_FINISHED_ = "26";
const QString _GET_USER_DATA_ = "30";               // 3..: data requests
const QString _GET_SCREEN_DATA_ = "31";
const QString _GET_IMAGE_COORDONATES_ = "32";
const QString _GET_BEZIER_DATA_ = "33";
const QString _GET_ID_IMAGE_WILL_MOVE_ = "34";
const QString _GET_IMAGES_LEFT_ = "35";
const QString _GET_ID_IMAGES_CAN_MOVE = "36";
const QString _GET_LAST_IMAGE_PROPS_ = "37";
const QString _PREPARE_MOVE_ = "38";
const QString _SET_SPEED_ = "40";                   // 4..: set data
const QString _SPECIFIED_LEVEL_ = "41";
const QString _SET_BUTTONS_ = "42";
const QString _PLAYER_DONE_GOOD_MOVE_ = "50";       // 5..: asynchronus messages
const QString _PLAYER_DONE_BAD_MOVE_ = "51";
const QString _ROBOT_DONE_BAD_MOVE_ = "52";
const QString _ROBOT_DONE_GOOD_MOVE_ = "53";
const QString _MOVE_TO_SPACE_ = "60";               // 6..: data requests not in old game
const QString _GET_LIBRARY_PROPS_ = "61";
const QString _GET_CATEGORY_PROPS_ = "62";
const QString _LOCK_ALL_IMAGES_ = "63";
const QString _UNLOCK_ALL_IMAGES_ = "64";
const QString _SET_ONE_AT_TIME_ = "65";
const QString _ROBOT_TURN_SELECTION_ = "66";
const QString _GET_SHOWN_IM_PROPS_ = "67";
const QString _SET_FEEDBACK_ON_ = "68";
const QString _SET_FEEDBACK_OFF_ = "69";

// MESSAGES TO SERVER
const QString _GREET_ = "i am a touchscreen 2";
const QString _CONFIRM_ = "confirm";
const QString _FAIL_ = "fail";
const QString _EXIT_ = "exit";
const QString _USER_DATA_ = "user";
const QString _SCREEN_DATA_ = "screen";
const QString _BEZIER_DATA_ = "bezier";
const QString _IMAGE_ID_ = "imgid";
const QString _IMAGES_LEFT_ = "imgsleft";
const QString _IMAGES_IDS_  = "imgsids";
const QString _IMAGE_PROPS_ = "imgprops";
const QString _MOVE_DATA_ = "movedata";
const QString _COORDS_ = "coordinates";
const QString _MOVE_FAIL_ = "movefail";
const QString _NEW_RESET_ = "new or reset underway";
const QString _LIBRARY_DATA_ = "libdata";
const QString _CATEGORY_PROPS_ = "catprops";
const QString _ONE_SHOWN_PROPS_ = "oneshown";
const QString _PLAYER_TOUCH_IMAGE_ = "playertouch";
const QString _PLAYER_RELEASE_IMAGE_ = "playerrelease";
const QString _ROBOT_TURN_LOCATION_ = "turnlocation";

// INTERNAL MESSAGES FOR GAME ENGINE
const QString _PLAYER_MOVE_ = "player completed move";
const QString _ROBOT_MOVE_ = "robot completed move";
const QString _PLAYER_NEW_GAME_ = "player new game";
const QString _PLAYER_RESET_GAME_ = "player reset game";
const int _DEFAULT_SPEED_ = 400;                    // speed in pix per sec

#endif // MESSAGES_H
