/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeMesh.cpp,v $
Language:  C++
Date:      $Date: 2007-06-25 09:21:29 $
Version:   $Revision: 1.4 $
Authors:   Daniele Giunchi
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

#include "mafPipeMesh.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mmgLutPreset.h"
#include "mafAxes.h"
#include "mmaMaterial.h"
#include "mmgLutPreset.h"
#include "mafVMEOutputMesh.h"

#include "mafDataVector.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMEMesh.h"
#include "mafParabolicMeshToLinearMeshFilter.h"
#include "mmgMaterialButton.h"

#include "mafVMEItemVTK.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"

#include "vtkPointData.h"
#include "vtkGeometryFilter.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkDataSetMapper.h"
#include "vtkPolyData.h"
#include "vtkUnstructuredGrid.h"
#include "vtkProperty.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkLookupTable.h"
#include "vtkCellData.h"

#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeMesh);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeMesh::mafPipeMesh()
:mafPipe()
//----------------------------------------------------------------------------
{
	m_Mapper          = NULL;
	m_Actor           = NULL;
	m_OutlineBox      = NULL;
	m_OutlineMapper   = NULL;
	m_OutlineProperty = NULL;
	m_OutlineActor    = NULL;
	m_Gui             = NULL;
  m_LinearizationFilter = NULL;
	m_GeometryFilter      = NULL;

  m_Wireframe = 0;
  m_ScalarIndex = 0;
  m_NumberOfArrays = 0;
  m_Table						= NULL;

  m_ActiveScalarType = POINT_TYPE;
  m_PointCellArraySeparation = 0;

	m_ScalarsName = NULL;
	m_ScalarsVTKName = NULL;

  m_ScalarMapActive = 0;
  m_UseVTKProperty  = 1;
}
//----------------------------------------------------------------------------
void mafPipeMesh::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;
	m_Mapper          = NULL;
	m_Actor           = NULL;
  m_ActorWired      = NULL;
	m_OutlineBox      = NULL;
	m_OutlineMapper   = NULL;
	m_OutlineProperty = NULL;
	m_OutlineActor    = NULL;
	m_Axes            = NULL;

	ExecutePipe();

	AddActorsToAssembly(m_AssemblyFront);

  if(m_RenFront)
	  m_Axes = new mafAxes(m_RenFront, m_Vme);
}
//----------------------------------------------------------------------------
void mafPipeMesh::ExecutePipe()
//----------------------------------------------------------------------------
{
  m_Vme->Update();
  m_Vme->GetOutput()->GetVTKData()->Update();

  CreateFieldDataControlArrays();

	assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputMesh));
	mafVMEOutputMesh *mesh_output = mafVMEOutputMesh::SafeDownCast(m_Vme->GetOutput());
	assert(mesh_output);
	mesh_output->Update();
	vtkUnstructuredGrid *data = vtkUnstructuredGrid::SafeDownCast(mesh_output->GetVTKData());
	assert(data);
	data->Update();

	m_MeshMaterial = mesh_output->GetMaterial();
  //m_MeshMaterial->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;

  

  m_PointCellArraySeparation = data->GetPointData()->GetNumberOfArrays();
  m_NumberOfArrays = m_PointCellArraySeparation + data->GetCellData()->GetNumberOfArrays();
 

  // point type scalars
	vtkDataArray *scalars = data->GetPointData()->GetScalars();

  double sr[2] = {0,1};
  if(scalars)
  {
    scalars->Modified();
    scalars->GetRange(sr);
    m_ActiveScalarType = POINT_TYPE;
  }
  else
  {
    scalars = data->GetCellData()->GetScalars();
    if(scalars)
    {
      scalars->Modified();
      scalars->GetRange(sr);
      m_ActiveScalarType = CELL_TYPE;
    }
  }

	if(m_MeshMaterial->m_ColorLut)
	{
	  m_Table = m_MeshMaterial->m_ColorLut;
	}
	else
	{
    vtkNEW(m_Table);
    lutPreset(4,m_Table);

		m_MeshMaterial->m_ColorLut = m_Table;
	}

  m_Table->SetValueRange(sr);
  m_Table->SetHueRange(0.667, 0.0);
  m_Table->SetTableRange(sr);
  m_Table->Build();

  // create the linearization filter
  vtkNEW(m_LinearizationFilter);
  m_LinearizationFilter->SetInput(data);
  m_LinearizationFilter->Update();

	vtkNEW(m_GeometryFilter);
	m_GeometryFilter->SetInput(m_LinearizationFilter->GetOutput());
	m_GeometryFilter->Update();

	vtkNEW(m_Mapper);
  m_Mapper->ImmediateModeRenderingOn();
  m_Mapper->SetColorModeToMapScalars();
  m_Mapper->SetLookupTable(m_Table);
	m_Mapper->SetInput(m_GeometryFilter->GetOutput());
	m_Mapper->SetScalarRange(sr);

  if(m_ActiveScalarType == POINT_TYPE)
    m_Mapper->SetScalarModeToUsePointData();
  if(m_ActiveScalarType == CELL_TYPE)
    m_Mapper->SetScalarModeToUseCellData();

  if(m_ScalarMapActive)
	  m_Mapper->ScalarVisibilityOn();
  else
    m_Mapper->ScalarVisibilityOff();

  m_Mapper->Update();


  vtkNEW(m_MapperWired);
  m_MapperWired->SetInput(m_GeometryFilter->GetOutput());
  m_MapperWired->SetScalarRange(0,0);
  m_MapperWired->ScalarVisibilityOff();

	vtkNEW(m_Actor);
	m_Actor->SetMapper(m_Mapper);
  //m_Actor->GetProperty()->SetOpacity(0.5);

  if (m_MeshMaterial->m_MaterialType == mmaMaterial::USE_LOOKUPTABLE)
  {
    m_UseVTKProperty = 0;
  }
  if (m_MeshMaterial->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
  {
    m_UseVTKProperty = 1;
    m_Actor->SetProperty(m_MeshMaterial->m_Prop);
  }

  vtkNEW(m_ActorWired);
  m_ActorWired->SetMapper(m_MapperWired);
  m_ActorWired->GetProperty()->SetRepresentationToWireframe();

  
 // m_ActorWired->GetProperty()->SetLineWidth(3);
  //m_ActorWired->SetScale(1.1);
  
  // selection highlight
	vtkNEW(m_OutlineBox);
	m_OutlineBox->SetInput(data);  

	vtkNEW(m_OutlineMapper);
	m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());

	vtkNEW(m_OutlineProperty);
	m_OutlineProperty->SetColor(1,1,1);
	m_OutlineProperty->SetAmbient(1);
	m_OutlineProperty->SetRepresentationToWireframe();
	m_OutlineProperty->SetInterpolationToFlat();

	vtkNEW(m_OutlineActor);
	m_OutlineActor->SetMapper(m_OutlineMapper);
	m_OutlineActor->VisibilityOff();
	m_OutlineActor->PickableOff();
	m_OutlineActor->SetProperty(m_OutlineProperty);

}
//----------------------------------------------------------------------------
void mafPipeMesh::AddActorsToAssembly(vtkMAFAssembly *assembly)
//----------------------------------------------------------------------------
{
  assembly->AddPart(m_Actor);
  assembly->AddPart(m_ActorWired);
	assembly->AddPart(m_OutlineActor);	
}
//----------------------------------------------------------------------------
void mafPipeMesh::RemoveActorsFromAssembly(vtkMAFAssembly *assembly)
//----------------------------------------------------------------------------
{
	assembly->RemovePart(m_Actor);
  assembly->RemovePart(m_ActorWired);
	assembly->RemovePart(m_OutlineActor);
}
//----------------------------------------------------------------------------
mafPipeMesh::~mafPipeMesh()
//----------------------------------------------------------------------------
{
	RemoveActorsFromAssembly(m_AssemblyFront);

  vtkDEL(m_LinearizationFilter);
	vtkDEL(m_GeometryFilter);
	vtkDEL(m_Mapper);
	vtkDEL(m_Actor);
  vtkDEL(m_ActorWired);
  vtkDEL(m_MapperWired);
	vtkDEL(m_OutlineBox);
	vtkDEL(m_OutlineMapper);
	vtkDEL(m_OutlineProperty);
	vtkDEL(m_OutlineActor);
	cppDEL(m_Axes);
  /*cppDEL(m_ScalarsName);
  cppDEL(m_ScalarsVTKName);*/
  //vtkDEL(m_Table);
}
//----------------------------------------------------------------------------
void mafPipeMesh::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
void mafPipeMesh::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mmgGui *mafPipeMesh::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_WIREFRAME,_("Wireframe"), &m_Wireframe);
  
  m_Gui->Bool(ID_USE_VTK_PROPERTY,"property",&m_UseVTKProperty);
  m_MaterialButton = new mmgMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_MaterialButton->Enable(m_UseVTKProperty != 0);

  m_Gui->Combo(ID_SCALARS,"",&m_ScalarIndex,m_NumberOfArrays,m_ScalarsName);	
  

  m_Gui->Bool(ID_SCALAR_MAP_ACTIVE,_("enable scalar field mapping"), &m_ScalarMapActive, 1);
  m_Gui->Lut(ID_LUT,"lut",m_Table);

  m_Gui->Enable(ID_SCALARS, m_ScalarMapActive != 0);
  m_Gui->Enable(ID_LUT, m_ScalarMapActive != 0);
  
  m_Gui->Divider();
  m_Gui->Label("");
  m_Gui->Update();
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeMesh::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
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
        }
        break;
      case ID_SCALARS:
        {
          if(m_ScalarIndex < m_PointCellArraySeparation)
          {
            m_ActiveScalarType = POINT_TYPE;
          }
          else 
          {
            m_ActiveScalarType = CELL_TYPE;
          }
          UpdateScalars();
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        }
        break;
      case ID_LUT:
        {
          double sr[2];
          m_Table->GetTableRange(sr);
          m_Mapper->SetScalarRange(sr);
        }
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        break;
      case ID_SCALAR_MAP_ACTIVE:
        {
          if(m_ScalarMapActive)
            m_Mapper->ScalarVisibilityOn();
          else
            m_Mapper->ScalarVisibilityOff();

          m_Gui->Enable(ID_SCALARS, m_ScalarMapActive != 0);
          m_Gui->Enable(ID_LUT, m_ScalarMapActive != 0);
          m_Gui->Update();

          UpdateScalars();
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        }
        break;
      case ID_USE_VTK_PROPERTY:
        if (m_UseVTKProperty != 0)
        {
          m_Actor->SetProperty(m_MeshMaterial->m_Prop);
          //m_MeshMaterial->m_MaterialType = mmaMaterial::USE_VTK_PROPERTY;
        }
        else
        {
          m_Actor->SetProperty(NULL);
          //m_MeshMaterial->m_MaterialType = mmaMaterial::USE_LOOKUPTABLE;
        }
        m_MaterialButton->Enable(m_UseVTKProperty != 0);
        m_MaterialButton->UpdateMaterialIcon();
        m_Gui->Update();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        break;
			default:
				mafEventMacro(*e);
				break;
		}
	}
  else if(maf_event->GetId() == VME_TIME_SET)
  {
    UpdateScalars();
    UpdateProperty();
  }
}
//----------------------------------------------------------------------------
void mafPipeMesh::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
	m_Actor->SetPickable(enable);
	m_Actor->Modified();
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeMesh::SetWireframeOn()
//----------------------------------------------------------------------------
{
  m_Actor->GetProperty()->SetRepresentationToWireframe();
  m_Actor->Modified();
  m_ActorWired->SetVisibility(0);
  m_ActorWired->Modified();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeMesh::SetWireframeOff()
//----------------------------------------------------------------------------
{
  m_Actor->GetProperty()->SetRepresentationToSurface();
  m_Actor->Modified();
  m_ActorWired->SetVisibility(1);
  m_ActorWired->Modified();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeMesh::UpdateScalars()
//----------------------------------------------------------------------------
{
  
  m_Vme->GetOutput()->GetVTKData()->Update();
  m_Vme->Update();
  
  if(m_ActiveScalarType == POINT_TYPE)
  {
    m_Vme->GetOutput()->GetVTKData()->GetPointData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
    m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetScalars()->Modified();
  }
  else if(m_ActiveScalarType == CELL_TYPE)
  {
    m_Vme->GetOutput()->GetVTKData()->GetCellData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
    m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetScalars()->Modified();
  }
  m_Vme->Modified();
  m_Vme->GetOutput()->GetVTKData()->Update();
  m_Vme->Update();
  

  for (mafDataVector::Iterator it = ((mafVMEMesh *)m_Vme)->GetDataVector()->Begin(); it != ((mafVMEMesh *)m_Vme)->GetDataVector()->End(); it++)
  {
    mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
    assert(item);

    vtkUnstructuredGrid *outputVTK = vtkUnstructuredGrid::SafeDownCast(item->GetData());
    if(outputVTK)
    {
      if(m_ActiveScalarType == POINT_TYPE)
      {
        outputVTK->GetPointData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
        outputVTK->GetPointData()->GetScalars()->Modified();
      }
      else if(m_ActiveScalarType == CELL_TYPE)
      {
        outputVTK->GetCellData()->SetActiveScalars(m_ScalarsVTKName[m_ScalarIndex].c_str());
        outputVTK->GetCellData()->GetScalars()->Modified();
      }
      outputVTK->Modified();
      outputVTK->Update();
      
    }
  }
  m_Vme->Modified();
  m_Vme->Update();
  
  UpdatePipeFromScalars();
  
  
}
//----------------------------------------------------------------------------
void mafPipeMesh::UpdatePipeFromScalars()
//----------------------------------------------------------------------------
{
  vtkUnstructuredGrid *data = vtkUnstructuredGrid::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
  data->Update();
  double sr[2];
  if(m_ActiveScalarType == POINT_TYPE)
    data->GetPointData()->GetScalars()->GetRange(sr);
  else if(m_ActiveScalarType == CELL_TYPE)
    data->GetCellData()->GetScalars()->GetRange(sr);

  m_Table->SetTableRange(sr);
  m_Table->SetValueRange(sr);
  m_Table->SetHueRange(0.667, 0.0);
//  m_Table->Build();


  if(m_ActiveScalarType == POINT_TYPE)
    m_Mapper->SetScalarModeToUsePointData();
  if(m_ActiveScalarType == CELL_TYPE)
    m_Mapper->SetScalarModeToUseCellData();

  m_Mapper->SetLookupTable(m_Table);
  m_Mapper->SetScalarRange(sr);
  m_Mapper->Update();

  m_Actor->Modified();

  UpdateProperty();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

}

//----------------------------------------------------------------------------
void mafPipeMesh::CreateFieldDataControlArrays()
//----------------------------------------------------------------------------
{
  //String array allocation
  int numPointScalars = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfArrays();
  int numCellScalars = m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetNumberOfArrays();

  wxString *tempScalarsPointsName=new wxString[numPointScalars + numCellScalars];
  int count=0;

  int pointArrayNumber;
  for(pointArrayNumber = 0;pointArrayNumber<numPointScalars;pointArrayNumber++)
  {
    if(strcmp(m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetArrayName(pointArrayNumber),"")!=0)
    {
      count++;
      tempScalarsPointsName[count-1]=m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetArrayName(pointArrayNumber);
    }
  }
  for(int cellArrayNumber=0;cellArrayNumber<numCellScalars;cellArrayNumber++)
  {
    if(strcmp(m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetArrayName(cellArrayNumber),"")!=0)
    {
      count++;
      tempScalarsPointsName[count-1]=m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetArrayName(cellArrayNumber);
    }
  }

  m_ScalarsName = new wxString[count];
  m_ScalarsVTKName = new wxString[count];

  for(int j=0;j<count;j++)
  {
    m_ScalarsVTKName[j]=tempScalarsPointsName[j];
    if(j<pointArrayNumber)
      m_ScalarsName[j]="[POINT] " + tempScalarsPointsName[j];
    else
      m_ScalarsName[j]="[CELL] " + tempScalarsPointsName[j];
  }

  m_PointCellArraySeparation = pointArrayNumber;

}