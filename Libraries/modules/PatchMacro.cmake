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

				EXEC_PROGRAM("${PATCH_EXECUTABLE} -N ${MyDiffName} ${MyDiffFile} " ${SOURCE_DIR}${MyDiffDir} RETURN_VALUE PATCH_RETVAL)
				IF (${PATCH_RETVAL} LESS 0)
					MESSAGE(FATAL_ERROR "Cannot patch ${PACKAGE_NAME} package: patch ended with return value ${PATCH_RETVAL}.\nNote: This happens usually on MS Windows, if you do not run CMAKE as administrator. Resolution of this problem is as follows. First, close CMAKE and then delete both Sources and Build subdirectories of package ${PACKAGE_NAME} from the directory where to build the binaries (or preferably the whole directory). After that execute CMAKE, this time as administrator (use the right click on the CMAKE shortcut) - please do not mistake this for running CMAKE under a user account with administration rights, and repeat the process. Should the problem persists, contact MAF2 developer group.\n")
				ENDIF(${PATCH_RETVAL} LESS 0)
 			ENDFOREACH ( MyDiffFile ${MyDiffFileList} )
		ELSE(PATCH_EXECUTABLE)	
	   	MESSAGE(FATAL_ERROR "Cannot patch ${PACKAGE_NAME} package: patch command not found!")  
		ENDIF(PATCH_EXECUTABLE)			
	ELSE (MyDiffFileList)	
	#	MESSAGE("No file to patch for package ${PACKAGE_NAME} ")
	ENDIF (MyDiffFileList)
	
	MARK_AS_ADVANCED(PATCH_EXECUTABLE)
ENDMACRO(FIND_AND_APPLAY_PATCHES)
 
 

 