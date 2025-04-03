/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipePointSet
 Authors: Silvano Imboden - Paolo Quadrani
 
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

#include "albaPipePointSet.h"
#include "albaSceneNode.h"
#include "mmaMaterial.h"

#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaVMEOutputPointSet.h"

// from albaPipePointSet
#include "vtkALBAAssembly.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipePointSet);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipePointSet::albaPipePointSet()
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipePointSet::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;
  m_Vme->GetOutput()->Update();

  assert(m_Vme->GetOutput()->IsALBAType(albaVMEOutputPointSet));
  albaVMEOutputPointSet *pointset_output = albaVMEOutputPointSet::SafeDownCast(m_Vme->GetOutput());
  assert(pointset_output);
  pointset_output->Update();
  vtkPolyData *data = pointset_output->GetPointSetData();
  assert(data);

  m_PointSetMapper = vtkPolyDataMapper::New();
	m_PointSetMapper->SetInputData(data);
	m_PointSetMapper->ScalarVisibilityOff();
  
  m_PointSetActor = vtkActor::New();
	m_PointSetActor->SetProperty(pointset_output->GetMaterial()->m_Prop);
	m_PointSetActor->GetProperty()->SetInterpolationToGouraud();
	m_PointSetActor->SetMapper(m_PointSetMapper);

  m_AssemblyFront->AddPart(m_PointSetActor);

  // selection hilight
	m_OutlineFilter = vtkOutlineCornerFilter::New();
	m_OutlineFilter->SetInputData(data);

	m_OutlineMapper = vtkPolyDataMapper::New();
	m_OutlineMapper->SetInputConnection(m_OutlineFilter->GetOutputPort());

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
albaPipePointSet::~albaPipePointSet()
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
void albaPipePointSet::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_PointSetActor->GetVisibility()) 
    m_OutlineActor->SetVisibility(sel);
}
//----------------------------------------------------------------------------
void albaPipePointSet::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
