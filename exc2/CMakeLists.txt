PROJECT(MyProject)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# specify sources and headers
SET(HDRS account.h checking.h) # module headers go here (*.h)
SET(SRCS account.cpp checking.cpp main.cpp) # module implementations go here (*.cpp)

# C++ standard
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -Wall -Werror")
IF (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.2)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wvla")
ENDIF (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 5.2)

# default C++ compiler definitions
IF (NOT CMAKE_BUILD_TYPE)
   IF (UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O")
   ENDIF (UNIX)
ENDIF (NOT CMAKE_BUILD_TYPE)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

# build and link executable
ADD_EXECUTABLE(main ${HDRS} ${SRCS}) # compile main executable
