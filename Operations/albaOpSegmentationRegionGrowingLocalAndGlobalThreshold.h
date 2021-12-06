/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpSegmentationRegionGrowingLocalAndGlobalThreshold
 Authors: Matteo Giacomoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpSegmentationRegionGrowingLocalAndGlobalThreshold_H__
#define __albaOpSegmentationRegionGrowingLocalAndGlobalThreshold_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaVMEVolumeGray;
class albaVMESurface;
class albaGUILutSlider;
class albaGUIHistogramWidget;
class albaGUIDialog;
class vtkImageData;

/** 
class name : albaOpSegmentationRegionGrowingLocalAndGlobalThreshold
*/
class ALBA_EXPORT albaOpSegmentationRegionGrowingLocalAndGlobalThreshold: public albaOp
{
public:

  /** constructor. */
  albaOpSegmentationRegionGrowingLocalAndGlobalThreshold(wxString label = "Region Growing");
  /** destructor. */
  ~albaOpSegmentationRegionGrowingLocalAndGlobalThreshold(); 

  /** RTTI macro */
  albaTypeMacro(albaOpSegmentationRegionGrowingLocalAndGlobalThreshold, albaOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
  /*virtual*/ albaOp* Copy();

  /** Precess events coming from other objects */
  /*virtual*/ void OnEvent(albaEventBase *alba_event);

  /** Builds operation's interface by calling CreateOpDialog() method. */
  /*virtual*/ void OpRun();

  /** Execute the operation. */
  /*virtual*/ void OpDo();

  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

protected:
  
	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
  /*virtual*/ void OpStop(int result);

  /** Create and initialize the dialog where the user could see the histogram */
  void CreateHistogramDialog();

  /** Create the operation gui */
  void CreateGui();

  /** Perform the region growing */
  void RegionGrowing();

  /** Perform the morphological closing operation */
  void MorphologicalMathematics();

  /** Execute the fitting with Levenberg-Marquet algorithm */
  void FittingLM();

  void HistogramEqualization();

  void ComputeParam();

  void WriteHistogramFiles();

  //GUI Stuff
  albaString m_Point1;
  double m_Point1Value;
  double m_Point1HistogramValue;
  albaString m_Point2;
  double m_Point2Value;
  double m_Point2HistogramValue;
  albaString m_Point3;
  double m_Point3Value;
  albaString m_Point4;
  double m_Point4Value;
  int m_CurrentPoint;
  double m_LowerLabel;
  double m_UpperLabel;
  int m_SphereRadius;
  int m_ApplyConnectivityFilter;
  albaGUILutSlider *m_SliderLabels;
  albaGUILutSlider *m_SliderThresholds;
  albaGUI *m_GuiLabels;
  int m_SoftParam1Value;
  int m_SoftParam2Value;
  int m_SoftParam3Value;
  albaString m_SoftParam1;
  albaString m_SoftParam2;
  albaString m_SoftParam3;
  albaString m_BoneParam1;
  albaString m_BoneParam2;
  albaString m_BoneParam3;
  int m_EliminateHistogramValues;
  double m_ValuesToEliminate;

  bool m_ComputedMedianFilter;
  albaGUIHistogramWidget *m_Histogram;
  albaGUIDialog *m_Dialog;

  albaVMEVolumeGray *m_VolumeInput; //<<<Input volume
  albaVMEVolumeGray *m_VolumeOutputMorpho; //<<<Output volume after morphological mathematics operation
  albaVMEVolumeGray *m_VolumeOutputRegionGrowing; //<<<Output volume after region growing operation
  albaVMESurface *m_SurfaceOutput; //<<<Output surface extracted from m_VolumeOutputMorpho

  vtkImageData *m_SegmentedImage;
  vtkImageData *m_MorphoImage;

  double m_Threshold;

};
#endif
