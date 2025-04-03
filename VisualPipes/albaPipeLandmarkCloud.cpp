/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeLandmarkCloud
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

#include "albaPipeLandmarkCloud.h"
#include "albaSceneNode.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h"
#include "mmaMaterial.h"

#include "albaVMELandmark.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMEOutput.h"
#include "albaVMEOutputPointSet.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"
#include "vtkSphereSource.h"
#include "vtkPolyDataNormals.h"
#include "vtkALBAExtendedGlyph3D.h"
#include "vtkALBALandmarkCloudOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkPolyData.h"
#include "vtkDataSet.h"
#include "vtkRenderer.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeLandmarkCloud);
//----------------------------------------------------------------------------
//#include "albaMemDbg.h"

//----------------------------------------------------------------------------
albaPipeLandmarkCloud::albaPipeLandmarkCloud()
:albaPipe()
//----------------------------------------------------------------------------
{
  m_Landmark = NULL;
  m_Cloud    = NULL;
  m_MaterialButton  = NULL;
  m_SphereSource  = NULL;
  m_Normals       = NULL;
  m_Glyph         = NULL;
  m_CloudMapper   = NULL;
  m_CloudActor    = NULL;
  m_CloudSelectionActor     = NULL;
	m_CloundCornerFilter = NULL;

  m_Radius = 1.0;
  m_ScalarVisibility = false;
  m_RenderingDisplayListFlag = 0;
}
//----------------------------------------------------------------------------
void albaPipeLandmarkCloud::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;
  m_SphereSource  = NULL;
  m_Normals       = NULL;
  m_Glyph         = NULL;
  m_CloudMapper   = NULL;
  m_CloudActor    = NULL;
  m_CloudSelectionActor     = NULL;

  double r = 0;
  int resolution = 15;

  m_Vme->AddObserver(this);

  assert(m_Vme->IsALBAType(albaVMELandmarkCloud) || m_Vme->IsALBAType(albaVMELandmark));
  
  m_Cloud     = albaVMELandmarkCloud::SafeDownCast(m_Vme);
  m_Landmark  = albaVMELandmark::SafeDownCast(m_Vme);
  
  if (m_Cloud)
  {
    CreateCloudPipe(m_Cloud->GetOutput()->GetVTKData(), m_Cloud->GetRadius(), m_Cloud->GetSphereResolution());
  }
  else
  {
    CreateCloudPipe(m_Landmark->GetOutput()->GetVTKData(), m_Landmark->GetRadius(), m_Landmark->GetSphereResolution());
  }
}
//----------------------------------------------------------------------------
albaPipeLandmarkCloud::~albaPipeLandmarkCloud()
//----------------------------------------------------------------------------
{
  if (m_Vme)
  {
  	m_Vme->RemoveObserver(this);
  }

  RemoveClosedCloudPipe();

  cppDEL(m_MaterialButton);
}
//----------------------------------------------------------------------------
void albaPipeLandmarkCloud::Select(bool sel)
//----------------------------------------------------------------------------
{
	m_Selected = sel;
	if(m_CloudActor && m_CloudActor->GetVisibility()) 
    m_CloudSelectionActor->SetVisibility(sel);
}
//----------------------------------------------------------------------------
albaGUI *albaPipeLandmarkCloud::CreateGui()
//----------------------------------------------------------------------------
{
	assert(m_Gui == NULL);
	m_Gui = new albaGUI(this);
	if(m_Vme && m_Vme->IsALBAType(albaVMELandmarkCloud))
	{
		
		m_Gui->Bool(ID_SCALAR_VISIBILITY,_("scalar vis."), &m_ScalarVisibility,0,_("turn on/off the scalar visibility"));
		m_Gui->Divider();
		m_MaterialButton = new albaGUIMaterialButton(m_Vme,this);
		m_Gui->AddGui(m_MaterialButton->GetGui());
		m_Gui->Divider();
	}

	return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeLandmarkCloud::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void albaPipeLandmarkCloud::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
      case ID_SCALAR_VISIBILITY:
      {
        if(m_CloudMapper) m_CloudMapper->SetScalarVisibility(m_ScalarVisibility);
        if (m_ScalarVisibility)
        {
          vtkPolyData *data = (vtkPolyData *)m_Vme->GetOutput()->GetVTKData();
          if(data == NULL) return;
          double range[2];
          data->GetScalarRange(range);
          m_CloudMapper->SetScalarRange(range);
        }
				GetLogicManager()->CameraUpdate();
      }
      break;
      default:
        albaEventMacro(*e);
      break;
    }
  }
  else if (alba_event->GetId() == albaVMELandmarkCloud::CLOUD_RADIUS_MODIFIED)
  {
    if (m_SphereSource)
    {
			double radius = m_Cloud->GetRadius();
      m_SphereSource->SetRadius(radius);
			m_CloundCornerFilter->SetCloudRadius(radius);
      GetLogicManager()->CameraUpdate();
    }
  }
  else if (alba_event->GetId() == albaVMELandmarkCloud::CLOUD_SPHERE_RES)
  {
    if (m_SphereSource)
    {
      m_SphereSource->SetThetaResolution(m_Cloud->GetSphereResolution());
      m_SphereSource->SetPhiResolution(m_Cloud->GetSphereResolution());
			GetLogicManager()->CameraUpdate();
    }
  }
  
}
//----------------------------------------------------------------------------
void albaPipeLandmarkCloud::CreateCloudPipe(vtkDataSet *data, double radius, double resolution)
//----------------------------------------------------------------------------
{
  vtkNEW(m_SphereSource);
  m_SphereSource->SetRadius(radius);
  m_SphereSource->SetThetaResolution(resolution);
  m_SphereSource->SetPhiResolution(resolution);
  m_SphereSource->Update();

  vtkNEW(m_Normals);
  m_Normals->SetInputConnection(m_SphereSource->GetOutputPort());
  m_Normals->Update();

  vtkNEW(m_Glyph);
  m_Glyph->SetInputData(data);
  m_Glyph->SetSource(m_Normals->GetOutput());
  m_Glyph->OrientOff();
  m_Glyph->ScalingOff();
  m_Glyph->ScalarVisibilityOn();
  m_Glyph->Update();

  vtkNEW(m_CloudMapper);
  m_CloudMapper->SetInputConnection(m_Glyph->GetOutputPort());
  m_CloudMapper->ScalarVisibilityOff();
 
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

  if (m_AssemblyFront)
  {
  	m_AssemblyFront->AddPart(m_CloudActor);
  }
  else if (m_RenFront)
  {
    m_RenFront->AddActor(m_CloudActor);
  }

  // selection highlight
  vtkNEW(m_CloundCornerFilter);
  m_CloundCornerFilter->SetInputData(data);
	m_CloundCornerFilter->SetCloudRadius(radius);

  vtkALBASmartPointer<vtkPolyDataMapper> corner_mapper;
  corner_mapper->SetInputConnection(m_CloundCornerFilter->GetOutputPort());

  vtkALBASmartPointer<vtkProperty> corner_props;
  corner_props->SetColor(1,1,1);
  corner_props->SetAmbient(1);
  corner_props->SetRepresentationToWireframe();
  corner_props->SetInterpolationToFlat();

  vtkNEW(m_CloudSelectionActor);
  m_CloudSelectionActor->SetMapper(corner_mapper);
  m_CloudSelectionActor->VisibilityOff();
  m_CloudSelectionActor->PickableOff();
  m_CloudSelectionActor->SetProperty(corner_props);
  m_CloudSelectionActor->SetScale(1.01,1.01,1.01);

  if (m_AssemblyFront)
  {
  	m_AssemblyFront->AddPart(m_CloudSelectionActor);
  }
  else if (m_RenFront)
  {
    m_RenFront->AddActor(m_CloudSelectionActor);
  }
}
//----------------------------------------------------------------------------
void albaPipeLandmarkCloud::RemoveClosedCloudPipe()
//----------------------------------------------------------------------------
{
  if(m_CloudActor)
  {
    if (m_AssemblyFront)
    {
      m_AssemblyFront->RemovePart(m_CloudActor);
    }
    else if (m_RenFront)
    {
      m_RenFront->RemoveActor(m_CloudActor);
    }
  }
  if (m_CloudSelectionActor)
  {
    if (m_AssemblyFront)
    {
      m_AssemblyFront->RemovePart(m_CloudSelectionActor);
    }
    else if (m_RenFront)
    {
      m_RenFront->RemoveActor(m_CloudSelectionActor);
    }
  }

  vtkDEL(m_SphereSource);
  vtkDEL(m_Normals);
  vtkDEL(m_Glyph);
  vtkDEL(m_CloudMapper);
  vtkDEL(m_CloudActor);
	vtkDEL(m_CloudSelectionActor);
	vtkDEL(m_CloundCornerFilter);
}
