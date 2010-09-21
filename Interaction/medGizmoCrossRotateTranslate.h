/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGizmoCrossRotateTranslate.h,v $
Language:  C++
Date:      $Date: 2010-09-21 14:50:29 $
Version:   $Revision: 1.1.2.1 $
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
#include "medViewCompoundWindowing.h"
#include "mafVMESurface.h"
#include "mafGizmoInterface.h"

//----------------------------------------------------------------------------
// forward references :
//----------------------------------------------------------------------------

class mafViewVTK;
class medGizmoCrossTranslate;
class medGizmoCrossRotate;

class medGizmoCrossRotateTranslate : mafGizmoInterface
{
public:
	
	medGizmoCrossRotateTranslate();
	~medGizmoCrossRotateTranslate();

	void Create(mafVME *input, mafObserver* listener = NULL, bool BuildGUI = true, int axis = X);
	void SetInput(mafVME *vme);
	void SetRefSys(mafVME *refSys);
	
	/**
	Set/Get the gizmo abs pose at the current time stamp*/
	void SetAbsPose(mafMatrix *absPose);
	mafMatrix *GetAbsPose();
    
	void Show(bool show);

	void SetName(mafString name);
	mafString GetName() {return m_NameRTG;}
	void OnEvent(mafEventBase *maf_event);
	enum AXIS {X = 0, Y, Z};
	
	mafString m_NameRTG;

	medGizmoCrossTranslate *m_GizmoCrossTranslate;
	medGizmoCrossRotate		*m_GizmoCrossRotate;

	mafObserver *m_Listener;
};
#endif
