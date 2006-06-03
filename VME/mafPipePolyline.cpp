/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolyline.cpp,v $
  Language:  C++
  Date:      $Date: 2006-06-03 11:04:18 $
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

#include "mafPipePolyline.h"
#include "mafPipePolyline.h"
#include "mafDecl.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mmaMaterial.h"

#include "mafVME.h"
#include "mafVMEOutputPolyline.h"

#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkTubeFilter.h"
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
  m_Representation  = 0; // line by default
  m_TubeRadius      = 1.0;
  m_Capping         = 0;

  m_Tube            = NULL;
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

  mafVMEOutputPolyline *out_polyline = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  assert(out_polyline);
  vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
  assert(data);

  vtkNEW(m_Tube);
  m_Tube->UseDefaultNormalOff();
  m_Tube->SetInput(data);
  m_Tube->SetRadius(m_TubeRadius);
  m_Tube->SetCapping(m_Capping);
  m_Tube->SetNumberOfSides(20);
  m_Tube->UseDefaultNormalOff();

  m_Mapper = vtkPolyDataMapper::New();

  if (m_Representation)
  {
    m_Tube->Update();
    m_Mapper->SetInput(m_Tube->GetOutput());
  }
  else
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

  vtkDEL(m_Tube);
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
//----------------------------------------------------------------------------
mmgGui *mafPipePolyline::CreateGui()
//----------------------------------------------------------------------------
{
  const wxString representation_string[] = {_("line"), _("tube")};
  int num_choices = 2;
  m_Gui = new mmgGui(this);
  m_Gui->Combo(ID_POLYLINE_REPRESENTATION,"",&m_Representation,num_choices,representation_string);
  m_Gui->Double(ID_TUBE_RADIUS,_("radius"),&m_TubeRadius,0);
  m_Gui->Bool(ID_TUBE_CAPPING,_("capping"),&m_Capping);

  m_Gui->Enable(ID_TUBE_RADIUS, m_Representation != 0);
  m_Gui->Enable(ID_TUBE_CAPPING, m_Representation != 0);

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipePolyline::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_POLYLINE_REPRESENTATION:
      m_Gui->Enable(ID_TUBE_RADIUS, m_Representation != 0);
      m_Gui->Enable(ID_TUBE_CAPPING, m_Representation != 0);
      UpdateProperty();
      break;
    case ID_TUBE_RADIUS:
      m_Tube->SetRadius(m_TubeRadius);
      break;
    case ID_TUBE_CAPPING:
      m_Tube->SetCapping(m_Capping);
      break;
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
}
//----------------------------------------------------------------------------
void mafPipePolyline::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  mafVMEOutputPolyline *out_polyline = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  vtkPolyData *data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
  if (m_Representation)
  {
    m_Tube->Update();
    m_Mapper->SetInput(m_Tube->GetOutput());
  }
  else
    m_Mapper->SetInput(data);
}
