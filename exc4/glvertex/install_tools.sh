#!/bin/bash
# unix install script
# copyright by Stefan Roettger
# contact: snroettg at gmail

# print information about supported platforms
cat PLATFORMS.txt
echo

# installation directory for package management
INSTALL=/usr/local
[ "$1" != "" ] && [ "$1" != "all" ] && INSTALL="$1"

# installation arguments
ALL=0
[ "$1" = "all" ] || [ "$2" = "all" ] && ALL=1

# installation options
KATE=1
EMACS=1
BRACKETS=$ALL
KDEVELOP=$ALL
QTCREATOR=1
DOXYGEN=1
IMAGEMAGICK=1
GNUPLOT=1
GIMP=$ALL
INKSCAPE=$ALL
BLENDER=$ALL

# sudo command
SUDO=sudo
[ "${INSTALL#$HOME}" != "$INSTALL" ] && SUDO=""

echo "TOOL INSTALLATION IN PROGRESS"
echo

# tools
df="df -k"
ping="ping -c3"
curl="curl -OLkf"
svn="svn --non-interactive --trust-server-cert"
rm="rm -rf"

# check path
echo checking current path...
path=$PWD
echo " current path is $path"
if [ ! "${path// /}" = "$path" ]; then
   echo
   echo CURRENT PATH MUST NOT CONTAIN SPACE
   exit 1
fi

# check capacity
echo detecting drive capacity...
free=`$df . | grep "/" | awk '{print $4}'`
typeset -i space
space=$free/1024/1024
echo " $space"GB" left on drive"
if [ $space -le 1 ]; then
   echo
   echo NO SPACE LEFT ON DRIVE
   exit 1
fi

# internet check
echo detecting internet connection...
$ping www.nsa.gov >& /dev/null
failure=$?
if [ $failure != 0 ]; then
   echo
   echo NO INTERNET CONNECTION
   exit $failure
else
   echo " internet connection is ok"
fi

# platform check
echo detecting platform...
source detection.sh
echo " detected platform: $description"
if [ $type = "unknown" ]; then
   echo
   echo UNSUPPORTED PLATFORM FROM DISTRIBUTOR $distributor
   echo PLEASE INSTALL MANUALLY
   exit 1
fi

# install development tools
failure=0
if [ $type = "debian" ]; then
   # debian and related
   if [ ! -x "$(which g++ 2>/dev/null)" ] || [ ! -x "$(which cmake 2>/dev/null)" ] || [ ! -x "$(which svn 2>/dev/null)" ]; then
      echo installing development tools...
      echo enter root password when prompted...
      sudo $installerquiet update $updateopts
      sudo $installer install $installopts g++ gdb cmake cmake-curses-gui curl subversion git tcsh
      failure=$?
   fi
elif [ $type = "opensuse" ]; then
   # opensuse
   if [ ! -x "$(which g++ 2>/dev/null)" ] || [ ! -x "$(which cmake 2>/dev/null)" ] || [ ! -x "$(which svn 2>/dev/null)" ]; then
      echo installing development tools...
      echo enter root password when prompted...
      sudo $installerquiet update $updateopts
      sudo $installer install $installopts gcc-c++ cmake curl subversion git tcsh
      failure=$?
   fi
elif [ $type = "fedora" ]; then
   # fedora
   if [ ! -x "$(which g++ 2>/dev/null)" ] || [ ! -x "$(which cmake 2>/dev/null)" ] || [ ! -x "$(which svn 2>/dev/null)" ]; then
      echo installing development tools...
      echo enter root password when prompted...
      sudo $installerquiet update $updateopts
      sudo $installer install $installopts gcc-c++ cmake curl subversion git tcsh
      failure=$?
   fi
elif [ $type = "macosx" ]; then
   # macosx
   if [ ! -x "$(which c++ 2>/dev/null)" ]; then
      echo "please install XCode first"
      failure=1
   fi
   if [ $failure = 0 ]; then
      # install cmake from source
      if [ ! -x "$(which cmake 2>/dev/null)" ]; then
         echo installing cmake...
         cmake="cmake"
         cmakeversion="2.8.11"
         if [ $majorversion -ge 1010 ] && [ $majorversion -le 1015 ]; then
            cmakeversion="3.7.2"
         elif [ $majorversion -ge 110 ] && [ $majorversion -le 115 ]; then
            cmakeversion="3.20.6"
         elif [ $majorversion -ge 120 ]; then
            cmakeversion="3.23.0"
         fi
         if [ ! -e $cmake-$cmakeversion.tar.gz ]; then
            cmake_url="http://cmake.org/files/v${cmakeversion%.*}/$cmake-$cmakeversion.tar.gz"
            echo trying to get $cmake_url
            $curl $cmake_url
            if [ -e $cmake-$cmakeversion.tar.gz ]; then
               tar zxf $cmake-$cmakeversion.tar.gz
            fi
         fi
         if [ -d $cmake-$cmakeversion ]; then
            echo enter root password when prompted...
            (cd $cmake-$cmakeversion; ./configure && make -j8 && sudo make install)
            failure=$?
         else
            echo could not download cmake $cmakeversion
            failure=1
         fi
         if [ $failure = 0 ]; then
            $rm $cmake-$cmakeversion
         fi
      fi
   fi
fi
if [ $failure != 0 ]; then
   echo failed to install development tools
fi

# install function
function install_packages() {
   $installerquery $@ >&/dev/null
   done=$?
   if [ ! $done = 0 ]; then
      echo installing dependencies...
      echo enter root password when prompted...
      sudo $installerquiet update $updateopts
      sudo $installer install $installopts $@
      failure=$?
   fi
}

# install dependencies
if [ $failure = 0 ]; then
   if [ $type = "debian" ]; then
      # debian and related
      install_packages mesa-utils freeglut3-dev xorg-dev
      if [ $platform = "ubuntu" ]; then
         if [ $majorversion -lt 2004 ]; then
            install_packages libqt4-dev libqt4-opengl-dev
         else
            install_packages qtbase5-dev libqt5opengl5-dev
         fi
      elif [ $platform = "mint" ]; then
         if [ $majorversion -lt 20 ]; then
            install_packages libqt4-dev libqt4-opengl-dev
	 else
            install_packages qtbase5-dev libqt5opengl5-dev
         fi
      elif [ $platform = "debian" ]; then
         if [ $majorversion -lt 11 ]; then
            install_packages libqt4-dev libqt4-opengl-dev
	 else
            install_packages qtbase5-dev libqt5opengl5-dev
         fi
      fi
   elif [ $type = "opensuse" ]; then
      # opensuse
      install_packages freeglut-devel xorg-x11-devel
      if [ $majorversion -le 151 ]; then
         install_packages libqt4-devel
      else
         install_packages libqt5-qtbase-devel
      fi
   elif [ $type = "fedora" ]; then
      # fedora
      install_packages freeglut-devel
      install_packages libX11-devel libXrandr-devel libXinerama-devel libXcursor-devel
      install_packages qt5-qtbase-devel
   elif [ $type = "macosx" ]; then
      # macosx
      if [ $majorversion -ge 105 ]; then
         # install Qt from source
         qmake_path="$(which qmake 2>/dev/null)"
         if [ ! -x "$qmake_path" ]; then
            [ -d "/usr/local/qt4" ] && qmake_path="/usr/local/qt4/bin/qmake"
            [ -d "/usr/local/qt5" ] && qmake_path="/usr/local/qt5/bin/qmake"
         fi
         if [ -x "$qmake_path" ]; then
             echo found Qt installation in $qmake_path
         else
            qtdownload="https://download.qt.io/archive/qt"
            qt="qt-everywhere-opensource-src"
            qtoptions=""
            if [ $majorversion = 105 ]; then
               qtversion="4.7.4"
               qtpath="${qtversion%.*}/"
               qtminspace=20
            elif [ $majorversion = 106 ] || [ $majorversion = 107 ]; then
               qtversion="4.8.7"
               qtpath="${qtversion%.*}/$qtversion/"
               qtminspace=20
            elif [ $majorversion = 108 ] || [ $majorversion = 109 ] || [ $majorversion = 1010 ]; then
               qtversion="5.7.1"
               qtpath="${qtversion%.*}/$qtversion/"
               qtminspace=30
            elif [ $majorversion = 1011 ] || [ $majorversion = 1012 ]; then
               qtversion="5.9.9" # LTS until 05/2020
               qtpath="${qtversion%.*}/$qtversion/single/"
               qtminspace=33
            elif [ $majorversion = 1013 ] || [ $majorversion = 1014 ]; then
               qtversion="5.12.12" # LTS until 12/2021
               qt="qt-everywhere-src"
               qtpath="${qtversion%.*}/$qtversion/single/"
               qtminspace=35
               qtoptions="-skip qtwebengine"
            elif [ $majorversion = 1015 ]; then
               qtversion="5.15.6" # LTS until 05/2023
               qt="qt-everywhere-src"
               qtpath="${qtversion%.*}/$qtversion/single/"
               qtminspace=30
               qtoptions="-skip qtwebengine"
            elif [ $majorversion -ge 110 ] && [ $majorversion -lt 120 ]; then
               qtversion="6.2.2" # LTS until 09/2024
               qt="qt-everywhere-src"
               qtpath="${qtversion%.*}/$qtversion/single/"
               qtminspace=10
               qtoptions="-skip qtwebengine"
            elif [ $majorversion -ge 120 ] && [ $majorversion -lt 130 ]; then
               qtversion="6.2.4" # LTS until 09/2024
               qt="qt-everywhere-src"
               qtpath="${qtversion%.*}/$qtversion/single/"
               qtminspace=10
               qtoptions="-skip qtwebengine"
            elif [ $majorversion -ge 130 ] && [ $majorversion -lt 140 ]; then
               qtversion="6.3.2"
               qt="qt-everywhere-src"
               qtpath="${qtversion%.*}/$qtversion/single/"
               qtminspace=10
               qtoptions="-skip qtwebengine"
            else
               echo Qt is not supported on this platform
               qtversion=""
            fi
            if [ $qtversion != "" ]; then
               if [ $space -lt $qtminspace ]; then
                  echo
                  echo you only have $space"GB" left
                  echo you need at least $qtminspace"GB"
                  echo
                  echo NOT ENOUGH SPACE LEFT ON DRIVE
                  exit 1
               fi
               if [ "$PWD" = *\ * ]; then
                  echo
                  echo your path contains white space
                  echo you need to remove or replace it
                  echo
                  echo IMPROPER DIRECTORY PATH
                  exit 1
               fi
               echo installing Qt $qtversion
               if [ ! -e $qt-$qtversion.tar.gz ]; then
                  qt_url1="$qtdownload/$qtpath$qt-$qtversion.tar.gz"
                  echo trying to get $qt_url1
                  $curl $qt_url1
                  if [ -e $qt-$qtversion.tar.gz ]; then
                     tar zxf $qt-$qtversion.tar.gz
                  else
                     qt_url2="$qtdownload/$qtpath$qt-$qtversion.tar.xz"
                     echo trying to get $qt_url2
                     $curl $qt_url2
                     if [ -e $qt-$qtversion.tar.xz ]; then
                        tar zxf $qt-$qtversion.tar.xz
                     fi
                  fi
               fi
               if [ -d $qt-$qtversion ]; then
                  cd $qt-$qtversion
                  options="-opengl -no-pch -no-framework ${qtoptions}"
                  prefix="/usr/local/qt${qtversion%.*.*}"
                  ./configure $options -prefix $prefix -release -nomake examples -nomake tests -opensource -confirm-license
                  failure=$?
                  if [ $failure = 0 ]; then
                     echo enter root password when prompted...
                     cmake --build . --parallel && sudo cmake --install .
                     failure=$?
                  fi
                  if [ $failure = 0 ]; then
                     sudo ln -s $prefix/bin/qmake /usr/local/bin
                  fi
                  cd ..
               else
                  echo could not download Qt $qtversion
                  failure=1
               fi
               if [ $failure = 0 ]; then
                  $rm $qt-$qtversion
               fi
            fi
         fi
      else
         echo failed to install Qt
         failure=1
      fi
   else
      echo unsupported platform: $type
      failure=1
   fi
   failure=0 # ignore qt failures
fi
if [ $failure = 0 ]; then
   glfw=0
   glfwversion="3.2.1"
   [ $type = "debian" ] && [ ! -e "/usr/include/GLFW/glfw3.h" ] && glfw=1
   [ $type = "debian" ] && [ $platform = "ubuntu" ] && [ $majorversion -ge 2004 ] && glfwversion="3.3.2"
   [ $type = "opensuse" ] && [ ! -e "/usr/include/GLFW/glfw3.h" ] && glfw=1
   [ $type = "fedora" ] && [ ! -e "/usr/include/GLFW/glfw3.h" ] && glfw=1
   [ $type = "macosx" ] && [ $majorversion -ge 1010 ] && glfw=1
   if [ $glfw != 0 ]; then
      # install glfw from source
      if [ ! -e "$INSTALL/include/GLFW/glfw3.h" ]; then
         echo installing glfw
         if [ ! -d glfw ]; then
            glfw_url="https://github.com/glfw/glfw/tags/$glfwversion"
            echo trying to download $glfw_url
            $svn export $glfw_url glfw
         fi
         if [ -d glfw ]; then
            if [ $SUDO != "" ]; then echo enter root password when prompted...; fi
            (cd glfw; cmake -DCMAKE_INSTALL_PREFIX="$INSTALL" . && make -j8 && $SUDO make install)
            failure=$?
         else
            echo could not download glfw
            failure=1
         fi
         if [ $failure = 0 ]; then
            $rm glfw
         fi
      fi
   fi
   failure=0 # ignore glfw failures
fi
if [ $failure != 0 ]; then
   echo automatic installation is not supported on this platform
   echo please install the dependencies manually
fi

# install optional dependencies
if [ $failure = 0 ]; then
   bullet=0
   [ $type = "debian" ] && [ ! -e "/usr/local/include/bullet/btBulletDynamicsCommon.h" ] && bullet=1
   [ $type = "opensuse" ] && [ ! -e "/usr/local/include/bullet/btBulletDynamicsCommon.h" ] && bullet=1
   if [ $bullet != 0 ]; then
      # install bullet
      if [ $type = "debian" ]; then
         # debian and related
         install_packages libbullet-dev libbullet-extras-dev
      elif [ $type = "opensuse" ]; then
         # opensuse
         install_packages libbullet-devel
      fi
   fi
   # install opencv
   if [ $type = "debian" ]; then
      # debian and related
      install_packages libopencv-dev
   elif [ $type = "opensuse" ]; then
      # opensuse
      install_packages opencv-devel
   fi
   # install ncurses and sdl
   if [ $type = "debian" ]; then
      # debian and related
      install_packages libncurses5-dev
      if [ $platform = "ubuntu" ] && [ $majorversion -le 2104 ]; then
          install_packages libsdl1.2-dev libsdl-mixer1.2-dev joystick
      else
          install_packages libsdl2-dev libsdl2-mixer-dev joystick
      fi
   fi
fi

# install editors
if [ $failure = 0 ]; then
   # install kate
   if [ ! $KATE = 0 ]; then
      if [ ! -x "$(which kate 2>/dev/null)" ]; then
         if [ $type = "debian" ] || [ $type = "opensuse" ]; then
            sudo $installer install $installopts kate
         fi
      fi
   fi
fi
if [ $failure = 0 ]; then
   # install emacs
   if [ ! $EMACS = 0 ]; then
      if [ ! -x "$(which emacs 2>/dev/null)" ]; then
         if [ $type = "debian" ] && [ $platform = "ubuntu" ] && [ $majorversion -lt 1710 ]; then
            sudo $installer install $installopts emacs24
         elif [ $type = "debian" ] || [ $type = "opensuse" ] || [ $type = "fedora" ]; then
            sudo $installer install $installopts emacs
         fi
      fi
   fi
fi
if [ $failure = 0 ]; then
   # install brackets
   if [ ! $BRACKETS = 0 ]; then
      if [ ! -x "$(which brackets 2>/dev/null)" ]; then
         if [ $platform = "ubuntu" ]; then
            if [ $majorversion = 1604 ]; then
               sudo add-apt-repository --yes ppa:webupd8team/brackets
               sudo $installerquiet update $updateopts
               sudo $installer install $installopts brackets
            elif [ $majorversion -ge 1804 ]; then
               snap install --classic brackets
            fi
         elif [ $type = "opensuse" ]; then
            if [ $majorversion -ge 132 ] && [ $majorversion -lt 150 ]; then
               sudo $installerquiet update $updateopts
               sudo $installer install $installopts brackets
            fi
         fi
      fi
   fi
fi

# install generators
if [ $failure = 0 ]; then
   # install doxygen
   if [ ! $DOXYGEN = 0 ]; then
      if [ ! -x "$(which doxygen 2>/dev/null)" ]; then
         if [ $type = "debian" ] || [ $type = "opensuse" ]; then
            sudo $installer install $installopts doxygen graphviz
         elif [ $platform = "fedora" ]; then
            sudo $installer install $installopts doxygen graphviz
         fi
      fi
   fi
fi

# install IDEs
if [ $failure = 0 ]; then
   # install kdevelop
   if [ ! $KDEVELOP = 0 ]; then
      if [ ! -x "$(which kdevelop 2>/dev/null)" ]; then
         if [ $type = "debian" ]; then
            sudo $installer install $installopts kdevelop
         elif [ $type = "opensuse" ]; then
            if [ $majorversion -lt 150 ]; then
               sudo $installer install $installopts kdevelop4
            else
               sudo $installer install $installopts kdevelop5
            fi
         elif [ $type = "fedora" ]; then
            sudo $installer install $installopts kdevelop4
         fi
      fi
   fi
fi
if [ $failure = 0 ]; then
   # install qtcreator
   if [ ! $QTCREATOR = 0 ]; then
      if [ ! -x "$(which qtcreator 2>/dev/null)" ]; then
         if [ $type = "debian" ]; then
            sudo $installer install $installopts qtcreator
            if [ $platform = "ubuntu" ]; then
                sudo $installer install $installopts qt5-doc
            fi
         elif [ $type = "opensuse" ]; then
            if [ $majorversion -lt 150 ]; then
               sudo $installer install $installopts qtcreator
            else
               sudo $installer install $installopts libqt5-creator
            fi
         fi
      fi
   fi
fi

# install image tools
if [ $failure = 0 ]; then
   # install imagemagick
   if [ ! $IMAGEMAGICK = 0 ]; then
      if [ ! -x "$(which convert 2>/dev/null)" ]; then
         if [ $type = "debian" ] || [ $type = "opensuse" ]; then
            sudo $installer install $installopts imagemagick
         elif [ $platform = "fedora" ]; then
            sudo $installer install $installopts ImageMagick
         fi
      fi
   fi
fi
if [ $failure = 0 ]; then
   # install gnuplot
   if [ ! $GNUPLOT = 0 ]; then
      if [ ! -x "$(which gnuplot 2>/dev/null)" ]; then
         if [ $type = "debian" ] || [ $type = "opensuse" ] || [ $type = "fedora" ]; then
            sudo $installer install $installopts gnuplot
         fi
      fi
   fi
fi
if [ $failure = 0 ]; then
   # install gimp
   if [ ! $GIMP = 0 ]; then
      if [ ! -x "$(which gimp 2>/dev/null)" ]; then
         if [ $type = "debian" ] || [ $type = "opensuse" ] || [ $type = "fedora" ]; then
            sudo $installer install $installopts gimp
         fi
      fi
   fi
fi
if [ $failure = 0 ]; then
   # install inkscape
   if [ ! $INKSCAPE = 0 ]; then
      if [ ! -x "$(which inkscape 2>/dev/null)" ]; then
         if [ $type = "debian" ]; then
            sudo $installer install $installopts inkscape
         fi
      fi
   fi
fi

# install 3D tools
if [ $failure = 0 ]; then
   # install blender
   if [ ! $BLENDER = 0 ]; then
      if [ ! -x "$(which blender 2>/dev/null)" ]; then
         if [ $type = "debian" ] && [ $platform = "ubuntu" ] && [ $majorversion -ge 1804 ]; then
            snap install --classic blender
         elif [ $type = "debian" ] || [ $type = "opensuse" ] || [ $type = "fedora" ]; then
            sudo $installer install $installopts blender
         fi
      fi
   fi
fi

# final summary
echo
if [ $failure = 0 ]; then
   echo TOOL INSTALLATION COMPLETE
else
   echo TOOL INSTALLATION INCOMPLETE
fi

exit $failure
