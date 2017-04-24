# dream-sandtray-category

This is the sandtray category game as used in the DREAM project (http://www.dream2020.eu/). Please see the license file if you wish to use the code. Below is a guide for configuring and building the software. The code has been extensively tested on Windows; the Linux build has been neglected since early development so it may need tweaks to compile.

The aim of the game is to put images into the correct category or to select a favourite from a set of choices. The game is designed to be used with a robot; communication with a robot is handled via socket connections (urbi send and receive classes; naming due to legacy use of urbi on the robot side, but it is agnostic).

Please note that this work was done for a European project and is no longer under development by myself. I do not intend to provide a detailed guide to the code, but if you do wish to use the code and anything is unclear then get in touch and I'll see what I can do :)

### Guidance for building and running the Qt Sandtray

REQUIRED: Qt SDK 4.8.5 (recommended Qt Creator 3.0.1)

1. When creating your options file, set the build directory for both debug and release to the same folder - specific for your OS. The folder for building must be above the source directory or QT complains - still builds, but it's annoying!

2. 
   1. The project is now intended to be built with statically linked libraries - this should require no action if you have a standard Qt development install. To use Phonon, if your Qt is built for static, then you need to rebuild for shared; for windows:
      $ configure -shared
      $ nmake sub-src
      (cup of coffee...)

   2. Make sure all Qt libraries are installed (at last test, the repo version on Linux doesn't have everything needed!)

   3. Make sure phonon and Qt core versions match exactly

3. When trying to deploy on another machine, the Qt libraries used need to be copied into the same directory as the executable, these are:

   - QtCore4.(dll/lib etc)

   - QtGui4

   - QtNetwork4

   - phonon4

   - QtOpenGL4 (not used in project, but phonon is dependent)

     With the above libraries, double-check the names; there are many similar ones with _d in the name which are for debug and not release.

   - To make phonon work when deployed, a further set of libraries are needed. These go inside a folder called 'phonon_backend' in the release folder:

     - phonon4
     - phonon_ds94 (found in C:\Qt\4.8.5\plugins\phonon_backend on Windows)


4. Make sure that the settings.ini file is in the right place relative to the executable (one directory level higher).  Everything else can be specified from the settings file; this is the only file that has to have a fixed position relative to the executable. The settings file path is defined in gamedata.cpp - if you are building for Mac then there is a line to uncomment, as the executable location is different on Mac vs Linux/Windows.

5. This has been tested on Windows for development and deployment, and Linux/Mac in development only. Most of it is very straightforward; Phonon causes the most problems if included. In theory, the whole project is entirely cross platform and should work with either the standard g++ or Microsoft Visual Studio 2010 compilers, however, it hasn't been thoroughly tested.

6. If you are on a machine which isn't touchscreen, then see line 28 in main.cpp to turn on/off the cursor (useful for testing).

7. To get the most out of the game with a robot (and make it work in some cases), image libraries should be formatted as follows:
   - Top level folder - as specified in settings.ini (default is 'libraries' one directory above the executable)
     - Libraries: in separate folders, named, e.g. lib1_mode0_food, lib2_mode1_aliens, lib3_prime_numbers - they will be shown in alphabetical order; properties can be sent to the robot and are split by underscores, starting at the second position. The mode0 and mode1 tags are used to switch the game format between sorting (mode0) and favourite selection (mode1).
   - Fixed elements: naming convention SENXFIL_desc, where X is a number representing the order and 'desc' describes the image.
   - Categories: naming convention SENXBLA_Y_blank, where X is a number representing the order, and Y is a letter representing the unique id for that category.
   - Images: naming convention X123_first_second (e.g. A001_yellow_banana, B002_red_apple), where X is the category unique ID and 123 is a unique identifier for the image (this is unused by the game, so isn't strictly necessary, but keeps from naming clashes)
     Only images in .png or .jpg format will be read in. .png are preferred as they contain transparency; .jpg will look poor.
     Do not put any non-library folders in the top-level libraries folder.

8. To reserve image libraries, i.e. for test sets, or so only the robot can move on to new libraries, change the 'TestLibStart=-1' value to the index of the library you want to start the reservation from; all libraries after this will be reserved.
   To reserve no libraries, then set the value to -1.

9. When modifying the settings file, be sure to use a good text editor - Windows notepad will NOT do - use Notepad++/gedit in Linux/TextWrangler in MacOS. Incorrect line endings in settings causes the program to crash on launch.

If on Windows, jom and clink are thoroughly recommended (http://qt-project.org/wiki/jom ../.. https://code.google.com/p/clink/).
