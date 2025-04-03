/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeVector
 Authors: Roberto Mucci
 
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

#include "albaPipeVector.h"
#include "wx/busyinfo.h"

#include "albaSceneNode.h"
#include "albaGUI.h"
#include "albaGUIMaterialButton.h"
#include "mmaMaterial.h"
#include "albaVMEVector.h"

#include "vtkALBASmartPointer.h"
#include "vtkALBAAssembly.h"

#include "vtkAppendPolyData.h"
#include "vtkConeSource.h"
#include "vtkCellArray.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkLineSource.h"
#include "vtkMath.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"

#include <vector>

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeVector);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeVector::albaPipeVector()
:albaPipe()
//----------------------------------------------------------------------------
{
  m_Data            = NULL;
  m_Sphere          = NULL;
  m_ArrowTip        = NULL;
  m_Apd             = NULL;
  m_Data            = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineActor    = NULL;

  m_UseArrow = 1;
  m_UseSphere = 1;
  m_UseVTKProperty = 1;
  m_Interval = 0;
  m_Step = 20;
  m_UseBunch = 0;

  m_UseArrow = 1;
  m_UseSphere = 1;
  m_UseVTKProperty  = 1;
  m_AllBunch = 0;
}
//----------------------------------------------------------------------------
void albaPipeVector::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{  
  Superclass::Create(n);
  m_Selected = false;

  albaVMEOutputPolyline *out_polyline = albaVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  assert(out_polyline);
  m_Data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
  assert(m_Data);

  m_Vector = albaVMEVector::SafeDownCast(m_Vme);
  m_Vector->GetTimeStamps(m_TimeVector);
 
  m_Vme->AddObserver(this);

  vtkNEW(m_ArrowTip);  //Create the arrow 
  m_ArrowTip->SetResolution(20);

  vtkNEW(m_Sphere);   //Create the sphere on the Cop
  m_Sphere->SetRadius(10);
  m_Sphere->SetPhiResolution(20);
  m_Sphere->SetThetaResolution(20);
  
  m_Mapper = vtkPolyDataMapper::New();
  m_MapperBunch = vtkPolyDataMapper::New();
 
  UpdateProperty(); 

  m_Sphere->Update();
  m_Apd = vtkAppendPolyData::New();
	m_Apd->AddInputData(m_Data);
	//Here AddInputData is used because we need to remove the input
	m_Apd->AddInputData(m_Sphere->GetOutput());
  m_Apd->AddInputData(m_ArrowTip->GetOutput());
  m_Apd->Update();
  m_Mapper->SetInputConnection(m_Apd->GetOutputPort());
 

  m_Actor = vtkActor::New();
  m_Actor->SetMapper(m_Mapper);
  m_Material = out_polyline->GetMaterial();
  if (m_Material)
  m_Actor->SetProperty(m_Material->m_Prop);

  vtkNEW(m_Bunch);
  m_ActorBunch = vtkActor::New();
  m_ActorBunch->GetProperty()->SetColor(0, 255, 0); //Color of bunch of vectors (Green)
  m_ActorBunch->SetMapper(m_MapperBunch);

  m_AssemblyFront->AddPart(m_Actor);
  m_AssemblyFront->AddPart(m_ActorBunch);

  vtkALBASmartPointer<vtkOutlineCornerFilter> corner;
  corner->SetInputData(m_Data);  

  vtkALBASmartPointer<vtkPolyDataMapper> corner_mapper;
  corner_mapper->SetInputConnection(corner->GetOutputPort());

  vtkALBASmartPointer<vtkProperty> corner_props;
  corner_props->SetColor(1,1,1);
  corner_props->SetAmbient(1);
  corner_props->SetRepresentationToWireframe();
  corner_props->SetInterpolationToFlat();

  m_OutlineActor = vtkActor::New();
  m_OutlineActor->SetMapper(corner_mapper);
  m_OutlineActor->VisibilityOff();
  m_OutlineActor->PickableOff();
  m_OutlineActor->SetProperty(corner_props);

  m_AssemblyFront->AddPart(m_OutlineActor);
}

//----------------------------------------------------------------------------
albaPipeVector::~albaPipeVector()
//----------------------------------------------------------------------------
{
  m_Vme->RemoveObserver(this);
  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_ActorBunch);
  m_AssemblyFront->RemovePart(m_OutlineActor);

  vtkDEL(m_Sphere);
  vtkDEL(m_ArrowTip);
  vtkDEL(m_Apd);
  vtkDEL(m_Bunch);
  vtkDEL(m_Mapper);
  vtkDEL(m_MapperBunch);
  vtkDEL(m_Actor);
  vtkDEL(m_ActorBunch);
  vtkDEL(m_OutlineActor);
}
//----------------------------------------------------------------------------
void albaPipeVector::Select(bool sel)
//----------------------------------------------------------------------------
{
  m_Selected = sel;
  if(m_Actor->GetVisibility()) 
  {
    m_OutlineActor->SetVisibility(sel);
  }
}
//----------------------------------------------------------------------------
void albaPipeVector::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{ 

  double pointCop[3];
  m_Data->GetPoint(0,pointCop);

  if (m_UseSphere == true)
  {
    m_Sphere->SetCenter(pointCop);
  }
  
  if (m_UseArrow == true)
  {
    double pointForce[3];
    m_Data->GetPoint(1,pointForce);
    double length = sqrt(vtkMath::Distance2BetweenPoints(pointCop, pointForce));
    
    m_ArrowTip->SetCenter(pointForce[0],pointForce[1],pointForce[2]);
    m_ArrowTip->SetRadius(length/35.0);
    m_ArrowTip->SetHeight(length/15.0);
    double direction[3];
    direction[0] = pointForce[0] - pointCop[0];
    direction[1] = pointForce[1] - pointCop[1];
    direction[2] = pointForce[2] - pointCop[2];
    m_ArrowTip->SetDirection(direction);
  }
}
//----------------------------------------------------------------------------
void albaPipeVector::AllVector(bool fromTag)
//----------------------------------------------------------------------------
{
//  if(!m_TestMode)
    wxBusyInfo wait(_("Creating Vectogram, please wait..."));
 
  m_MatrixVector = m_Vector->GetMatrixVector();

  albaTimeStamp t0;
  t0 = m_Vector->GetTimeStamp();

  if (m_MatrixVector)
  {
    int i;
    for (i = 0; i< m_TimeVector.size(); i++)
    {
      if (m_TimeVector[i] > t0)
      {
        break;
      }
    }
    i--;

    if (i<0)
    {
      i=0;
    }
    int minValue = (i-m_Interval) < 0 ? 0 : (i-m_Interval);
    int maxValue = (i + m_Interval) >= m_TimeVector.size() ? m_TimeVector.size() - 1 : (i + m_Interval);

    for (albaTimeStamp n = (minValue + 1); n <= maxValue; n= n + m_Step) //Cicle to draw vectors
    {
      vtkALBASmartPointer<vtkLineSource> line;
     
      double point1[3];
      double point2[3];

      m_Vector->SetTimeStamp(m_TimeVector[n]);
      m_Vector->GetOutput()->GetVTKData()->GetPoint(0, point1);
      m_Vector->GetOutput()->GetVTKData()->GetPoint(1, point2);

      line->SetPoint1(point1);
      line->SetPoint2(point2);
 
      m_Bunch->AddInputConnection(line->GetOutputPort());
    } 

    if (m_Bunch->GetTotalNumberOfInputConnections() == 0)
    {
      m_MapperBunch->SetInputConnection(m_Bunch->GetOutputPort());
    }
  }
}
//----------------------------------------------------------------------------
albaGUI *albaPipeVector::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new albaGUI(this);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_VTK_PROPERTY,_("property"),&m_UseVTKProperty);
  m_MaterialButton = new albaGUIMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_MaterialButton->Enable(m_UseVTKProperty != 0);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_ARROW,_("Arrow"),&m_UseArrow,1,_("To visualize the arrow tip"));
  m_Gui->Bool(ID_USE_SPHERE,_("COP"),&m_UseSphere,1,_("To visualize sphere on COP"));
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_BUNCH,_("Vectogram"),&m_UseBunch,0,_("To visualize the Vectogram"));
  m_Gui->Divider();
  m_Gui->Integer(ID_STEP,_("Step:"),&m_Step,0,(m_TimeVector.size()),_("1 To visualize every vector"));
  m_Gui->Divider();
  m_Gui->Integer(ID_INTERVAL,_("Interval:"),&m_Interval,0,(m_TimeVector.size()),_("Interval of frames to visualize"));
  m_Gui->Divider();
  m_Gui->Bool(ID_ALL_BUNCH,_("Complete"),&m_AllBunch,0,_("To visualize the whole bunch"));

  m_Gui->Enable(ID_ALL_BUNCH, m_UseBunch == 1);
  m_Gui->Enable(ID_INTERVAL, m_UseBunch == 1 && m_AllBunch == 0);
  m_Gui->Enable(ID_STEP, m_UseBunch == 1);
  m_Gui->Divider();
	return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeVector::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId()) 
    {
      case ID_USE_VTK_PROPERTY:
        if (m_UseVTKProperty != 0)
        {
          m_Actor->SetProperty(m_Material->m_Prop);
        }
        else
        {
          m_Actor->SetProperty(NULL);
        }
        m_MaterialButton->Enable(m_UseVTKProperty != 0);
				GetLogicManager()->CameraUpdate();
      break;
      case ID_USE_ARROW:
        if (m_UseArrow == false)
        {
          m_Apd->RemoveInputData(m_ArrowTip->GetOutput());
          m_Apd->Update();
        }
        else
        {
          UpdateProperty();
          m_Apd->AddInputData(m_ArrowTip->GetOutput());
          m_Apd->Update();
        }
				GetLogicManager()->CameraUpdate();
      break;
      case ID_USE_SPHERE:
        if (m_UseSphere == false)
        {
          m_Apd->RemoveInputData(m_Sphere->GetOutput());
          m_Apd->Update();
        }
        else
        {
          UpdateProperty();
					m_Apd->AddInputData(m_Sphere->GetOutput());
          m_Apd->Update();
        }
				GetLogicManager()->CameraUpdate();
      break;
      case ID_USE_BUNCH:
        if (m_UseBunch == true)
        {
          m_Gui->Update();
          AllVector();
        }
        else
        {
          m_Interval = 0;
          m_AllBunch = 0;
          m_Gui->Update();
          m_Bunch->RemoveAllInputs();
        }
        EnableWidget();
				GetLogicManager()->CameraUpdate();
      break;
      case ID_INTERVAL:
        m_Bunch->RemoveAllInputs();
        AllVector();
				GetLogicManager()->CameraUpdate();
      break;
      case ID_STEP:
        m_Bunch->RemoveAllInputs();
        AllVector();
				GetLogicManager()->CameraUpdate();
      break;
      case ID_ALL_BUNCH:
        if (m_AllBunch == true)
        {
          m_Bunch->RemoveAllInputs();
          m_Interval = m_TimeVector.size();
          m_Gui->Update();
          AllVector();
        }
        else
        {
          m_Interval = 0;
          m_Gui->Update();
          m_Bunch->RemoveAllInputs();
        }
        EnableWidget();
				GetLogicManager()->CameraUpdate();
      break;
      default:
        albaEventMacro(*e);
      break;
    }
  }
  
  if (alba_event->GetId() == VME_TIME_SET)
  {
    UpdateProperty();
  }
}
//----------------------------------------------------------------------------
void albaPipeVector::EnableWidget()
//----------------------------------------------------------------------------
{
  m_Gui->Enable(ID_INTERVAL, m_UseBunch == 1 && m_AllBunch == 0);
  m_Gui->Enable(ID_STEP, m_UseBunch == 1);
  m_Gui->Enable(ID_ALL_BUNCH, m_UseBunch == 1);
}

