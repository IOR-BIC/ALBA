  #################################################################
  # Module:    $RCSfile: FindGTK2.cmake,v $
  # Author:    Silvano Imboden
  # Language:  CMake 2.05
  # Date:      $Date: 2006-10-05 11:06:52 $
  # Version:   $Revision: 1.3 $
  ##################################################################
  
  # ---------------------------------------------------------------------
  # Find GTK 2.0
  # ---------------------------------------------------------------------
  # assume that:
  # gtk2.0 is present and correctly installed  <=> pkg-config exist and is in the path
  # ---------------------------------------------------------------------
  #
  # variables defined by this file
  #
  # GTK2_FOUND 1/0 -- if 0 the other variable may contain thrash
  # GTK_VERSION
  # GTK_INCLUDE_DIRECTORIES
  # GTK_DEFINES
  # GTK_LIBS
  # GTK_EXTRA_LIBS
  # ---------------------------------------------------------------------
  
  SET(GTK2_FOUND 0)
  
  # -----------------
  # search pkg-config
  # -----------------
  FIND_PROGRAM( PKG-CONFIG pkg-config)
  
  MESSAGE("DBG: PKG-CONFIG = >>${PKG-CONFIG}<<") 
  
  IF( PKG-CONFIG STREQUAL "PKG-CONFIG-NOTFOUND" )
    SET(PKG-CONFIG-FOUND 0)
    MESSAGE( FATAL_ERROR "pkg-config was not found, thus gtk2.0 is probably not installed on this system" )
  ELSE( PKG-CONFIG STREQUAL "PKG-CONFIG-NOTFOUND" )
    SET(PKG-CONFIG-FOUND 1)
  ENDIF( PKG-CONFIG STREQUAL "PKG-CONFIG-NOTFOUND" )
  
  # -----------------
  #  query pkg-config 
  # -----------------
  IF( ${PKG-CONFIG-FOUND} )
  
    #GET_FILENAME_COMPONENT( PKG-CONFIG_PATH "${PKG_CONFIG}" PATH )
    #GET_FILENAME_COMPONENT( PKG-CONFIG_NAME "${PKG_CONFIG}" NAME )
  
    # -----------------
    # check if gtk2.0 exixst and setup GTK_VERSION
    # this command return the gtk version or an empty string if gtk2.0 is not found 
    # -----------------
    # using pkg-config in place of ${PKG_CONFIG} returned by FIND_PROGRAM 
    # because of a strange error, probably related to versioning of CMake.
    # anyway - pkg-config should be in the PATH - and so it work anyway
    # -----------------
    EXEC_PROGRAM( pkg-config 
                ARGS gtk+-2.0 --modversion --silence-errors
                OUTPUT_VARIABLE GTK_VERSION 
                RETURN_VALUE RET1 )
    
    #MESSAGE("DBG: res=>>${RET1}<< GTK_VERSION=>>${GTK_VERSION}<<") 
    
    IF( GTK_VERSION )
      SET(GTK2_FOUND 1)
    ELSE( GTK_VERSION )
      MESSAGE( FATAL_ERROR "gtk2.0 was not found by pkg-config" )
    ENDIF( GTK_VERSION )
    
    # ---------------------------------------------------------
    # find include directories -> GTK_INCLUDE_DIRECTORIES
    # ---------------------------------------------------------
    EXEC_PROGRAM( pkg-config
                ARGS gtk+-2.0 --cflags-only-I
                OUTPUT_VARIABLE GTK_INCLUDE 
                RETURN_VALUE RET2)
    
    ## remove prefix -I 
    STRING(REGEX REPLACE "[-][I]" "" GTK_INCLUDE_DIRECTORIES  "${GTK_INCLUDE}" )
    SEPARATE_ARGUMENTS(GTK_INCLUDE_DIRECTORIES)
    
    #MESSAGE("DBG: res=>>${RET2}<< GTK_INCLUDE = >>${GTK_INCLUDE}<<") 
    #MESSAGE("DBG: GTK_INCLUDE_DIRECTORIES = >>${GTK_INCLUDE_DIRECTORIES}<<")
      
    # ---------------------------------------------------------
    # find defined simbols -> GTK_DEFINES
    # ---------------------------------------------------------
    EXEC_PROGRAM( pkg-config 
                ARGS gtk+-2.0 --cflags-only-other
                OUTPUT_VARIABLE GTK_FLAGS
                RETURN_VALUE RET3)
    
    ## extract strings beginninig with (-D) 
    STRING(REGEX MATCHALL "[-][D]([^ ;])+" GTK_DEFINES "${GTK_FLAGS}" )
    
    #MESSAGE("DBG: res=>>${RET3}<< GTK_FLAGS = >>${GTK_FLAGS}<<") 
    #MESSAGE("DBG: GTK_DEFINES = >>${GTK_DEFINES}<<")
    
    # ---------------------------------------------------------
    # find link libs -> GTK_LIBS
    # ---------------------------------------------------------
    EXEC_PROGRAM( pkg-config 
                ARGS gtk+-2.0 --libs-only-l
                OUTPUT_VARIABLE GTK_LIBS
                RETURN_VALUE RET4)
    
    STRING(REGEX REPLACE "[-][l]" "" GTK_LIBS "${GTK_LIBS}" )
    SEPARATE_ARGUMENTS(GTK_LIBS)
    
    #MESSAGE("DBG: res=>>${RET4}<< GTK_LIBS = >>${GTK_LIBS}<<") 
    #MESSAGE("DBG: GTK_LIBS = >>${GTK_LIBS}<<")
    
    # ---------------------------------------------------------
    # find link libs -> GTK_EXTRA_LIBS (libs not prefixed with -l)
    # ---------------------------------------------------------
    EXEC_PROGRAM( pkg-config 
                ARGS gtk+-2.0 --libs-only-other
                OUTPUT_VARIABLE GTK_EXTRA_LIBS
                RETURN_VALUE RET5)
    
    #MESSAGE("DBG: res=>>${RET5}<< GTK_EXTRA_LIBS = >>${GTK_EXTRA_LIBS}<<") 
    #MESSAGE("DBG: GTK_EXTRA_LIBS = >>${GTK_EXTRA_LIBS}<<")
  
  ENDIF( ${PKG-CONFIG-FOUND} )
