# header list
SET(DICOM_HDRS
   dirbase.h
   dicombase.h
   )

# module list
SET(DICOM_SRCS
   dirbase.cpp
   dicombase.cpp
   )

# Windows definitions
IF (MSVC)
   ADD_DEFINITIONS(-D_CRT_SECURE_NO_DEPRECATE)
ENDIF (MSVC)

# DCMTK definitions
IF (BUILD_WITH_DCMTK)
   INCLUDE_DIRECTORIES(${DCMTK_INCLUDE_DIR})
   INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
   INCLUDE_DIRECTORIES(${ICONV_INCLUDE_DIR})
   ADD_DEFINITIONS(-DHAVE_DCMTK)
   IF (NOT WIN32)
      ADD_DEFINITIONS(-DHAVE_CONFIG_H)
   ENDIF (NOT WIN32)
ENDIF (BUILD_WITH_DCMTK)

# library
SET(DICOM_NAME DICOM)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../headers)
ADD_LIBRARY(${DICOM_NAME} ${DICOM_SRCS} ${DICOM_HDRS})
SET(DICOM_LIBRARY ${DICOM_NAME} CACHE INTERNAL "")
