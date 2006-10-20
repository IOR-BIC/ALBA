/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeScalar.cpp,v $
  Language:  C++
  Date:      $Date: 2006-10-20 08:51:27 $
  Version:   $Revision: 1.3 $
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

#include "mafPipeScalar.h"
#include "mafPipeScalar.h"
#include "mafDecl.h"
#include "mafSceneNode.h"
#include "mmgGui.h"

#include "mafVME.h"
#include "mafVMEOutputScalar.h"
#include "mafVMEScalar.h"
#include "mafTagItem.h"
#include "mafTagArray.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"

#include "vtkSphereSource.h"
#include "vtkGlyph3D.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkLookupTable.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeScalar);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeScalar::mafPipeScalar()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Radius  = 5.0;
  m_Sphere  = NULL;
  m_Glyph   = NULL;
  m_Mapper  = NULL;
  m_Actor   = NULL;
}
//----------------------------------------------------------------------------
void mafPipeScalar::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);
  
  m_Selected = false;

  mafVMEScalar *scalar = mafVMEScalar::SafeDownCast(m_Vme);
  assert(scalar);

  vtkDataSet *ds = scalar->GetScalarOutput()->GetVTKData();
  
  vtkNEW(m_Sphere);
  m_Sphere->SetRadius(m_Radius);

  vtkNEW(m_Glyph);
  m_Glyph->SetInput(ds);
  m_Glyph->SetSource(m_Sphere->GetOutput());
  
  vtkNEW(m_Mapper);
  m_Mapper->SetInput(m_Glyph->GetOutput());
  m_Mapper->ScalarVisibilityOn();

  vtkNEW(m_Actor);
  m_Actor->SetMapper(m_Mapper);

  m_AssemblyFront->AddPart(m_Actor);
}
//----------------------------------------------------------------------------
mafPipeScalar::~mafPipeScalar()
//----------------------------------------------------------------------------
{
  m_AssemblyFront->RemovePart(m_Actor);

  vtkDEL(m_Sphere);
  vtkDEL(m_Glyph);
  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
}
//----------------------------------------------------------------------------
void mafPipeScalar::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
}
//----------------------------------------------------------------------------
mmgGui *mafPipeScalar::CreateGui()
//----------------------------------------------------------------------------
{
  m_Gui = new mmgGui(this);
  m_Gui->Double(ID_RADIUS,_("radius"),&m_Radius,0.001);
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
        m_Sphere->SetRadius(m_Radius);
        m_Sphere->Update();
      break;
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void mafPipeScalar::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
