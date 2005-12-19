/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolyline.cpp,v $
  Language:  C++
  Date:      $Date: 2005-12-19 14:55:09 $
  Version:   $Revision: 1.1 $
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

#include "mafPipePolyline.h"
#include "mafSceneNode.h"
#include "mafVME.h"
#include "mafVMEOutputPolyline.h"
#include "mmaMaterial.h"

#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipePolyline);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipePolyline::mafPipePolyline()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
}
//----------------------------------------------------------------------------
void mafPipePolyline::Create(mafSceneNode *n)
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

  mafVMEOutputPolyline *out_polyline = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  assert(out_polyline);
  vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
  assert(data);

  m_Mapper = vtkPolyDataMapper::New();
	m_Mapper->SetInput(data);
	m_Mapper->ImmediateModeRenderingOff();

  m_Actor = vtkActor::New();
	m_Actor->SetMapper(m_Mapper);
  mmaMaterial *material = out_polyline->GetMaterial();
  if (material)
    m_Actor->SetProperty(material->m_Prop);

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
}
//----------------------------------------------------------------------------
mafPipePolyline::~mafPipePolyline()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_OutlineActor);

	vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
}
//----------------------------------------------------------------------------
void mafPipePolyline::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_Actor->GetVisibility()) 
	{
			m_OutlineActor->SetVisibility(sel);
	}
}
