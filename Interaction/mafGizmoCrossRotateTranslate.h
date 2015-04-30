/*=========================================================================

 Program: MAF2
 Module: mafGizmoCrossRotateTranslate
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

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

#ifndef __mafGizmoCrossRotateTranslate_H__
#define __mafGizmoCrossRotateTranslate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "mafDefines.h"
#include "mafVMESurface.h"
#include "mafGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafViewVTK;
class mafGizmoCrossTranslate;
class mafGizmoCrossRotate;


/** 

 Gizmo used to perform constrained roto-translation on a given plane.
 Input must be a mafVMESlicer slicing a mafVMEVolumeGray (TO IMPROVE)
 Movement is costrained inside the sliced volume.
*/
class MAF_EXPORT mafGizmoCrossRotateTranslate : mafGizmoInterface
{
public:
	
	mafGizmoCrossRotateTranslate();
	~mafGizmoCrossRotateTranslate();

	enum NORMAL {X = 0, Y = 1, Z = 2};

	
	enum COMPONENT {GREW = 0, GTAEW, GTPEW, GRNS , GTANS , GTPNS };
	enum COLOR {RED = 0, GREEN , BLUE };
	void SetColor(int component, int color);

	void Create(mafVME *input, mafObserver* listener = NULL, bool BuildGUI = true, int normal = X);
	void SetInput(mafVME *vme);

	/** Superclass override */
	void SetRenderWindowHeightPercentage(double percentage);

	/** Superclass override */
	void SetAutoscale(bool autoscale);

	/** Superclass override */
	void SetAlwaysVisible(bool alwaysVisible);

	void SetRefSys(mafVME *refSys);
	
	/**
	Set/Get the gizmo abs pose at the current time stamp*/
	void SetAbsPose(mafMatrix *absPose);
	mafMatrix *GetAbsPose();
    
	void Show(bool show);

	void SetName(mafString name);
	mafString GetName() {return m_NameRTG;}
	void OnEvent(mafEventBase *maf_event);
	
	mafString m_NameRTG;

	mafGizmoCrossTranslate *m_GizmoCrossTranslate;
	mafGizmoCrossRotate		*m_GizmoCrossRotate;

	mafObserver *m_Listener;
};


#endif
