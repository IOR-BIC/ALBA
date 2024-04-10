/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeScalar
 Authors: Paolo Quadrani
 
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

#include "albaPipeScalar.h"
#include "albaDecl.h"
#include "albaSceneNode.h"
#include "albaGUI.h"

#include "albaTagItem.h"
#include "albaTagArray.h"

#include "albaVME.h"
#include "albaVMEScalar.h"
#include "albaVMEOutputScalar.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkRenderer.h"

#include "vtkPolyDataMapper.h"
#include "vtkTextProperty.h"
#include "vtkCubeAxesActor2D.h"
#include "vtkPolyData.h"
#include "vtkActor.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeScalar);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeScalar::albaPipeScalar()
:albaPipe()
//----------------------------------------------------------------------------
{
  m_CubeAxes  = NULL;
  m_Actor     = NULL;
}
//----------------------------------------------------------------------------
void albaPipeScalar::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;

  vtkDataSet *ds = m_Vme->GetOutput()->GetVTKData();

  vtkALBASmartPointer<vtkTextProperty> tprop;
  tprop->SetColor(1, 1, 1);
  tprop->ShadowOn();

  vtkALBASmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInputData((vtkPolyData *)ds);
  mapper->ScalarVisibilityOn();
  mapper->SetScalarRange(ds->GetScalarRange());

  vtkNEW(m_Actor);
  m_Actor->SetMapper(mapper);

  m_AssemblyFront->AddPart(m_Actor);

  vtkNEW(m_CubeAxes);
  m_CubeAxes->SetInputData(ds);
  m_CubeAxes->SetCamera(m_RenFront->GetActiveCamera());
  m_CubeAxes->SetLabelFormat("%6.4g");
  m_CubeAxes->SetNumberOfLabels(5);
  m_CubeAxes->SetFlyModeToOuterEdges();
  m_CubeAxes->SetFontFactor(0.4);
  m_CubeAxes->SetAxisTitleTextProperty(tprop);
  m_CubeAxes->SetAxisLabelTextProperty(tprop);

  m_RenFront->AddActor2D(m_CubeAxes);
}
//----------------------------------------------------------------------------
albaPipeScalar::~albaPipeScalar()
//----------------------------------------------------------------------------
{
  m_RenFront->RemoveActor2D(m_CubeAxes);
  vtkDEL(m_CubeAxes);

  m_AssemblyFront->RemovePart(m_Actor);
  vtkDEL(m_Actor);
}
//----------------------------------------------------------------------------
void albaPipeScalar::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
}
//----------------------------------------------------------------------------
albaGUI *albaPipeScalar::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new albaGUI(this);
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeScalar::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_RADIUS:
      break;
    }
		GetLogicManager()->CameraUpdate();
  }
}
//----------------------------------------------------------------------------
void albaPipeScalar::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
