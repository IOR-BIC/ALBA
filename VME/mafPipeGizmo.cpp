/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeGizmo.cpp,v $
  Language:  C++
  Date:      $Date: 2008-02-01 12:50:52 $
  Version:   $Revision: 1.2 $
  Authors:   Paolo Quadrani
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

#include "mafPipeGizmo.h"
#include "mafSceneNode.h"
#include "mmaMaterial.h"

#include "mafVMEGizmo.h"

#include "vtkMAFAssembly.h"
#include "vtkMAFSmartPointer.h"

#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeGizmo);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeGizmo::mafPipeGizmo()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Actor           = NULL;
  m_OutlineActor    = NULL;
}
//----------------------------------------------------------------------------
void mafPipeGizmo::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;
  m_Actor           = NULL;
  m_OutlineActor    = NULL;

  assert(m_Vme->IsMAFType(mafVMEGizmo));
  mafVMEGizmo *gizmo = mafVMEGizmo::SafeDownCast(m_Vme);
  assert(gizmo);
  gizmo->Update();
  vtkPolyData *data = gizmo->GetData();
  assert(data);

  vtkMAFSmartPointer<vtkPolyDataMapper> m_Mapper;
	m_Mapper->SetInput(data);
	m_Mapper->ImmediateModeRenderingOff();

  m_Actor = vtkActor::New();
	m_Actor->SetMapper(m_Mapper);
  mmaMaterial *material = gizmo->GetMaterial();
  if (material)
    m_Actor->SetProperty(material->m_Prop);

  m_AssemblyFront->AddPart(m_Actor);

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
}
//----------------------------------------------------------------------------
mafPipeGizmo::~mafPipeGizmo()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_OutlineActor);

  vtkDEL(m_Actor);
  vtkDEL(m_OutlineActor);
}
//----------------------------------------------------------------------------
void mafPipeGizmo::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
			m_OutlineActor->SetVisibility(sel);
	}
}
