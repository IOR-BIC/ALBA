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
            make_directory(${nextDir})
            configure_files(${srcTemplatePath} ${nextDir})
        endif(NOT IS_DIRECTORY ${srcTemplatePath})
    endforeach(templateFile)
endmacro(configure_files)

get_filename_component(CUR_ABSOLUTE_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
configure_files("${CUR_ABSOLUTE_DIR}" "${MAF_BINARY_DIR}/qa")
add_subdirectory(doc_output)
