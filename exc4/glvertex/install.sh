#!/bin/bash
# unix install script
# copyright by Stefan Roettger
# contact: snroettg at gmail

# installation directory for glVertex headers
INSTALL=/usr/local
[ "$GLVERTEX_PATH" != "" ] && INSTALL="$GLVERTEX_PATH"
[ "$1" != "" ] && [ "$1" != "lib" ] && [ "$1" != "all" ] && INSTALL="$1"

# installation arguments
OPTION=""
[ "$1" = "lib" ] || [ "$2" = "lib" ] && OPTION="lib"
[ "$1" = "all" ] || [ "$2" = "all" ] && OPTION="all"

# install tools for supported platforms
failure=0
if [ "$OPTION" != "lib" ]; then
   ./install_tools.sh $OPTION
   failure=$?
fi

# sudo command
SUDO=sudo
[ "${INSTALL#$HOME}" != "$INSTALL" ] && SUDO=""

# installation notification
if [ $failure = 0 ]; then
   echo
   echo "FRAMEWORK INSTALLATION IN PROGRESS"
fi

# install glVertex
if [ $failure = 0 ]; then
   install="$INSTALL"/include
   if [ -e "$install"/glvertex.txt ]; then
      diff README.txt "$install"/glvertex.txt >&/dev/null
      equality=$?
      if [ $equality == 0 ]; then
         echo glVertex headers already exist in $install
      else
         $SUDO rm "$install"/glslmath.h "$install"/glvertex*.h "$install"/glvertex.txt >&/dev/null
         echo removed existing glVertex headers from $install
      fi
   fi
   if [ ! -e "$install"/glvertex.txt ]; then
      echo installing glVertex headers...
      if [ $SUDO != "" ]; then echo enter root password when prompted...; fi
      if [ ! -d "$install" ]; then $SUDO mkdir -p "$install"; fi
      $SUDO cp -fp README.txt "$install"/glvertex.txt >&/dev/null
      $SUDO cp -fp *.h "$install" >&/dev/null
      failure=$?
      if [ $failure = 0 ]; then
         VERSION=$(grep "glVertex version" README.txt | sed -e 's/.*version\ \([0-9\.]*\).*/\1/')
         echo installed glVertex $VERSION headers in $install
      fi
      install="$INSTALL"/share
      if [ ! -d "$install" ]; then $SUDO mkdir -p "$install"; fi
      $SUDO cp -fp *.obj "$install" >&/dev/null
      if [ -d "obj" ]; then
         $SUDO cp -fp obj/*.obj "$install" >&/dev/null
      fi
   fi
fi

# installation success notification
if [ $failure = 0 ]; then
   echo FRAMEWORK INSTALLATION COMPLETE
fi

# detect Qt version
qt=""
if [ $failure = 0 ]; then
   echo
   qmake_path="$(which qmake 2>/dev/null)"
   if [ ! -x "$qmake_path" ]; then
       [ -d "/usr/local/qt4" ] && qmake_path="/usr/local/qt4/bin/qmake"
       [ -d "/usr/local/qt5" ] && qmake_path="/usr/local/qt5/bin/qmake"
       [ -x "$(which qmake-qt5 2>/dev/null)" ] && qmake_path="$(which qmake-qt5 2>/dev/null)"
   fi
   if [ -x "$qmake_path" ]; then
      echo detecting Qt version...
      qt=`$qmake_path -v | grep "Using" | sed -e 's/.*\ \([0-9]*\.[0-9]*\).*/\1/'`
      echo ...detected Qt version $qt
   else
      echo ...unable to detect Qt
   fi
fi

# run glVertex tests
if [ $failure = 0 ]; then
   if [ -d examples ]; then
      echo running glVertex tests...
      make test
      failure=$?
      if [ $failure = 0 ]; then
         echo ...glVertex tests finished
      else
         echo ...unable to finish glVertex tests successfully
      fi
   fi
fi

# create glVertex documentation
if [ $failure = 0 ]; then
   echo creating glVertex documentation
   make docs
fi

# final installation notification
echo
if [ $failure = 0 ]; then
   echo FRAMEWORK INSTALLATION AND TESTING COMPLETE
fi

exit $failure
