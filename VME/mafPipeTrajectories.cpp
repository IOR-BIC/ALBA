/*=========================================================================

 Program: MAF2
 Module: mafPipeTrajectories
 Authors: Roberto Mucci
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafPipeTrajectories.h"
#include "mafDecl.h"
#include "mafSceneNode.h"
#include "mafGUI.h"
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
mafCxxTypeMacro(mafPipeTrajectories);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
mafPipeTrajectories::mafPipeTrajectories()
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
void mafPipeTrajectories::Create(mafSceneNode *n)
//----------------------------------------------------------------------------
{
  Superclass::Create(n);

  m_Selected = false;
  m_Landmark  = mafVMELandmark::SafeDownCast(m_Vme);
  m_Landmark->GetLocalTimeStamps(m_TimeVector);
  //mafVMEGenericAbstract *vmeGeneric = mafVMEGenericAbstract::SafeDownCast(m_Landmark->GetParent());
 // m_MatrixVector = vmeGeneric->GetMatrixVector();
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
 
 m_AssemblyFront->AddPart(m_Actor);

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
  
  m_AssemblyFront->AddPart(m_OutlineActor);
}
//----------------------------------------------------------------------------
mafPipeTrajectories::~mafPipeTrajectories()
//----------------------------------------------------------------------------
{
  m_Landmark->GetEventSource()->RemoveObserver(this);
  m_AssemblyFront->RemovePart(m_Actor);
  m_AssemblyFront->AddPart(m_OutlineActor);

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
void mafPipeTrajectories::Select(bool sel)
//----------------------------------------------------------------------------
{
  m_Selected = sel;
  if(m_Actor->GetVisibility()) 
  {
    m_OutlineActor->SetVisibility(sel);
  }
}
//----------------------------------------------------------------------------
mafGUI *mafPipeTrajectories::CreateGui()
//----------------------------------------------------------------------------
{
  
  m_Gui = new mafGUI(this);
  m_Gui->Integer(ID_INTERVAL,"Interval:",&m_Interval,0,(m_TimeVector.size()),"Interval of frames to visualize");
 
  m_Gui->Divider();

  return m_Gui;
}
//----------------------------------------------------------------------------
void mafPipeTrajectories::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
      case ID_INTERVAL:
        UpdateProperty();
        mafEventMacro(mafEvent(this,CAMERA_UPDATE));
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
void mafPipeTrajectories::UpdateProperty(bool fromTag)
//----------------------------------------------------------------------------
{
  double xyz[3];
  double xyzTransform[3];
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
    mafMatrix *m = m_MatrixVector->GetKeyMatrix(i);
    mafTransform::GetPosition(*m,xyzTransform);

    //Landmark center position. Set to zero, because position is applied by the current transformation matrix
    m_Sphere->SetCenter(0, 0, 0);
    sphere_visibility = m_Landmark->GetLandmarkVisibility(m_TimeVector[i]);

    //start construct the landmark trajectory
    m = m_MatrixVector->GetKeyMatrix(minValue);
    mafTransform::GetPosition(*m,xyz);

    //Subtract the position of the current transformation matrix, from the position of the "minValue" transformation.
    //It is necessary because current transformation matrix is applied in visualization.
    points->InsertNextPoint(xyz[0] - xyzTransform[0], xyz[1] - xyzTransform[1], xyz[2] - xyzTransform[2]);
   
    for (mafTimeStamp n = (minValue + 1); n <= maxValue; n++)
    {
      m = m_MatrixVector->GetKeyMatrix(n);
      mafTransform::GetPosition(*m,xyz);

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

  m_Traj->AddInput(line);
  if (sphere_visibility)
  {
    m_Traj->AddInput(m_Sphere->GetOutput());
  }
  
  m_Traj->Update();
}
