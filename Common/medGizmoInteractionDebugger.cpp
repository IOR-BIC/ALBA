/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: medGizmoInteractionDebugger.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-09 14:21:21 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

const bool DEBUG_MODE = true;

#include "mafDefines.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the MAF must include "mafDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------

#include "medGizmoInteractionDebugger.h"
#include "mafDecl.h"
#include "mafEvent.h"
#include "mmaMaterial.h"
#include "mmgMaterialButton.h"
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
#include "vtkLineSource.h"
#include "vtkPolyData.h"
#include "vtkProperty.h"
#include "vtkSphereSource.h"
#include "vtkAppendPolyData.h"
#include "vtkTransform.h"

// new stuff
#include "medVMEPolylineGraph.h"
#include "mafPolylineGraph.h"
#include "vtkIdList.h"
#include "medCurvilinearAbscissaOnSkeletonHelper.h"

const double defaultLineLength = 50;
 

medGizmoInteractionDebugger::medGizmoInteractionDebugger(mafNode* imputVme, mafObserver *listener, const char* name) 
{
  Constructor(imputVme, listener, name);
}

void medGizmoInteractionDebugger::CreateInteractor()
{  
  RefSysVME = InputVME;

  mafMatrix *absMatrix;
  absMatrix = RefSysVME->GetOutput()->GetAbsMatrix();

  mafNEW(m_GizmoInteractor);
  m_LeftMouseInteractor = m_GizmoInteractor->CreateBehavior(MOUSE_LEFT);

  m_LeftMouseInteractor->SetListener(this);
  m_LeftMouseInteractor->SetVME(InputVME);

  m_LeftMouseInteractor->GetTranslationConstraint()->GetRefSys()->SetTypeToView();

  m_LeftMouseInteractor->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
  m_LeftMouseInteractor->EnableTranslation(true);    

  m_VmeGizmo->SetBehavior(m_GizmoInteractor);
}

void medGizmoInteractionDebugger::Constructor(mafNode *imputVme, mafObserver *listener, const char* name)
{
  m_CurvilinearAbscissaHelper = NULL;
  m_VmeGizmo = NULL;

  m_Name = name;
  m_Listener = listener;

  InputVME = mafVME::SafeDownCast(imputVme);

  CreateVMEGizmo();

  m_CurvilinearAbscissaHelper = new medCurvilinearAbscissaOnSkeletonHelper( m_VmeGizmo );
}
//----------------------------------------------------------------------------
void medGizmoInteractionDebugger::Destructor()
//----------------------------------------------------------------------------
{
  m_LineSource->Delete();
  m_SphereSource->Delete();
  m_AppendPolyData->Delete();

  m_VmeGizmo->SetBehavior(NULL);
  mafDEL(m_GizmoInteractor);

  m_VmeGizmo->ReparentTo(NULL);
  mafDEL(m_VmeGizmo);

  cppDEL(m_CurvilinearAbscissaHelper);
}
//----------------------------------------------------------------------------
medGizmoInteractionDebugger::~medGizmoInteractionDebugger()
//----------------------------------------------------------------------------
{
  Destructor();
}
void medGizmoInteractionDebugger::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_VmeGizmo->GetMaterial()->m_Diffuse[0] = col[0];
  m_VmeGizmo->GetMaterial()->m_Diffuse[1] = col[1];
  m_VmeGizmo->GetMaterial()->m_Diffuse[2] = col[2];
  m_VmeGizmo->GetMaterial()->UpdateProp();
}
void medGizmoInteractionDebugger::SetConstraintPolylineGraph( medVMEPolylineGraph* constraintPolylineGraph )
{
  m_CurvilinearAbscissaHelper->SetConstraintPolylineGraph(constraintPolylineGraph);
}

void medGizmoInteractionDebugger::CreateGizmoVTKData()
{
  m_LineSource = vtkLineSource::New();
  m_SphereSource = vtkSphereSource::New();

  m_AppendPolyData = vtkAppendPolyData::New();
  m_AppendPolyData->SetInput(m_LineSource->GetOutput());
  m_AppendPolyData->AddInput(m_SphereSource->GetOutput());
  m_AppendPolyData->Update();
}

vtkIdType medGizmoInteractionDebugger::GetActiveBranchId()
{
  return m_CurvilinearAbscissaHelper->GetActiveBranchId();
}



//----------------------------------------------------------------------------
void medGizmoInteractionDebugger::OnEvent(mafEventBase *maf_event)
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
void medGizmoInteractionDebugger::Show(bool show)
//----------------------------------------------------------------------------
{
  assert(m_VmeGizmo);
  mafEventMacro(mafEvent(this,VME_SHOW,m_VmeGizmo,show));  
}

void medGizmoInteractionDebugger::SetCurvilinearAbscissa( vtkIdType branchId, double s )
{   
  m_CurvilinearAbscissaHelper->SetCurvilinearAbscissa(branchId, s);
}

double medGizmoInteractionDebugger::GetCurvilinearAbscissa()
{
  return m_CurvilinearAbscissaHelper->GetCurvilinearAbscissa();
}


void medGizmoInteractionDebugger::SetGizmoLength( double lineLength )
{
  m_LineSource->SetPoint1(0,  lineLength/2,0);
  m_LineSource->SetPoint2(0, -lineLength/2,0);

  m_SphereSource->SetCenter(0, 0, 0);
  m_SphereSource->SetRadius(lineLength / 8);
}

void medGizmoInteractionDebugger::CreateVMEGizmo()
{

  mafVMERoot *root = mafVMERoot::SafeDownCast(InputVME->GetRoot());
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

void medGizmoInteractionDebugger::DestroyVMEGizmo()
{
  assert(m_VmeGizmo != NULL);

  m_VmeGizmo->SetBehavior(NULL);
  mafDEL(m_GizmoInteractor);

  m_VmeGizmo->ReparentTo(NULL);
  mafDEL(m_VmeGizmo);
}

void medGizmoInteractionDebugger::LogTransformEvent( mafEvent *e )
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

