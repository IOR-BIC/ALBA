/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipePolyline.cpp,v $
  Language:  C++
  Date:      $Date: 2006-12-14 09:55:55 $
  Version:   $Revision: 1.6 $
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
#include "mafTagItem.h"
#include "mafTagArray.h"

#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkTubeFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkGlyph3D.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"

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
  m_SphereRadius      = 1.0;
  m_Capping         = 0;

  m_Sphere          = NULL;
  m_Glyph           = NULL;
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

  InitializeFromTag();

  vtkNEW(m_Sphere);
  m_Sphere->SetRadius(m_SphereRadius);
  m_Sphere->SetPhiResolution(m_SphereResolution);
  m_Sphere->SetThetaResolution(m_SphereResolution);

  vtkNEW(m_Glyph);
  m_Glyph->SetInput(data);
  m_Glyph->SetSource(m_Sphere->GetOutput());

  vtkNEW(m_Tube);
  m_Tube->UseDefaultNormalOff();
  m_Tube->SetInput(data);
  m_Tube->SetRadius(m_TubeRadius);
  m_Tube->SetCapping(m_Capping);
  m_Tube->SetNumberOfSides(m_TubeResolution);

  m_Mapper = vtkPolyDataMapper::New();

  if (m_Representation == TUBE)
  {
    m_Tube->Update();
    m_Mapper->SetInput(m_Tube->GetOutput());
  }
  else if (m_Representation == GLYPH)
  {
    m_Glyph->Update();
    vtkAppendPolyData *apd = vtkAppendPolyData::New();
    apd->AddInput(data);
    apd->AddInput(m_Glyph->GetOutput());
    apd->Update();
    m_Mapper->SetInput(apd->GetOutput());
    apd->Delete();
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

  vtkDEL(m_Sphere);
  vtkDEL(m_Glyph);
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
  const wxString representation_string[] = {_("line"), _("tube"), _("sphere")};
  int num_choices = 3;
  m_Gui = new mmgGui(this);
  m_Gui->Combo(ID_POLYLINE_REPRESENTATION,"",&m_Representation,num_choices,representation_string);
  m_Gui->Label(_("tube"));
  m_Gui->Double(ID_TUBE_RADIUS,_("radius"),&m_TubeRadius,0);
  m_Gui->Double(ID_TUBE_RESOLUTION,_("resolution"),&m_TubeResolution,0);
  m_Gui->Bool(ID_TUBE_CAPPING,_("capping"),&m_Capping);
  m_Gui->Divider(2);
  m_Gui->Label(_("sphere"));
  m_Gui->Double(ID_SPHERE_RADIUS,_("radius"),&m_SphereRadius,0);
  m_Gui->Double(ID_SPHERE_RESOLUTION,_("resolution"),&m_SphereResolution,0);

  m_Gui->Enable(ID_TUBE_RADIUS, m_Representation == TUBE);
  m_Gui->Enable(ID_TUBE_CAPPING, m_Representation == TUBE);
  m_Gui->Enable(ID_TUBE_RESOLUTION, m_Representation == TUBE);
  m_Gui->Enable(ID_SPHERE_RADIUS, m_Representation == GLYPH);
  m_Gui->Enable(ID_SPHERE_RESOLUTION, m_Representation == GLYPH);
	m_Gui->Divider();

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
      {
        SetRepresentation(m_Representation);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("REPRESENTATION");
        item->SetValue(m_Representation);
      }
      break;
      case ID_TUBE_RADIUS:
      {
        m_Tube->SetRadius(m_TubeRadius);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
        item->SetValue(m_TubeRadius);
      }
      break;
      case ID_TUBE_CAPPING:
      {
        m_Tube->SetCapping(m_Capping);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_CAPPING");
        item->SetValue(m_Capping);
      }
      break;
      case ID_TUBE_RESOLUTION:
      {
        m_Tube->SetNumberOfSides(m_TubeResolution);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("TUBE_RESOLUTION");
        item->SetValue(m_TubeResolution);
      }
      break;
      case ID_SPHERE_RADIUS:
      {
        m_Sphere->SetRadius(m_SphereRadius);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
        item->SetValue(m_SphereRadius);
      }
      break;
      case ID_SPHERE_RESOLUTION:
      {
        m_Sphere->SetPhiResolution(m_SphereResolution);
        m_Sphere->SetThetaResolution(m_SphereResolution);
        mafTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RESOLUTION");
        item->SetValue(m_SphereResolution);
      }
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
  if (m_Representation == TUBE)
  {
    m_Tube->Update();
    m_Mapper->SetInput(m_Tube->GetOutput());
  }
  else if (m_Representation == GLYPH)
  {
    m_Glyph->Update();
    vtkAppendPolyData *apd = vtkAppendPolyData::New();
    apd->AddInput(data);
    apd->AddInput(m_Glyph->GetOutput());
    apd->Update();
    m_Mapper->SetInput(apd->GetOutput());
    apd->Delete();
  }
  else
    m_Mapper->SetInput(data);
}
//----------------------------------------------------------------------------
void mafPipePolyline::InitializeFromTag()
//----------------------------------------------------------------------------
{
  mafTagItem *item = NULL;
  if (!m_Vme->GetTagArray()->IsTagPresent("REPRESENTATION"))
  {
    item = new mafTagItem();
    item->SetName("REPRESENTATION");
    item->SetValue(0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("REPRESENTATION");
  m_Representation = (int)item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("SPHERE_RADIUS"))
  {
    item = new mafTagItem();
    item->SetName("SPHERE_RADIUS");
    item->SetValue(1.0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
  m_SphereRadius = item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("SPHERE_RESOLUTION"))
  {
    item = new mafTagItem();
    item->SetName("SPHERE_RESOLUTION");
    item->SetValue(10.0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("SPHERE_RESOLUTION");
  m_SphereResolution = item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_RADIUS"))
  {
    item = new mafTagItem();
    item->SetName("TUBE_RADIUS");
    item->SetValue(1.0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("TUBE_RADIUS");
  m_TubeRadius = item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_RESOLUTION"))
  {
    item = new mafTagItem();
    item->SetName("TUBE_RESOLUTION");
    item->SetValue(10.0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("TUBE_RESOLUTION");
  m_TubeResolution = item->GetValueAsDouble();

  if (!m_Vme->GetTagArray()->IsTagPresent("TUBE_CAPPING"))
  {
    item = new mafTagItem();
    item->SetName("TUBE_CAPPING");
    item->SetValue(0);
    m_Vme->GetTagArray()->SetTag(*item);
    cppDEL(item);
  }
  item = m_Vme->GetTagArray()->GetTag("TUBE_CAPPING");
  m_Capping = (int)item->GetValueAsDouble();
}
//----------------------------------------------------------------------------
void mafPipePolyline::SetRepresentation(int representation)
//----------------------------------------------------------------------------
{
  if (representation < POLYLINE)
  {
    m_Representation = POLYLINE;
  }
  else if (representation > GLYPH)
  {
    m_Representation = GLYPH;
  }
  else
    m_Representation = representation;

  if (m_Gui)
  {
    m_Gui->Enable(ID_TUBE_RADIUS, m_Representation == TUBE);
    m_Gui->Enable(ID_TUBE_CAPPING, m_Representation == TUBE);
    m_Gui->Enable(ID_TUBE_RESOLUTION, m_Representation == TUBE);
    m_Gui->Enable(ID_SPHERE_RADIUS, m_Representation == GLYPH);
    m_Gui->Enable(ID_SPHERE_RESOLUTION, m_Representation == GLYPH);
  }
  UpdateProperty();
}
//----------------------------------------------------------------------------
void mafPipePolyline::SetRadius(double radius)
//----------------------------------------------------------------------------
{
  m_TubeRadius = radius; 
  m_SphereRadius = radius;

  m_Sphere->SetRadius(m_SphereRadius);
  mafTagItem *item = m_Vme->GetTagArray()->GetTag("SPHERE_RADIUS");
  item->SetValue(m_SphereRadius);

  mafEventMacro(mafEvent(this,CAMERA_UPDATE));
}
