/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: mafPipeMeshSlice.cpp,v $
Language:  C++
Date:      $Date: 2008-05-28 16:02:13 $
Version:   $Revision: 1.13 $
Authors:   Daniele Giunchi , Stefano Perticoni
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

#include "mafPipeMeshSlice.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mmgLutPreset.h"
#include "mafAxes.h"
#include "mmaMaterial.h"
#include "mmgLutPreset.h"
#include "mafVMEOutputMesh.h"
#include "mafEventSource.h"
#include "mafAbsMatrixPipe.h"

#include "mafDataVector.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMEMesh.h"
#include "mafParabolicMeshToLinearMeshFilter.h"
#include "mmgMaterialButton.h"
#include "vtkMAFMeshCutter.h"

#include "mafVMEItemVTK.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkMAFToLinearTransform.h"

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
#include "vtkPlane.h"
#include "vtkPolyDataNormals.h"

#include <vector>

// local debug facility
const bool DEBUG_MODE = true;

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeMeshSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeMeshSlice::mafPipeMeshSlice()
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
  m_Plane           = NULL;
  m_Cutter          = NULL;

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

  m_BorderElementsWiredActor = 1;
  
  m_Origin[0] = 0;
  m_Origin[1] = 0;
  m_Origin[2] = 0;

  m_Normal[0] = 0;
  m_Normal[1] = 0;
  m_Normal[2] = 1;
  
  m_Border = 1;
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
	Superclass::Create(n);

	m_Selected = false;
	m_Mapper          = NULL;
	m_Actor           = NULL;
	m_OutlineBox      = NULL;
	m_OutlineMapper   = NULL;
	m_OutlineProperty = NULL;
	m_OutlineActor    = NULL;
	m_Axes            = NULL;
  m_ActorWired      = NULL;
  m_MapperWired     = NULL;

  m_Vme->GetEventSource()->AddObserver(this);

	ExecutePipe();

	AddActorsToAssembly(m_AssemblyFront);

  /*if(m_RenFront)
	  m_Axes = new mafAxes(m_RenFront, m_Vme);*/
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::ExecutePipe()
//----------------------------------------------------------------------------
{
  m_Vme->Update();
  m_Vme->GetOutput()->GetVTKData()->Update();

  m_Selected = false;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_Axes            = NULL;

  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputMesh));
  vtkUnstructuredGrid *data = NULL;
  

  if(m_Vme->GetOutput()->IsMAFType(mafVMEOutputMesh))
  {
    mafVMEOutputMesh *mesh_output = mafVMEOutputMesh::SafeDownCast(m_Vme->GetOutput());
    assert(mesh_output);
    mesh_output->Update();
    data = vtkUnstructuredGrid::SafeDownCast(mesh_output->GetVTKData());
    data->Update();
    m_MeshMaterial = mesh_output->GetMaterial();
  }

  CreateFieldDataControlArrays();

  assert(data);
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

  m_PointCellArraySeparation = data->GetPointData()->GetNumberOfArrays();
  m_NumberOfArrays = m_PointCellArraySeparation + data->GetCellData()->GetNumberOfArrays();

  m_Plane = vtkPlane::New();
  m_Cutter = vtkMAFMeshCutter::New();

  m_Plane->SetOrigin(m_Origin);
  m_Plane->SetNormal(m_Normal);

  vtkMAFToLinearTransform* m_VTKTransform = vtkMAFToLinearTransform::New();
  m_VTKTransform->SetInputMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrixPointer());
  m_Plane->SetTransform(m_VTKTransform);

  m_Cutter->SetInput(data);
  m_Cutter->SetCutFunction(m_Plane);
  m_Cutter->GetOutput()->Update();
  m_Cutter->Update();

  vtkNEW(m_NormalFilter);
  m_NormalFilter->SetInput(m_Cutter->GetOutput());
  m_NormalFilter->FlipNormalsOn(); //this is On because off slice of all views have camera position in the bottom 
  m_NormalFilter->Update();

  /*if(m_MeshMaterial->m_ColorLut)
  {
    m_Table = m_MeshMaterial->m_ColorLut;
  }
  else
  {
    vtkNEW(m_Table);
    lutPreset(4,m_Table);

    m_MeshMaterial->m_ColorLut = m_Table;
  }*/


  m_Table = vtkLookupTable::New();
  m_Table->SetTableRange(sr[0], sr[1]);
  m_Table->SetNumberOfColors(256);
  m_Table->Build();

  m_Mapper = vtkPolyDataMapper::New();
  m_Mapper->SetInput(m_NormalFilter->GetOutput());
  m_Mapper->SetScalarVisibility(m_ScalarMapActive);
  m_Mapper->SetScalarRange(sr);

  m_Mapper->SetColorModeToMapScalars() ;

  m_Mapper->SetScalarModeToUsePointFieldData() ;
  m_Mapper->ColorByArrayComponent(0, 0) ;
  m_Mapper->SetLookupTable(m_Table) ;
  m_Mapper->SetUseLookupTableScalarRange(1) ;



  if(m_Vme->IsAnimated())
  {
    m_RenderingDisplayListFlag = 1;
    m_Mapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
  }
  else
  {
    m_RenderingDisplayListFlag = 0;
    m_Mapper->ImmediateModeRenderingOff();
  }

  m_Actor = vtkActor::New();
  m_Actor->SetMapper(m_Mapper);

  if(m_MeshMaterial)
    m_Actor->SetProperty(m_MeshMaterial->m_Prop);

  vtkNEW(m_MapperWired);
  m_MapperWired->SetInput(m_NormalFilter->GetOutput());
  m_MapperWired->SetScalarRange(0,0);
  m_MapperWired->ScalarVisibilityOff();

  vtkNEW(m_ActorWired);
  m_ActorWired->SetMapper(m_MapperWired);
  m_ActorWired->GetProperty()->SetRepresentationToWireframe();

  //m_Actor->GetProperty()->SetLineWidth (1);
  

  // selection highlight
  m_OutlineBox = vtkOutlineCornerFilter::New();
  m_OutlineBox->SetInput(data);  

  m_OutlineMapper = vtkPolyDataMapper::New();
  m_OutlineMapper->SetInput(m_OutlineBox->GetOutput());

  m_OutlineProperty = vtkProperty::New();
  m_OutlineProperty->SetColor(1,1,1);
  m_OutlineProperty->SetAmbient(1);
  m_OutlineProperty->SetRepresentationToWireframe();
  m_OutlineProperty->SetInterpolationToFlat();

  m_OutlineActor = vtkActor::New();
  m_OutlineActor->SetMapper(m_OutlineMapper);
  m_OutlineActor->VisibilityOff();
  m_OutlineActor->PickableOff();
  m_OutlineActor->SetProperty(m_OutlineProperty);

  m_AssemblyFront->AddPart(m_OutlineActor);

  m_VTKTransform->Delete();
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::AddActorsToAssembly(vtkMAFAssembly *assembly)
//----------------------------------------------------------------------------
{
  assembly->AddPart(m_Actor);
  assembly->AddPart(m_ActorWired);
	assembly->AddPart(m_OutlineActor);	
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::RemoveActorsFromAssembly(vtkMAFAssembly *assembly)
//----------------------------------------------------------------------------
{
	assembly->RemovePart(m_Actor);
  assembly->RemovePart(m_ActorWired);
	assembly->RemovePart(m_OutlineActor);
}
//----------------------------------------------------------------------------
mafPipeMeshSlice::~mafPipeMeshSlice()
//----------------------------------------------------------------------------
{
	m_Vme->GetEventSource()->RemoveObserver(this);
	RemoveActorsFromAssembly(m_AssemblyFront);

  vtkDEL(m_Plane);
  vtkDEL(m_Cutter);
  vtkDEL(m_NormalFilter);
	vtkDEL(m_Mapper);
	vtkDEL(m_Actor);
  vtkDEL(m_Table);
	vtkDEL(m_OutlineBox);
	vtkDEL(m_OutlineMapper);
	vtkDEL(m_OutlineProperty);
	vtkDEL(m_OutlineActor);
	cppDEL(m_Axes);
  vtkDEL(m_MapperWired);
  vtkDEL(m_ActorWired);
  /*cppDEL(m_ScalarsName);
  cppDEL(m_ScalarsVTKName);*/
  //vtkDEL(m_Table);

  delete []m_ScalarsName;
  delete []m_ScalarsVTKName;
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
		m_OutlineActor->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
mmgGui *mafPipeMeshSlice::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_WIREFRAME,_("Wireframe"), &m_Wireframe, 1);
  m_Gui->Bool(ID_WIRED_ACTOR_VISIBILITY,_("Border Elem."), &m_BorderElementsWiredActor, 1);
  
  m_Gui->Bool(ID_USE_VTK_PROPERTY,"property",&m_UseVTKProperty , 1);
  m_MaterialButton = new mmgMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_MaterialButton->Enable(m_UseVTKProperty != 0);

  m_Gui->Combo(ID_SCALARS,"",&m_ScalarIndex,m_NumberOfArrays,m_ScalarsName);	
  

  m_Gui->Bool(ID_SCALAR_MAP_ACTIVE,_("enable scalar field mapping"), &m_ScalarMapActive, 1);
  m_Gui->Lut(ID_LUT,"lut",m_Table);

  m_Gui->Enable(ID_SCALARS, m_ScalarMapActive != 0);
  m_Gui->Enable(ID_LUT, m_ScalarMapActive != 0);
  m_Gui->FloatSlider(ID_BORDER_CHANGE,_("Border"),&m_Border,1.0,5.0);
  m_Gui->Divider();
  m_Gui->Update();
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::OnEvent(mafEventBase *maf_event)
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
      case ID_WIRED_ACTOR_VISIBILITY:
        {
          if(m_BorderElementsWiredActor == 0) 
            SetWiredActorVisibilityOff();
          else
            SetWiredActorVisibilityOn();
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
          
          m_Mapper->SetScalarVisibility(m_ScalarMapActive);
          
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
        }
        else
        {
          m_Actor->SetProperty(NULL);
        }
        m_MaterialButton->Enable(m_UseVTKProperty != 0);
        m_MaterialButton->UpdateMaterialIcon();
        
        m_Gui->Update();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
        break;
      case ID_BORDER_CHANGE:
		  {
			  m_Actor->GetProperty()->SetLineWidth(m_Border);
			  m_Actor->Modified();
			  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
		  }
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
void mafPipeMeshSlice::SetSlice(double *Origin)
//----------------------------------------------------------------------------
{
	m_Origin[0] = Origin[0];
	m_Origin[1] = Origin[1];
	m_Origin[2] = Origin[2];
	
	if(m_Plane && m_Cutter)
	{
		m_Plane->SetOrigin(m_Origin);
		m_Cutter->SetCutFunction(m_Plane);
		m_Cutter->Update();
    
    UpdateVtkPolyDataNormalFilterActiveScalar();
    
    m_NormalFilter->Update();
	}

  if (true == DEBUG_MODE && NULL != m_Mapper)
  {
    int scalarVisibility = m_Mapper->GetScalarVisibility();
    m_Mapper->SetScalarVisibility(m_ScalarMapActive);

    std::ostringstream stringStream;
    stringStream << "scalar visibility:" << (scalarVisibility ? "true" : "false")  << std::endl;
    
    double tr[2];
    m_Table->GetTableRange(tr);
    stringStream << "LUT sr: " << "[" << tr[0] << " , " << tr[1] << "]"  << std::endl;

    mafLogMessage(stringStream.str().c_str());
  }
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::SetNormal(double *Normal)
//----------------------------------------------------------------------------
{
	m_Normal[0] = Normal[0];
	m_Normal[1] = Normal[1];
	m_Normal[2] = Normal[2];
	

	if(m_Plane && m_Cutter)
	{
		m_Plane->SetNormal(m_Normal);
		m_Cutter->SetCutFunction(m_Plane);
		m_Cutter->Update();
    m_NormalFilter->Update();
	}
}
//----------------------------------------------------------------------------
double mafPipeMeshSlice::GetThickness()
//----------------------------------------------------------------------------
{
	return m_Border;
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::SetThickness(double thickness)
//----------------------------------------------------------------------------
{
	m_Border=thickness;
	m_Actor->GetProperty()->SetLineWidth(m_Border);
  m_Actor->Modified();
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::SetActorPicking(int enable)
//----------------------------------------------------------------------------
{
	m_Actor->SetPickable(enable);
	m_Actor->Modified();
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::SetWireframeOn()
//----------------------------------------------------------------------------
{
  m_Actor->GetProperty()->SetRepresentationToWireframe();
  m_Actor->Modified();
  m_ActorWired->SetVisibility(0);
  m_ActorWired->Modified();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::SetWireframeOff()
//----------------------------------------------------------------------------
{
  m_Actor->GetProperty()->SetRepresentationToSurface();
  m_Actor->Modified();
  m_ActorWired->SetVisibility(1);
  m_ActorWired->Modified();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::SetWiredActorVisibilityOn()
//----------------------------------------------------------------------------
{
  m_ActorWired->SetVisibility(1);
  m_ActorWired->Modified();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::SetWiredActorVisibilityOff()
//----------------------------------------------------------------------------
{
  m_ActorWired->SetVisibility(0);
  m_ActorWired->Modified();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::SetFlipNormalOn()
//----------------------------------------------------------------------------
{
  m_NormalFilter->FlipNormalsOn();
  m_NormalFilter->Update();
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::SetFlipNormalOff()
//----------------------------------------------------------------------------
{
  m_NormalFilter->FlipNormalsOff();
  m_NormalFilter->Update();
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::UpdateScalars()
//----------------------------------------------------------------------------
{
  m_Vme->GetOutput()->GetVTKData()->Update();
  m_Vme->Update();

  UpdateVtkPolyDataNormalFilterActiveScalar();
  UpdateLUTAndMapperFromNewActiveScalars();
 
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::UpdateLUTAndMapperFromNewActiveScalars()
//----------------------------------------------------------------------------
{
  vtkUnstructuredGrid *data = vtkUnstructuredGrid::SafeDownCast(m_Vme->GetOutput()->GetVTKData());
  data->Update();
  double sr[2];

  mafString activeScalarName = m_ScalarsVTKName[m_ScalarIndex].c_str();

  if(m_ActiveScalarType == POINT_TYPE)
    data->GetPointData()->GetScalars(activeScalarName.GetCStr())->GetRange(sr);
  else if(m_ActiveScalarType == CELL_TYPE)
    data->GetCellData()->GetScalars(activeScalarName.GetCStr())->GetRange(sr);

  if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Scalar Range: [" << sr[0] << " , " << sr[1] << "]"  << std::endl;
      mafLogMessage(stringStream.str().c_str());
    }
  
  m_Table->SetTableRange(sr);

  if(m_ActiveScalarType == POINT_TYPE)
    m_Mapper->SetScalarModeToUsePointData();
  if(m_ActiveScalarType == CELL_TYPE)
    m_Mapper->SetScalarModeToUseCellData();

  m_Mapper->SetInput(m_NormalFilter->GetOutput());
  m_Mapper->SetLookupTable(m_Table);  
  m_Mapper->UseLookupTableScalarRangeOn();
  if (DEBUG_MODE)
    {
      double tr[2];
      m_Table->GetTableRange(tr);

      std::ostringstream stringStream;
      stringStream << "LUT sr: " << "[" << tr[0] << " , " << tr[1] << "]"  << std::endl;
      mafLogMessage(stringStream.str().c_str());
    }
  m_Mapper->Update();

  m_Actor->Modified();

  UpdateProperty();
  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

}

//----------------------------------------------------------------------------
void mafPipeMeshSlice::CreateFieldDataControlArrays()
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

  delete []tempScalarsPointsName;

}

void mafPipeMeshSlice::UpdateVtkPolyDataNormalFilterActiveScalar()
{
  m_NormalFilter->Update();

  vtkPolyData *pd = m_NormalFilter->GetOutput();

  if(m_ActiveScalarType == POINT_TYPE)
  {
    mafString activeScalarName = m_ScalarsVTKName[m_ScalarIndex].c_str();
    int res = pd->GetPointData()->SetActiveScalars(activeScalarName.GetCStr());
    
    if (res == -1)
    {
      // the array is not in the list of active arrays
      return;
    }
    pd->GetPointData()->GetScalars()->Modified();

    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Active Scalar: POINT TYPE, " << activeScalarName.GetCStr() << std::endl;
      mafLogMessage(stringStream.str().c_str());
    }
  }
  else if(m_ActiveScalarType == CELL_TYPE)
  {
    mafString activeScalarName = m_ScalarsVTKName[m_ScalarIndex].c_str();
    int res = pd->GetCellData()->SetActiveScalars(activeScalarName.GetCStr());

    if (res == -1)
    {
      // the array is not in the list of active arrays
      return;
    }

    pd->GetCellData()->GetScalars()->Modified();

    if (DEBUG_MODE)
    {
      std::ostringstream stringStream;
      stringStream << "Active Scalar: CELL TYPE, " << activeScalarName.GetCStr() << std::endl;
      mafLogMessage(stringStream.str().c_str());
    }
  }
  m_NormalFilter->GetOutput()->Update();
  m_NormalFilter->Update();
}