/*=========================================================================

 Program: MAF2
 Module: mafGizmoInteractionDebugger
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

const bool DEBUG_MODE = true;

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "mafGizmoInteractionDebugger.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmaMaterial.h"
#include "mafGUIMaterialButton.h"
#include "mafInteractorCompositorMouse.h"
#include "mafInteractorGenericMouse.h"
#include "mafSmartPointer.h"
#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "mafMatrix.h"
#include "mafAbsMatrixPipe.h" 
#include "mafVMERoot.h"
#include "mafRefSys.h"

#include "vtkMath.h"
#include "vtkLineSource.h"
#include "vtkPlaneSource.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"

// new stuff
#include "mafVMEPolylineGraph.h"
#include "mafPolylineGraph.h"
#include "vtkIdList.h"
#include "mafCurvilinearAbscissaOnSkeletonHelper.h"

const double defaultLineLength = 50;
 

mafGizmoInteractionDebugger::mafGizmoInteractionDebugger(mafNode* imputVme, mafObserver *listener, const char* name, bool testMode) 
{
  Constructor(imputVme, listener, name, testMode);
}

void mafGizmoInteractionDebugger::CreateInteractor()
{  
  m_RefSysVME = m_InputVME;

  mafMatrix *absMatrix;
  absMatrix = m_RefSysVME->GetOutput()->GetAbsMatrix();

  mafNEW(m_GizmoInteractor);
  m_LeftMouseInteractor = m_GizmoInteractor->CreateBehavior(MOUSE_LEFT);

  m_LeftMouseInteractor->SetListener(this);
  m_LeftMouseInteractor->SetVME(m_InputVME);

  m_LeftMouseInteractor->GetTranslationConstraint()->GetRefSys()->SetTypeToView();

  m_LeftMouseInteractor->GetTranslationConstraint()->SetConstraintModality(mafInteractorConstraint::FREE, mafInteractorConstraint::FREE, mafInteractorConstraint::LOCK);
  m_LeftMouseInteractor->EnableTranslation(true);    

  m_VmeGizmo->SetBehavior(m_GizmoInteractor);
}

void mafGizmoInteractionDebugger::Constructor(mafNode *imputVme, mafObserver *listener, const char* name, bool testMode)
{
  m_CurvilinearAbscissaHelper = NULL;
  m_VmeGizmo = NULL;

  m_Name = name;
  m_Listener = listener;

  m_InputVME = mafVME::SafeDownCast(imputVme);

  CreateVMEGizmo();

  m_CurvilinearAbscissaHelper = new mafCurvilinearAbscissaOnSkeletonHelper( m_VmeGizmo, this , testMode);
}
//----------------------------------------------------------------------------
void mafGizmoInteractionDebugger::Destructor()
//----------------------------------------------------------------------------
{
  m_LineSource->Delete();
  m_SphereSource->Delete();
  m_PlaneSource->Delete();
  m_AppendPolyData->Delete();

  m_VmeGizmo->SetBehavior(NULL);
  mafDEL(m_GizmoInteractor);

  m_VmeGizmo->ReparentTo(NULL);
  mafDEL(m_VmeGizmo);

  cppDEL(m_CurvilinearAbscissaHelper);
}
//----------------------------------------------------------------------------
mafGizmoInteractionDebugger::~mafGizmoInteractionDebugger()
//----------------------------------------------------------------------------
{
  Destructor();
}
void mafGizmoInteractionDebugger::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_VmeGizmo->GetMaterial()->m_Diffuse[0] = col[0];
  m_VmeGizmo->GetMaterial()->m_Diffuse[1] = col[1];
  m_VmeGizmo->GetMaterial()->m_Diffuse[2] = col[2];
  m_VmeGizmo->GetMaterial()->UpdateProp();
}
void mafGizmoInteractionDebugger::SetConstraintPolylineGraph( mafVMEPolylineGraph* constraintPolylineGraph )
{
  m_CurvilinearAbscissaHelper->SetConstraintPolylineGraph(constraintPolylineGraph);
}

void mafGizmoInteractionDebugger::CreateGizmoVTKData()
{
  m_LineSource = vtkLineSource::New();
  m_SphereSource = vtkSphereSource::New();
  m_PlaneSource = vtkPlaneSource::New();
 
  m_AppendPolyData = vtkAppendPolyData::New();
  m_AppendPolyData->SetInput(m_LineSource->GetOutput());
  m_AppendPolyData->AddInput(m_SphereSource->GetOutput());
  m_AppendPolyData->AddInput(m_PlaneSource->GetOutput());
  m_AppendPolyData->Update();
}

vtkIdType mafGizmoInteractionDebugger::GetActiveBranchId()
{
  return m_CurvilinearAbscissaHelper->GetActiveBranchId();
}



//----------------------------------------------------------------------------
void mafGizmoInteractionDebugger::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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

        mafEventMacro(mafEvent(this, CAMERA_UPDATE));
      }
      break;

    default:
      mafEventMacro(*e);
      break; 
    }
  }
} 

//----------------------------------------------------------------------------
void mafGizmoInteractionDebugger::Show(bool show)
//----------------------------------------------------------------------------
{
  assert(m_VmeGizmo);
  mafEventMacro(mafEvent(this,VME_SHOW,m_VmeGizmo,show));  
}

void mafGizmoInteractionDebugger::SetCurvilinearAbscissa( vtkIdType branchId, double s )
{   
  m_CurvilinearAbscissaHelper->SetCurvilinearAbscissa(branchId, s);
}

double mafGizmoInteractionDebugger::GetCurvilinearAbscissa()
{
  return m_CurvilinearAbscissaHelper->GetCurvilinearAbscissa();
}


void mafGizmoInteractionDebugger::SetGizmoLength( double lineLength )
{
  m_LineSource->SetPoint1(0,  lineLength,0);
  m_LineSource->SetPoint2(0, -lineLength,0);

  m_PlaneSource->SetOrigin(0,0,0);
  m_PlaneSource->SetPoint1(0,lineLength,0);
  m_PlaneSource->SetPoint2(lineLength,0,0);

  m_SphereSource->SetCenter(0, 0, 0);
  m_SphereSource->SetRadius(lineLength / 8);
}

void mafGizmoInteractionDebugger::CreateVMEGizmo()
{

  mafVMERoot *root = mafVMERoot::SafeDownCast(m_InputVME->GetRoot());
  assert(root);

  assert(m_VmeGizmo == NULL);

  CreateGizmoVTKData();
  SetGizmoLength(defaultLineLength);

  mafNEW(m_VmeGizmo);
  m_VmeGizmo->SetName(m_Name);
  m_VmeGizmo->ReparentTo(root); 
  m_VmeGizmo->SetData(m_AppendPolyData->GetOutput());  
  assert(m_VmeGizmo->GetData()->GetNumberOfPoints());

  CreateInteractor();
}

void mafGizmoInteractionDebugger::DestroyVMEGizmo()
{
  assert(m_VmeGizmo != NULL);

  m_VmeGizmo->SetBehavior(NULL);
  mafDEL(m_GizmoInteractor);

  m_VmeGizmo->ReparentTo(NULL);
  mafDEL(m_VmeGizmo);
}

void mafGizmoInteractionDebugger::LogTransformEvent( mafEvent *e )
{
  std::ostringstream stringStream;

  long id = (long)(e->GetId());
  assert(id == ID_TRANSFORM);

  long mouseAction = e->GetArg();

  mafString mouseActionString;
  switch(mouseAction)
  {
  case mafInteractorGenericMouse::MOUSE_DOWN:
    mouseActionString.Append("MOUSE DOWN");
    break;
  case mafInteractorGenericMouse::MOUSE_MOVE:
    mouseActionString.Append("MOUSE MOVE");
    break;
  case mafInteractorGenericMouse::MOUSE_UP:
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

mafGUI * mafGizmoInteractionDebugger::GetGui()
{
  assert(m_CurvilinearAbscissaHelper);
  return m_CurvilinearAbscissaHelper->GetGui();
}
