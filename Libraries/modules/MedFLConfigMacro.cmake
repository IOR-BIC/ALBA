#
# Macro to synchronously configure a sub project
# This simply includes a subproject Configure.cmake script
# that should do all necessary configuration. A couple of useful
# variables are also defined. 
# Notice SUBPROJECT_NAME SUBPROJECT_DIR SUBPROJECT_SOURCE_DIR SUBPROJECT_BINARY_DIR are not 
# rentrant variables
#
MACRO(MedFL_SUBPROJECT SUBPROJECT_NAME SUBPROJECT_DIR)

  # if not previous configure errors
  IF (NOT CONFIGURE_ERROR)  
    IF (EXISTS ${MedFL_SOURCE_DIR}/${SUBPROJECT_DIR}/Configure.cmake)
      MESSAGE (STATUS "Configuring subproject ${SUBPROJECT_NAME}")
      # Set some useful variables
      SET(SUBPROJECT_BINARY_DIR ${MedFL_BINARY_DIR}/${SUBPROJECT_DIR})
      SET (${SUBPROJECT_NAME}_BINARY_DIR ${MedFL_BINARY_DIR}/${SUBPROJECT_DIR})
      SET(SUBPROJECT_SOURCE_DIR ${MedFL_SOURCE_DIR}/${SUBPROJECT_DIR})
      SET (${SUBPROJECT_NAME}_SOURCE_DIR ${MedFL_SOURCE_DIR}/${SUBPROJECT_DIR})
        
      # set flag to advise the subfolder was found
      SET (${SUBPROJECT_NAME}_CONFIG_FOUND 1)
      # execute subproject configuration
      INCLUDE (${SUBPROJECT_SOURCE_DIR}/Configure.cmake)
    ENDIF (EXISTS ${MedFL_SOURCE_DIR}/${SUBPROJECT_DIR}/Configure.cmake)
  ELSE (NOT CONFIGURE_ERROR)
    MESSAGE ("Not configuring ${SUBPROJECT_NAME} due to previuos errors")
  ENDIF (NOT CONFIGURE_ERROR)
ENDMACRO(MedFL_SUBPROJECT)