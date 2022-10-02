#!/bin/bash
# unix script for generating xcode project files

qmake_path="$(which qmake)" 1>&2 >/dev/null
if [ ! -x "$qmake_path" ]; then
   [ -d "/usr/local/qt4" ] && qmake_path="/usr/local/qt4/bin/qmake"
   [ -d "/usr/local/qt5" ] && qmake_path="/usr/local/qt5/bin/qmake"
fi
if [ -x "$qmake_path" ]; then
   for profile in *.pro
   do
      xcodedir=${profile%.*}.xcodeproj
      if [ ! -d $xcodedir ]; then
         "$qmake_path" -spec macx-xcode $profile
      fi
   done
fi
