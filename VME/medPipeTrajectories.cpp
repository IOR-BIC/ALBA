/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medPipeTrajectories.cpp,v $
  Language:  C++
  Date:      $Date: 2008-01-21 15:36:47 $
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

#include "medPipeTrajectories.h"
#include "mafDecl.h"
#include "mafSceneNode.h"
#include "mmgGui.h"
#include "mmaMaterial.h"
#include "mafMatrixVector.h"

#include "mafMatrix.h"
#include "mafTransform.h"
#include "mafVME.h"
#include "mafVMEOutputPolyline.h"
#include "mafVMELandmarkCloud.h"
#include "mafVMELandmark.h"
#include "mafEventSource.h"
#include "mmuTimeSet.h"

#include "vtkAppendPolyData.h"
#include "vtkCellArray.h"
#include "vtkMAFAssembly.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkMAFSmartPointer.h"

//----------------------------------------------------------------------------
mafCxxTypeMacro(medPipeTrajectories);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
medPipeTrajectories::medPipeTrajectories()
:mafPipe()
//----------------------------------------------------------------------------
{
  m_Traj            = NULL;
  m_Sphere          = NULL;
  m_Mapper          = NULL;
  m_Actor           = NULL;
  m_OutlineBox      = NULL;
  m_OutlineMapper   = NULL;
  m_OutlineProperty = NULL;
  m_OutlineActor    = NULL;
  m_Interval = 0;
}
//----------------------------------------------------------------------------
void medPipeTrajectories::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;
  m_Landmark  = mafVMELandmark::SafeDownCast(m_Vme);
  m_Landmark->GetLocalTimeStamps(m_TimeVector);
  m_MatrixVector = m_Landmark->GetMatrixVector();

  m_Vme->GetEventSource()->AddObserver(this);

  double radius;
  if(mafVMELandmarkCloud *cloud = mafVMELandmarkCloud::SafeDownCast(m_Vme->GetParent()))
  {
    radius = cloud->GetRadius();
  }
  else
  {
    radius = 10;
  }
  

  //Create a sphere in the center of the trajectory
  vtkNEW(m_Sphere);
  m_Sphere->SetRadius(radius);
  m_Sphere->SetPhiResolution(20);
  m_Sphere->SetThetaResolution(20);

  vtkNEW(m_Traj);
  
  UpdateProperty();

  m_Mapper = vtkPolyDataMapper::New();
  m_Mapper->SetInput(m_Traj->GetOutput());
  
  if(m_Vme->IsAnimated())				
    m_Mapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
  else
    m_Mapper->ImmediateModeRenderingOff();

  m_Actor = vtkActor::New();
  m_Actor->SetMapper(m_Mapper);
  mmaMaterial *material = m_Landmark->GetMaterial();
  if (material)
    m_Actor->SetProperty(material->m_Prop);
 
  m_RenFront->AddActor(m_Actor);
//  m_AssemblyFront->AddPart(m_Actor);

  // selection highlight
  m_OutlineBox = vtkOutlineCornerFilter::New();
  m_OutlineBox->SetInput(m_Traj->GetOutput());  

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
  
  m_RenFront->AddActor(m_OutlineActor);
//  m_AssemblyFront->AddPart(m_OutlineActor);
}
//----------------------------------------------------------------------------
medPipeTrajectories::~medPipeTrajectories()
//----------------------------------------------------------------------------
{
  m_Landmark->GetEventSource()->RemoveObserver(this);
  m_RenFront->RemoveActor(m_Actor);
  m_RenFront->RemoveActor(m_OutlineActor);

  vtkDEL(m_Traj);
  vtkDEL(m_Sphere);
  vtkDEL(m_Mapper);
  vtkDEL(m_Actor);
  vtkDEL(m_OutlineBox);
  vtkDEL(m_OutlineMapper);
  vtkDEL(m_OutlineProperty);
  vtkDEL(m_OutlineActor);
}
//----------------------------------------------------------------------------
void medPipeTrajectories::Select(bool sel)
//----------------------------------------------------------------------------
{
  m_Selected = sel;
  if(m_Actor->GetVisibility()) 
  {
    m_OutlineActor->SetVisibility(sel);
  }
}
//----------------------------------------------------------------------------
mmgGui *medPipeTrajectories::CreateGui()
//----------------------------------------------------------------------------
{
  
  m_Gui = new mmgGui(this);
  m_Gui->Integer(ID_INTERVAL,"Interval:",&m_Interval,0,(m_TimeVector.size()),"Interval of frames to visualize");
 
  m_Gui->Divider();

  return m_Gui;
}
//----------------------------------------------------------------------------
void medPipeTrajectories::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_INTERVAL:
        UpdateProperty();
        break;
      default:
        mafEventMacro(*e);
    }
  }

  if (maf_event->GetId() == VME_OUTPUT_DATA_UPDATE)
  {
    UpdateProperty();
  }
}
//----------------------------------------------------------------------------
void medPipeTrajectories::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  double xyz[3];
  mafTimeStamp t0;
  t0 = m_Landmark->GetTimeStamp();

  m_Traj->RemoveAllInputs();

  vtkMAFSmartPointer<vtkPolyData> line;
  vtkMAFSmartPointer<vtkPoints> points;
  vtkMAFSmartPointer<vtkCellArray> cellArray;

  int pointId[2], counter = 0;
  bool current, previous, sphere_visibility;

  if (m_MatrixVector)
  {
    for (int i = 0; i< m_TimeVector.size(); i++)
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
    //Landmark center position
    mafMatrix *m = m_MatrixVector->GetKeyMatrix(i);
 
    mafTransform::GetPosition(*m,xyz);
    m_Sphere->SetCenter(xyz[0], xyz[1], xyz[2]);
    sphere_visibility = m_Landmark->GetLandmarkVisibility(m_TimeVector[i]);

    //start construct the landmark trajectory
    m = m_MatrixVector->GetKeyMatrix(minValue);
    mafTransform::GetPosition(*m,xyz);
    points->InsertNextPoint(xyz[0], xyz[1], xyz[2]);
   
    for (mafTimeStamp n = (minValue + 1); n <= maxValue; n++)
    {
      m = m_MatrixVector->GetKeyMatrix(n);
      mafTransform::GetPosition(*m,xyz);
      points->InsertNextPoint(xyz[0], xyz[1], xyz[2]);

      previous = m_Landmark->GetLandmarkVisibility(m_TimeVector[n-1]);
      current = m_Landmark->GetLandmarkVisibility(m_TimeVector[n]);

      if (previous && current)
      {
        pointId[0] = counter;
        pointId[1] = counter + 1;
        cellArray->InsertNextCell(2, pointId);  
      }
      counter++;
    }
  } 
  line->SetPoints(points);
  line->SetLines(cellArray);
  line->Modified();

  m_Traj->AddInput(line);
  if (sphere_visibility)
  {
    m_Traj->AddInput(m_Sphere->GetOutput());
  }
  
  m_Traj->Update();
}
