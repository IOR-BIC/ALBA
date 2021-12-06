/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSegmentationRegionGrowingConnectedThreshold
 Authors: Matteo Giacomoni, Alessandro Chiarini, Grazia Di Cosmo, Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpSegmentationRegionGrowingConnectedThreshold_H__
#define __albaOpSegmentationRegionGrowingConnectedThreshold_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVME;
class albaInteractorPicker;
class albaInteractor;
class albaVMESurface;
class albaVMEVolumeGray;
class albaOpVolumeResample;
class vtkSphereSource;

//----------------------------------------------------------------------------
// albaOpSegmentationRegionGrowingConnectedThreshold :
//----------------------------------------------------------------------------
/** */
class ALBA_EXPORT albaOpSegmentationRegionGrowingConnectedThreshold: public albaOp
{
public:
  albaOpSegmentationRegionGrowingConnectedThreshold(wxString label = "Connected Threshold");
  ~albaOpSegmentationRegionGrowingConnectedThreshold();

  albaTypeMacro(albaOpSegmentationRegionGrowingConnectedThreshold, albaOp);

  /*virtual*/ albaOp* Copy();

  /** Builds operation's interface. */
  /*virtual*/ void OpRun();

  /*virtual*/ void OnEvent(albaEventBase *alba_event);

  /** Execute the operation. */
  void OpDo();

  enum CONNECTED_THRESHOLD_WIDGET_IDs
  {
    ID_LOWER = MINID,
    ID_UPPER,
    ID_REPLACE,
    ID_SEED,
    ID_ITERATIONS,
    ID_TIME_STEP,
    ID_CONDUCTANCE,
    ID_USE_SPACING,
  };

  /** Perform the segmentation algorithm */
  void Algorithm();

  /** Set the lower threshold */
  void SetLowerThreshold(int lower){m_Lower = lower;};
  /** Get the lower threshold */
  int GetLowerThreshold(){return m_Lower;};

  /** Set the upper threshold */
  void SetUpperThreshold(int upper){m_Upper = upper;};
  /** Get the upper threshold */
  int GetUpperThreshold(){return m_Upper;};

  /** Set the replace value */
  void SetReplaceValue(int replace){m_Replace = replace;};
  /** Get the replace value */
  int GetReplaceValue(){return m_Replace;};

  /** Set the seed */
  void SetSeed(int *seed);
  /** Get the seed */
  void GetSeed(int *seed);

  /** Returns the output Volume*/
  albaVMEVolumeGray *GetOutputVolume(){return m_VolumeOut;};
    
protected:

	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result);
  
  /** Create the operation GUI. */
  void CreateGui();

  /** Resample volume if it is a rectilinear grid */ 
  int CreateResample();
  
  /** Check the spacing and if is too little and could create some memory problems return false */
  bool CheckSpacing();

  
  int m_Lower;
  int m_Upper;
  int m_Replace;
  int *m_Seed;

  int m_NumIter;
  float m_TimeStep;
  float m_Conductance;
  int m_UseSpacing;

  int m_ImageDim;

  albaInteractorPicker           *m_Picker;
  albaInteractor       *m_OldBehavior;

  albaVMEVolumeGray *m_VolumeOut;
  albaVMESurface *m_SurfaceOut;
  albaVMESurface *m_Sphere;
  

  vtkSphereSource *m_SphereVTK;

  albaString m_SeedScalarValue;
  albaOpVolumeResample *m_Resample;

  albaVMEVolumeGray *m_ResampleInput;
  double m_VolumeSpacing[3];
  double m_VolumeBounds[6];

};

#endif
