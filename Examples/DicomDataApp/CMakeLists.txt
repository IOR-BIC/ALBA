# declare executables
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmftest dcmgpdir dump2dcm xml2dcm pdf2dcm)
  ADD_EXECUTABLE(${PROGRAM}ex ${PROGRAM})
ENDFOREACH(PROGRAM)
ADD_EXECUTABLE(dcmodifyex dcmodify mdfconen mdfdsman)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcm2xml dcmconv dcmcrle dcmdrle dcmdump dcmgpdir dcmodify dump2dcm xml2dcm pdf2dcm)
  TARGET_LINK_LIBRARIES(${PROGRAM}ex netapi32 wsock32 ofstd dcmdata ${ZLIB_LIBS})
  ADD_DEPENDENCIES(${PROGRAM}ex BUILD_Offis_LIBRARY)
ENDFOREACH(PROGRAM)

TARGET_LINK_LIBRARIES(dcmftestex ofstd ${ZLIB_LIBS})
TARGET_LINK_LIBRARIES(xml2dcmex ${LIBXML_LIBS})
