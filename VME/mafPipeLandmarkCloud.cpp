/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeLandmarkCloud.cpp,v $
  Language:  C++
  Date:      $Date: 2006-09-20 15:35:37 $
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

#include "mafPipeLandmarkCloud.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mafEventSource.h"
#include "mmaMaterial.h"

#include "mafVMELandmark.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMEOutput.h"
#include "mafVMEOutputPointSet.h"

#include "vtkMAFAssembly.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataNormals.h"
#include "vtkExtendedGlyph3D.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkDataSet.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(mafPipeLandmarkCloud);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeLandmarkCloud::mafPipeLandmarkCloud()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Landmark = NULL;
  m_Cloud    = NULL;
  m_MaterialButton  = NULL;

  m_Radius = 1.0;
  m_ScalarVisibility = 0;
  m_RenderingDisplayListFlag = 0;
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloud::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;
  m_SphereSource  = NULL;
  m_Normals       = NULL;
  m_Glyph         = NULL;
  m_CloudMapper   = NULL;
  m_CloudActor    = NULL;
  m_OutlineFilter = NULL;
  m_CloudSelectionMapper    = NULL;
  m_CloudSelectionProperty  = NULL;
  m_CloudSelectionActor     = NULL;

  double r = 0;
  int resolution = 15;

  m_Vme->GetEventSource()->AddObserver(this);

  assert(m_Vme->IsMAFType(mafVMELandmarkCloud) || m_Vme->IsMAFType(mafVMELandmark));
  
  m_Cloud     = mafVMELandmarkCloud::SafeDownCast(m_Vme);
  m_Landmark  = mafVMELandmark::SafeDownCast(m_Vme);
  
  if (m_Cloud && m_Cloud->IsOpen())
  {
    int num_lm = m_Cloud->GetNumberOfLandmarks();
    for (int i = 0; i < num_lm; i++)
    {
      mafVME *child_lm = m_Cloud->GetLandmark(i);
      mafEvent e(this,VME_SHOW,child_lm,true);
      m_Cloud->ForwardUpEvent(&e);
    }
  }
  else
  {
    if (m_Cloud)
    {
      CreateClosedCloudPipe(m_Cloud->GetOutput()->GetVTKData(), m_Cloud->GetRadius(), m_Cloud->GetSphereResolution());
    }
    else
    {
      CreateClosedCloudPipe(m_Landmark->GetOutput()->GetVTKData(), m_Landmark->GetRadius(), m_Landmark->GetSphereResolution());
    }
  }
}
//----------------------------------------------------------------------------
mafPipeLandmarkCloud::~mafPipeLandmarkCloud()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);

  if (m_Cloud && m_Cloud->IsOpen())
  {
    int num_lm = m_Cloud->GetNumberOfLandmarks();
    for (int i = 0; i < num_lm; i++)
    {
      mafVME *child_lm = m_Cloud->GetLandmark(i);
      mafEvent e(this,VME_SHOW,child_lm,false);
      m_Cloud->ForwardUpEvent(&e);
    }
  }
  else
  {
    RemoveClosedCloudPipe();
  }
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloud::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_CloudActor && m_CloudActor->GetVisibility()) 
    m_CloudSelectionActor->SetVisibility(sel);
}
//----------------------------------------------------------------------------
mmgGui *mafPipeLandmarkCloud::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Bool(ID_SCALAR_VISIBILITY,_("scalar vis."), &m_ScalarVisibility,0,_("turn on/off the scalar visibility"));
  m_Gui->Divider();
  m_MaterialButton = new mmgMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_Gui->Bool(ID_RENDERING_DISPLAY_LIST,_("display list"),&m_RenderingDisplayListFlag,0,_("turn on/off \nrendering displaylist calculation"));

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloud::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
/*	double r = 10;
  double resolution = 15;

  if(m_Cloud)
  {
    r = m_Cloud->GetRadius();
    resolution = m_Cloud->GetSphereResolution();
  }
	else if(m_Landmark)
  {
    r = m_Landmark->GetRadius();
    resolution = ((mafVMELandmarkCloud *)m_Landmark->GetParent())->GetSphereResolution();
  }

	m_SphereSource->SetRadius(r);
	m_SphereSource->SetThetaResolution(resolution);
	m_SphereSource->SetPhiResolution(resolution);
	m_SphereSource->Update();
	m_Glyph->Update();*/
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloud::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId()) 
    {
      case ID_SCALAR_VISIBILITY:
      {
        m_CloudMapper->SetScalarVisibility(m_ScalarVisibility);
        if (m_ScalarVisibility)
        {
          vtkPolyData *data = (vtkPolyData *)m_Vme->GetOutput()->GetVTKData();
          if(data == NULL) return;
          double range[2];
          data->GetScalarRange(range);
          m_CloudMapper->SetScalarRange(range);
        }
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      }
      break;
      case ID_RENDERING_DISPLAY_LIST:
        m_CloudMapper->SetImmediateModeRendering(m_RenderingDisplayListFlag);
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
      default:
        mafEventMacro(*e);
      break;
    }
  }
  else if (maf_event->GetId() == mafVMELandmarkCloud::CLOUDE_OPEN_CLOSE)
  {
    if(m_Cloud->IsOpen())
    {
      RemoveClosedCloudPipe();
      int num_lm = m_Cloud->GetNumberOfLandmarks();
      for (int i = 0; i < num_lm; i++)
      {
        mafVME *child_lm = m_Cloud->GetLandmark(i);
        mafEvent e(this,VME_SHOW,child_lm,true);
        //m_Cloud->ForwardUpEvent(&e);
        mafEventMacro(e);
      }
    }
    else
    {
      /*int num_lm = m_Cloud->GetNumberOfLandmarks();
      for (int i = 0; i < num_lm; i++)
      {
        mafVME *child_lm = m_Cloud->GetLandmark(i);
        mafEvent e(this,VME_SHOW,child_lm,false);
        m_Cloud->ForwardUpEvent(&e);
      }*/
      CreateClosedCloudPipe(m_Cloud->GetOutput()->GetVTKData(), m_Cloud->GetRadius(), m_Cloud->GetSphereResolution());
    }
    mafEventMacro(mafEvent(this,CAMERA_UPDATE));
  }
  else if (maf_event->GetId() == mafVMELandmarkCloud::CLOUDE_RADIUS_MODIFIED)
  {
    if (m_SphereSource)
    {
      m_SphereSource->SetRadius(m_Cloud->GetRadius());
    }
  }
  else if (maf_event->GetId() == mafVMELandmarkCloud::CLOUDE_RADIUS_MODIFIED)
  {
    if (m_SphereSource)
    {
      m_SphereSource->SetThetaResolution(m_Cloud->GetSphereResolution());
      m_SphereSource->SetPhiResolution(m_Cloud->GetSphereResolution());
    }
  }
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloud::CreateClosedCloudPipe(vtkDataSet *data, double radius, double resolution)
//----------------------------------------------------------------------------
{
  vtkNEW(m_SphereSource);
  m_SphereSource->SetRadius(radius);
  m_SphereSource->SetThetaResolution(resolution);
  m_SphereSource->SetPhiResolution(resolution);
  m_SphereSource->Update();

  vtkNEW(m_Normals);
  m_Normals->SetInput(m_SphereSource->GetOutput());
  m_Normals->Update();

  vtkNEW(m_Glyph);
  m_Glyph->SetInput(data);
  m_Glyph->SetSource(m_Normals->GetOutput());
  m_Glyph->OrientOff();
  m_Glyph->ScalingOff();
  m_Glyph->ScalarVisibilityOn();
  m_Glyph->Update();

  vtkNEW(m_CloudMapper);
  m_CloudMapper->SetInput(m_Glyph->GetOutput());
  m_CloudMapper->ScalarVisibilityOff();
  if(m_Vme->IsAnimated())				
    m_CloudMapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
  else
    m_CloudMapper->ImmediateModeRenderingOff();

  vtkNEW(m_CloudActor);
  if (m_Cloud)
  {
    m_CloudActor->SetProperty(m_Cloud->GetMaterial()->m_Prop);
  }
  else
  {
    m_CloudActor->SetProperty(m_Landmark->GetMaterial()->m_Prop);
  }
  m_CloudActor->GetProperty()->SetInterpolationToGouraud();
  m_CloudActor->SetMapper(m_CloudMapper);

  m_AssemblyFront->AddPart(m_CloudActor);

  // selection hilight
  vtkNEW(m_OutlineFilter);
  m_OutlineFilter->SetInput(m_Glyph->GetOutput());  

  vtkNEW(m_CloudSelectionMapper);
  m_CloudSelectionMapper->SetInput(m_OutlineFilter->GetOutput());

  vtkNEW(m_CloudSelectionProperty);
  m_CloudSelectionProperty->SetColor(1,1,1);
  m_CloudSelectionProperty->SetAmbient(1);
  m_CloudSelectionProperty->SetRepresentationToWireframe();
  m_CloudSelectionProperty->SetInterpolationToFlat();

  vtkNEW(m_CloudSelectionActor);
  m_CloudSelectionActor->SetMapper(m_CloudSelectionMapper);
  m_CloudSelectionActor->VisibilityOff();
  m_CloudSelectionActor->PickableOff();
  m_CloudSelectionActor->SetProperty(m_CloudSelectionProperty);
  m_CloudSelectionActor->SetScale(1.01,1.01,1.01);

  m_AssemblyFront->AddPart(m_CloudSelectionActor);
}
//----------------------------------------------------------------------------
void mafPipeLandmarkCloud::RemoveClosedCloudPipe()
//----------------------------------------------------------------------------
{
  if(m_CloudActor)
  {
    m_AssemblyFront->RemovePart(m_CloudActor);
  }
  if (m_CloudSelectionActor)
  {
    m_AssemblyFront->RemovePart(m_CloudSelectionActor);
  }

  vtkDEL(m_SphereSource);
  vtkDEL(m_Normals);
  vtkDEL(m_Glyph);
  vtkDEL(m_CloudMapper);
  vtkDEL(m_CloudActor);

  vtkDEL(m_OutlineFilter);
  vtkDEL(m_CloudSelectionMapper);
  vtkDEL(m_CloudSelectionProperty);
  vtkDEL(m_CloudSelectionActor);
}
