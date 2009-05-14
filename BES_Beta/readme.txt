The following classes should be committed down into openMAF (without 
_BES suffix), as they are usually slight enhancements of original classes.


Common
------
  mafDbg.h
  mafDataChecksum.h

Interaction
-----------
  mafGizmoROI_BES.cpp
  mafGizmoROI_BES.h  

Operations
----------
  mafOpImporterRAWVolume_BES.cpp
  mafOpImporterRAWVolume_BES.h  
  mafOpImporterBBF.cpp
  mafOpImporterBBF.h

VME
---
  mafPipeSlice.cpp
  mafPipeSlice.h
  mafPipePolylineSlice_BES.cpp 	
  mafPipePolylineSlice_BES.h
  mafPipeSurfaceSlice_BES.cpp 	
  mafPipeSurfaceSlice_BES.h
  mafPipeVolumeSlice_BES.cpp 		
  mafPipeVolumeSlice_BES.h
  mafPipeMeshSlice_BES.cpp 		
  mafPipeMeshSlice_BES.h
  mafVMEVolumeLarge.cpp
  mafVMEVolumeLarge.h
  mafVMEVolumeLargeUtils.cpp
  mafVMEVolumeLargeUtils.h  
  mafVMEVolumeLarge.xpm

vtkMED
------
  vtkMAFVolumeSlicer_BES.cpp	
  vtkMAFVolumeSlicer_BES.h
  vtkMAFMeshSlicer_BES.cpp
  vtkMAFIdType64.h

BES_Beta/IO (VME)
-----------
  mafBrickedFile.cpp
  mafBrickedFile.h
  mafBrickedFileReader.cpp
  mafBrickedFileReader.h
  mafBrickedFileWriter.cpp
  mafBrickedFileWriter.h
  mafVolumeLargeReader.cpp
  mafVolumeLargeReader.h
  mafVolumeLargeWriter.cpp
  mafVolumeLargeWriter.h  

BES_Beta/vtkMAF (vtkMED)
---------------
  vtkMAFMeshSlicer_BES.h
  vtkMAFDataArrayDescriptor.cxx
  vtkMAFDataArrayDescriptor.h	
  vtkMAFFileDataProvider.cxx
  vtkMAFFileDataProvider.h
  vtkMAFLargeDataProvider.cxx
  vtkMAFLargeDataProvider.h
  vtkMAFLargeDataSet.cxx
  vtkMAFLargeDataSet.h
  vtkMAFLargeDataSetCallback.cxx
  vtkMAFLargeDataSetCallback.h
  vtkMAFLargeImageData.cxx
  vtkMAFLargeImageData.h
  vtkMAFLargeImageReader.cxx
  vtkMAFLargeImageReader.h
  vtkMAFLargeImageSource.cxx
  vtkMAFLargeImageSource.h
  vtkMAFFile.cxx
  vtkMAFFile.h	
  vtkMAFDataArrayLayout.cpp
  vtkMAFDataArrayLayout.h
  vtkMAFMultiFileDataProvider.cpp
  vtkMAFMultiFileDataProvider.h


