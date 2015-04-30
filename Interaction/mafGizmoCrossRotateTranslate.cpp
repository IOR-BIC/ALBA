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

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGizmoCrossRotateTranslate.h"

#include "mafDecl.h"

#include "mafGizmoCrossTranslate.h"
#include "mafGizmoCrossRotate.h"
#include "mafEvent.h"
#include "mafVMESlicer.h"
#include "mafVMEVolumeGray.h"

#include "mafGizmoCrossRotateAxis.h"
#include "mafGizmoCrossTranslateAxis.h"
#include "mafGizmoCrossTranslatePlane.h"

mafGizmoCrossRotateTranslate::mafGizmoCrossRotateTranslate()
{
	m_NameRTG = "UNDEFINED_RTG_NAME";
	m_GizmoCrossTranslate = NULL;
	m_GizmoCrossRotate = NULL;
	m_Listener = NULL;
}

mafGizmoCrossRotateTranslate::~mafGizmoCrossRotateTranslate()
{

}

void mafGizmoCrossRotateTranslate::Create(mafVME *input, mafObserver* listener, bool BuildGUI, int normal)
{
	// input should be a mafVMESlicer slicing a mafVMEVolumeGray
	mafVMESlicer *slicer = NULL;
	slicer = mafVMESlicer::SafeDownCast(input);

	assert(slicer);

	mafVMEVolumeGray *slicedVolume = NULL;

	slicedVolume = mafVMEVolumeGray::SafeDownCast(slicer->GetSlicedVMELink());
	assert(slicedVolume);
	
	m_GizmoCrossTranslate = new mafGizmoCrossTranslate(input, this, BuildGUI, normal);
	m_GizmoCrossRotate = new mafGizmoCrossRotate(input, this, BuildGUI, normal);
	m_Listener = listener;

	SetAutoscale(true);
	SetAlwaysVisible(true);

	// TODO REFACTOR THIS:
    // Tested at 1280 * 1024 and 1280 * 720
	// SetRenderWindowHeightPercentage needs some fixing in order to be 
    // dependent on render window height only 
	SetRenderWindowHeightPercentage(0.30);
}

/** 
Set input vme for the gizmo*/
void mafGizmoCrossRotateTranslate::SetInput(mafVME *vme)
{
	m_GizmoCrossTranslate->SetInput(vme);
	m_GizmoCrossRotate->SetInput(vme);
}

void mafGizmoCrossRotateTranslate::SetRefSys(mafVME *refSys)
{
	m_GizmoCrossTranslate->SetRefSys(refSys);
	m_GizmoCrossRotate->SetRefSys(refSys);
}

void mafGizmoCrossRotateTranslate::SetAbsPose( mafMatrix *absPose)
{
	m_GizmoCrossRotate->SetAbsPose(absPose);
	m_GizmoCrossTranslate->SetAbsPose(absPose);
}

mafMatrix * mafGizmoCrossRotateTranslate::GetAbsPose()
{
	return m_GizmoCrossTranslate->GetAbsPose();
}

void mafGizmoCrossRotateTranslate::Show(bool show)
{
	m_GizmoCrossTranslate->Show(show);
	m_GizmoCrossRotate->Show(show);
}

void mafGizmoCrossRotateTranslate::SetName( mafString name )
{
	m_NameRTG = name;
	m_GizmoCrossRotate->SetName(name);
	m_GizmoCrossTranslate->SetName(name);
}

void mafGizmoCrossRotateTranslate::OnEvent(mafEventBase *maf_event)
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

void mafGizmoCrossRotateTranslate::SetAutoscale( bool autoscale )
{
	mafGizmoInterface::SetAutoscale(autoscale);

	m_GizmoCrossRotate->SetAutoscale(autoscale);
	m_GizmoCrossTranslate->SetAutoscale(autoscale);
}

void mafGizmoCrossRotateTranslate::SetAlwaysVisible( bool alwaysVisible )
{
	mafGizmoInterface::SetAlwaysVisible(alwaysVisible);

	m_GizmoCrossRotate->SetAlwaysVisible(alwaysVisible);
	m_GizmoCrossTranslate->SetAlwaysVisible(alwaysVisible);
}

void mafGizmoCrossRotateTranslate::SetRenderWindowHeightPercentage(double percentage)
{
	mafGizmoInterface::SetRenderWindowHeightPercentage(percentage);

	m_GizmoCrossRotate->SetRenderWindowHeightPercentage(percentage);
	m_GizmoCrossTranslate->SetRenderWindowHeightPercentage(percentage);
}

void mafGizmoCrossRotateTranslate::SetColor(int component, int color)
{
	double colorToSet[3] = {0,0,0};

	if (color == RED)
	{
		colorToSet[0] = 1.0;
	}
	else if (color == GREEN)
	{
		colorToSet[1] = 1.0;
	}
	else if (color == BLUE)
	{
		colorToSet[2] = 1.0;
	}
	
	if (component == GREW)
	{
		m_GizmoCrossRotate->GetGizmoCrossRotateAxisEW()->SetColor(colorToSet);	
	}
	else if (component == GTAEW)
	{
		m_GizmoCrossTranslate->GetGTUpDown()->SetColor(colorToSet);
	}
	else if (component == GTPEW)
	{
		m_GizmoCrossTranslate->GetGTPlane()->SetColor(mafGizmoCrossTranslatePlane::S0, colorToSet);
	}
	else if (component == GRNS)
	{
		m_GizmoCrossRotate->GetGizmoCrossRotateAxisNS()->SetColor(colorToSet);	
	}
	else if (component == GTANS)
	{
	    m_GizmoCrossTranslate->GetGTLeftRight()->SetColor(colorToSet);
	}
	else if (component == GTPNS)
	{
		m_GizmoCrossTranslate->GetGTPlane()->SetColor(mafGizmoCrossTranslatePlane::S1,colorToSet);
	}
}