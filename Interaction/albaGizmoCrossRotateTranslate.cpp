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

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGizmoCrossRotateTranslate.h"

#include "albaDecl.h"

#include "albaGizmoCrossTranslate.h"
#include "albaGizmoCrossRotate.h"
#include "albaEvent.h"
#include "albaVMESlicer.h"
#include "albaVMEVolumeGray.h"

#include "albaGizmoCrossRotateAxis.h"
#include "albaGizmoCrossTranslateAxis.h"
#include "albaGizmoCrossTranslatePlane.h"

albaGizmoCrossRotateTranslate::albaGizmoCrossRotateTranslate()
{
	m_NameRTG = "UNDEFINED_RTG_NAME";
	m_GizmoCrossTranslate = NULL;
	m_GizmoCrossRotate = NULL;
	m_Listener = NULL;
}

albaGizmoCrossRotateTranslate::~albaGizmoCrossRotateTranslate()
{
	delete m_GizmoCrossTranslate;
	delete m_GizmoCrossRotate;
}

void albaGizmoCrossRotateTranslate::Create(albaVME *input, albaObserver* listener, bool BuildGUI, int normal)
{
	// input should be a albaVMESlicer slicing a albaVMEVolumeGray
	albaVMESlicer *slicer = NULL;
	slicer = albaVMESlicer::SafeDownCast(input);

	albaVMEVolumeGray *slicedVolume = NULL;
	
	if (slicer)
		slicedVolume = albaVMEVolumeGray::SafeDownCast(slicer->GetSlicedVMELink());
	else
		slicedVolume = albaVMEVolumeGray::SafeDownCast(input);

	assert(slicedVolume);
	
	m_GizmoCrossTranslate = new albaGizmoCrossTranslate(input, this, BuildGUI, normal);
	m_GizmoCrossRotate = new albaGizmoCrossRotate(input, this, BuildGUI, normal);
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
void albaGizmoCrossRotateTranslate::SetInput(albaVME *vme)
{
	m_GizmoCrossTranslate->SetInput(vme);
	m_GizmoCrossRotate->SetInput(vme);
}

void albaGizmoCrossRotateTranslate::SetRefSys(albaVME *refSys)
{
	m_GizmoCrossTranslate->SetRefSys(refSys);
	m_GizmoCrossRotate->SetRefSys(refSys);
}

void albaGizmoCrossRotateTranslate::SetAbsPose( albaMatrix *absPose)
{
	m_GizmoCrossRotate->SetAbsPose(absPose);
	m_GizmoCrossTranslate->SetAbsPose(absPose);
}

albaMatrix * albaGizmoCrossRotateTranslate::GetAbsPose()
{
	return m_GizmoCrossTranslate->GetAbsPose();
}

void albaGizmoCrossRotateTranslate::Show(bool show)
{
	m_GizmoCrossTranslate->Show(show);
	m_GizmoCrossRotate->Show(show);
}

void albaGizmoCrossRotateTranslate::SetName( albaString name )
{
	m_NameRTG = name;
	m_GizmoCrossRotate->SetName(name);
	m_GizmoCrossTranslate->SetName(name);
}

void albaGizmoCrossRotateTranslate::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))	
	{
		switch (e->GetId())
		{
			//receiving pose matrices from the fan
			case ID_TRANSFORM:
			{
				if (alba_event->GetSender() == m_GizmoCrossTranslate)
				{
					this->m_GizmoCrossRotate->SetAbsPose(m_GizmoCrossTranslate->GetAbsPose());
					albaEventMacro(*alba_event);
				}
				else if (alba_event->GetSender() == m_GizmoCrossRotate)
				{
					this->m_GizmoCrossTranslate->SetAbsPose(m_GizmoCrossRotate->GetAbsPose());
					albaEventMacro(*alba_event);
				}
			}
			break;
			default:
			{
				albaEventMacro(*e);
			}
		}
	}
}

void albaGizmoCrossRotateTranslate::SetAutoscale( bool autoscale )
{
	albaGizmoInterface::SetAutoscale(autoscale);

	m_GizmoCrossRotate->SetAutoscale(autoscale);
	m_GizmoCrossTranslate->SetAutoscale(autoscale);
}

void albaGizmoCrossRotateTranslate::SetAlwaysVisible( bool alwaysVisible )
{
	albaGizmoInterface::SetAlwaysVisible(alwaysVisible);

	m_GizmoCrossRotate->SetAlwaysVisible(alwaysVisible);
	m_GizmoCrossTranslate->SetAlwaysVisible(alwaysVisible);
}

void albaGizmoCrossRotateTranslate::SetRenderWindowHeightPercentage(double percentage)
{
	albaGizmoInterface::SetRenderWindowHeightPercentage(percentage);

	m_GizmoCrossRotate->SetRenderWindowHeightPercentage(percentage);
	m_GizmoCrossTranslate->SetRenderWindowHeightPercentage(percentage);
}

void albaGizmoCrossRotateTranslate::SetColor(int component, int color)
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
		m_GizmoCrossTranslate->GetGTPlane()->SetColor(albaGizmoCrossTranslatePlane::S0, colorToSet);
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
		m_GizmoCrossTranslate->GetGTPlane()->SetColor(albaGizmoCrossTranslatePlane::S1,colorToSet);
	}
}