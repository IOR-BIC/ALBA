#
# Copyright and description to be placed here
#

PROJECT(medViews)

# Set your list of sources here.
SET(PROJECT_SRCS

  # Rotated Volumes Debugger Stuff
  
  medViewRXRotatedVolumesDebugger.cpp
  medViewRXRotatedVolumesDebugger.h
  
  medViewSliceRotatedVolumesDebugger.cpp
  medViewSliceRotatedVolumesDebugger.h
  
  medViewRXCTRotatedVolumesDebugger.cpp
  medViewRXCTRotatedVolumesDebugger.h
  
  medViewOrthoSliceRotatedVolumesDebugger.cpp
  medViewOrthoSliceRotatedVolumesDebugger.h
  
  # End Rotated Volumes Debugger Stuff
  
  mafViewArbitrarySlice.cpp
  mafViewArbitrarySlice.h
  mafViewOrthoSlice.cpp
  mafViewOrthoSlice.h
  mafViewRX.cpp
  mafViewRX.h
  mafViewRXCT.cpp
  mafViewRXCT.h
  mafViewSlice.cpp
  mafViewSlice.h
  mafViewGlobalSlice.cpp
  mafViewGlobalSlice.h
  mafViewGlobalSliceCompound.cpp
  mafViewGlobalSliceCompound.h
  mafViewImage.cpp
  mafViewImage.h
  mafViewSingleSlice.cpp
  mafViewSingleSlice.h
  mafViewSingleSliceCompound.cpp
  mafViewSingleSliceCompound.h
  mafViewRXCompound.cpp
  mafViewRXCompound.h
  mafView3D.cpp
  mafView3D.h
  mafViewImageCompound.cpp
  mafViewImageCompound.h
  mafViewCT.cpp
  mafViewCT.h
  medViewCompoundWindowing.cpp
  medViewCompoundWindowing.h
  medViewVTKCompound.cpp
  medViewVTKCompound.h
  #medViewEmgGraph.cpp
  #medViewEmgGraph.h
  medViewSlicer.cpp
  medViewSlicer.h
  medViewSliceOnCurve.cpp
  medViewSliceOnCurve.h
  medViewSliceCompound.cpp
  medViewSliceCompound.h
  medViewSliceOnCurveCompound.cpp
  medViewSliceOnCurveCompound.h
)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafVME mafInteraction mafCore)

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMED)

# Create the library.
ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})

