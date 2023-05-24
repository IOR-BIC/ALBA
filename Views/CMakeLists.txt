#
# Copyright and description to be placed here
#

PROJECT(albaViews)

# Set your list of sources here.
SET(PROJECT_SRCS
  
  albaViewArbitrarySlice.cpp				albaViewArbitrarySlice.h
  albaViewOrthoSlice.cpp						albaViewOrthoSlice.h
  albaViewRX.cpp										albaViewRX.h
  albaViewRXCT.cpp									albaViewRXCT.h
  albaViewSlice.cpp								albaViewSlice.h
  albaViewGlobalSlice.cpp					albaViewGlobalSlice.h
  albaViewGlobalSliceCompound.cpp	albaViewGlobalSliceCompound.h
  albaViewImage.cpp								albaViewImage.h
  albaViewRXCompound.cpp						albaViewRXCompound.h
  albaView3D.cpp										albaView3D.h
  albaViewImageCompound.cpp				albaViewImageCompound.h
  albaViewCT.cpp										albaViewCT.h
  albaViewCompoundWindowing.cpp		albaViewCompoundWindowing.h
  albaViewVTKCompound.cpp					albaViewVTKCompound.h
  albaViewSlicer.cpp								albaViewSlicer.h
  albaViewSliceOnCurve.cpp					albaViewSliceOnCurve.h
  albaViewSliceOnCurveCompound.cpp	albaViewSliceOnCurveCompound.h
  albaViewSliceGlobal.cpp					albaViewSliceGlobal.h
  albaViewArbitraryOrthoSlice.cpp	albaViewArbitraryOrthoSlice.h
  albaViewSliceBlendRX.cpp					albaViewSliceBlendRX.h
  albaViewSliceBlend.cpp						albaViewSliceBlend.h
  albaViewSliceSegmentation.cpp		albaViewSliceSegmentation.h
  albaViewIsosurfaceCompound.cpp		albaViewIsosurfaceCompound.h
  albaViewVirtualRX.cpp					albaViewVirtualRX.h
)

# List libraries that are needed by this project.
SET(PROJECT_LIBS albaVME albaInteraction albaCore)

SET(PROJECT_LIBS ${PROJECT_LIBS} vtkALBA)

IF(ALBA_BUILD_ALBADLL)
  SET(BUILD_SHARED_LIBS 1)
  ADD_DEFINITIONS(-DALBA_EXPORTS)
  
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} albaDLL vtkALBA albaCommon albaVME albaInteraction)
ELSE(ALBA_BUILD_ALBADLL)
  # Create the library.
  ADD_LIBRARY(${PROJECT_NAME} ${PROJECT_SRCS})
  TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_LIBS})
ENDIF(ALBA_BUILD_ALBADLL)

ADD_DEPENDENCIES(${PROJECT_NAME} albaCommon albaVME albaInteraction albaService)
