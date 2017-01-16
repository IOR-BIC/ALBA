/*=========================================================================

 Program: MAF2
 Module: mafPipeScalar
 Authors: Paolo Quadrani
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipeScalar.h"
#include "mafDecl.h"
#include "mafSceneNode.h"
#include "mafGUI.h"

#include "mafTagItem.h"
#include "mafTagArray.h"

#include "mafVME.h"
#include "mafVMEScalar.h"
#include "mafVMEOutputScalar.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"

#include "vtkPolyDataMapper.h"
#include "vtkTextProperty.h"
#include "vtkCubeAxesActor2D.h"
#include "vtkPolyData.h"
#include "vtkActor.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeScalar);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeScalar::mafPipeScalar()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_CubeAxes  = NULL;
  m_Actor     = NULL;
}
//----------------------------------------------------------------------------
void mafPipeScalar::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;

  vtkDataSet *ds = m_Vme->GetOutput()->GetVTKData();

  vtkMAFSmartPointer<vtkTextProperty> tprop;
  tprop->SetColor(1, 1, 1);
  tprop->ShadowOn();

  vtkMAFSmartPointer<vtkPolyDataMapper> mapper;
  mapper->SetInput((vtkPolyData *)ds);
  mapper->ScalarVisibilityOn();
  mapper->SetScalarRange(ds->GetScalarRange());

  vtkNEW(m_Actor);
  m_Actor->SetMapper(mapper);

  m_AssemblyFront->AddPart(m_Actor);

  vtkNEW(m_CubeAxes);
  m_CubeAxes->SetInput(ds);
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
mafPipeScalar::~mafPipeScalar()
//----------------------------------------------------------------------------
{
  m_RenFront->RemoveActor2D(m_CubeAxes);
  vtkDEL(m_CubeAxes);

  m_AssemblyFront->RemovePart(m_Actor);
  vtkDEL(m_Actor);
}
//----------------------------------------------------------------------------
void mafPipeScalar::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
}
//----------------------------------------------------------------------------
mafGUI *mafPipeScalar::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mafGUI(this);
	m_Gui->Divider();
  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeScalar::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
void mafPipeScalar::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
