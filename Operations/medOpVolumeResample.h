/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medOpVolumeResample.h,v $
  Language:  C++
  Date:      $Date: 2009-02-10 19:32:45 $
  Version:   $Revision: 1.3.2.2 $
  Authors:   Marco Petrone
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#ifndef __medOpVolumeResample_H__
#define __medOpVolumeResample_H__

using namespace std;
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
class mafGizmoTranslate;
class mafGizmoRotate;
class mafGizmoHandle;
class mafGizmoROI;
class mafVMESurface;

//----------------------------------------------------------------------------
class medOpVolumeResample: public mafOp
{
public:
             
            	 medOpVolumeResample(const wxString &label = "VolumeResample",bool showShadingPlane = false);
	virtual     ~medOpVolumeResample();
	virtual void OnEvent(mafEventBase *maf_event);
	
  mafTypeMacro(medOpVolumeResample, mafOp);

  mafOp* Copy();

  bool Accept(mafNode* vme);
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

protected:
  
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

	void OnEventThis(mafEventBase *maf_event);
	void OnEventGizmoTranslate(mafEventBase *maf_event);
	void OnEventGizmoRotate(mafEventBase *maf_event);
	void OnEventGizmoROI(mafEventBase *maf_event);

	void PostMultiplyEventMatrix(mafEventBase *maf_event);

  mafVMEVolumeGray *m_ResampledVme;

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
	
	void UpdateGizmoData(mafEvent *e);
	void UpdateGui();
	void GizmoDelete();

  // Set resample bounds to some defaults
  void SetBoundsToVMEBounds();
  void SetBoundsToVME4DBounds();
  void SetBoundsToVMELocalBounds();

  void AutoSpacing();

  void InternalUpdateBounds(double bounds[6],bool center);

	void InizializeVMEDummy();
  
	int m_GizmoChoose;

	mafGizmoTranslate	*m_GizmoTranslate;
	mafGizmoRotate		*m_GizmoRotate;
	mafGizmoROI				*m_GizmoROI;
	mafVMESurface			*m_VMEDummy;
	mafMatrix					*m_CenterVolumeRefSysMatrix;

	int	m_ShowHandle;
	int m_ShowGizmoTransform;

	double	m_MaxBoundX;
	double	m_MaxBoundY;
	double	m_MaxBoundZ;

  bool    m_ViewSelectedMessage;
  bool    m_ShowShadingPlane;
  
  void PrintVolume( ostream& os , mafNode *volume , const char *logMessage = NULL );
  static void PrintDouble6(ostream& os, double array[6], const char *logMessage = NULL );
  static void PrintDouble3(ostream& os, double array[3], const char *logMessage = NULL);
  static void PrintInt3(ostream& os, int array[3], const char *logMessage = NULL);

  friend class medOpVolumeResampleTest;
};
#endif
