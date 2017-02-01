/*=========================================================================

 Program: MAF2
 Module: mafPipeGizmo
 Authors: Paolo Quadrani
 
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

#include "mafPipeGizmo.h"
#include "mafSceneNode.h"
#include "mmaMaterial.h"

#include "mafVMEGizmo.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"

#include "mafEventSource.h"

#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkTextProperty.h"
#include "vtkCaptionActor2D.h"
#include "mafGizmoAutoscaleHelper.h"
#include "mafGizmoInterface.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeGizmo);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeGizmo::mafPipeGizmo()
:mafPipe()
//----------------------------------------------------------------------------
{
	m_Mapper          = NULL;
	m_GizmoActor           = NULL;
	m_OutlineActor    = NULL;
	m_CaptionActor    = NULL;
	m_GizmoAutoscaleHelper = NULL;
	m_Mediator = NULL;
	m_Caption = "";

}
//----------------------------------------------------------------------------
void mafPipeGizmo::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;
	m_GizmoActor           = NULL;
	m_OutlineActor    = NULL;
	m_Mediator = NULL;

	assert(m_Vme->IsMAFType(mafVMEGizmo));
	mafVMEGizmo *inputVMEGizmo = NULL;
	inputVMEGizmo = mafVMEGizmo::SafeDownCast(m_Vme);
	assert(inputVMEGizmo);
	inputVMEGizmo->Update();
	vtkPolyData *data = inputVMEGizmo->GetData();
	assert(data);

	m_Vme->GetEventSource()->AddObserver(this);

	m_Mapper = vtkPolyDataMapper::New();
	m_Mapper->SetInput(data);
	m_Mapper->ImmediateModeRenderingOff();

	m_GizmoActor = vtkActor::New();
	m_GizmoActor->SetMapper(m_Mapper);
	mmaMaterial *material = inputVMEGizmo->GetMaterial();
	if (material)
		m_GizmoActor->SetProperty(material->m_Prop);

	mafGizmoInterface *mediator = NULL;
	mediator = dynamic_cast<mafGizmoInterface *> (inputVMEGizmo->GetMediator());
	
	m_Mediator = mediator;

	if (m_Mediator && m_Mediator->GetAlwaysVisible())
	{
		m_AlwaysVisibleAssembly->AddPart(m_GizmoActor);
	}
	else
	{
		m_AssemblyFront->AddPart(m_GizmoActor);
	}

	if (m_Mediator && m_Mediator->GetAutoscale() == true)
	{
		mafNEW(m_GizmoAutoscaleHelper);
		m_GizmoAutoscaleHelper->FollowScaleOn();

		if (mediator->GetAlwaysVisible() == true)
		{
			m_GizmoAutoscaleHelper->SetRenderer(m_AlwaysVisibleRenderer);
		}
		else
		{
			m_GizmoAutoscaleHelper->SetRenderer(m_RenFront);
		}
		
		m_GizmoAutoscaleHelper->SetActor(m_GizmoActor);
		m_GizmoAutoscaleHelper->SetVME(m_Vme);
	}

	// selection highlight
	vtkMAFSmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInput(data);  

	vtkMAFSmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInput(corner->GetOutput());

	vtkMAFSmartPointer<vtkProperty> corner_props;
	corner_props->SetColor(1,1,1);
	corner_props->SetAmbient(1);
	corner_props->SetRepresentationToWireframe();
	corner_props->SetInterpolationToFlat();

	m_OutlineActor = vtkActor::New();
	m_OutlineActor->SetMapper(corner_mapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(corner_props);

	m_AssemblyFront->AddPart(m_OutlineActor);

	//caption
	vtkNEW(m_CaptionActor);
	m_CaptionActor->SetPosition(0,0);
	//m_CaptionActor->GetCaptionTextProperty()->SetFontFamilyToTimes();
	m_CaptionActor->GetCaptionTextProperty()->SetFontFamilyToArial();
	m_CaptionActor->GetCaptionTextProperty()->BoldOn();
	m_CaptionActor->GetCaptionTextProperty()->AntiAliasingOn();
	m_CaptionActor->GetCaptionTextProperty()->ItalicOff();
	m_CaptionActor->GetCaptionTextProperty()->ShadowOn();
	m_CaptionActor->SetPadding(0);

	m_CaptionActor->ThreeDimensionalLeaderOff();
	m_CaptionActor->SetCaption(m_Caption);

	m_CaptionActor->SetHeight(0.03);
	//m_CaptionActor->SetWidth(0.05);
	m_CaptionActor->BorderOff();

	m_CaptionActor->GetCaptionTextProperty()->ShadowOn();
	m_CaptionActor->SetVisibility(inputVMEGizmo->GetTextVisibility());

	double *colour;
	colour = inputVMEGizmo->GetTextColour();
	m_CaptionActor->GetCaptionTextProperty()->SetColor(colour);
	m_CaptionActor->SetCaption(inputVMEGizmo->GetTextValue());
	m_CaptionActor->SetAttachmentPoint(inputVMEGizmo->GetTextPosition());


	if (m_RenFront != NULL)
	{
		double h,w;
		int *size = m_RenFront->GetSize();
		h = m_CaptionActor->GetHeight();
		w = m_CaptionActor->GetWidth();

		if(w < h)
		{
			w *= size[0];
			h = w*size[1];
		}
		else
		{
			w = h*size[0];
			h *= size[1];
		}


		double newPosition[2];
		newPosition[0] =  - w/2.;
		newPosition[1] =  - h/2.;

		m_CaptionActor->SetPosition(newPosition);

		m_CaptionActor->LeaderOff();
	}

	if(NULL != m_RenFront)
		m_RenFront->AddActor2D(m_CaptionActor);

}

//----------------------------------------------------------------------------
mafPipeGizmo::~mafPipeGizmo()
//----------------------------------------------------------------------------
{
	m_Vme->GetEventSource()->RemoveObserver(this);

	if (m_Mediator && m_Mediator->GetAlwaysVisible())
	{
		m_AlwaysVisibleAssembly->RemovePart(m_GizmoActor);
	} 
	else
	{
		m_AssemblyFront->RemovePart(m_GizmoActor);
	}
	

	m_AssemblyFront->RemovePart(m_OutlineActor);
	if(NULL != m_RenFront)
		m_RenFront->RemoveActor2D(m_CaptionActor);

	mafDEL(m_GizmoAutoscaleHelper);
	vtkDEL(m_Mapper);
	vtkDEL(m_GizmoActor);
	vtkDEL(m_OutlineActor);
	vtkDEL(m_CaptionActor);
}
//----------------------------------------------------------------------------
void mafPipeGizmo::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_GizmoActor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
void mafPipeGizmo::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{
		default:
			mafEventMacro(*e);
			break;
		}
	}
	else if(maf_event->GetSender() == m_Vme && maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
	{
		UpdatePipe();
	}
}
//----------------------------------------------------------------------------
void mafPipeGizmo::UpdatePipe()
//----------------------------------------------------------------------------
{
	assert(m_Vme->IsMAFType(mafVMEGizmo));
	mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(m_Vme);
	assert(gizmo);
	gizmo->Update();
	vtkPolyData *data = gizmo->GetData();
	assert(data);

	if(m_CaptionActor != NULL)
	{
		double *colour;
		colour = gizmo->GetTextColour();
		m_CaptionActor->GetCaptionTextProperty()->SetColor(colour);
		m_CaptionActor->SetVisibility(gizmo->GetTextVisibility());
		m_CaptionActor->SetCaption(gizmo->GetTextValue());
		m_CaptionActor->SetAttachmentPoint(gizmo->GetTextPosition());


		double h,w;
		int *size = m_RenFront->GetSize();
		h = m_CaptionActor->GetHeight();
		w = m_CaptionActor->GetWidth();

		if(w < h)
		{
			w *= size[0];
			h = w*size[1];
		}
		else
		{
			w = h*size[0];
			h *= size[1];
		}

		double newPosition[2];
		newPosition[0] =  - w/2.;
		newPosition[1] =  - h/2.;

		m_CaptionActor->SetPosition(newPosition);


	}
}


