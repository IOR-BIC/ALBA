/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoPolylineGraph
 Authors: Josef Kohout, Stefano Perticoni
 
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

#include "albaGizmoPolylineGraph.h"
#include "albaDecl.h"
#include "albaEvent.h"
#include "mmaMaterial.h"
#include "albaGUIMaterialButton.h"
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"
#include "albaSmartPointer.h"
#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaVMEPolyline.h"
#include "albaVMEPolylineSpline.h"
#include "albaMatrix.h"
#include "albaAbsMatrixPipe.h" 
#include "albaVMERoot.h"
#include "albaRefSys.h"

#include "vtkMath.h"
#include "vtkArrowSource.h"
#include "vtkPlaneSource.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"

// new stuff
#include "albaVMEPolylineGraph.h"
#include "albaPolylineGraph.h"
#include "vtkIdList.h"
#include "albaCurvilinearAbscissaOnSkeletonHelper.h"

#include "albaMemDbg.h"

albaGizmoPolylineGraph::albaGizmoPolylineGraph(albaVME* imputVme, albaObserver *listener, const char* name, bool showOnlyDirectionAxis, bool testMode) 
{ 
  m_Name = name;
  m_InputVME = imputVme;
  m_Listener = listener;
  m_TestMode = testMode;
  
  m_VmeGizmo = NULL;
  m_CurvilinearAbscissaHelper = NULL;
  m_GizmoInteractor = NULL;
  m_LeftMouseInteractor = NULL;
  m_SphereSource = NULL;
  m_AppendPolyData = NULL;
  for (int i = 0; i < 3; i++)
  {
    m_Axis[i] = NULL;
    m_AxisSource[i] = NULL;
  }
  m_ShowOnlyDirectionAxis = showOnlyDirectionAxis;
}

//------------------------------------------------------------------------
//Initializes the object. May not be called twice (unless Done method was called).
/*virtual*/ void albaGizmoPolylineGraph::InternalInitInstance()
//------------------------------------------------------------------------
{
  CreateVMEGizmo();
  CreateInteractor();

  m_CurvilinearAbscissaHelper = new albaCurvilinearAbscissaOnSkeletonHelper( m_VmeGizmo, NULL, m_TestMode);
}

//------------------------------------------------------------------------
//Finalizes the object. May not be called twice (unless Init method was called).
/*virtual*/ void albaGizmoPolylineGraph::InternalRelease()
//------------------------------------------------------------------------
{
  DestroyInteractor();
  DestroyVMEGizmo();

  cppDEL(m_CurvilinearAbscissaHelper);
}

//------------------------------------------------------------------------
//Creates the gizmo. Called from ctor.
/*virtual*/ void albaGizmoPolylineGraph::CreateVMEGizmo()
//------------------------------------------------------------------------
{
  albaVMERoot *root = albaVMERoot::SafeDownCast(m_InputVME->GetRoot());
  assert(root);

  assert(m_VmeGizmo == NULL);
  CreateGizmoVTKData();

  //determine the default size of the gizmo
  double bounds[6];
  m_InputVME->GetOutput()->GetVMEBounds(bounds);

  double max = fabs(bounds[1] - bounds[0]);
  for (int i = 2; i < 6; i += 2)
  {
    double tmp = fabs(bounds[i + 1] - bounds[i]);
    if (tmp > max)
      max = tmp;
  }

  SetGizmoLength(0.2*max);

  albaNEW(m_VmeGizmo);
  m_VmeGizmo->SetName(m_Name);
  m_VmeGizmo->ReparentTo(root); 
  m_VmeGizmo->SetDataConnection(m_AppendPolyData->GetOutputPort());  
  assert(m_VmeGizmo->GetData()->GetNumberOfPoints());  
}

//------------------------------------------------------------------------
//Destroys the gizmo. Called from dtor.
void albaGizmoPolylineGraph::DestroyVMEGizmo()
//------------------------------------------------------------------------
{  
  m_VmeGizmo->ReparentTo(NULL);
  albaDEL(m_VmeGizmo);

  DestroyGizmoVTKData();  
}


//------------------------------------------------------------------------
//Creates VTK pipeline for the gizmo. Called from CreateVMEGizmo.
/*virtual*/ void albaGizmoPolylineGraph::CreateGizmoVTKData()
//------------------------------------------------------------------------
{ 
  m_AppendPolyData = vtkAppendPolyData::New();  
  m_SphereSource = vtkSphereSource::New();
  m_AppendPolyData->SetInputConnection(m_SphereSource->GetOutputPort());

  if(m_ShowOnlyDirectionAxis == false)
  {
  for (int i = 0; i < 3; i++)
  {
    m_AxisSource[i] = vtkArrowSource::New();    

    //-----------------
    // rotate the axis on the i axis (default axis is X)
    //-----------------  
    vtkTransform* trans = vtkTransform::New();  
    //medCurvilinear places z in the direction to the viewer => rotate it
    //rotate the whole system
    trans->RotateY(180);

    if (i == 1)
      trans->RotateZ(90);	
    else if (i == 2)
      trans->RotateY(-90);

    vtkTransformPolyDataFilter* PDF = vtkTransformPolyDataFilter::New();
    PDF->SetInputConnection(m_AxisSource[i]->GetOutputPort());
    PDF->SetTransform(trans);
    trans->Delete();

    //Scaling (to be set in SetLength)
    trans = vtkTransform::New();
    m_Axis[i] = vtkTransformPolyDataFilter::New();
    m_Axis[i]->SetInputConnection(PDF->GetOutputPort());
    m_Axis[i]->SetTransform(trans);
    trans->Delete();
    PDF->Delete();

    m_AppendPolyData->AddInputConnection(m_Axis[i]->GetOutputPort());

  
    }

  m_PlaneSource = vtkPlaneSource::New();
  m_AppendPolyData->AddInputConnection(m_PlaneSource->GetOutputPort());  
  }
  else
  {
    m_AxisSource[2] = vtkArrowSource::New();    

    //-----------------
    // rotate the axis on the i axis (default axis is X)
    //-----------------  
    vtkTransform* trans = vtkTransform::New();  
    //medCurvilinear places z in the direction to the viewer => rotate it
    //rotate the whole system
    trans->RotateY(180);
    trans->RotateY(-90);

    vtkTransformPolyDataFilter* PDF = vtkTransformPolyDataFilter::New();
    PDF->SetInputConnection(m_AxisSource[2]->GetOutputPort());
    PDF->SetTransform(trans);
    trans->Delete();

    //Scaling (to be set in SetLength)
    trans = vtkTransform::New();
    m_Axis[2] = vtkTransformPolyDataFilter::New();
    m_Axis[2]->SetInputConnection(PDF->GetOutputPort());
    m_Axis[2]->SetTransform(trans);
    trans->Delete();
    PDF->Delete();

    m_AppendPolyData->AddInputConnection(m_Axis[2]->GetOutputPort());
  }
   
  m_AppendPolyData->Update();
}

//----------------------------------------------------------------------------
//Destroys VTK pipeline for the gizmo. Called from DestroyVMEGizmo.
/*virtual*/ void albaGizmoPolylineGraph::DestroyGizmoVTKData()
//----------------------------------------------------------------------------
{
	m_SphereSource->Delete();
  if(m_ShowOnlyDirectionAxis == false)
	{
		for (int i = 0; i < 3; i++)
		{
			m_AxisSource[i]->Delete();
			m_Axis[i]->Delete();
		}
		m_PlaneSource->Delete();
	}
	else
	{
		m_AxisSource[2]->Delete();
		m_Axis[2]->Delete();
	}

	m_AppendPolyData->Delete();
}
//------------------------------------------------------------------------
//Creates the interactor for the gizmo. Called from ctor. 
/*virtual*/ void albaGizmoPolylineGraph::CreateInteractor()
//------------------------------------------------------------------------
{  
  m_RefSysVME = m_InputVME;

  albaNEW(m_GizmoInteractor);
  m_LeftMouseInteractor = m_GizmoInteractor->CreateBehavior(MOUSE_LEFT);
  m_LeftMouseInteractor->SetListener(this);
  m_LeftMouseInteractor->SetVME(m_InputVME);
  m_LeftMouseInteractor->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  m_LeftMouseInteractor->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  m_LeftMouseInteractor->EnableTranslation(true);    

  m_VmeGizmo->SetBehavior(m_GizmoInteractor);
}

//------------------------------------------------------------------------
//Destroys the interactor of the gizmo. Called from dtor.
/*virtual*/ void albaGizmoPolylineGraph::DestroyInteractor()
//------------------------------------------------------------------------
{
  m_VmeGizmo->SetBehavior(NULL);
  albaDEL(m_GizmoInteractor);
}

//------------------------------------------------------------------------
//Set the length of the gizmo
/*virtual*/ void albaGizmoPolylineGraph::SetGizmoLength( double lineLength )
//------------------------------------------------------------------------
{
  m_SphereSource->SetRadius(lineLength / 8);

  if(m_ShowOnlyDirectionAxis == false)
  {
  for (int i = 0; i < 3; i++)
  {
    vtkTransform* trans = vtkTransform::New();
    //if (i != 2)
    //  trans->Scale(lineLength, lineLength, lineLength);
    //else
    //  trans->Scale(1.5*lineLength, 1.5*lineLength, 2*lineLength);
    double dblScale = i != 2 ? lineLength : 1.5*lineLength;
    trans->Scale(dblScale, dblScale, dblScale);

    m_Axis[i]->SetTransform(trans);
    trans->Delete();
  }

  m_PlaneSource->SetPoint1(-0.5*lineLength, 0, 0);
  m_PlaneSource->SetPoint2(0, 0.5*lineLength, 0);
  }
  else
  {
    
    vtkTransform* trans = vtkTransform::New();
    //if (i != 2)
    //  trans->Scale(lineLength, lineLength, lineLength);
    //else
    //  trans->Scale(1.5*lineLength, 1.5*lineLength, 2*lineLength);
    double dblScale = 1.5*lineLength;
    trans->Scale(dblScale, dblScale, dblScale);

    m_Axis[2]->SetTransform(trans);
    trans->Delete();
  }
  
}

//------------------------------------------------------------------------
/*virtual*/ void albaGizmoPolylineGraph::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_VmeGizmo->GetMaterial()->m_Diffuse[0] = col[0];
  m_VmeGizmo->GetMaterial()->m_Diffuse[1] = col[1];
  m_VmeGizmo->GetMaterial()->m_Diffuse[2] = col[2];
  m_VmeGizmo->GetMaterial()->UpdateProp();
}

//----------------------------------------------------------------------------
/*virtual*/ void albaGizmoPolylineGraph::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case ID_TRANSFORM:      
      OnTransform(e);
      break;

    default:
      albaEventMacro(*e);
      break; 
    }
  }
} 

//------------------------------------------------------------------------
//Handles ID_TRANSFORM event
/*virtual*/ void albaGizmoPolylineGraph::OnTransform(albaEvent *e)
//------------------------------------------------------------------------
{
  m_CurvilinearAbscissaHelper->MoveOnSkeleton(e);
	GetLogicManager()->CameraUpdate();

  // forward isa transform events to the listener op
  // instanciating the gizmo; the sender is changed to "this" so that the operation can check for
  // gizmo sending events
  e->SetSender(this);
  albaEventMacro(*e);
}

//----------------------------------------------------------------------------
/*virtual*/ void albaGizmoPolylineGraph::Show(bool show)
//----------------------------------------------------------------------------
{
  assert(m_VmeGizmo);
  GetLogicManager()->VmeShow(m_VmeGizmo, show);
}

//------------------------------------------------------------------------
void albaGizmoPolylineGraph::LogTransformEvent( albaEvent *e )
//------------------------------------------------------------------------
{
  std::ostringstream stringStream;

  long id = (long)(e->GetId());
  assert(id == ID_TRANSFORM);

  long mouseAction = e->GetArg();

  albaString mouseActionString;
  switch(mouseAction)
  {
  case albaInteractorGenericMouse::MOUSE_DOWN:
    mouseActionString.Append("MOUSE DOWN");
    break;
  case albaInteractorGenericMouse::MOUSE_MOVE:
    mouseActionString.Append("MOUSE MOVE");
    break;
  case albaInteractorGenericMouse::MOUSE_UP:
    mouseActionString.Append("MOUSE UP");
    break;
  default:
    mouseActionString.Append("Unknown mouse action: mouseActionID = ");
    mouseActionString << (long)mouseAction;
    break;
  }


  stringStream << mouseActionString.GetCStr() << std::endl;
  e->GetMatrix()->Print(stringStream);

  albaLogMessage(stringStream.str().c_str());
}

