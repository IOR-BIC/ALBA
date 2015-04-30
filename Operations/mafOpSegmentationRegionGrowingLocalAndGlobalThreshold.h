/*=========================================================================

 Program: MAF2
 Module: mafOpSegmentationRegionGrowingLocalAndGlobalThreshold
 Authors: Matteo Giacomoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpSegmentationRegionGrowingLocalAndGlobalThreshold_H__
#define __mafOpSegmentationRegionGrowingLocalAndGlobalThreshold_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafVMEVolumeGray;
class mafVMESurface;
class mafGUILutSlider;
class mafGUIHistogramWidget;
class mafGUIDialog;
class vtkImageData;

/** 
class name : mafOpSegmentationRegionGrowingLocalAndGlobalThreshold
*/
class MAF_EXPORT mafOpSegmentationRegionGrowingLocalAndGlobalThreshold: public mafOp
{
public:

  /** constructor. */
  mafOpSegmentationRegionGrowingLocalAndGlobalThreshold(wxString label = "Region Growing");
  /** destructor. */
  ~mafOpSegmentationRegionGrowingLocalAndGlobalThreshold(); 

  /** RTTI macro */
  mafTypeMacro(mafOpSegmentationRegionGrowingLocalAndGlobalThreshold, mafOp);

  /** Return a copy of itself, this needs to put the operation into the undo stack. */
  /*virtual*/ mafOp* Copy();

  /** Precess events coming from other objects */
  /*virtual*/ void OnEvent(mafEventBase *maf_event);

  /** Return true for the acceptable vme type. */
  /*virtual*/ bool Accept(mafNode* vme);

  /** Builds operation's interface by calling CreateOpDialog() method. */
  /*virtual*/ void OpRun();

  /** Execute the operation. */
  /*virtual*/ void OpDo();

  /** Makes the undo for the operation. */
  /*virtual*/ void OpUndo();

protected:
  
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
  mafString m_Point1;
  double m_Point1Value;
  double m_Point1HistogramValue;
  mafString m_Point2;
  double m_Point2Value;
  double m_Point2HistogramValue;
  mafString m_Point3;
  double m_Point3Value;
  mafString m_Point4;
  double m_Point4Value;
  int m_CurrentPoint;
  double m_LowerLabel;
  double m_UpperLabel;
  int m_SphereRadius;
  int m_ApplyConnectivityFilter;
  mafGUILutSlider *m_SliderLabels;
  mafGUILutSlider *m_SliderThresholds;
  mafGUI *m_GuiLabels;
  int m_SoftParam1Value;
  int m_SoftParam2Value;
  int m_SoftParam3Value;
  mafString m_SoftParam1;
  mafString m_SoftParam2;
  mafString m_SoftParam3;
  mafString m_BoneParam1;
  mafString m_BoneParam2;
  mafString m_BoneParam3;
  int m_EliminateHistogramValues;
  double m_ValuesToEliminate;

  bool m_ComputedMedianFilter;
  mafGUIHistogramWidget *m_Histogram;
  mafGUIDialog *m_Dialog;

  mafVMEVolumeGray *m_VolumeInput; //<<<Input volume
  mafVMEVolumeGray *m_VolumeOutputMorpho; //<<<Output volume after morphological mathematics operation
  mafVMEVolumeGray *m_VolumeOutputRegionGrowing; //<<<Output volume after region growing operation
  mafVMESurface *m_SurfaceOutput; //<<<Output surface extracted from m_VolumeOutputMorpho

  vtkImageData *m_SegmentedImage;
  vtkImageData *m_MorphoImage;

  double m_Threshold;

};
#endif
