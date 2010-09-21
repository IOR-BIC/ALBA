/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGizmoCrossRotateTranslate.cpp,v $
Language:  C++
Date:      $Date: 2010-09-21 14:50:29 $
Version:   $Revision: 1.1.2.1 $
Authors:   Stefano Perticoni
==========================================================================
Copyright (c) 2002/2004
CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medGizmoCrossRotateTranslate.h"

#include "medDecl.h"

#include "medGizmoCrossTranslate.h"
#include "medGizmoCrossRotate.h"
#include "mafEvent.h"

medGizmoCrossRotateTranslate::medGizmoCrossRotateTranslate()
{
	m_NameRTG = "UNDEFINED_RTG_NAME";
	m_GizmoCrossTranslate = NULL;
	m_GizmoCrossRotate = NULL;
	m_Listener = NULL;
}

medGizmoCrossRotateTranslate::~medGizmoCrossRotateTranslate()
{

}

void medGizmoCrossRotateTranslate::Create(mafVME *input, mafObserver* listener, bool BuildGUI, int axis)
{
	m_GizmoCrossTranslate = new medGizmoCrossTranslate(input, this, BuildGUI, axis);
	m_GizmoCrossRotate = new medGizmoCrossRotate(input, this, BuildGUI, axis);
	m_Listener = listener;
}

/** 
Set input vme for the gizmo*/
void medGizmoCrossRotateTranslate::SetInput(mafVME *vme)
{
	m_GizmoCrossTranslate->SetInput(vme);
	m_GizmoCrossRotate->SetInput(vme);
}

void medGizmoCrossRotateTranslate::SetRefSys(mafVME *refSys)
{
	m_GizmoCrossTranslate->SetRefSys(refSys);
	m_GizmoCrossRotate->SetRefSys(refSys);
}

void medGizmoCrossRotateTranslate::SetAbsPose( mafMatrix *absPose)
{
	m_GizmoCrossRotate->SetAbsPose(absPose);
	m_GizmoCrossTranslate->SetAbsPose(absPose);
}

mafMatrix * medGizmoCrossRotateTranslate::GetAbsPose()
{
	return m_GizmoCrossTranslate->GetAbsPose();
}

void medGizmoCrossRotateTranslate::Show(bool show)
{
	m_GizmoCrossTranslate->Show(show);
	m_GizmoCrossRotate->Show(show);
}

void medGizmoCrossRotateTranslate::SetName( mafString name )
{
	m_NameRTG = name;
	m_GizmoCrossRotate->SetName(name);
	m_GizmoCrossTranslate->SetName(name);
}

void medGizmoCrossRotateTranslate::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))	
	{
		switch (e->GetId())
		{
			//receiving pose matrices from the fan
			case ID_TRANSFORM:
			{
				if (maf_event->GetSender() == m_GizmoCrossTranslate)
				{
					this->m_GizmoCrossRotate->SetAbsPose(m_GizmoCrossTranslate->GetAbsPose());
					mafEventMacro(*maf_event);
				}
				else if (maf_event->GetSender() == m_GizmoCrossRotate)
				{
					this->m_GizmoCrossTranslate->SetAbsPose(m_GizmoCrossRotate->GetAbsPose());
					mafEventMacro(*maf_event);
				}
			}
			break;
			default:
			{
				mafEventMacro(*e);
			}
		}
	}
}