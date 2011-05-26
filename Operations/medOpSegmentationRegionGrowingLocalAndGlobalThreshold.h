/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medOpSegmentationRegionGrowingLocalAndGlobalThreshold.h,v $
Language:  C++
Date:      $Date: 2011-05-26 08:16:40 $
Version:   $Revision: 1.1.2.10 $
Authors:   Matteo Giacomoni
==========================================================================
Copyright (c) 2009
SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)

MafMedical Library use license agreement

The software named MafMedical Library and any accompanying documentation, 
manuals or data (hereafter collectively "SOFTWARE") is property of the SCS s.r.l.
This is an open-source copyright as follows:
Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation and/or 
other materials provided with the distribution.
* Modified source versions must be plainly marked as such, and must not be misrepresented 
as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 'AS IS' 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR 
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND 
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

MafMedical is partially based on OpenMAF.
=========================================================================*/

#ifndef __medOpSegmentationRegionGrowingLocalAndGlobalThreshold_H__
#define __medOpSegmentationRegionGrowingLocalAndGlobalThreshold_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "medDefines.h"
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
class name : medOpSegmentationRegionGrowingLocalAndGlobalThreshold
*/
class MED_EXPORT medOpSegmentationRegionGrowingLocalAndGlobalThreshold: public mafOp
{
public:

  /** constructor. */
  medOpSegmentationRegionGrowingLocalAndGlobalThreshold(wxString label = "Region Growing");
  /** destructor. */
  ~medOpSegmentationRegionGrowingLocalAndGlobalThreshold(); 

  /** RTTI macro */
  mafTypeMacro(medOpSegmentationRegionGrowingLocalAndGlobalThreshold, mafOp);

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
