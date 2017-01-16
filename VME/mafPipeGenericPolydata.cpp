/*=========================================================================

 Program: MAF2
 Module: mafPipeGenericPolydata
 Authors: Gianluigi Crimi
 
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

#include "mafPipeGenericPolydata.h"
#include "mafSceneNode.h"
#include "mafGUI.h"
#include "mafGUIMaterialButton.h"
#include "mafAxes.h"
#include "mmaMaterial.h"
#include "mafGUILutPreset.h"
#include "mafDataVector.h"
#include "mafVMESurface.h"
#include "mafGUIMaterialButton.h"
#include "mafVMEItemVTK.h"
#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
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
#include "vtkMAFPolyDataNormals.h"


//----------------------------------------------------------------------------
mafPipeGenericPolydata::mafPipeGenericPolydata()
:mafPipeWithScalar()
{
	m_Mapper					= NULL;
	m_Actor						= NULL;
	m_OutlineActor    = NULL;
	m_Gui             = NULL;
  
	m_ShowCellsNormals = 0;
  m_Wireframe = 0;
  
  m_MaterialButton	= NULL;
	m_ActorWired			= NULL;
	m_MapperWired			= NULL;
	m_Axes						= NULL;
	m_InputAsPolydata = NULL;
	m_NormalsFilter   = NULL;

  m_UseVTKProperty  = 1;

  m_BorderElementsWiredActor = false;
	m_Border = 1;
}
//----------------------------------------------------------------------------
mafPipeGenericPolydata::~mafPipeGenericPolydata()
{
	RemoveActorsFromAssembly(m_AssemblyFront);
	
	vtkDEL(m_Mapper);
	vtkDEL(m_Actor);
	vtkDEL(m_ActorWired);
	vtkDEL(m_MapperWired);
	vtkDEL(m_OutlineActor);
	cppDEL(m_Axes);
	cppDEL(m_MaterialButton);
	cppDEL(m_NormalsFilter);
}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::Create(mafSceneNode *n)
{
	mafPipe::Create(n);

	m_Selected = false;
	m_Mapper          = NULL;
	m_Actor           = NULL;
  m_ActorWired      = NULL;
	m_OutlineActor    = NULL;
	m_Axes            = NULL;

	ExecutePipe();

	AddActorsToAssembly(m_AssemblyFront);

  if(m_RenFront)
	  m_Axes = new mafAxes(m_RenFront, m_Vme);
}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::ExecutePipe()
{
  m_Vme->Update();
  m_Vme->GetOutput()->GetVTKData()->Update();


	mafVMEOutput *vmeOutput = m_Vme->GetOutput();
	assert(vmeOutput);
	vmeOutput->Update();
	vtkDataSet *dataSet = vtkDataSet::SafeDownCast(vmeOutput->GetVTKData());
	assert(dataSet);
	dataSet->Update();

	vtkNEW(m_Mapper);
	m_Mapper->ImmediateModeRenderingOn();
	
	ManageScalarOnExecutePipe(dataSet);
	
	vtkPolyData *polyData=GetInputAsPolyData();

	vtkNEW(m_NormalsFilter);
	m_NormalsFilter->SetInput(polyData);
	m_NormalsFilter->AutoOrientNormalsOn();
	m_NormalsFilter->SetComputePointNormals(!m_ShowCellsNormals);
	m_NormalsFilter->SetComputeCellNormals(m_ShowCellsNormals);
	m_Mapper->SetInput(m_NormalsFilter->GetOutput());	


  m_Mapper->Update();
	m_Mapper->SetResolveCoincidentTopologyToPolygonOffset();

  vtkNEW(m_MapperWired);
  m_MapperWired->SetInput(polyData);
  m_MapperWired->SetScalarRange(0,0);
  m_MapperWired->ScalarVisibilityOff();

	vtkNEW(m_Actor);
	m_Actor->GetProperty()->BackfaceCullingOn();
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
	
	if(material && material->m_Prop )
		m_Wireframe=(material->m_Prop->GetRepresentation() == VTK_WIREFRAME);
	
	if(m_Wireframe)
		SetEdgesVisibilityOff();
  
  // selection highlight
  vtkMAFSmartPointer<vtkOutlineCornerFilter> corner;
	corner->SetInput(polyData);  

  vtkMAFSmartPointer<vtkPolyDataMapper> corner_mapper;
	corner_mapper->SetInput(corner->GetOutput());

  vtkMAFSmartPointer<vtkProperty> corner_props;
	corner_props->SetColor(1,1,1);
	corner_props->SetAmbient(1);
	corner_props->SetRepresentationToWireframe();
	corner_props->SetInterpolationToFlat();

	vtkNEW(m_OutlineActor);
	m_OutlineActor->SetMapper(corner_mapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(corner_props);

}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::AddActorsToAssembly(vtkMAFAssembly *assembly)
{
  assembly->AddPart(m_Actor);
  assembly->AddPart(m_ActorWired);
	assembly->AddPart(m_OutlineActor);	
}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::RemoveActorsFromAssembly(vtkMAFAssembly *assembly)
{
	if(assembly)
	{
		assembly->RemovePart(m_Actor);
		assembly->RemovePart(m_ActorWired);
		assembly->RemovePart(m_OutlineActor);
	}
}

//----------------------------------------------------------------------------
void mafPipeGenericPolydata::Select(bool sel)
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mafGUI *mafPipeGenericPolydata::CreateGui()
{
	assert(m_Gui == NULL);
	m_Gui = new mafGUI(this);
  
	m_Gui->Bool(ID_WIREFRAME,_("Wireframe"), &m_Wireframe, 1);
	m_Gui->FloatSlider(ID_BORDER_CHANGE,_("Thickness"),&m_Border,1.0,5.0);
	m_Gui->Enable(ID_BORDER_CHANGE,m_Wireframe);
	m_Gui->Divider(2);
	
	wxString normalSelector[2];
	m_Gui->Label("Lighting:");
	normalSelector[0]="Points Normals";
	normalSelector[1]="Cells Normals";
	m_Gui->Combo(ID_NORMALS_TYPE,"",&m_ShowCellsNormals,2,normalSelector);	
	m_Gui->Divider(2);

	m_Gui->Bool(ID_EDGE_VISIBILITY,_("Element Edges"), &m_BorderElementsWiredActor, 1);
	m_Gui->Enable(ID_EDGE_VISIBILITY,!m_Wireframe);

	m_Gui->Divider(2);
  m_Gui->Bool(ID_USE_VTK_PROPERTY,"Property",&m_UseVTKProperty, 1);
  m_MaterialButton = new mafGUIMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_MaterialButton->Enable(m_UseVTKProperty != 0);

	CreateScalarsGui(m_Gui);
  
  m_Gui->Divider();
  m_Gui->Label("");
  m_Gui->Update();
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::OnEvent(mafEventBase *maf_event)
{
	if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
		switch(e->GetId()) 
		{
      case ID_WIREFRAME:
        {
          if(m_Wireframe == 0) 
            SetWireframeOff();
          else
            SetWireframeOn();
          
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
					mafPipeWithScalar::OnEvent(e);

					if(m_Wireframe == 0) 
						SetWireframeOff();
					else
						SetWireframeOn();
        }
        break;
      case ID_USE_VTK_PROPERTY:
				{
					if (m_UseVTKProperty != 0)
					{
						m_Actor->SetProperty(m_ObjectMaterial->m_Prop);
					}
					else
					{
						m_Actor->SetProperty(NULL);
					}

					if(m_Wireframe == 0) 
						SetWireframeOff();
					else
						SetWireframeOn();

					if(m_Gui)
					{
						m_MaterialButton->Enable(m_UseVTKProperty != 0);
						m_MaterialButton->UpdateMaterialIcon();
						m_Gui->Update();
					}
					GetLogicManager()->CameraUpdate();
				}
				break;
			case ID_BORDER_CHANGE:
				{
					m_Actor->GetProperty()->SetLineWidth(m_Border);
					m_Actor->Modified();
					GetLogicManager()->CameraUpdate();

					m_MaterialButton->UpdateMaterialIcon();
					m_MaterialButton->GetGui()->Update();
					m_Gui->Update();
				}
				break;
			case VME_CHOOSE_MATERIAL:
				{
					mafEventMacro(*e);
					mmaMaterial *material = (mmaMaterial *)m_Vme->GetAttribute("MaterialAttributes");
					if(material && material->m_Prop )
					{
						bool newWireframe=(material->m_Prop->GetRepresentation() == VTK_WIREFRAME);
						if (newWireframe!=m_Wireframe)
						{
							m_Wireframe=newWireframe;
							if(m_Wireframe == 0) 
								SetWireframeOff();
							else
								SetWireframeOn();
						}
					}
				}
				break;
			default:
				mafPipeWithScalar::OnEvent(e);
				break;
		}
	}
  else if(maf_event->GetId() == VME_TIME_SET)
  {
    UpdateActiveScalarsInVMEDataVectorItems();
    UpdateProperty();
  }
}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::SetActorPicking(int enable)
{
	m_Actor->SetPickable(enable);
	m_Actor->Modified();
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::SetWireframeOn()
{
	m_Wireframe = true;
	m_Actor->GetProperty()->SetRepresentationToWireframe();
  m_ObjectMaterial->m_Representation=m_Actor->GetProperty()->GetRepresentation();
	m_Actor->Modified();
	m_ActorWired->SetVisibility(0);
	m_ActorWired->Modified();
	if(m_Gui)
	{ 
		m_Gui->Enable(ID_BORDER_CHANGE,true);
		m_Gui->Enable(ID_EDGE_VISIBILITY,false);
		m_Gui->Update();
	}
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::SetWireframeOff()
{
	m_Wireframe = false;
	m_Actor->GetProperty()->SetRepresentationToSurface();
	m_ObjectMaterial->m_Representation=m_Actor->GetProperty()->GetRepresentation();
  m_Actor->Modified();
	m_ActorWired->SetVisibility(m_BorderElementsWiredActor);
	m_ActorWired->Modified();
	if(m_Gui)
	{
		m_Gui->Enable(ID_BORDER_CHANGE,false);
		m_Gui->Enable(ID_EDGE_VISIBILITY,true);
		m_Gui->Update();
	}
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void mafPipeGenericPolydata::SetNormalsTypeToPoints()
{
	m_ShowCellsNormals=0;
	m_NormalsFilter->ComputeCellNormalsOff();
	m_NormalsFilter->ComputePointNormalsOn();
	m_NormalsFilter->Update();
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void mafPipeGenericPolydata::SetNormalsTypeToCells()
{
	m_ShowCellsNormals=1 ;
	m_NormalsFilter->ComputeCellNormalsOn();
	m_NormalsFilter->ComputePointNormalsOff();
	m_NormalsFilter->Update();
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
void mafPipeGenericPolydata::SetEdgesVisibilityOn()
{
	m_BorderElementsWiredActor=1;
  m_ActorWired->SetVisibility(1);
  m_ActorWired->Modified();
	GetLogicManager()->CameraUpdate();
}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::SetEdgesVisibilityOff()
{
	m_BorderElementsWiredActor=0;
  m_ActorWired->SetVisibility(0);
  m_ActorWired->Modified();
	GetLogicManager()->CameraUpdate();
}

//----------------------------------------------------------------------------
double mafPipeGenericPolydata::GetThickness()
{
	return m_Border;
}
//----------------------------------------------------------------------------
void mafPipeGenericPolydata::SetThickness(double thickness)
{
	m_Border=thickness;
	m_Actor->GetProperty()->SetLineWidth(m_Border);
	m_Actor->Modified();
	GetLogicManager()->CameraUpdate();
}

