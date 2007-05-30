/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeMesh.cpp,v $
Language:  C++
Date:      $Date: 2007-05-30 11:58:42 $
Version:   $Revision: 1.2 $
Authors:   Daniele Giunchi - Matteo Giacomoni
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
  m_ScalarPoints = 0;
  m_Table						= NULL;

  m_ActiveScalarType = POINT_TYPE;
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

	m_Vme->Update();
	assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputMesh));
	mafVMEOutputMesh *mesh_output = mafVMEOutputMesh::SafeDownCast(m_Vme->GetOutput());
	assert(mesh_output);
	mesh_output->Update();
	vtkUnstructuredGrid *data = vtkUnstructuredGrid::SafeDownCast(mesh_output->GetVTKData());
	assert(data);
	data->Update();


  // point type scalars
	vtkDataArray *scalars = data->GetPointData()->GetScalars();
  double sr[2] = {0,1};
  if(scalars)
  {
    scalars->GetRange(sr);
    m_ActiveScalarType = POINT_TYPE;
  }
  else
  {
    scalars = data->GetCellData()->GetScalars();
    scalars->GetRange(sr);
    m_ActiveScalarType = CELL_TYPE;
  }

  vtkNEW(m_Table);
  lutPreset(4,m_Table);


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

	m_Mapper->ScalarVisibilityOn();
  m_Mapper->Update();


  vtkNEW(m_MapperWired);
  m_MapperWired->SetInput(m_GeometryFilter->GetOutput());
  m_MapperWired->SetScalarRange(0,0);
  m_MapperWired->ScalarVisibilityOff();

	vtkNEW(m_Actor);
	m_Actor->SetMapper(m_Mapper);
  //m_Actor->GetProperty()->SetOpacity(0.5);

  vtkNEW(m_ActorWired);
  m_ActorWired->SetMapper(m_MapperWired);
  m_ActorWired->GetProperty()->SetRepresentationToWireframe();

  
 // m_ActorWired->GetProperty()->SetLineWidth(3);
  //m_ActorWired->SetScale(1.1);


	m_AssemblyFront->AddPart(m_Actor);

  m_AssemblyFront->AddPart(m_ActorWired);



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

	m_AssemblyFront->AddPart(m_OutlineActor);

	m_Axes = new mafAxes(m_RenFront, m_Vme);
}
//----------------------------------------------------------------------------
mafPipeMesh::~mafPipeMesh()
//----------------------------------------------------------------------------
{
	m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_ActorWired);
	m_AssemblyFront->RemovePart(m_OutlineActor);

  vtkDEL(m_LinearizationFilter);
	vtkDEL(m_GeometryFilter);
	vtkDEL(m_Mapper);
	vtkDEL(m_Actor);
  vtkDEL(m_ActorWired);
	vtkDEL(m_OutlineBox);
	vtkDEL(m_OutlineMapper);
	vtkDEL(m_OutlineProperty);
	vtkDEL(m_OutlineActor);
	cppDEL(m_Axes);
  vtkDEL(m_Table);
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

  int numPointScalars = m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetNumberOfArrays();
  int numCellScalars = m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetNumberOfArrays();

  wxString *tempScalarsPointsName=new wxString[numPointScalars + numCellScalars];
	int count=0;
  for(int i=0;i<numPointScalars;i++)
	{
		if(strcmp(m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetArrayName(i),"")!=0)
		{
			count++;
			tempScalarsPointsName[count-1]=m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetArrayName(i);
		}
	}
  for(int j=0;j<numCellScalars;j++)
	{
		if(strcmp(m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetArrayName(j),"")!=0)
		{
			count++;
			tempScalarsPointsName[count-1]=m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetArrayName(j);
		}
	}

	m_ScalarsPointsName = new wxString[count];

	for(int j=0;j<count;j++)
		m_ScalarsPointsName[j]=tempScalarsPointsName[j];

	assert(m_Gui == NULL);
	m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_WIREFRAME,_("Wireframe"), &m_Wireframe);
  m_Gui->Combo(ID_SCALARS,"",&m_ScalarPoints,numPointScalars+numCellScalars,m_ScalarsPointsName);
	
  m_Gui->Lut(ID_LUT,"lut",m_Table);

  m_Gui->Label("");
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
          if(m_Vme->GetOutput()->GetVTKData()->GetPointData()->GetArray(m_ScalarsPointsName[m_ScalarPoints].c_str()))
          {
            m_ActiveScalarType = POINT_TYPE;
          }
          else if (m_Vme->GetOutput()->GetVTKData()->GetCellData()->GetArray(m_ScalarsPointsName[m_ScalarPoints].c_str()))
          {
            m_ActiveScalarType = CELL_TYPE;
          }
          UpdateScalarsPoints();
          mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        }
        break;
      case ID_LUT:
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
			default:
				mafEventMacro(*e);
				break;
		}
	}
  else if(maf_event->GetId() == VME_TIME_SET)
  {
    mafVMEMesh *mesh=mafVMEMesh::SafeDownCast(m_Vme);
    mesh->Update();


    vtkUnstructuredGrid *data = vtkUnstructuredGrid::SafeDownCast(mesh->GetOutput()->GetVTKData());
    double sr[2];

    if(m_ActiveScalarType == POINT_TYPE)
      data->GetPointData()->GetScalars()->GetRange(sr);
    else if (m_ActiveScalarType == CELL_TYPE)
      data->GetCellData()->GetScalars()->GetRange(sr);

    
    m_Table->SetValueRange(sr);
    m_Table->SetTableRange(sr);
    m_Table->SetHueRange(0.667, 0.0);
    //m_Table->Build();

    m_Mapper->SetLookupTable(m_Table);
    m_Mapper->SetScalarRange(sr);


    if(m_ActiveScalarType == POINT_TYPE)
      m_Mapper->SetScalarModeToUsePointData();
    if(m_ActiveScalarType == CELL_TYPE)
      m_Mapper->SetScalarModeToUseCellData();

    m_Mapper->Update();

    m_Actor->Modified();

    //mafLogMessage("Scalar Range %.3f %.3f",sr[0],sr[1]);

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
void mafPipeMesh::UpdateScalarsPoints()
//----------------------------------------------------------------------------
{
  mafVMEMesh *mesh=mafVMEMesh::SafeDownCast(m_Vme);
  mesh->GetOutput()->GetVTKData()->Update();
  mesh->Update();
  
  if(m_ActiveScalarType == POINT_TYPE)
    mesh->GetOutput()->GetVTKData()->GetPointData()->SetActiveScalars(m_ScalarsPointsName[m_ScalarPoints].c_str());
  else if(m_ActiveScalarType == CELL_TYPE)
    mesh->GetOutput()->GetVTKData()->GetCellData()->SetActiveScalars(m_ScalarsPointsName[m_ScalarPoints].c_str());
  mesh->Modified();
  mesh->GetOutput()->GetVTKData()->Update();
  mesh->Update();
  

  for (mafDataVector::Iterator it = mesh->GetDataVector()->Begin(); it != mesh->GetDataVector()->End(); it++)
  {
    mafVMEItemVTK *item = mafVMEItemVTK::SafeDownCast(it->second);
    assert(item);

    vtkUnstructuredGrid *outputVTK = vtkUnstructuredGrid::SafeDownCast(item->GetData());
    if(outputVTK)
    {
      if(m_ActiveScalarType == POINT_TYPE)
        outputVTK->GetPointData()->SetActiveScalars(m_ScalarsPointsName[m_ScalarPoints].c_str());
      else if(m_ActiveScalarType == CELL_TYPE)
        outputVTK->GetCellData()->SetActiveScalars(m_ScalarsPointsName[m_ScalarPoints].c_str());
      outputVTK->Modified();
      outputVTK->Update();
      
    }
  }
  mesh->Modified();
  mesh->Update();
  m_Vme->Modified();
  m_Vme->Update();

  vtkUnstructuredGrid *data = vtkUnstructuredGrid::SafeDownCast(mesh->GetOutput()->GetVTKData());
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