/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeGenericPolydata
 Authors: Gianluigi Crimi
 
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

#include "albaPipeGenericPolydata.h"
#include "albaSceneNode.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h"
#include "albaAxes.h"
#include "mmaMaterial.h"
#include "albaGUILutPreset.h"
#include "albaDataVector.h"
#include "albaVMESurface.h"
#include "albaGUIMaterialButton.h"
#include "albaVMEItemVTK.h"
#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkPointData.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkCellData.h"

#include <vector>
#include "vtkALBAPolyDataNormals.h"
#include "vtkScalarBarActor.h"


//----------------------------------------------------------------------------
albaPipeGenericPolydata::albaPipeGenericPolydata()
:albaPipeWithScalar()
{
	m_Mapper					= NULL;
	m_Actor						= NULL;
	m_OutlineActor    = NULL;
	m_Gui             = NULL;
  
	m_ShowCellsNormals = 0;
  m_Representation = SURFACE_REP;
  
  m_MaterialButton	= NULL;
	m_ActorWired			= NULL;
	m_MapperWired			= NULL;
	m_Axes						= NULL;
	m_PolydataConnection = NULL;
	m_NormalsFilter   = NULL;

  m_UseVTKProperty  = 1;

	m_FlipNormals = false;
	m_SkipNormalFilter = false;
  m_BorderElementsWiredActor = false;
	m_ShowOutLine = true;
	m_Border = 1;
}
//----------------------------------------------------------------------------
albaPipeGenericPolydata::~albaPipeGenericPolydata()
{
	RemoveActorsFromAssembly(m_AssemblyFront);
	DeleteScalarBarActor();

	vtkDEL(m_Mapper);
	vtkDEL(m_Actor);
	vtkDEL(m_ActorWired);
	vtkDEL(m_MapperWired);
	vtkDEL(m_OutlineActor);
	cppDEL(m_Axes);
	cppDEL(m_MaterialButton);
	vtkDEL(m_NormalsFilter);
}
//----------------------------------------------------------------------------
void albaPipeGenericPolydata::Create(albaSceneNode *n)
{
	albaPipe::Create(n);

	m_Selected = false;
	m_Mapper          = NULL;
	m_Actor           = NULL;
  m_ActorWired      = NULL;
	m_ScalarBarActor  = NULL;
	m_OutlineActor    = NULL;
	m_Axes            = NULL;

	ExecutePipe();

	AddActorsToAssembly(m_AssemblyFront);

  if(m_RenFront)
	  m_Axes = new albaAxes(m_RenFront, m_Vme);
}
//----------------------------------------------------------------------------
void albaPipeGenericPolydata::ExecutePipe()
{
  m_Vme->Update();


	albaVMEOutput *vmeOutput = m_Vme->GetOutput();
	assert(vmeOutput);
	vmeOutput->Update();
	vtkDataSet *dataSet = vtkDataSet::SafeDownCast(vmeOutput->GetVTKData());
	assert(dataSet);

	vtkNEW(m_Mapper);
	
	ManageScalarOnExecutePipe(dataSet);
	
	vtkAlgorithmOutput *polyDataPort=GetPolyDataOutputPort();

	if (m_SkipNormalFilter)
	{
		m_Mapper->SetInputConnection(polyDataPort);
	}
	else
	{
		vtkNEW(m_NormalsFilter);
		m_NormalsFilter->SetFlipNormals(m_FlipNormals);
		m_NormalsFilter->SetComputePointNormals(!m_ShowCellsNormals);
		m_NormalsFilter->SetComputeCellNormals(m_ShowCellsNormals);
		m_NormalsFilter->SetInputConnection(polyDataPort);
		m_Mapper->SetInputConnection(m_NormalsFilter->GetOutputPort());
	}

  m_Mapper->Update();
	m_Mapper->SetResolveCoincidentTopologyToOff();

  vtkNEW(m_MapperWired);
  if(m_SkipNormalFilter)
  	m_MapperWired->SetInputConnection(polyDataPort);
  else
    m_MapperWired->SetInputConnection(m_NormalsFilter->GetOutputPort());
  m_MapperWired->SetScalarRange(0,0);
  m_MapperWired->ScalarVisibilityOff();

	vtkNEW(m_Actor);
	//m_Actor->GetProperty()->BackfaceCullingOn();
	m_Actor->SetMapper(m_Mapper);

  if (m_ObjectMaterial->m_MaterialType == mmaMaterial::USE_LOOKUPTABLE)
  {
    m_UseVTKProperty = 0;
  }
  if (m_ObjectMaterial->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
  {
    m_UseVTKProperty = 1;
    m_Actor->SetProperty(m_ObjectMaterial->m_Prop);
  }

  vtkNEW(m_ActorWired);
  m_ActorWired->SetMapper(m_MapperWired);
  m_ActorWired->GetProperty()->SetRepresentationToWireframe();
	m_ActorWired->SetVisibility(m_BorderElementsWiredActor);

	mmaMaterial *material = (mmaMaterial *)m_Vme->GetAttribute("MaterialAttributes");
	
	if (material && material->m_Prop)
	{
		if (material->m_Prop->GetRepresentation() == VTK_WIREFRAME)
			m_Representation = WIREFRAME_REP;
		else if (material->m_Prop->GetRepresentation() == VTK_POINTS)
			m_Representation = POINTS_REP;
		else
			m_Representation = SURFACE_REP;
	}

	SetRepresentation((REPRESENTATIONS)m_Representation);
	
	if(m_Representation == WIREFRAME_REP)
		SetEdgesVisibilityOff();
  
  // selection highlight
  vtkALBASmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInputConnection(polyDataPort);  

  vtkALBASmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInputConnection(corner->GetOutputPort());

  vtkALBASmartPointer<vtkProperty> corner_props;
	corner_props->SetColor(1,1,1);
	corner_props->SetAmbient(1);
	corner_props->SetRepresentationToWireframe();
	corner_props->SetInterpolationToFlat();

	vtkNEW(m_OutlineActor);
	m_OutlineActor->SetMapper(corner_mapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(corner_props);

	CreateScalarBarActor();

	if (m_ScalarMapActive)
		UpdateActiveScalarsInVMEDataVectorItems();
}
//----------------------------------------------------------------------------
void albaPipeGenericPolydata::AddActorsToAssembly(vtkALBAAssembly *assembly)
{
  assembly->AddPart(m_Actor);
  assembly->AddPart(m_ActorWired);
	assembly->AddPart(m_OutlineActor);	
}
//----------------------------------------------------------------------------
void albaPipeGenericPolydata::RemoveActorsFromAssembly(vtkALBAAssembly *assembly)
{
	if(assembly)
	{
		assembly->RemovePart(m_Actor);
		assembly->RemovePart(m_ActorWired);
		assembly->RemovePart(m_OutlineActor);
	}
}

//----------------------------------------------------------------------------
void albaPipeGenericPolydata::Select(bool sel)
{
	m_Selected = sel;
	if(m_Actor && m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel && m_ShowOutLine);

		ShowScalarBarActor(m_ShowScalarBar);
	}
}

//----------------------------------------------------------------------------
albaGUI *albaPipeGenericPolydata::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = new albaGUI(this);
  
	CreateGenericPolydataGui(m_Gui);

	CreateScalarsGui(m_Gui);

	m_Gui->Divider();
	m_Gui->Label("");
	m_Gui->Update();

	return m_Gui;
}

//----------------------------------------------------------------------------
void albaPipeGenericPolydata::CreateGenericPolydataGui(albaGUI *gui)
{
	gui->Label("Representation:");
	wxString representation[3] = { "Faces", "Wireframe", "Points" };
	gui->Combo(ID_REPRESENTATION, "", &m_Representation, 3, representation);
	gui->FloatSlider(ID_THICKNESS, _("Thickness"), &m_Border, 1.0, 10.0);
	SetRepresentation((REPRESENTATIONS)m_Representation);
	gui->Divider(2);

	if (!m_SkipNormalFilter)
	{
		wxString normalSelector[2] = { "Points Normals", "Cells Normals" };
		gui->Label("Lighting:");
		gui->Combo(ID_NORMALS_TYPE, "", &m_ShowCellsNormals, 2, normalSelector);
		gui->Divider(2);
	}

	gui->Bool(ID_EDGE_VISIBILITY, _("Element Edges"), &m_BorderElementsWiredActor, 1);
	gui->Enable(ID_EDGE_VISIBILITY, m_Representation != WIREFRAME_REP);

	gui->Divider(2);
	gui->Bool(ID_USE_VTK_PROPERTY, "Property", &m_UseVTKProperty, 1);
	m_MaterialButton = new albaGUIMaterialButton(m_Vme, this);
	gui->AddGui(m_MaterialButton->GetGui());
	m_MaterialButton->Enable(m_UseVTKProperty != 0);
}

//----------------------------------------------------------------------------
void albaPipeGenericPolydata::OnEvent(albaEventBase *alba_event)
{
	if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
 		switch(e->GetId()) 
		{
      case ID_REPRESENTATION:
        {
					SetRepresentation((REPRESENTATIONS)m_Representation);
          
          if(m_Gui)
          {
   					m_MaterialButton->UpdateMaterialIcon();
   					m_MaterialButton->GetGui()->Update();
   					m_Gui->Update();
          }
        }
        break;
			case ID_NORMALS_TYPE:
				{
					if(m_ShowCellsNormals == 0) 
						SetNormalsTypeToPoints();
					else
						SetNormalsTypeToCells();
				}
				break;
      case ID_EDGE_VISIBILITY:
        {
          if(m_BorderElementsWiredActor == 0) 
            SetEdgesVisibilityOff();
          else
            SetEdgesVisibilityOn();
        }
        break;
      case ID_SCALAR_MAP_ACTIVE:
        {
					albaPipeWithScalar::OnEvent(e);

					SetRepresentation((REPRESENTATIONS)m_Representation);
        }
        break;
      case ID_USE_VTK_PROPERTY:
				{
					if (m_UseVTKProperty != 0)
						m_Actor->SetProperty(m_ObjectMaterial->m_Prop);
					else
						m_Actor->SetProperty(NULL);

					SetRepresentation((REPRESENTATIONS)m_Representation);
					SetThickness(m_Border);

					if(m_Gui)
					{
						m_MaterialButton->Enable(m_UseVTKProperty != 0);
						m_MaterialButton->UpdateMaterialIcon();
						m_Gui->Update();
					}
					GetLogicManager()->CameraUpdate();
				}
				break;
			case ID_THICKNESS:
				{
					SetThickness(m_Border);

					m_MaterialButton->UpdateMaterialIcon();
					m_MaterialButton->GetGui()->Update();
					m_Gui->Update();
				}
				break;
			case VME_CHOOSE_MATERIAL:
				{
					albaEventMacro(*e);
					mmaMaterial *material = (mmaMaterial *)m_Vme->GetAttribute("MaterialAttributes");
					if (material && material->m_Prop)
					{
						if (material->m_Prop->GetRepresentation() == VTK_WIREFRAME)
							m_Representation = WIREFRAME_REP;
						else if (material->m_Prop->GetRepresentation() == VTK_POINTS)
							m_Representation = POINTS_REP;
						else
							m_Representation = SURFACE_REP;

						SetRepresentation((REPRESENTATIONS)m_Representation);
					}
				}
				break;
			default:
				albaPipeWithScalar::OnEvent(e);
				break;
		}
	}
  else if(alba_event->GetId() == VME_TIME_SET)
  {
    UpdateActiveScalarsInVMEDataVectorItems();
    UpdateProperty();
  }
}
//----------------------------------------------------------------------------
void albaPipeGenericPolydata::SetActorPicking(int enable)
{
	m_Actor->SetPickable(enable);
	m_Actor->Modified();
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaPipeGenericPolydata::SetRepresentation(REPRESENTATIONS rep)
{
	m_Representation = rep;

	if (m_Actor)
	{
		switch (rep)
		{
			case albaPipeGenericPolydata::SURFACE_REP:
				m_Actor->GetProperty()->SetRepresentationToSurface();
				m_ActorWired->SetVisibility(m_BorderElementsWiredActor);
				break;
			case albaPipeGenericPolydata::WIREFRAME_REP:
				m_Actor->GetProperty()->SetRepresentationToWireframe();
				m_ActorWired->SetVisibility(0);
				break;
			case albaPipeGenericPolydata::POINTS_REP:
				m_Actor->GetProperty()->SetRepresentationToPoints();
				m_ActorWired->SetVisibility(m_BorderElementsWiredActor);
				break;
			default:
				break;
		}
		m_Actor->Modified();
		m_ActorWired->Modified();
		m_ObjectMaterial->m_Representation = m_Actor->GetProperty()->GetRepresentation();
	}

		if (m_Gui)
	{
		m_Gui->Enable(ID_THICKNESS, m_Representation != SURFACE_REP);
		m_Gui->Enable(ID_EDGE_VISIBILITY, m_Representation != WIREFRAME_REP); 
		m_Gui->Update();
	}
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaPipeGenericPolydata::SetNormalsTypeToPoints()
{
	m_ShowCellsNormals=0;
	if (m_NormalsFilter)
	{
		m_NormalsFilter->ComputeCellNormalsOff();
		m_NormalsFilter->ComputePointNormalsOn();
		m_NormalsFilter->Update();
	}
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaPipeGenericPolydata::SetNormalsTypeToCells()
{
	m_ShowCellsNormals=1 ;
	if (m_NormalsFilter)
	{
		m_NormalsFilter->ComputeCellNormalsOn();
		m_NormalsFilter->ComputePointNormalsOff();
		m_NormalsFilter->Update();
	}
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaPipeGenericPolydata::SetFlipNormalOn()
//----------------------------------------------------------------------------
{
	m_FlipNormals = true;
	if (m_NormalsFilter)
	{
		m_NormalsFilter->FlipNormalsOn();
	}
}
//----------------------------------------------------------------------------
void albaPipeGenericPolydata::SetFlipNormalOff()
//----------------------------------------------------------------------------
{
	m_FlipNormals = false;
	if (m_NormalsFilter)
	{
		m_NormalsFilter->FlipNormalsOff();
	}
}

//----------------------------------------------------------------------------
void albaPipeGenericPolydata::SetEdgesVisibilityOn()
{
	m_BorderElementsWiredActor=1;
  m_ActorWired->SetVisibility(1);
  m_ActorWired->Modified();
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void albaPipeGenericPolydata::SetEdgesVisibilityOff()
{
	m_BorderElementsWiredActor=0;
  m_ActorWired->SetVisibility(0);
  m_ActorWired->Modified();
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
double albaPipeGenericPolydata::GetThickness()
{
	return m_Border;
}
//----------------------------------------------------------------------------
void albaPipeGenericPolydata::SetThickness(double thickness)
{
	m_Border=thickness;
	m_Actor->GetProperty()->SetLineWidth(m_Border);
	m_Actor->GetProperty()->SetPointSize(m_Border);
	m_Actor->Modified();
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void albaPipeGenericPolydata::SetShowOutLine(int showOutLine)
{
	m_ShowOutLine = showOutLine;
	Select(m_Selected);
}

