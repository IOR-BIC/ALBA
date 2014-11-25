/*=========================================================================

 Program: MAF2Medical
 Module: medOpSegmentationRegionGrowingConnectedThreshold
 Authors: Matteo Giacomoni, Alessandro Chiarini, Grazia Di Cosmo, Gianluigi Crimi
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __medOpSegmentationRegionGrowingConnectedThreshold_H__
#define __medOpSegmentationRegionGrowingConnectedThreshold_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafInteractorPicker;
class mafInteractor;
class mafVMESurface;
class mafVMEVolumeGray;
class medOpVolumeResample;
class vtkSphereSource;

//----------------------------------------------------------------------------
// medOpSegmentationRegionGrowingConnectedThreshold :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT medOpSegmentationRegionGrowingConnectedThreshold: public mafOp
{
public:
  medOpSegmentationRegionGrowingConnectedThreshold(wxString label = "Connected Threshold");
  ~medOpSegmentationRegionGrowingConnectedThreshold();

  mafTypeMacro(medOpSegmentationRegionGrowingConnectedThreshold, mafOp);

  /** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(mafNode *node);

  /*virtual*/ mafOp* Copy();

  /** Builds operation's interface. */
  /*virtual*/ void OpRun();

  /*virtual*/ void OnEvent(mafEventBase *maf_event);

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
  mafVMEVolumeGray *GetOutputVolume(){return m_VolumeOut;};
    
protected:
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

  mafInteractorPicker           *m_Picker;
  mafInteractor       *m_OldBehavior;

  mafVMEVolumeGray *m_VolumeOut;
  mafVMESurface *m_SurfaceOut;
  mafVMESurface *m_Sphere;
  

  vtkSphereSource *m_SphereVTK;

  mafString m_SeedScalarValue;
  medOpVolumeResample *m_Resample;

  mafVMEVolumeGray *m_ResampleInput;
  double m_VolumeSpacing[3];
  double m_VolumeBounds[6];

};

#endif
