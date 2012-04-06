/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGizmoCrossRotateTranslate.h,v $
Language:  C++
Date:      $Date: 2012-04-06 08:39:52 $
Version:   $Revision: 1.1.2.6 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
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

#ifndef __medGizmoCrossRotateTranslate_H__
#define __medGizmoCrossRotateTranslate_H__

//----------------------------------------------------------------------------
// Include:
//----------------------------------------------------------------------------
#include "medInteractionDefines.h"
#include "mafVMESurface.h"
#include "mafGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafViewVTK;
class medGizmoCrossTranslate;
class medGizmoCrossRotate;


/** 

 Gizmo used to perform constrained roto-translation on a given plane.
 Input must be a mafVMESlicer slicing a mafVMEVolumeGray (TO IMPROVE)
 Movement is costrained inside the sliced volume.
*/
class MED_INTERACTION_EXPORT medGizmoCrossRotateTranslate : mafGizmoInterface
{
public:
	
	medGizmoCrossRotateTranslate();
	~medGizmoCrossRotateTranslate();

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

	medGizmoCrossTranslate *m_GizmoCrossTranslate;
	medGizmoCrossRotate		*m_GizmoCrossRotate;

	mafObserver *m_Listener;
};


#endif
