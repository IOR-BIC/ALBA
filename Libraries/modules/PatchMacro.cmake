#
# Program:   MULTIMOD APPLICATION FRAMEWORK (MAF)
# Module:    $RCSfile: PatchMacro.cmake,v $
# Language:  CMake 1.2
# Date:      $Date: 2006-07-28 09:50:20 $
# Version:   $Revision: 1.5 $
#
# Description:
# Macro file for to find and patch file .diff and to apply patches.

############################################################################
##				macro to find and patch file .diff															##
##				FIND_AND_APPLAY_PATCHES																					##
##        PACKAGE_NAME name package for wich to apply patches							##
##        PATCH_DIR dir where to find .diff files		 											##
##        SOURCE_DIR dir where to find sources files to patch							##
############################################################################


MACRO(FIND_AND_APPLAY_PATCHES PACKAGE_NAME PATCH_DIR SOURCE_DIR)

#look if there are files to patch 
 	FILE(GLOB_RECURSE MyDiffFileList "${PATCH_DIR}/*.diff")
 	IF (MyDiffFileList)	
 		IF (WIN32)
			#searching for patching program
	  	SET (GNUWIN32_INSTALL_PATH ${MFL_SOURCE_DIR}/Extras)
  		FIND_PROGRAM(PATCH_EXECUTABLE  patch  ${GNUWIN32_INSTALL_PATH}/bin )
  	ENDIF (WIN32)
  	IF (UNIX)
  		FIND_PROGRAM (PATCH_EXECUTABLE patch  /usr/bin )
 		ENDIF(UNIX) 
		IF(PATCH_EXECUTABLE)
  		
  		#MESSAGE("${PACKAGE_NAME}: patching...")
 			FOREACH ( MyDiffFile ${MyDiffFileList} )
 				GET_FILENAME_COMPONENT(MyDiffDir ${MyDiffFile} PATH)
   			GET_FILENAME_COMPONENT(MyDiffName ${MyDiffFile} NAME)
   			STRING(REGEX REPLACE ".diff$" "" MyDiffName ${MyDiffName})
   			STRING(REGEX REPLACE "^${PATCH_DIR}" "" MyDiffDir ${MyDiffDir})
   			SET(MyPatchedTarget ${SOURCE_DIR}${MyDiffDir}/${MyDiffName})
   			
   			#CONFIGURE_FILE("${SOURCE_DIR}/${MyDiffDir}/${MyDiffName}" "${SOURCE_DIR}/${MyDiffDir}/${MyDiffName}.orig" COPYONLY IMMEDIATE)
   			#MESSAGE("found ${MyDiffFile} in ${MyDiffDir} patching ${MyDiffName} target ${MyPatchedTarget}")
				#patch -N ORIGFILE PATCHFILE  Ignore patches that appear to be reversed or already applied.
				EXEC_PROGRAM("${PATCH_EXECUTABLE} -N ${MyDiffName} ${MyDiffFile} " ${SOURCE_DIR}${MyDiffDir})
 			ENDFOREACH ( MyDiffFile ${MyDiffFileList} )
		ELSE(PATCH_EXECUTABLE)	
	   	MESSAGE(SEND_ERROR "Cannot patch ${PACKAGE_NAME} package: patch command not found!")  
		ENDIF(PATCH_EXECUTABLE)			
	ELSE (MyDiffFileList)	
	#	MESSAGE("No file to patch for package ${PACKAGE_NAME} ")
	ENDIF (MyDiffFileList)
	
	MARK_AS_ADVANCED(PATCH_EXECUTABLE)
ENDMACRO(FIND_AND_APPLAY_PATCHES)
 
 

 