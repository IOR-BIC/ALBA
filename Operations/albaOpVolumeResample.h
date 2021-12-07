/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaOpVolumeResample
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __albaOpVolumeResample_H__
#define __albaOpVolumeResample_H__

using namespace std;
//----------------------------------------------------------------------------
// Include :
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaOp.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------
class albaEvent;
class albaVME;
class albaVMEGizmo;
class vtkOutlineSource;
class albaVMEVolumeGray;
class albaGizmoTranslate;
class albaGizmoRotate;
class albaGizmoHandle;
class albaGizmoROI;
class albaVMESurface;

//----------------------------------------------------------------------------
class ALBA_EXPORT albaOpVolumeResample: public albaOp
{
public:
             
            	 albaOpVolumeResample(const wxString &label = "VolumeResample",bool showShadingPlane = false);
	virtual     ~albaOpVolumeResample();
	virtual void OnEvent(albaEventBase *alba_event);
	
  albaTypeMacro(albaOpVolumeResample, albaOp);

  albaOp* Copy();

  void OpRun();	
  void OpDo();
  void OpUndo();  
  
  void PrintSelf(ostream& os);
  

  /**
  Set spacing in test mode*/
  void SetSpacing(double spacing[3]);
  void GetSpacing(double spacing[3]);

  /**
  type of output volume bounds */
  enum BOUNDS
  {
    VME4DBOUNDS = 0,
    VMELOCALBOUNDS,
    VMEBOUNDS,
    CUSTOMBOUNDS,
  };

  /**
  Set bounds for test mode: boundsType is one of
  VME4DBOUNDS 
  VMELOCALBOUNDS
  VMEBOUNDS
  PERSONALBOUNDS
  */
  void SetBounds(double bounds[6],int boundsType);

  /**
  Perform resample in test mode*/
  void Resample();

  void AutoSpacing();

protected:
  
	/** Return true for the acceptable vme type. */
	bool InternalAccept(albaVME*node);

  /** Test facilities */
  void SetROIOrientation(double roiOrientation[3]);
  void SetROIPosition(double roiPosition[3]);

  void SetVolumePosition(double volumePosition[3]);

  // used by ShiftCenterResampled
  void SetNewVolumePosition(double position[3]);

  void SetMaxBounds(double maxBounds[3]);

  /** end test facilities */


  void ShiftCenterResampled();

	virtual void OpStop(int result);

	void OnEventThis(albaEventBase *alba_event);
	void OnEventGizmoTranslate(albaEventBase *alba_event);
	void OnEventGizmoRotate(albaEventBase *alba_event);
	void OnEventGizmoROI(albaEventBase *alba_event);

	void PostMultiplyEventMatrix(albaEventBase *alba_event);

  /** Check the spacing and if is too little and could create some memory problems return false */
  bool CheckSpacing();

  albaVMEVolumeGray *m_ResampledVme;

	double m_NewVolumePosition[3];
	double m_PrecedentPosition[3];

	double m_ROIPosition[3];
	double m_ROIOrientation[3];
	
  double m_VolumeCenterPosition[3];
	double m_VolumePosition[3];
	double m_VolumeOrientation[3];
  double m_VolumeSpacing[3];
  double m_VolumeBounds[6];

  double m_ZeroPadValue;

	//gizmo's functions
	void CreateGizmos();
  virtual void CreateGui();
	
	void UpdateGizmoData(albaEvent *e);
	void UpdateGui();
	void GizmoDelete();

  // Set resample bounds to some defaults
  void SetBoundsToVMEBounds();
  void SetBoundsToVME4DBounds();
  void SetBoundsToVMELocalBounds();

  void InternalUpdateBounds(double bounds[6],bool center);

	void InizializeVMEDummy();
  
	int m_GizmoChoose;

	albaGizmoTranslate	*m_GizmoTranslate;
	albaGizmoRotate		*m_GizmoRotate;
	albaGizmoROI				*m_GizmoROI;
	albaVMESurface			*m_VMEDummy;
	albaMatrix					*m_CenterVolumeRefSysMatrix;

	int	m_ShowHandle;
	int m_ShowGizmoTransform;

	double	m_MaxBoundX;
	double	m_MaxBoundY;
	double	m_MaxBoundZ;

  bool    m_ViewSelectedMessage;
  bool    m_ShowShadingPlane;
  
  void PrintVolume( ostream& os , albaVME *volume , const char *logMessage = NULL );
  static void PrintDouble6(ostream& os, double array[6], const char *logMessage = NULL );
  static void PrintDouble3(ostream& os, double array[3], const char *logMessage = NULL);
  static void PrintInt3(ostream& os, int array[3], const char *logMessage = NULL);

  friend class albaOpVolumeResampleTest;
};
#endif
