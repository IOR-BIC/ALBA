/*=========================================================================

 Program: MAF2
 Module: mafOpFilterVolume
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpFilterVolume_H__
#define __mafOpFilterVolume_H__

#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class vtkImageData;
class mafEvent;

//----------------------------------------------------------------------------
// mafOpFilterVolume :
//----------------------------------------------------------------------------
/** */
class MAF_EXPORT mafOpFilterVolume: public mafOp
{
public:
	mafOpFilterVolume(const wxString &label = "FilterVolume");
	~mafOpFilterVolume(); 
	virtual void OnEvent(mafEventBase *maf_event);
	
  mafTypeMacro(mafOpFilterVolume, mafOp);

  mafOp* Copy();

	/** Return true for the acceptable vme type. */
	bool Accept(mafVME*node);

	/** Builds operation's interface. */
	void OpRun();

	/** Execute the operation. */
	void OpDo();

	/** Makes the undo for the operation. */
	void OpUndo();

  void ApplyFiltersToInputData(bool apply) {m_ApplyDirectlyOnInput = apply;};

  void ApplyFiltersToInputDataOn() {ApplyFiltersToInputData(true);};

  void ApplyFiltersToInputDataOff() {ApplyFiltersToInputData(false);};

  /** Set the Standard Deviation for the smooth filter */
  void SetStandardDeviation(double stDev[3]){m_StandardDeviation[0] = stDev[0];m_StandardDeviation[1] = stDev[1];m_StandardDeviation[2] = stDev[2];};

  /** Set the Standard Deviation that use the smooth filter */
  void GetStandardDeviation(double stDev[3]){stDev[0] = m_StandardDeviation[0];stDev[1] = m_StandardDeviation[1];stDev[2] = m_StandardDeviation[2];};

  /** Set the Smooth Radius */
  void SetSmoothRadius(double radius[3]){m_SmoothRadius[0] = radius[0];m_SmoothRadius[1] = radius[1];m_SmoothRadius[2] = radius[2];};

  /** Get the Smooth Radius */
  void GetSmoothRadius(double radius[3]){radius[0] = m_SmoothRadius[0];radius[1] = m_SmoothRadius[1];radius[2] = m_SmoothRadius[2];};

  /** Set the Kernel Size for Median Filter */
  void SetKernelSize(int size[3]){m_KernelSize[0] = size[0];m_KernelSize[1] = size[1];m_KernelSize[2] = size[2];};

  /** Get the Kernel Size for Median Filter */
  void GetKernelSize(int size[3]){size[0] = m_KernelSize[0];size[1] = m_KernelSize[1];size[2] = m_KernelSize[2];};

protected:
	/** This method is called at the end of the operation and result contain the wxOK or wxCANCEL. */
	void OpStop(int result);

  /** Create the Operation GUI */
  void CreateGui();
	
	bool m_ClearInterfaceFlag;
	bool m_PreviewResultFlag;

	vtkImageData	*m_ResultImageData;
	vtkImageData	*m_OriginalImageData;
  vtkImageData  *m_InputData;

	int    m_Dimensionality;
  double m_SmoothRadius[3];
  double m_StandardDeviation[3];

  int m_KernelSize[3];

	double m_ReplaceRange[2];
	double m_ReplaceValue;

  int m_ApplyDirectlyOnInput; ///< For big volume data, torn on this flag to save memory and apply filters directly to the input data.

  /** Smooth the surface. */
	void OnSmooth();

  /** Execute median filter.*/
  void OnMedian();

	/** Make the preview of the surface filtering. */
	void OnPreview();  

	/** Clear all the surface applied filtering. */
	void OnClear();

	/** Replaces values inside range with defined value*/
	void OnReplace();

  /** friend test class */
  friend class mafOpFilterVolumeTest;
private:
	
};
#endif
