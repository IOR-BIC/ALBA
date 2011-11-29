macro(configure_files srcDir destDir)
    message(STATUS "Configuring directory ${destDir}")
    make_directory(${destDir})

    file(GLOB templateFiles RELATIVE ${srcDir} ${srcDir}/*)
    foreach(templateFile ${templateFiles})
        set(srcTemplatePath ${srcDir}/${templateFile})
        if(NOT IS_DIRECTORY ${srcTemplatePath})
            message(STATUS "Configuring file ${templateFile}")
            set(originalName ${templateFile})
            STRING(REGEX REPLACE ".conf$" "" templateFile ${originalName})
            set(result 0)
            STRING(COMPARE EQUAL ${templateFile} ${originalName} result)
            if(NOT result)
                configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    @ONLY)
            else(NOT result)
                configure_file(
                    ${srcTemplatePath}
                    ${destDir}/${templateFile}
                    COPYONLY)
            endif(NOT result)
        else(NOT IS_DIRECTORY ${srcTemplatePath})
            set(nextDir ${destDir}/${templateFile})
            set(resultCVSWin 0)
            set(resultSVNWin 0)
            set(resultCVSUnix 0)
            set(resultSVNUnix 0)
            STRING(COMPARE EQUAL ${templateFile} "CVS" resultCVSWin)
            STRING(COMPARE EQUAL ${templateFile} "SVN" resultSVNWin)
            STRING(COMPARE EQUAL ${templateFile} ".cvs" resultCVSUnix)
            STRING(COMPARE EQUAL ${templateFile} ".svn" resultSVNUnix)
            if(NOT(resultCVSWin OR resultSVNWin OR resultCVSUnix OR resultSVNUnix))
                make_directory(${nextDir})
                configure_files(${srcTemplatePath} ${nextDir})
            endif(NOT(resultCVSWin OR resultSVNWin OR resultCVSUnix OR resultSVNUnix))
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)
endmacro(configure_files)

get_filename_component(CUR_ABSOLUTE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

# SET(SRC_TMP_DIR ${CMAKE_BINARY_DIR}/qa/doc_output/TmpSrc)
# FILE(MAKE_DIRECTORY ${SRC_TMP_DIR})

# SET(SRC_DIRS  "${MED_BINARY_DIR}" "${MED_SOURCE_DIR}/Base" "${MED_BINARY_DIR}/Interaction" "${MED_SOURCE_DIR}/Interaction" "${MED_SOURCE_DIR}/Interaction/Drivers" 
              # "${MED_SOURCE_DIR}/Core" "${MED_SOURCE_DIR}/Gui" "${MED_SOURCE_DIR}/IO" "${MED_SOURCE_DIR}/Operations" "${MED_SOURCE_DIR}/VME" 
              # "${MED_SOURCE_DIR}/VME/DataPipes" "${MED_SOURCE_DIR}/vtkMED" "${MED_BINARY_DIR}/vtkMED" "${MED_SOURCE_DIR}/GPUAPI" 
              # "${MED_SOURCE_DIR}/GPUAPI/GLEW" "${MED_SOURCE_DIR}/GPUAPI/FBO")
              
# FILE(GLOB files_to_remove "${SRC_TMP_DIR}/*.*")

# FOREACH(f ${files_to_remove})
  # FILE(REMOVE "${f}")
# ENDFOREACH(f ${files_to_remove})

# FOREACH(dirs ${SRC_DIRS})
  # FILE(GLOB source1_file_list "${dirs}/*.cpp")
  # FILE(GLOB template_file_list "${dirs}/*.txx")
  # FILE(GLOB source2_file_list "${dirs}/*.cxx")
  
  # FOREACH(source1_file ${source1_file_list})
    # FILE(RELATIVE_PATH source1_file_name "${dirs}" "${source1_file}")
    # CONFIGURE_FILE("${source1_file}" "${CMAKE_BINARY_DIR}/qa/doc_output/TmpSrc/${source1_file_name}")
  # ENDFOREACH(source1_file ${source1_file_list})
  
  # FOREACH(template_file ${template_file_list})
    # FILE(RELATIVE_PATH template_file_name "${dirs}" "${template_file}")
    # CONFIGURE_FILE("${template_file}" "${CMAKE_BINARY_DIR}/qa/doc_output/TmpSrc/${template_file_name}")
  # ENDFOREACH(template_file ${template_file_list})
  
  # FOREACH(source2_file ${source2_file_list})
    # FILE(RELATIVE_PATH source2_file_name "${dirs}" "${source2_file}")
    # CONFIGURE_FILE("${source2_file}" "${CMAKE_BINARY_DIR}/qa/doc_output/TmpSrc/${source2_file_name}")
  # ENDFOREACH(source2_file ${source2_file_list})
# ENDFOREACH(dirs ${SRC_DIRS})

configure_files("${CUR_ABSOLUTE_DIR}" "${MED_BINARY_DIR}/qa")
add_subdirectory(doc_output)
