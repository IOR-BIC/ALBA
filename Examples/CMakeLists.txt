
#Create ExampleApp
IF (EXISTS ${MAF_SOURCE_DIR}/Examples/ExampleApp)
  SUBDIRS(ExampleApp)
ENDIF (EXISTS ${MAF_SOURCE_DIR}/Examples/ExampleApp)

#Create AlbaMaster App
IF (EXISTS ${MAF_SOURCE_DIR}/Examples/AlbaMaster)
  SUBDIRS(AlbaMaster)
ENDIF (EXISTS ${MAF_SOURCE_DIR}/Examples/AlbaMaster)