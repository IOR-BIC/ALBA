/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafPipeVector.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-21 11:08:31 $
  Version:   $Revision: 1.6 $
  Authors:   Roberto Mucci
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

#include "wx/busyinfo.h"
#include "mafPipeVector.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mmgMaterialButton.h"
#include "mmaMaterial.h"
#include "mafVMEVector.h"

#include "mafDataVector.h"
#include "mafEventSource.h"
#include "mafVMEGenericAbstract.h"

#include "vtkMAFSmartPointer.h"
#include "vtkMAFAssembly.h"

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
mafCxxTypeMacro(mafPipeVector);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeVector::mafPipeVector()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Data            = NULL;
  m_Sphere          = NULL;
  m_ArrowTip        = NULL;
  m_Apd             = NULL;
  m_Data            = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
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
void mafPipeVector::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{  
   
  Superclass::Create(n);
  m_Selected = false;
  mafVMEOutputPolyline *out_polyline = mafVMEOutputPolyline::SafeDownCast(m_Vme->GetOutput());
  assert(out_polyline);
  m_Data = vtkPolyData::SafeDownCast(out_polyline->GetVTKData());
  assert(m_Data);
  m_Data->Update(); 

  m_Vector  = mafVMEVector::SafeDownCast(m_Vme);
  m_Vector->GetTimeStamps(m_TimeVector);
 
  m_Vme->GetEventSource()->AddObserver(this);

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
  m_Apd->AddInput(m_Data);
  m_Apd->AddInput(m_Sphere->GetOutput());
  m_Apd->AddInput(m_ArrowTip->GetOutput());
  m_Apd->Update();
  m_Mapper->SetInput(m_Apd->GetOutput());
 

  int renderingDisplayListFlag = m_Vme->IsAnimated() ? 1 : 0;
  m_Mapper->SetImmediateModeRendering(renderingDisplayListFlag);

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
 

  m_OutlineBox = vtkOutlineCornerFilter::New();
  m_OutlineBox->SetInput(m_Data);  

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
mafPipeVector::~mafPipeVector()
//----------------------------------------------------------------------------
{
  m_Vme->GetEventSource()->RemoveObserver(this);
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
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
}
//----------------------------------------------------------------------------
void mafPipeVector::Select(bool sel)
//----------------------------------------------------------------------------
{
  m_Selected = sel;
  if(m_Actor->GetVisibility()) 
  {
    m_OutlineActor->SetVisibility(sel);
  }
}
//----------------------------------------------------------------------------
void mafPipeVector::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{ 
  m_Data->Update();

  double pointCop[3];
  m_Data->GetPoint(0,pointCop);

  if (m_UseSphere == TRUE)
  {
    m_Sphere->SetCenter(pointCop);
    m_Sphere->Update();
  }
  

  if (m_UseArrow == TRUE)
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
    m_ArrowTip->Update(); 
  }
}
//----------------------------------------------------------------------------
void mafPipeVector::AllVector(bool fromTag)
//----------------------------------------------------------------------------
{
//  if(!m_TestMode)
    wxBusyInfo wait(_("Creating Vectogram, please wait..."));

 
  m_MatrixVector = m_Vector->GetMatrixVector();

  mafTimeStamp t0;
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


    for (mafTimeStamp n = (minValue + 1); n <= maxValue; n= n + m_Step) //Cicle to draw vectors
    {
      vtkMAFSmartPointer<vtkLineSource> line;
     
      double point1[3];
      double point2[3];

      m_Vector->SetTimeStamp(m_TimeVector[n]);
      m_Vector->Update();

            
      m_Vector->GetOutput()->GetVTKData()->GetPoint(0, point1);
      m_Vector->GetOutput()->GetVTKData()->GetPoint(1, point2);

      line->SetPoint1(point1);
      line->SetPoint2(point2);
      line->Modified();
 
      m_Bunch->AddInput(line->GetOutput());
      m_Bunch->Update();
    } 

    if (m_Bunch->GetNumberOfInputs() == 0)
    {
      m_MapperBunch->SetInput(m_Bunch->GetOutput());
    }
    

    
    
    //m_Apd->AddInput(m_Bunch->GetOutput());
    //m_Apd->Update();
  }
}
//----------------------------------------------------------------------------
mmgGui *mafPipeVector::CreateGui()
//----------------------------------------------------------------------------
{
  assert(m_Gui == NULL);
  m_Gui = new mmgGui(this);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_VTK_PROPERTY,"property",&m_UseVTKProperty);
  m_MaterialButton = new mmgMaterialButton(m_Vme,this);
  m_Gui->AddGui(m_MaterialButton->GetGui());
  m_MaterialButton->Enable(m_UseVTKProperty != 0);
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_ARROW,"Arrow",&m_UseArrow,1,"To visualize the arrow tip");
  m_Gui->Bool(ID_USE_SPHERE,"COP",&m_UseSphere,1,"To visualize sphere on COP");
  m_Gui->Divider();
  m_Gui->Bool(ID_USE_BUNCH,"Vectogram",&m_UseBunch,0,"To visualize the vectogram");
  m_Gui->Divider();
  m_Gui->Integer(ID_STEP,"Step:",&m_Step,0,(m_TimeVector.size()),"1 To visualize every vector");
  m_Gui->Divider();
  m_Gui->Integer(ID_INTERVAL,"Interval:",&m_Interval,0,(m_TimeVector.size()),"Interval of frames to visualize");
  m_Gui->Divider();
  m_Gui->Bool(ID_ALL_BUNCH,"Complete",&m_AllBunch,0,"To visualize the whole bunch");

  m_Gui->Enable(ID_ALL_BUNCH, m_UseBunch == 1);
  m_Gui->Enable(ID_INTERVAL, m_UseBunch == 1 && m_AllBunch == 0);
  m_Gui->Enable(ID_STEP, m_UseBunch == 1);
  m_Gui->Divider();
	return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeVector::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
      mafEventMacro(mafEvent(this,CAMERA_UPDATE));
      break;
    
    case ID_USE_ARROW:
      if (m_UseArrow == FALSE)
      {
        m_Apd->RemoveInput(m_ArrowTip->GetOutput());
        m_Apd->Update();
      }
      else
      {
        UpdateProperty();
        m_Apd->AddInput(m_ArrowTip->GetOutput());
        m_Apd->Update();
      }
      break;

    case ID_USE_SPHERE:
      if (m_UseSphere == FALSE)
      {
        m_Apd->RemoveInput(m_Sphere->GetOutput());
        m_Apd->Update();
      }
      else
      {
        UpdateProperty();
        m_Apd->AddInput(m_Sphere->GetOutput());
        m_Apd->Update();
      }
      break;
    
    case ID_USE_BUNCH:
      if (m_UseBunch == TRUE)
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
      break;

    case ID_INTERVAL:
      m_Bunch->RemoveAllInputs();
      AllVector();
      break;

    case ID_STEP:
      m_Bunch->RemoveAllInputs();
      AllVector();
      break;

    case ID_ALL_BUNCH:
      if (m_AllBunch == TRUE)
      {
        m_Bunch->RemoveAllInputs();
        m_Interval = m_TimeVector.size();
        m_Gui->Update();
        AllVector();
      }
      else
      {
        m_Interval = 0;
        m_Bunch->RemoveAllInputs();
      }
      break;

      default:
        mafEventMacro(*e);
      break;
    }
  }

  
  if (maf_event->GetId() == VME_TIME_SET)
  {
    UpdateProperty();
  }

  mafEventMacro(mafEvent(this,CAMERA_UPDATE));

  if (m_Gui)
  {
    m_Gui->Enable(ID_INTERVAL, m_UseBunch == 1 && m_AllBunch == 0);
    m_Gui->Enable(ID_STEP, m_UseBunch == 1);
    m_Gui->Enable(ID_ALL_BUNCH, m_UseBunch == 1);

  }
}

