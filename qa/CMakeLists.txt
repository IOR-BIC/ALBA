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

SET(PROJ_SOURCE_DIR "${ALBA_SOURCE_DIR}")
SET(PROJ_BINARY_DIR "${ALBA_BINARY_DIR}")

get_filename_component(CUR_ABSOLUTE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
configure_files("${CUR_ABSOLUTE_DIR}" "${PROJ_BINARY_DIR}/qa")
add_subdirectory(doc_output)
