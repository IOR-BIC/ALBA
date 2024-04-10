/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaPipeTrajectories
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

#include "albaPipeTrajectories.h"
#include "albaDecl.h"
#include "albaSceneNode.h"
#include "albaGUI.h"
#include "mmaMaterial.h"
#include "albaMatrixVector.h"

#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVME.h"
#include "albaVMEOutputPolyline.h"
#include "albaVMELandmarkCloud.h"
#include "albaVMELandmark.h"
#include "mmuTimeSet.h"

#include "vtkAppendPolyData.h"
#include "vtkCellArray.h"
#include "vtkALBAAssembly.h"
#include "vtkRenderer.h"
#include "vtkSphereSource.h"
#include "vtkOutlineCornerFilter.h"
#include "vtkPolyDataMapper.h"
#include "vtkPolyData.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkALBASmartPointer.h"

//----------------------------------------------------------------------------
albaCxxTypeMacro(albaPipeTrajectories);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
albaPipeTrajectories::albaPipeTrajectories()
:albaPipe()
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
void albaPipeTrajectories::Create(albaSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;
  m_Landmark  = albaVMELandmark::SafeDownCast(m_Vme);
  m_Landmark->GetLocalTimeStamps(m_TimeVector);
  //albaVMEGenericAbstract *vmeGeneric = albaVMEGenericAbstract::SafeDownCast(m_Landmark->GetParent());
 // m_MatrixVector = vmeGeneric->GetMatrixVector();
   m_MatrixVector = m_Landmark->GetMatrixVector();

  m_Vme->AddObserver(this);

  double radius;
  if(albaVMELandmarkCloud *cloud = albaVMELandmarkCloud::SafeDownCast(m_Vme->GetParent()))
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
  m_Mapper->SetInputConnection(m_Traj->GetOutputPort());
  
  if(m_Vme->IsAnimated())				
    m_Mapper->ImmediateModeRenderingOn();	 //avoid Display-Lists for animated items.
  else
    m_Mapper->ImmediateModeRenderingOff();

  m_Actor = vtkActor::New();
  m_Actor->SetMapper(m_Mapper);
  mmaMaterial *material = m_Landmark->GetMaterial();
  if (material)
    m_Actor->SetProperty(material->m_Prop);
 
	m_AssemblyFront->AddPart(m_Actor);

  // selection highlight
  m_OutlineBox = vtkOutlineCornerFilter::New();
  m_OutlineBox->SetInputConnection(m_Traj->GetOutputPort());  

  m_OutlineMapper = vtkPolyDataMapper::New();
  m_OutlineMapper->SetInputConnection(m_OutlineBox->GetOutputPort());

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
albaPipeTrajectories::~albaPipeTrajectories()
//----------------------------------------------------------------------------
{
  m_Landmark->RemoveObserver(this);
  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->RemovePart(m_OutlineActor);

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
void albaPipeTrajectories::Select(bool sel)
//----------------------------------------------------------------------------
{
  m_Selected = sel;
  if(m_Actor->GetVisibility()) 
  {
    m_OutlineActor->SetVisibility(sel);
  }
}
//----------------------------------------------------------------------------
albaGUI *albaPipeTrajectories::CreateGui()
//----------------------------------------------------------------------------
{
  
  m_Gui = new albaGUI(this);
  m_Gui->Integer(ID_INTERVAL,"Interval:",&m_Interval,0,(m_TimeVector.size()),"Interval of frames to visualize");
 
  m_Gui->Divider();

  return m_Gui;
}
//----------------------------------------------------------------------------
void albaPipeTrajectories::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
      case ID_INTERVAL:
        UpdateProperty();
				GetLogicManager()->CameraUpdate();
        break;
      default:
        albaEventMacro(*e);
    }
  }

  if (alba_event->GetId() == VME_OUTPUT_DATA_UPDATE)
  {
    UpdateProperty();
  }
}

//----------------------------------------------------------------------------
void albaPipeTrajectories::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  double xyz[3];
  double xyzTransform[3];
  albaTimeStamp t0;
  t0 = m_Landmark->GetTimeStamp();

  m_Traj->RemoveAllInputs();

  vtkALBASmartPointer<vtkPolyData> line;
  vtkALBASmartPointer<vtkPoints> points;
  vtkALBASmartPointer<vtkCellArray> cellArray;

	vtkIdType pointId[2], counter = 0;
  bool current, previous, sphere_visibility;

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
    
    //Get landmark position form the current transformation matrix
    albaMatrix *m = m_MatrixVector->GetKeyMatrix(i);
    albaTransform::GetPosition(*m,xyzTransform);

    //Landmark center position. Set to zero, because position is applied by the current transformation matrix
    m_Sphere->SetCenter(0, 0, 0);
    sphere_visibility = m_Landmark->GetLandmarkVisibility(m_TimeVector[i]);

    //start construct the landmark trajectory
    m = m_MatrixVector->GetKeyMatrix(minValue);
    albaTransform::GetPosition(*m,xyz);

    //Subtract the position of the current transformation matrix, from the position of the "minValue" transformation.
    //It is necessary because current transformation matrix is applied in visualization.
    points->InsertNextPoint(xyz[0] - xyzTransform[0], xyz[1] - xyzTransform[1], xyz[2] - xyzTransform[2]);
   
    for (albaTimeStamp n = (minValue + 1); n <= maxValue; n++)
    {
      m = m_MatrixVector->GetKeyMatrix(n);
      albaTransform::GetPosition(*m,xyz);

      //Subtract the position of the current transformation matrix, from the position of the "n" transformation: 
      points->InsertNextPoint(xyz[0] - xyzTransform[0], xyz[1] - xyzTransform[1], xyz[2] - xyzTransform[2]); //transform

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

  m_Traj->AddInputData(line);
  if (sphere_visibility)
  {
    m_Traj->AddInputConnection(m_Sphere->GetOutputPort());
  }
  
  m_Traj->Update();
}
