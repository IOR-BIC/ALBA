/*=========================================================================

 Program: MAF2
 Module: mafOpExtractGeometry
 Authors: Eleonora Mambrini, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpExtractGeometry_H__
#define __mafOpExtractGeometry_H__

//----------------------------------------------------------------------------
// Includes :
//----------------------------------------------------------------------------
#include "mafOp.h"
#include "mafDefines.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafGUIDialog;
class mafGUIFloatSlider;
class mafVMESurface;
class mafVMEVolumeGray;

class vtkImageData;
class vtkMAFVolumeToClosedSmoothSurface;
class vtkPolyData;

class MAF_EXPORT mafOpExtractGeometry: public mafOp
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
    ID_AUTO_CONTOUR_VALUE,
    ID_CONTOUR_VALUE,
    ID_PROCESSING_TYPE,
  };

  /** constructor. */
  mafOpExtractGeometry(const wxString &label = "Extract Geometry from binary volume");
  
  /** destructor. */
  ~mafOpExtractGeometry(); 

  /** Precess events coming from other objects */
  void OnEvent(mafEventBase *maf_event);

  /** RTTI macro */
  mafTypeMacro(mafOpExtractGeometry, mafOp);

  /** return the copy of the operation object */
  mafOp* Copy();

  /** Return true for the acceptable vme type. */
  bool Accept(mafNode *node);

  /** Builds operation's interface. */
  void OpRun();

  /** Execute the operation. */
  void OpDo();

  /** Makes the undo for the operation. */
  void OpUndo();

protected:

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

  mafGUI *m_ResampleGui;
  mafGUI *m_ExtractSurfaceGui;

  mafGUIFloatSlider *m_SurfaceContourValueSlider;

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
  int m_AutoSurfaceContourValue;

  mafVMEVolumeGray *m_VolumeInput;
  mafVMEVolumeGray *m_ResampledVolume;
  mafVMESurface *m_SurfaceOutput;
  vtkImageData *m_OriginalData;
  vtkPolyData *m_SurfaceData;

  double m_VolumeSpacing[3];

  vtkMAFVolumeToClosedSmoothSurface *m_SurfaceExtractor;
  
  double m_ScalarRange[2];

  int m_ProcessingType;

};
#endif
