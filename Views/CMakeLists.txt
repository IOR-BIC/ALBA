#
# Copyright and description to be placed here
#

PROJECT(mafViews)

# Set your list of sources here.
SET(PROJECT_SRCS
  
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
  mafViewCompoundWindowing.cpp
  mafViewCompoundWindowing.h
  mafViewVTKCompound.cpp
  mafViewVTKCompound.h
  mafViewSlicer.cpp
  mafViewSlicer.h
  mafViewSliceOnCurve.cpp
  mafViewSliceOnCurve.h
  mafViewSliceOnCurveCompound.cpp
  mafViewSliceOnCurveCompound.h
  mafViewSliceGlobal.cpp
  mafViewSliceGlobal.h
  mafViewArbitraryOrthoSlice.cpp
  mafViewArbitraryOrthoSlice.h
  mafViewSliceBlendRX.cpp
  mafViewSliceBlendRX.h
  mafViewSliceBlend.cpp
  mafViewSliceBlend.h
  mafViewSliceNotInterpolated.cpp
  mafViewSliceNotInterpolated.h
  mafViewSliceNotInterpolatedCompound.cpp
  mafViewSliceNotInterpolatedCompound.h
  mafViewIsosurfaceCompound.cpp
  mafViewIsosurfaceCompound.h
)

# List libraries that are needed by this project.
SET(PROJECT_LIBS mafVME mafInteraction mafCore)

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkMAF)

IF(MAF_BUILD_MAFDLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DMAF_EXPORTS)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} mafDLL vtkMAF mafCommon mafVME mafInteraction)
ELSE(MAF_BUILD_MAFDLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF(MAF_BUILD_MAFDLL)

ADD_DEPENDENCIES(${PROJECT_NAME} mafCommon mafVME mafInteraction)
