/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeLandmarkCloud.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-22 13:46:02 $
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

#include "mafPipeLandmarkCloud.h"
#include "mafSceneNode.h"
#include "mmaMaterial.h"
#include "mafEventSource.h"
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
  m_Radius = 1.0;
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

  assert(m_Vme->IsMAFType(mafVMELandmarkCloud));
  m_Cloud = mafVMELandmarkCloud::SafeDownCast(m_Vme);
  
  if (m_Cloud->IsOpen())
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
    CreateClosedCloudPipe();
  }
}
//----------------------------------------------------------------------------
mafPipeLandmarkCloud::~mafPipeLandmarkCloud()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);

  if (m_Cloud->IsOpen())
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
        m_Cloud->ForwardUpEvent(&e);
      }
    }
    else
    {
      int num_lm = m_Cloud->GetNumberOfLandmarks();
      for (int i = 0; i < num_lm; i++)
      {
        mafVME *child_lm = m_Cloud->GetLandmark(i);
        mafEvent e(this,VME_SHOW,child_lm,false);
        m_Cloud->ForwardUpEvent(&e);
      }
      CreateClosedCloudPipe();
    }
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
void mafPipeLandmarkCloud::CreateClosedCloudPipe()
//----------------------------------------------------------------------------
{
  m_SphereSource = vtkSphereSource::New();
  m_SphereSource->SetRadius(m_Cloud->GetRadius());
  m_SphereSource->SetThetaResolution(m_Cloud->GetSphereResolution());
  m_SphereSource->SetPhiResolution(m_Cloud->GetSphereResolution());
  m_SphereSource->Update();

  m_Normals = vtkPolyDataNormals::New();
  m_Normals->SetInput(m_SphereSource->GetOutput());
  m_Normals->Update();

  m_Glyph = vtkExtendedGlyph3D::New();
  m_Glyph->SetInput(m_Cloud->GetPointSetOutput()->GetPointSetData());
  m_Glyph->SetSource(m_Normals->GetOutput());
  m_Glyph->OrientOff();
  m_Glyph->ScalingOff();
  m_Glyph->ScalarVisibilityOn();
  m_Glyph->Update();

  m_CloudMapper = vtkPolyDataMapper::New();
  m_CloudMapper->SetInput(m_Glyph->GetOutput());
  m_CloudMapper->ScalarVisibilityOff();
  if(m_Vme->IsAnimated())				
    m_CloudMapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
  else
    m_CloudMapper->ImmediateModeRenderingOff();

  m_CloudActor = vtkActor::New();
  m_CloudActor->SetProperty(m_Cloud->GetPointSetOutput()->GetMaterial()->m_Prop);
  m_CloudActor->GetProperty()->SetInterpolationToGouraud();
  m_CloudActor->SetMapper(m_CloudMapper);

  m_AssemblyFront->AddPart(m_CloudActor);

  // selection hilight
  m_OutlineFilter = vtkOutlineCornerFilter::New();
  m_OutlineFilter->SetInput(m_Glyph->GetOutput());  

  m_CloudSelectionMapper = vtkPolyDataMapper::New();
  m_CloudSelectionMapper->SetInput(m_OutlineFilter->GetOutput());

  m_CloudSelectionProperty = vtkProperty::New();
  m_CloudSelectionProperty->SetColor(1,1,1);
  m_CloudSelectionProperty->SetAmbient(1);
  m_CloudSelectionProperty->SetRepresentationToWireframe();
  m_CloudSelectionProperty->SetInterpolationToFlat();

  m_CloudSelectionActor = vtkActor::New();
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
