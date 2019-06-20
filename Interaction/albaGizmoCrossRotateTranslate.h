/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoCrossRotateTranslate
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 
//======================== WORK IN PROGRESS !!!!! ======================== 

#ifndef __albaGizmoCrossRotateTranslate_H__
#define __albaGizmoCrossRotateTranslate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "albaDefines.h"
#include "albaVMESurface.h"
#include "albaGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class albaViewVTK;
class albaGizmoCrossTranslate;
class albaGizmoCrossRotate;


/** 

 Gizmo used to perform constrained roto-translation on a given plane.
 Input must be a albaVMESlicer slicing a albaVMEVolumeGray (TO IMPROVE)
 Movement is costrained inside the sliced volume.
*/
class ALBA_EXPORT albaGizmoCrossRotateTranslate : albaGizmoInterface
{
public:
	
	albaGizmoCrossRotateTranslate();
	~albaGizmoCrossRotateTranslate();

	enum NORMAL {X = 0, Y = 1, Z = 2};

	
	enum COMPONENT {GREW = 0, GTAEW, GTPEW, GRNS , GTANS , GTPNS };
	enum COLOR {RED = 0, GREEN , BLUE };
	void SetColor(int component, int color);

	void Create(albaVME *input, albaObserver* listener = NULL, bool BuildGUI = true, int normal = X);
	void SetInput(albaVME *vme);

	/** Superclass override */
	void SetRenderWindowHeightPercentage(double percentage);

	/** Superclass override */
	void SetAutoscale(bool autoscale);

	/** Superclass override */
	void SetAlwaysVisible(bool alwaysVisible);

	void SetRefSys(albaVME *refSys);
	
	/**
	Set/Get the gizmo abs pose at the current time stamp*/
	void SetAbsPose(albaMatrix *absPose);
	albaMatrix *GetAbsPose();
    
	void Show(bool show);

	void SetName(albaString name);
	albaString GetName() {return m_NameRTG;}
	void OnEvent(albaEventBase *alba_event);
	
	albaString m_NameRTG;

	albaGizmoCrossTranslate *m_GizmoCrossTranslate;
	albaGizmoCrossRotate		*m_GizmoCrossRotate;

	albaObserver *m_Listener;
};


#endif
