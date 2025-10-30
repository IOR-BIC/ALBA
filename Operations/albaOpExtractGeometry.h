/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpExtractGeometry
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpExtractGeometry_H__
#define __albaOpExtractGeometry_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "albaOp.h"
#include "albaDefines.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaGUIDialog;
class albaGUIFloatSlider;
class albaVMESurface;
class albaVMEVolumeGray;

class vtkImageData;
class vtkALBAVolumeToClosedSmoothSurface;
class vtkPolyData;

class ALBA_EXPORT albaOpExtractGeometry: public albaOp
{
public:
  //----------------------------------------------------------------------------
  // Constants:
  //----------------------------------------------------------------------------
  enum GUI_IDS
  {
    ID_OK = MINID, 
    ID_CANCEL,
    ID_RESAMPLE_VOLUME_SPACING,
    ID_RESAMPLE_OK,
    ID_VOLUME_SMOOTHING,
    ID_VOLUME_SMOOTHING_REPETITIONS,
    ID_EXTRACT_GEOMETRY,
    ID_SURFACE_OPTIMIZATION,
    ID_CONNECTIVITY, 
    ID_CLEAN_SURFACE,
    ID_SMOOTH_SURFACE,
    ID_SMOOTH_SURFACE_ITERATIONS,
    ID_DECIMATE_SURFACE,
    ID_DECIMATE_SURFACE_RATE,
    ID_DECIMATE_SURFACE_TOPOLOGY, 
    ID_CONTOUR_VALUE,
    ID_PROCESSING_TYPE,
  };

  /** constructor. */
  albaOpExtractGeometry(const wxString &label = "Extract Geometry from binary volume");
  
  /** destructor. */
  ~albaOpExtractGeometry(); 

  /** Precess events coming from other objects */
  void OnEvent(albaEventBase *alba_event);

  /** RTTI macro */
  albaTypeMacro(albaOpExtractGeometry, albaOp);

  /** return the copy of the operation object */
  albaOp* Copy();

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
   void OpStop(int result);

  /** Compute derived surface. */
  int GenerateIsosurface();
  /** Smooth original input volume. */
  void VolumeSmoothing();
  /** Apply connectivity filter. */
  void SurfaceConnectivity();
  /** Apply cleaning filter. */
  void SurfaceCleaning();
  /** Apply surface smoothing filter. */
  void SurfaceSmoothing();
  /** Apply geometry decimation filter. */
  void SurfaceDecimation();

  /** Create the operation GUI. */
  void CreateGui();
  /** Create surface extraction pipeline GUI. */
  void CreateExtractSurfaceGui();
  /** Create surface decimation GUI. */
  void CreateSurfaceDecimationGui();
  /** Crate the resampling optional step GUI. */
  void CreateResampleGui();
  /** Resample input volume: called in case of vtkRectilinearGrid input data. */
  int Resample();

  albaGUI *m_ResampleGui;
  albaGUI *m_ExtractSurfaceGui;

  albaGUIFloatSlider *m_SurfaceContourValueSlider;

  int m_VolumeSmoothing;
  int m_Connectivity;
  int m_CleanSurface;
  int m_SmoothSurface;
  int m_DecimateSurface;

  int m_DecimateReductionRate;
  int m_DecimatePreserveTopology;
  int m_SmoothSurfaceIterationsNumber;
  int m_VolumeSmoothingRepetitions;
  double m_SurfaceContourValue;
  
  albaVMEVolumeGray *m_VolumeInput;
  albaVMEVolumeGray *m_ResampledVolume;
  albaVMESurface *m_SurfaceOutput;
  vtkImageData *m_OriginalData;
  vtkPolyData *m_SurfaceData;

  double m_VolumeSpacing[3];

  vtkALBAVolumeToClosedSmoothSurface *m_SurfaceExtractor;
  
  double m_ScalarRange[2];

  int m_ProcessingType;

};
#endif
