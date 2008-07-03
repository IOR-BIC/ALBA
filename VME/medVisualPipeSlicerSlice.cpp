/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medVisualPipeSlicerSlice.cpp,v $
  Language:  C++
  Date:      $Date: 2008-07-03 12:06:18 $
  Version:   $Revision: 1.2 $
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

#include "medVisualPipeSlicerSlice.h"
#include "mafSceneNode.h"
#include "mafVMESurface.h"
#include "mmaMaterial.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mafAxes.h"
#include "mafDataVector.h"
#include "mafVMEGenericAbstract.h"
#include "mafVMESlicer.h"
#include "mafAbsMatrixPipe.h"
#include "vtkMAFToLinearTransform.h"
#include "mafVMEOutputPointSet.h"
#include "mafEventSource.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkAppendPolyData.h"

#include "vtkPointData.h"

#include "vtkMAFFixedCutter.h"
#include "vtkPlane.h"
#include "vtkSphereSource.h"
#include "vtkImageData.h"


#include <vector>

//----------------------------------------------------------------------------
mafCxxTypeMacro(medVisualPipeSlicerSlice);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medVisualPipeSlicerSlice::medVisualPipeSlicerSlice()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Mapper1          = NULL;
  m_Actor1           = NULL;
  m_Mapper2          = NULL;
  m_Actor2           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_MaterialButton  = NULL;
  m_Cutter1			= NULL;
  m_Cutter2			= NULL;
  m_Plane1			= NULL;
  m_Plane2			= NULL;
  m_Sphere      = NULL;
  
  m_SphereActor = NULL;
  m_SphereMapper = NULL;
  m_SphereProperty = NULL;

  m_Origin1[0] = 0;
  m_Origin1[1] = 0;
  m_Origin1[2] = 0;

  m_Origin2[0] = 0;
  m_Origin2[1] = 0;
  m_Origin2[2] = 0;

  m_Normal[0] = 0;
  m_Normal[1] = 0;
  m_Normal[2] = 1;

  m_ScalarVisibility = 0;
  m_RenderingDisplayListFlag = 0;
  m_Border=1;

	m_ShowSelection = false;
}
//----------------------------------------------------------------------------
void medVisualPipeSlicerSlice::Create(mafSceneNode *n/*, bool use_axes*/)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;
  
  
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_Axes            = NULL;

  assert(m_Vme->IsMAFType(mafVMESlicer));
  vtkPolyData *data = NULL;
  mmaMaterial *material = NULL;
  
  if(m_Vme->GetOutput()->IsMAFType(mafVMEOutputSurface))
  {
    mafVMEOutputSurface *surface_output = mafVMEOutputSurface::SafeDownCast(m_Vme->GetOutput());
    assert(surface_output);
    surface_output->Update();
    data = vtkPolyData::SafeDownCast(surface_output->GetVTKData());
    data->Update();
    material = surface_output->GetMaterial();
  }
  
  
  
  assert(data);
  vtkDataArray *scalars = data->GetPointData()->GetScalars();
  double sr[2] = {0,1};

	m_Plane1	= vtkPlane::New();
  m_Plane2	= vtkPlane::New();
	m_Cutter1 = vtkMAFFixedCutter::New();
  m_Cutter2 = vtkMAFFixedCutter::New();
  
  
  m_Plane1->SetOrigin(m_Origin1);
  m_Plane1->SetNormal(m_Normal);

  m_Plane2->SetOrigin(m_Origin2);
  m_Plane2->SetNormal(m_Normal);

	
	vtkMAFToLinearTransform* m_VTKTransform = vtkMAFToLinearTransform::New();
  m_VTKTransform->SetInputMatrix(m_Vme->GetAbsMatrixPipe()->GetMatrixPointer());
	m_Plane1->SetTransform(m_VTKTransform);
  m_Plane2->SetTransform(m_VTKTransform);

	m_Cutter1->SetInput(data);
	m_Cutter1->SetCutFunction(m_Plane1);
	m_Cutter1->Update();

  m_Cutter2->SetInput(data);
  m_Cutter2->SetCutFunction(m_Plane2);
  m_Cutter2->Update();

  if(scalars != NULL)
  {
    m_ScalarVisibility = 1;
    scalars->GetRange(sr);
  }

  assert(material);  // all vme that use PipeSurface must have the material correctly set

  vtkNEW(m_Sphere);
  double center[3];
  mafVMESlicer::SafeDownCast(m_Vme)->GetOutput()->GetVTKData()->GetCenter(center);
  m_Sphere->SetCenter(center);

  double spacing[3];
  mafVMESlicer::SafeDownCast(m_Vme)->GetSurfaceOutput()->GetMaterial()->GetMaterialTexture()->GetSpacing(spacing);
  double meanSpacing = 2*(spacing[0] + spacing[1] + spacing[2]);
  m_Sphere->SetRadius(meanSpacing);
  m_Sphere->Update();

  
  m_SphereMapper = vtkPolyDataMapper::New();
  m_SphereMapper->SetInput(m_Sphere->GetOutput());
  
  m_SphereActor = vtkActor::New();
  m_SphereActor->SetProperty(material->m_Prop);
  m_SphereActor->SetMapper(m_SphereMapper);

  m_AssemblyFront->AddPart(m_SphereActor);

  m_Mapper1 = vtkPolyDataMapper::New();
  m_Mapper1->SetInput(m_Cutter1->GetOutput());
  m_Mapper1->SetScalarVisibility(m_ScalarVisibility);
  m_Mapper1->SetScalarRange(sr);

  m_Mapper2 = vtkPolyDataMapper::New();
  m_Mapper2->SetInput(m_Cutter2->GetOutput());
  m_Mapper2->SetScalarVisibility(m_ScalarVisibility);
  m_Mapper2->SetScalarRange(sr);
  
	if(m_Vme->IsAnimated())
  {
    m_RenderingDisplayListFlag = 1;
    m_Mapper1->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
    m_Mapper2->ImmediateModeRenderingOn();
  }
	else
  {
    m_RenderingDisplayListFlag = 0;
    m_Mapper1->ImmediateModeRenderingOff();
    m_Mapper2->ImmediateModeRenderingOff();
  }

  
 
  m_Actor1 = vtkActor::New();
  m_Actor1->SetMapper(m_Mapper1);
  //if (material->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
  //{
  m_Actor1->SetProperty(material->m_Prop);
  //}

  
  m_Actor1->GetProperty()->SetLineWidth (1);
  m_AssemblyFront->AddPart(m_Actor1);

  m_Actor2 = vtkActor::New();
  m_Actor2->SetMapper(m_Mapper2);
  //if (material->m_MaterialType == mmaMaterial::USE_VTK_PROPERTY)
  //{
  m_Actor2->SetProperty(material->m_Prop);
  //}


  m_Actor2->GetProperty()->SetLineWidth (1);
  m_AssemblyFront->AddPart(m_Actor2);

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
medVisualPipeSlicerSlice::~medVisualPipeSlicerSlice()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);
  m_AssemblyFront->RemovePart(m_Actor1);
  m_AssemblyFront->RemovePart(m_Actor2);
  m_AssemblyFront->RemovePart(m_OutlineActor);
  m_AssemblyFront->RemovePart(m_SphereActor);

  vtkDEL(m_Sphere);
  vtkDEL(m_Mapper1);
  vtkDEL(m_Actor1);
  vtkDEL(m_Mapper2);
  vtkDEL(m_Actor2);
  vtkDEL(m_SphereActor);
  vtkDEL(m_SphereMapper);
  vtkDEL(m_SphereProperty);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
  vtkDEL(m_Plane1);
  vtkDEL(m_Cutter1);
  vtkDEL(m_Plane2);
  vtkDEL(m_Cutter2);
  cppDEL(m_Axes);
}
//----------------------------------------------------------------------------
void medVisualPipeSlicerSlice::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor1 && m_Actor1->GetVisibility() && m_ShowSelection) 
	{
			m_OutlineActor->SetVisibility(sel);
      //m_Axes->SetVisibility(sel);
	}
}
//----------------------------------------------------------------------------
mmgGui *medVisualPipeSlicerSlice::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->FloatSlider(ID_BORDER_CHANGE,_("Border"),&m_Border,1.0,5.0);
  m_Gui->Divider();
	return m_Gui;
}
//----------------------------------------------------------------------------
void medVisualPipeSlicerSlice::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
	  case ID_BORDER_CHANGE:
		  {
			  m_Actor1->GetProperty()->SetLineWidth(m_Border);
			  m_Actor1->Modified();
        m_Actor2->GetProperty()->SetLineWidth(m_Border);
        m_Actor2->Modified();
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
void medVisualPipeSlicerSlice::SetSlice1(double *Origin1)
//----------------------------------------------------------------------------
{
	m_Origin1[0] = Origin1[0];
	m_Origin1[1] = Origin1[1];
	m_Origin1[2] = Origin1[2];

  
  if(m_Vme)
  {
    double center[3];
    mafVMESlicer::SafeDownCast(m_Vme)->GetOutput()->GetVTKData()->GetCenter(center);
    m_Sphere->SetCenter(center);
    m_Sphere->Update();
  }
  
	
	if(m_Plane1 && m_Cutter1)
	{
		m_Plane1->SetOrigin(m_Origin1);
		m_Cutter1->SetCutFunction(m_Plane1);
		m_Cutter1->Update();
	}
}
//----------------------------------------------------------------------------
void medVisualPipeSlicerSlice::SetSlice2(double *Origin2)
//----------------------------------------------------------------------------
{
  m_Origin2[0] = Origin2[0];
  m_Origin2[1] = Origin2[1];
  m_Origin2[2] = Origin2[2];

  if(m_Vme)
  {
    double center[3];
    mafVMESlicer::SafeDownCast(m_Vme)->GetOutput()->GetVTKData()->GetCenter(center);
    m_Sphere->SetCenter(center);
    m_Sphere->Update();
  }

  if(m_Plane2 && m_Cutter2)
  {
    m_Plane2->SetOrigin(m_Origin2);
    m_Cutter2->SetCutFunction(m_Plane2);
    m_Cutter2->Update();
  }
}
//----------------------------------------------------------------------------
void medVisualPipeSlicerSlice::SetNormal(double *Normal)
//----------------------------------------------------------------------------
{
	m_Normal[0] = Normal[0];
	m_Normal[1] = Normal[1];
	m_Normal[2] = Normal[2];
	
  if(m_Vme)
  {
    double center[3];
    mafVMESlicer::SafeDownCast(m_Vme)->GetOutput()->GetVTKData()->GetCenter(center);
    m_Sphere->SetCenter(center);
    m_Sphere->Update();
  }

	if(m_Plane1 && m_Cutter1 && m_Plane2 && m_Cutter2)
	{
		m_Plane1->SetNormal(m_Normal);
		m_Cutter1->SetCutFunction(m_Plane1);
		m_Cutter1->Update();

    m_Plane2->SetNormal(m_Normal);
    m_Cutter2->SetCutFunction(m_Plane2);
    m_Cutter2->Update();
	}
}
//----------------------------------------------------------------------------
double medVisualPipeSlicerSlice::GetThickness()
//----------------------------------------------------------------------------
{
	return m_Border;
}
//----------------------------------------------------------------------------
void medVisualPipeSlicerSlice::SetThickness(double thickness)
//----------------------------------------------------------------------------
{
	m_Border=thickness;
	m_Actor1->GetProperty()->SetLineWidth(m_Border);
  m_Actor1->Modified();
  m_Actor2->GetProperty()->SetLineWidth(m_Border);
  m_Actor2->Modified();
	mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}