PROJECT (mafOSG00)

SET(PROJECT_SRCS 
mafOsgApp.cpp
wxWinEventAdapter.h
wxWinEventAdapter.cpp
mVueGlCanvas.cpp
mVueGlCanvas.h
DragAndDrop.h
DragAndDrop.cpp
)


SET( BASE_OSG_PATH "" CACHE PATH "root directory of the OpenSceneGraph Tree" )

SET( OSG_DIR          ${BASE_OSG_PATH}/OpenSceneGraph  )
SET( OPENTHREADS_DIR  ${BASE_OSG_PATH}/OpenThreads     )
SET( PRODUCER_DIR     ${BASE_OSG_PATH}/Producer        )
SET( DEPENDENCIES_DIR ${BASE_OSG_PATH}/3rdParty        )

IF(NOT EXISTS ${OSG_DIR})
  MESSAGE(ERROR "Did not find OSG base directory - check your settings")
ENDIF(NOT EXISTS ${OSG_DIR})

IF(NOT EXISTS ${OPENTHREADS_DIR})
  MESSAGE(ERROR "Did not find OpenThreads directory - check your settings")
ENDIF(NOT EXISTS ${OPENTHREADS_DIR})

IF(NOT EXISTS ${PRODUCER_DIR})
  MESSAGE(ERROR "Did not find Producer directory - check your settings")
ENDIF(NOT EXISTS ${PRODUCER_DIR})

IF(NOT EXISTS ${DEPENDENCIES_DIR})
  MESSAGE(ERROR "Did not find OSG 3rdParty directory - check your settings")
ENDIF(NOT EXISTS ${DEPENDENCIES_DIR})

IF (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} WIN32 ${PROJECT_SRCS} "${PROJECT_SOURCE_DIR}/app.rc")
  CONFIGURE_FILE( "${MAF_SOURCE_DIR}/Gui/manifest.in" "${EXECUTABLE_OUTPUT_PATH}/Debug/${PROJECT_NAME}.exe.manifest" COPYONLY)
ELSE (WIN32)
  ADD_EXECUTABLE(${PROJECT_NAME} ${PROJECT_SRCS} )
ENDIF (WIN32)

###################################################
# include
SET( OSG_INCLUDE_PATHS  
	${OSG_DIR}/include
	${OPENTHREADS_DIR}/include
	${PRODUCER_DIR}/include
	${DEPENDENCIES_DIR}/include
)
INCLUDE_DIRECTORIES(${OSG_INCLUDE_PATHS})

###################################################
# libs
SET( OSG_LIBS_PATHS  
	${OSG_DIR}/libs
	${OPENTHREADS_DIR}/libs
	${PRODUCER_DIR}/libs
	${DEPENDENCIES_DIR}/libs
)
#non serve --- le dll, lib e annessi, le butto tutte nella bin/debug o bin/release
#LINK_DIRECTORIES(${OSG_LIBS_PATHS})

SET(PROJECT_LIBS
  #mafVME 
  #mafOperations 
  
  mafBase
  mafGui

  debug osgd.lib 
  debug osgDBd.lib 
  debug osgUtild.lib 
  debug osgProducerd.lib 
  debug Producerd.lib 
  debug Opengl32.lib 
  debug osgGAd.lib 
  debug osgTextd.lib 
  debug osgUtild.lib 
  debug osgDBd.lib 
  debug osgd.lib 
  debug glu32.lib 
  debug OpenThreadsd.lib 

  optimized osg.lib 
  optimized osgDB.lib 
  optimized osgUtil.lib 
  optimized osgProducer.lib 
  optimized Producer.lib 
  optimized Opengl32.lib 
  optimized osgGA.lib 
  optimized osgText.lib 
  optimized osgUtil.lib 
  optimized osgDB.lib 
  optimized osg.lib 
  optimized glu32.lib 
  optimized OpenThreads.lib 
)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

###################################################
# grabbato dai settings del progetto

#libs:
#
#kernel32.lib 
#user32.lib 
#gdi32.lib 
#winspool.lib 
#comdlg32.lib 
#advapi32.lib 
#shell32.lib 
#ole32.lib 
#oleaut32.lib 
#uuid.lib 
#odbc32.lib 
#odbccp32.lib  

#osgd.lib 
#osgDBd.lib 
#osgUtild.lib 
#osgProducerd.lib 
#Producerd.lib 
#Opengl32.lib 
#osgGAd.lib 
#osgTextd.lib 
#osgUtild.lib 
#osgDBd.lib 
#osgd.lib 
#glu32.lib 
#OpenThreadsd.lib 

#Opengl32.lib 

###################################################
# symbols define
# WIN32, WINDOWS, DEBUG --- niente di + speciale































