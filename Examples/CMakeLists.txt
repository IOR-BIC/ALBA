
#Create AlbaMaster App
IF (EXISTS ${ALBA_SOURCE_DIR}/Examples/AlbaMaster)
  SUBDIRS(AlbaMaster)
ENDIF (EXISTS ${ALBA_SOURCE_DIR}/Examples/AlbaMaster)

#Create AlbaViewer App
IF (EXISTS ${ALBA_SOURCE_DIR}/Examples/AlbaViewer)
  SUBDIRS(AlbaViewer)
ENDIF (EXISTS ${ALBA_SOURCE_DIR}/Examples/AlbaViewer)