/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: medGizmoPolylineGraph.cpp,v $
Language:  C++
Date:      $Date: 2009-05-12 08:48:38 $
Version:   $Revision: 1.1.2.1 $
Authors:   Josef Kohout, Stefano Perticoni
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

#include "medGizmoPolylineGraph.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmaMaterial.h"
#include "mafGUIMaterialButton.h"
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"
#include "mafSmartPointer.h"
#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "mafMatrix.h"
#include "mafAbsMatrixPipe.h" 
#include "mafVMERoot.h"

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
#include "medVMEPolylineGraph.h"
#include "mafPolylineGraph.h"
#include "vtkIdList.h"
#include "medCurvilinearAbscissaOnSkeletonHelper.h"

#include "mafMemDbg.h"

medGizmoPolylineGraph::medGizmoPolylineGraph(mafNode* imputVme, mafObserver *listener, const char* name) 
{ 
  m_Name = name;
  m_InputVME = mafVME::SafeDownCast(imputVme);
  m_Listener = listener;
  
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
}

//------------------------------------------------------------------------
//Initializes the object. May not be called twice (unless Done method was called).
/*virtual*/ void medGizmoPolylineGraph::InternalInitInstance()
//------------------------------------------------------------------------
{
  CreateVMEGizmo();
  CreateInteractor();

  m_CurvilinearAbscissaHelper = new medCurvilinearAbscissaOnSkeletonHelper( m_VmeGizmo );
}

//------------------------------------------------------------------------
//Finalizes the object. May not be called twice (unless Init method was called).
/*virtual*/ void medGizmoPolylineGraph::InternalRelease()
//------------------------------------------------------------------------
{
  DestroyInteractor();
  DestroyVMEGizmo();

  cppDEL(m_CurvilinearAbscissaHelper);
}

//------------------------------------------------------------------------
//Creates the gizmo. Called from ctor.
/*virtual*/ void medGizmoPolylineGraph::CreateVMEGizmo()
//------------------------------------------------------------------------
{
  mafVMERoot *root = mafVMERoot::SafeDownCast(m_InputVME->GetRoot());
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

  mafNEW(m_VmeGizmo);
  m_VmeGizmo->SetName(m_Name);
  m_VmeGizmo->ReparentTo(root); 
  m_VmeGizmo->SetData(m_AppendPolyData->GetOutput());  
  assert(m_VmeGizmo->GetData()->GetNumberOfPoints());  
}

//------------------------------------------------------------------------
//Destroys the gizmo. Called from dtor.
void medGizmoPolylineGraph::DestroyVMEGizmo()
//------------------------------------------------------------------------
{  
  m_VmeGizmo->ReparentTo(NULL);
  mafDEL(m_VmeGizmo);

  DestroyGizmoVTKData();  
}


//------------------------------------------------------------------------
//Creates VTK pipeline for the gizmo. Called from CreateVMEGizmo.
/*virtual*/ void medGizmoPolylineGraph::CreateGizmoVTKData()
//------------------------------------------------------------------------
{ 
  m_AppendPolyData = vtkAppendPolyData::New();  
  m_SphereSource = vtkSphereSource::New();
  m_AppendPolyData->SetInput(m_SphereSource->GetOutput());

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
    PDF->SetInput(m_AxisSource[i]->GetOutput());
    PDF->SetTransform(trans);
    trans->Delete();

    //Scaling (to be set in SetLength)
    trans = vtkTransform::New();
    m_Axis[i] = vtkTransformPolyDataFilter::New();
    m_Axis[i]->SetInput(PDF->GetOutput());
    m_Axis[i]->SetTransform(trans);
    trans->Delete();
    PDF->Delete();

    m_AppendPolyData->AddInput(m_Axis[i]->GetOutput());
  }

  m_PlaneSource = vtkPlaneSource::New();
  m_AppendPolyData->AddInput(m_PlaneSource->GetOutput());    
  m_AppendPolyData->Update();
}

//----------------------------------------------------------------------------
//Destroys VTK pipeline for the gizmo. Called from DestroyVMEGizmo.
/*virtual*/ void medGizmoPolylineGraph::DestroyGizmoVTKData()
//----------------------------------------------------------------------------
{  
  m_SphereSource->Delete();
  for (int i = 0; i < 3; i++)
  {
    m_AxisSource[i]->Delete();
    m_Axis[i]->Delete();
  }
  m_PlaneSource->Delete();
  m_AppendPolyData->Delete();
}

//------------------------------------------------------------------------
//Creates the interactor for the gizmo. Called from ctor. 
/*virtual*/ void medGizmoPolylineGraph::CreateInteractor()
//------------------------------------------------------------------------
{  
  m_RefSysVME = m_InputVME;

  mafMatrix *absMatrix;
  absMatrix = m_RefSysVME->GetOutput()->GetAbsMatrix();

  mafNEW(m_GizmoInteractor);
  m_LeftMouseInteractor = m_GizmoInteractor->CreateBehavior(MOUSE_LEFT);

  m_LeftMouseInteractor->SetListener(this);
  m_LeftMouseInteractor->SetVME(m_InputVME);

  m_LeftMouseInteractor->GetTranslationConstraint()->GetRefSys()->SetTypeToView();

  m_LeftMouseInteractor->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
  m_LeftMouseInteractor->EnableTranslation(true);    

  m_VmeGizmo->SetBehavior(m_GizmoInteractor);
}

//------------------------------------------------------------------------
//Destroys the interactor of the gizmo. Called from dtor.
/*virtual*/ void medGizmoPolylineGraph::DestroyInteractor()
//------------------------------------------------------------------------
{
  m_VmeGizmo->SetBehavior(NULL);
  mafDEL(m_GizmoInteractor);
}

//------------------------------------------------------------------------
//Set the length of the gizmo
/*virtual*/ void medGizmoPolylineGraph::SetGizmoLength( double lineLength )
//------------------------------------------------------------------------
{
  m_SphereSource->SetRadius(lineLength / 8);

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

//------------------------------------------------------------------------
/*virtual*/ void medGizmoPolylineGraph::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_VmeGizmo->GetMaterial()->m_Diffuse[0] = col[0];
  m_VmeGizmo->GetMaterial()->m_Diffuse[1] = col[1];
  m_VmeGizmo->GetMaterial()->m_Diffuse[2] = col[2];
  m_VmeGizmo->GetMaterial()->UpdateProp();
}

//----------------------------------------------------------------------------
/*virtual*/ void medGizmoPolylineGraph::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    switch(e->GetId())
    {
    case ID_TRANSFORM:      
      OnTransform(e);
      break;

    default:
      mafEventMacro(*e);
      break; 
    }
  }
} 

//------------------------------------------------------------------------
//Handles ID_TRANSFORM event
/*virtual*/ void medGizmoPolylineGraph::OnTransform(mafEvent *e)
//------------------------------------------------------------------------
{
  m_CurvilinearAbscissaHelper->MoveOnSkeleton(e);
  mafEventMacro(mafEvent(this, CAMERA_UPDATE));      

  // forward isa transform events to the listener op
  // instanciating the gizmo; the sender is changed to "this" so that the operation can check for
  // gizmo sending events
  e->SetSender(this);
  mafEventMacro(*e);
}

//----------------------------------------------------------------------------
/*virtual*/ void medGizmoPolylineGraph::Show(bool show)
//----------------------------------------------------------------------------
{
  assert(m_VmeGizmo);
  mafEventMacro(mafEvent(this,VME_SHOW,m_VmeGizmo,show));  
}

//------------------------------------------------------------------------
void medGizmoPolylineGraph::LogTransformEvent( mafEvent *e )
//------------------------------------------------------------------------
{
  std::ostringstream stringStream;

  long id = (long)(e->GetId());
  assert(id == ID_TRANSFORM);

  long mouseAction = e->GetArg();

  mafString mouseActionString;
  switch(mouseAction)
  {
  case mmiGenericMouse::MOUSE_DOWN:
    mouseActionString.Append("MOUSE DOWN");
    break;
  case mmiGenericMouse::MOUSE_MOVE:
    mouseActionString.Append("MOUSE MOVE");
    break;
  case mmiGenericMouse::MOUSE_UP:
    mouseActionString.Append("MOUSE UP");
    break;
  default:
    mouseActionString.Append("Unknown mouse action: mouseActionID = ");
    mouseActionString << (long)mouseAction;
    break;
  }


  stringStream << mouseActionString.GetCStr() << std::endl;
  e->GetMatrix()->Print(stringStream);

  mafLogMessage(stringStream.str().c_str());
}

