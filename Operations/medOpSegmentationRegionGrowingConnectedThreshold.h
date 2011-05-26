/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medOpSegmentationRegionGrowingConnectedThreshold.h,v $
Language:  C++
Date:      $Date: 2011-05-26 08:16:40 $
Version:   $Revision: 1.1.2.6 $
Authors:   Matteo Giacomoni, Alessandro Chiarini
==========================================================================
Copyright (c) 2008
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
=========================================================================*/

#ifndef __medOpSegmentationRegionGrowingConnectedThreshold_H__
#define __medOpSegmentationRegionGrowingConnectedThreshold_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mafInteractorPicker;
class mafInteractor;
class mafVMESurface;
class mafVMEVolumeGray;

class vtkSphereSource;

//----------------------------------------------------------------------------
// medOpSegmentationRegionGrowingConnectedThreshold :
//----------------------------------------------------------------------------
/** */
class MED_EXPORT medOpSegmentationRegionGrowingConnectedThreshold: public mafOp
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
  
protected:

  /*virtual*/ void OpStop(int result);

  void CreateGui();

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
  mafVMESurface *m_Sphere;

  vtkSphereSource *m_SphereVTK;

  mafString m_SeedScalarValue;

};

#endif
