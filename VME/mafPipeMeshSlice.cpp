/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeMeshSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2007-10-15 14:28:41 $
  Version:   $Revision: 1.4 $
  Authors:   Matteo Giacomoni - Daniele Giunchi
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
	SCS s.r.l. - BioComputing Competence Centre (www.scsolutions.it - www.b3c.it)
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
#include "mafAxes.h"
#include "mafDataVector.h"
#include "mafVMEGenericAbstract.h"
#include "mafAbsMatrixPipe.h"
#include "vtkMAFToLinearTransform.h"
#include "mafVMEOutputMesh.h"
#include "mafEventSource.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPointData.h"
#include "vtkMAFMeshCutter.h"
#include "vtkPlane.h"
#include "vtkDelaunay2D.h"
#include "vtkUnstructuredGrid.h"
#include "vtkLookupTable.h"
#include <vector>

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
  m_Cutter          = NULL;
  m_Plane           = NULL;

  m_Origin[0] = 0;
  m_Origin[1] = 0;
  m_Origin[2] = 0;

  m_Normal[0] = 0;
  m_Normal[1] = 0;
  m_Normal[2] = 1;

  m_ScalarVisibility = 0;
  m_RenderingDisplayListFlag = 0;
  m_Border = 1;

  m_ShowSelection = false;
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::Create(mafSceneNode *n/*, bool use_axes*/)
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

  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputMesh));
  vtkUnstructuredGrid *data = NULL;
  //mmaMaterial *material = NULL;
  
  if(m_Vme->GetOutput()->IsMAFType(mafVMEOutputMesh))
  {
    mafVMEOutputMesh *surface_output = mafVMEOutputMesh::SafeDownCast(m_Vme->GetOutput());
    assert(surface_output);
    surface_output->Update();
    data = vtkUnstructuredGrid::SafeDownCast(surface_output->GetVTKData());
    data->Update();
    //material = surface_output->GetMaterial();
  }
  
  
  assert(data);
  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  double sr[2] = {0,1};

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
  //vtkNEW(m_Filter);
  //m_Filter->SetInput(m_Cutter->GetOutput());
  //m_Filter->Update();
  if(scalars != NULL)
  {
    m_ScalarVisibility = 1;
    scalars->GetRange(sr);
  }

  vtkLookupTable *lut = vtkLookupTable::New();
  lut->SetTableRange(sr[0], sr[1]);
  lut->SetNumberOfColors(256);
  lut->Build();

  m_Mapper = vtkPolyDataMapper::New();
  m_Mapper->SetInput(m_Cutter->GetOutput());
  m_Mapper->SetScalarVisibility(m_ScalarVisibility);
  m_Mapper->SetScalarRange(sr);

  m_Mapper->ScalarVisibilityOn() ;
  m_Mapper->SetColorModeToMapScalars() ;

  m_Mapper->SetScalarModeToUsePointFieldData() ;
  m_Mapper->ColorByArrayComponent(0, 0) ;
  m_Mapper->SetLookupTable(lut) ;
  m_Mapper->SetUseLookupTableScalarRange(1) ;
  
  vtkDEL(lut);
  
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
 
  m_Actor->GetProperty()->SetLineWidth (1);
  m_AssemblyFront->AddPart(m_Actor);

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

  m_Axes = new mafAxes(m_RenFront, m_Vme);
  m_Axes->SetVisibility(0);
}
//----------------------------------------------------------------------------
mafPipeMeshSlice::~mafPipeMeshSlice()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);
  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_OutlineActor);

  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
  vtkDEL(m_Plane);
  vtkDEL(m_Cutter);
  cppDEL(m_Axes);
}
//----------------------------------------------------------------------------
void mafPipeMeshSlice::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor && m_Actor->GetVisibility() && m_ShowSelection) 
	{
			m_OutlineActor->SetVisibility(sel);
      //m_Axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
mmgGui *mafPipeMeshSlice::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->FloatSlider(ID_BORDER_CHANGE,_("Border"),&m_Border,1.0,5.0);
  m_Gui->Divider();
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
