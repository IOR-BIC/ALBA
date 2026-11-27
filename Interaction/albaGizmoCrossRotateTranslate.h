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
 Input must be albaVMEVolumeGray
 Movement is costrained inside the volume.
*/
class ALBA_EXPORT albaGizmoCrossRotateTranslate : public albaGizmoInterface
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

	/** Forward the ID_TRANSFORM events of the inner gizmos keeping them as sender:
	the listener uses the sender to know the plane and whether it is a translation or a rotation */
	void OnEvent(albaEventBase *alba_event);

	/** Return the inner translate gizmo */
	albaGizmoCrossTranslate *GetGizmoCrossTranslate() {return m_GizmoCrossTranslate;}

	/** Return the inner rotate gizmo */
	albaGizmoCrossRotate *GetGizmoCrossRotate() {return m_GizmoCrossRotate;}

protected:
	albaGizmoCrossTranslate *m_GizmoCrossTranslate;
	albaGizmoCrossRotate *m_GizmoCrossRotate;
};


#endif
