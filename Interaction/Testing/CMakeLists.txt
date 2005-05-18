PROJECT (mafInteraction_TEST)

LINK_LIBRARIES(mafInteraction)

# wxWidgets specific classes
#IF (MAF_USE_WX)
#ENDIF (MAF_USE_WX)

# VTK specific classes
#IF (MAF_USE_VTK)
#  LINK_LIBRARIES(vtkGraphics vtkRendering)
#ENDIF (MAF_USE_VTK)  

# classes using VTK+wxWidgets
#IF (MAF_USE_WX AND MAF_USE_VTK)
#ENDIF (MAF_USE_WX AND MAF_USE_VTK)

ADD_EXECUTABLE(testAgent testAgent.cpp)
ADD_TEST(testAgent ${EXECUTABLE_OUTPUT_PATH}/testAgent)
