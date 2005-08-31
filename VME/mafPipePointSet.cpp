/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePointSet.cpp,v $
  Language:  C++
  Date:      $Date: 2005-08-31 09:14:26 $
  Version:   $Revision: 1.2 $
  Authors:   Silvano Imboden - Paolo Quadrani
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

#include "mafPipePointSet.h"
#include "mafSceneNode.h"
#include "mmaMaterial.h"

#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafVMEOutputPointSet.h"

// from mafPipePointSet
#include "vtkMAFAssembly.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipePointSet);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipePointSet::mafPipePointSet()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void mafPipePointSet::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;
  m_Vme->GetOutput()->Update();

  assert(m_Vme->GetOutput()->IsMAFType(mafVMEOutputPointSet));
  mafVMEOutputPointSet *pointset_output = mafVMEOutputPointSet::SafeDownCast(m_Vme->GetOutput());
  assert(pointset_output);
  pointset_output->Update();
  vtkPolyData *data = pointset_output->GetPointSetData();
  assert(data);

  m_PointSetMapper = vtkPolyDataMapper::New();
	m_PointSetMapper->SetInput(data);
	m_PointSetMapper->ScalarVisibilityOff();
  if(m_Vme->IsAnimated())				
    m_PointSetMapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
  else
    m_PointSetMapper->ImmediateModeRenderingOff();

  m_PointSetActor = vtkActor::New();
	m_PointSetActor->SetProperty(pointset_output->GetMaterial()->m_Prop);
	m_PointSetActor->GetProperty()->SetInterpolationToGouraud();
	m_PointSetActor->SetMapper(m_PointSetMapper);

  m_AssemblyFront->AddPart(m_PointSetActor);

  // selection hilight
	m_OutlineFilter = vtkOutlineCornerFilter::New();
	m_OutlineFilter->SetInput(data);

	m_OutlineMapper = vtkPolyDataMapper::New();
	m_OutlineMapper->SetInput(m_OutlineFilter->GetOutput());

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
	m_OutlineActor->SetScale(1.01,1.01,1.01);

  m_AssemblyFront->AddPart(m_OutlineActor);
}
//----------------------------------------------------------------------------
mafPipePointSet::~mafPipePointSet()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_PointSetActor);
  m_AssemblyFront->RemovePart(m_OutlineActor);

  vtkDEL(m_PointSetMapper);
  vtkDEL(m_PointSetActor);

  vtkDEL(m_OutlineFilter);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
}
//----------------------------------------------------------------------------
void mafPipePointSet::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_PointSetActor->GetVisibility()) 
    m_OutlineActor->SetVisibility(sel);
}
//----------------------------------------------------------------------------
void mafPipePointSet::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
