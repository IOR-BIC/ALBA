/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoInteractionDebugger
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const bool DEBUG_MODE = true;

#include "albaDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "albaGizmoInteractionDebugger.h"
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
#include "vtkLineSource.h"
#include "vtkPlaneSource.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"

// new stuff
#include "albaVMEPolylineGraph.h"
#include "albaPolylineGraph.h"
#include "vtkIdList.h"
#include "albaCurvilinearAbscissaOnSkeletonHelper.h"

const double defaultLineLength = 50;
 

albaGizmoInteractionDebugger::albaGizmoInteractionDebugger(albaVME* imputVme, albaObserver *listener, const char* name, bool testMode) 
{
  Constructor(imputVme, listener, name, testMode);
}

void albaGizmoInteractionDebugger::CreateInteractor()
{  
  m_RefSysVME = m_InputVME;

  albaMatrix *absMatrix;
  absMatrix = m_RefSysVME->GetOutput()->GetAbsMatrix();

  albaNEW(m_GizmoInteractor);
  m_LeftMouseInteractor = m_GizmoInteractor->CreateBehavior(MOUSE_LEFT);

  m_LeftMouseInteractor->SetListener(this);
  m_LeftMouseInteractor->SetVME(m_InputVME);

  m_LeftMouseInteractor->GetTranslationConstraint()->GetRefSys()->SetTypeToView();

  m_LeftMouseInteractor->GetTranslationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  m_LeftMouseInteractor->EnableTranslation(true);    

  m_VmeGizmo->SetBehavior(m_GizmoInteractor);
}

void albaGizmoInteractionDebugger::Constructor(albaVME *imputVme, albaObserver *listener, const char* name, bool testMode)
{
  m_CurvilinearAbscissaHelper = NULL;
  m_VmeGizmo = NULL;

  m_Name = name;
  m_Listener = listener;

  m_InputVME = imputVme;

  CreateVMEGizmo();

  m_CurvilinearAbscissaHelper = new albaCurvilinearAbscissaOnSkeletonHelper( m_VmeGizmo, this , testMode);
}
//----------------------------------------------------------------------------
void albaGizmoInteractionDebugger::Destructor()
//----------------------------------------------------------------------------
{
  m_LineSource->Delete();
  m_SphereSource->Delete();
  m_PlaneSource->Delete();
  m_AppendPolyData->Delete();

  m_VmeGizmo->SetBehavior(NULL);
  albaDEL(m_GizmoInteractor);

  m_VmeGizmo->ReparentTo(NULL);
  albaDEL(m_VmeGizmo);

  cppDEL(m_CurvilinearAbscissaHelper);
}
//----------------------------------------------------------------------------
albaGizmoInteractionDebugger::~albaGizmoInteractionDebugger()
//----------------------------------------------------------------------------
{
  Destructor();
}
void albaGizmoInteractionDebugger::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_VmeGizmo->GetMaterial()->m_Diffuse[0] = col[0];
  m_VmeGizmo->GetMaterial()->m_Diffuse[1] = col[1];
  m_VmeGizmo->GetMaterial()->m_Diffuse[2] = col[2];
  m_VmeGizmo->GetMaterial()->UpdateProp();
}
void albaGizmoInteractionDebugger::SetConstraintPolylineGraph( albaVMEPolylineGraph* constraintPolylineGraph )
{
  m_CurvilinearAbscissaHelper->SetConstraintPolylineGraph(constraintPolylineGraph);
}

void albaGizmoInteractionDebugger::CreateGizmoVTKData()
{
  m_LineSource = vtkLineSource::New();
  m_SphereSource = vtkSphereSource::New();
  m_PlaneSource = vtkPlaneSource::New();
 
  m_AppendPolyData = vtkAppendPolyData::New();
  m_AppendPolyData->SetInputConnection(m_LineSource->GetOutputPort());
  m_AppendPolyData->AddInputConnection(m_SphereSource->GetOutputPort());
  m_AppendPolyData->AddInputConnection(m_PlaneSource->GetOutputPort());
  m_AppendPolyData->Update();
}

vtkIdType albaGizmoInteractionDebugger::GetActiveBranchId()
{
  return m_CurvilinearAbscissaHelper->GetActiveBranchId();
}



//----------------------------------------------------------------------------
void albaGizmoInteractionDebugger::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    switch(e->GetId())
    {
    case ID_TRANSFORM:
      {
        {
          assert(true);

          if (DEBUG_MODE)
          {
            LogTransformEvent(e);        
          }

          m_CurvilinearAbscissaHelper->MoveOnSkeleton(e);
        }

				GetLogicManager()->CameraUpdate();
      }
      break;

    default:
      albaEventMacro(*e);
      break; 
    }
  }
} 

//----------------------------------------------------------------------------
void albaGizmoInteractionDebugger::Show(bool show)
//----------------------------------------------------------------------------
{
  assert(m_VmeGizmo);
  GetLogicManager()->VmeShow(m_VmeGizmo, show);
}

void albaGizmoInteractionDebugger::SetCurvilinearAbscissa( vtkIdType branchId, double s )
{   
  m_CurvilinearAbscissaHelper->SetCurvilinearAbscissa(branchId, s);
}

double albaGizmoInteractionDebugger::GetCurvilinearAbscissa()
{
  return m_CurvilinearAbscissaHelper->GetCurvilinearAbscissa();
}


void albaGizmoInteractionDebugger::SetGizmoLength( double lineLength )
{
  m_LineSource->SetPoint1(0,  lineLength,0);
  m_LineSource->SetPoint2(0, -lineLength,0);

  m_PlaneSource->SetOrigin(0,0,0);
  m_PlaneSource->SetPoint1(0,lineLength,0);
  m_PlaneSource->SetPoint2(lineLength,0,0);

  m_SphereSource->SetCenter(0, 0, 0);
  m_SphereSource->SetRadius(lineLength / 8);
}

void albaGizmoInteractionDebugger::CreateVMEGizmo()
{

  albaVMERoot *root = albaVMERoot::SafeDownCast(m_InputVME->GetRoot());
  assert(root);

  assert(m_VmeGizmo == NULL);

  CreateGizmoVTKData();
  SetGizmoLength(defaultLineLength);

  albaNEW(m_VmeGizmo);
  m_VmeGizmo->SetName(m_Name);
  m_VmeGizmo->ReparentTo(root); 
  m_VmeGizmo->SetData(m_AppendPolyData->GetOutput());  
  assert(m_VmeGizmo->GetData()->GetNumberOfPoints());

  CreateInteractor();
}

void albaGizmoInteractionDebugger::DestroyVMEGizmo()
{
  assert(m_VmeGizmo != NULL);

  m_VmeGizmo->SetBehavior(NULL);
  albaDEL(m_GizmoInteractor);

  m_VmeGizmo->ReparentTo(NULL);
  albaDEL(m_VmeGizmo);
}

void albaGizmoInteractionDebugger::LogTransformEvent( albaEvent *e )
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

albaGUI * albaGizmoInteractionDebugger::GetGui()
{
  assert(m_CurvilinearAbscissaHelper);
  return m_CurvilinearAbscissaHelper->GetGui();
}
