This frame work is intended for C++ and OpenGL teaching purposes.
Copyright 2014-2022 by Stefan Roettger
Licensed under the MIT license

To compile the frame work, type the following on the command line:

 ./build.sh

To open the frame work with KDevelop on Ubuntu:
* install KDevelop: sudo apt-get install kdevelop
* in KDevelop open the project by selecting the CMakeLists.txt file

To open the frame work with QtCreator on Ubuntu:
* install QtCreator: sudo apt-get install qtcreator
* in QtCreator open the project by selecting the vis-framework.pro file

To run the app:
* start it from the console without arguments: ./myqtapp
* or hit the "Run" button in KDevelop or QtCreator

To run the app with an OpenGL context:
* start it from the console with the following argument: ./myqtapp --gl

To run the app with an OpenGL core profile (via glVertex library):
* start it from the console with the following argument: ./myqtapp --lgl

To run the app as OpenGL ARB program editor:
* start it from the console with the following argument: ./myqtapp --arbeditor

To run the app as OpenGL GLSL program editor:
* start it from the console with the following argument: ./myqtapp --glsleditor

To ask the app for a help text:
* start it from the console with the following argument: ./myqtapp --help
