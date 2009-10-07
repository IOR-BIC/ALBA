/*=========================================================================
Program:   @neufuse
Module:    $RCSfile: medOpSegmentationRegionGrowingConnectedThreshold.h,v $
Language:  C++
Date:      $Date: 2009-10-07 14:09:49 $
Version:   $Revision: 1.1.2.1 $
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
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafNode;
class mmiPicker;
class mafInteractor;
class mafVMESurface;

class vtkSphereSource;

//----------------------------------------------------------------------------
// medOpSegmentationRegionGrowingConnectedThreshold :
//----------------------------------------------------------------------------
/** */
class medOpSegmentationRegionGrowingConnectedThreshold: public mafOp
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

protected:

  /*virtual*/ void OpStop(int result);

  void CreateGui();

  void Algorithm();

  int m_Lower;
  int m_Upper;
  int m_Replace;
  int *m_Seed;

  int m_NumIter;
  float m_TimeStep;
  float m_Conductance;
  int m_UseSpacing;

  int m_ImageDim;

  mmiPicker           *m_Picker;
  mafInteractor       *m_OldBehavior;

  mafVMESurface *m_Sphere;

  vtkSphereSource *m_SphereVTK;

  mafString m_SeedScalarValue;

};

#endif
