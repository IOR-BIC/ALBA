/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mmoVolumeResample.h,v $
  Language:  C++
  Date:      $Date: 2006-01-16 12:12:02 $
  Version:   $Revision: 1.2 $
  Authors:   Marco Petrone
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __mmoVolumeResample_H__
#define __mmoVolumeResample_H__

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
class mmoVolumeResample: public mafOp
{
public:
             
            	 mmoVolumeResample(wxString label);
	virtual     ~mmoVolumeResample();
	virtual void OnEvent(mafEventBase *maf_event);
	mafOp* Copy();

	bool Accept(mafNode* vme);
	void OpRun();	
	void OpDo();
	void OpUndo();  

protected:
  mafVMEVolumeGray *m_ResampledVme;
	
  double m_VolumePosition[3];
  double m_VolumeOrientation[3];
  double m_VolumeSpacing[3];
  double m_VolumeBounds[6];

  double m_ZeroPadValue;

	//gizmo's functions
	void CreateGizmoCube();
  virtual void CreateGui();
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
