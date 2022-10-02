#!/bin/bash
# unix build script

(cd glvertex; ./install.sh)
failure=$?

if [ $failure = 0 ]; then
   ./generate.sh
fi

echo
if [ ! $failure = 0 ]; then
   echo MANUAL INSTALLATION REQUIRED!
else
   echo COMPILATION IN PROGRESS
   echo
   cmake . && make -j8
   failure=$?
   echo
   if [ $failure = 0 ]; then
      echo BUILD COMPLETE
      echo
      ./myqtapp --help
      echo
      echo NOW TYPE: ./myqtapp --help
   else
      echo BUILD INCOMPLETE!
   fi
fi
