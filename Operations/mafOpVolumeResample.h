/*=========================================================================

 Program: MAF2
 Module: mafOpVolumeResample
 Authors: Marco Petrone
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __mafOpVolumeResample_H__
#define __mafOpVolumeResample_H__

//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "mafOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class mafEvent;
class mafVME;
class mafVMEGizmo;
class vtkOutlineSource;
class mafVMEVolumeGray;

//----------------------------------------------------------------------------
class MAF_EXPORT mafOpVolumeResample: public mafOp
{
public:
             
            	 mafOpVolumeResample(const wxString &label = "VolumeResample");
	virtual     ~mafOpVolumeResample();
	virtual void OnEvent(mafEventBase *maf_event);
	
  mafTypeMacro(mafOpVolumeResample, mafOp);

  mafOp* Copy();

	bool Accept(mafNode* vme);
	void OpRun();	
	void OpDo();
	void OpUndo();  

	/**
	Set spacing for test mode*/
	void SetSpacing(double Spacing[3]);

	/**
	Set bounds for test mode*/
	void SetBounds(double Bounds[6],int Type);

	/**
	Resample the input volume*/
	void Resample();

protected:
  mafVMEVolumeGray *m_ResampledVme;

	double m_OldVolumePosition[3];
	double m_NewVolumePosition[3];
	double m_PrecedentPosition[3];
	
  double m_VolumePosition[3];
  double m_VolumeOrientation[3];
  double m_VolumeSpacing[3];
  double m_VolumeBounds[6];

  double m_ZeroPadValue;

	//gizmo's functions
	void CreateGizmoCube();
  virtual void CreateGui();
	void ShiftCenterResampled();
	void UpdateGizmoData();
	void UpdateGui();
	void GizmoDelete();

  // Set resample bounds to some defaults
  void SetBoundsToVMEBounds();
  void SetBoundsToVME4DBounds();
  void SetBoundsToVMELocalBounds();

  void AutoSpacing();

  void InternalUpdateBounds(double bounds[6],bool center);

	vtkOutlineSource *m_ResampleBox;

	mafVMEGizmo *m_ResampleBoxVme;
};
#endif
