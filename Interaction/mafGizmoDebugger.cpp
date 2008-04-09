/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoDebugger.cpp,v $
  Language:  C++
  Date:      $Date: 2008-04-09 14:18:48 $
  Version:   $Revision: 1.1 $
  Authors:   Stefano Perticoni
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

#include "mafGizmoDebugger.h"
#include "mafDecl.h"
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

const double defaultLineLength = 50;

//----------------------------------------------------------------------------
mafGizmoDebugger::mafGizmoDebugger(mafNode* imputVme, mafObserver *listener, const char* name) 
//----------------------------------------------------------------------------
{
  Constructor(imputVme, listener, name);
}
//----------------------------------------------------------------------------
void mafGizmoDebugger::CreateInteractor1()
{
  mafNEW(m_GizmoInteractor);
  m_LeftMouseInteractor = m_GizmoInteractor->CreateBehavior(MOUSE_LEFT);
  m_LeftMouseInteractor->SetListener(this);
  m_LeftMouseInteractor->SetVME(m_VmeGizmoDebugger);
  m_LeftMouseInteractor->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal(InputVME);
  m_LeftMouseInteractor->EnableTranslation(true);
  m_LeftMouseInteractor->ResultMatrixConcatenationOn();

}

void mafGizmoDebugger::CreateInteractor2()
{
  // from void mafGuiTransformMouse::CreateISA()
  // used for unconstrained move interaction


  RefSysVME = InputVME;

  mafMatrix *absMatrix;
  absMatrix = RefSysVME->GetOutput()->GetAbsMatrix();

  mafNEW(m_GizmoInteractor);
  m_LeftMouseInteractor = m_GizmoInteractor->CreateBehavior(MOUSE_LEFT);

  m_LeftMouseInteractor->SetListener(this);
  m_LeftMouseInteractor->SetVME(InputVME);
  m_LeftMouseInteractor->GetTranslationConstraint()->GetRefSys()->SetTypeToView();
  
  // set the pivot point
  m_LeftMouseInteractor->GetTranslationConstraint()->GetRefSys()->SetMatrix(absMatrix);
  m_LeftMouseInteractor->GetPivotRefSys()->SetTypeToCustom(absMatrix);

  m_LeftMouseInteractor->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::FREE, mmiConstraint::LOCK);
  m_LeftMouseInteractor->EnableTranslation(true);

}
void mafGizmoDebugger::Constructor(mafNode *imputVme, mafObserver *listener, const char* name)
//----------------------------------------------------------------------------
{

  m_Name = name;
  m_Listener = listener;
  m_ConstraintPolyline = NULL;
  m_VmeGizmoDebugger = NULL;

  m_LineSource = vtkLineSource::New();
  m_SphereSource = vtkSphereSource::New();

  m_AppendPolyData = vtkAppendPolyData::New();
  m_AppendPolyData->SetInput(m_LineSource->GetOutput());
  m_AppendPolyData->AddInput(m_SphereSource->GetOutput());
  m_AppendPolyData->Update();

  SetLength(defaultLineLength);
  
  // register the input vme

  InputVME = mafVME::SafeDownCast(imputVme);

  m_ConstraintModality = FREE;

  CreateVMEGizmo();
  
  m_VmeGizmoDebugger->SetData(m_AppendPolyData->GetOutput());

  assert(m_VmeGizmoDebugger->GetData()->GetNumberOfPoints());

  CreateInteractor2();

  m_VmeGizmoDebugger->SetBehavior(m_GizmoInteractor);
}
//----------------------------------------------------------------------------
void mafGizmoDebugger::Destructor()
//----------------------------------------------------------------------------
{
  m_LineSource->Delete();
  m_SphereSource->Delete();
  m_AppendPolyData->Delete();

  m_VmeGizmoDebugger->SetBehavior(NULL);
  mafDEL(m_GizmoInteractor);

  m_VmeGizmoDebugger->ReparentTo(NULL);
  mafDEL(m_VmeGizmoDebugger);
}
//----------------------------------------------------------------------------
mafGizmoDebugger::~mafGizmoDebugger()
//----------------------------------------------------------------------------
{
  Destructor();
}
void mafGizmoDebugger::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_VmeGizmoDebugger->GetMaterial()->m_Diffuse[0] = col[0];
  m_VmeGizmoDebugger->GetMaterial()->m_Diffuse[1] = col[1];
  m_VmeGizmoDebugger->GetMaterial()->m_Diffuse[2] = col[2];
  m_VmeGizmoDebugger->GetMaterial()->UpdateProp();
}
//----------------------------------------------------------------------------
void mafGizmoDebugger::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
	{
    switch(e->GetId())
    {
      case ID_TRANSFORM:
      {
        assert(true);
        // coming from the interactor
        // nothing is done for the moment since this needs to be implemented at interactor level

        LogTransformEvent(e);

        Move(e);
      }
      break;
      default:
        mafEventMacro(*e);
      break; 
    }
	}
}

//----------------------------------------------------------------------------
void mafGizmoDebugger::Show(bool show)
//----------------------------------------------------------------------------
{
  assert(m_VmeGizmoDebugger);
  mafEventMacro(mafEvent(this,VME_SHOW,m_VmeGizmoDebugger,show));
  
}
//----------------------------------------------------------------------------
void mafGizmoDebugger::SetInput( mafVME *vme )
//----------------------------------------------------------------------------
{
  if (m_VmeGizmoDebugger != NULL)
  {
    Destructor();
    Constructor(vme, m_Listener, m_Name);
  }
}

void mafGizmoDebugger::SetCurvilinearAbscissa( double s )
{
  FindGizmoAbsPose(s);
  m_CurvilinearAbscissa = s;
}

double mafGizmoDebugger::GetCurvilinearAbscissa()
{
  return m_CurvilinearAbscissa;
}

void mafGizmoDebugger::FindGizmoAbsPose( double s )
{
  mafMatrix moverAbsMatrix;
  GetConstrainedAbsPose(m_ConstraintPolyline, s, moverAbsMatrix);

  // set the new pose to the Gizmo
  m_VmeGizmoDebugger->SetPose(moverAbsMatrix, -1);
 
  mafSmartPointer<mafMatrix> mat2Send;
  mat2Send->DeepCopy(&moverAbsMatrix);

  SendTransformMatrix(mat2Send, ID_TRANSFORM, mafGizmoDebugger::ABS_POSE );
}
  

void mafGizmoDebugger::FindBoundaryVerticesID( double s, int &idMin, int &idMax, double &sFromIdMin )
{
  idMin = idMax = -1;

  mafVMEOutputPolyline *outPolyline = mafVMEOutputPolyline::SafeDownCast(m_ConstraintPolyline->GetOutput());
  assert(outPolyline);

  double lineLenght = outPolyline->CalculateLength();

  if (s <  0)
  {
    s = 0;
    mafLogMessage("out of constrain polyline: setting s to 0") ;
  }
  else if (s  > lineLenght)
  {
    s = lineLenght;
    mafLogMessage("out of constrain polyline: setting s to constraint length") ;
  }

  vtkPolyData* polydata = vtkPolyData::SafeDownCast(m_ConstraintPolyline->GetOutput()->GetVTKData());
  assert(polydata != NULL);

  vtkPoints *pts = vtkPolyData::SafeDownCast(polydata)->GetPoints();
  assert(pts  !=  NULL);

  double endP[3], startP[3];

  int i = 0;
  double lastSum = 0;
  double sum = 0;

  do 
  {
    lastSum = sum;

    pts->GetPoint(i, startP);
    pts->GetPoint(i+1, endP);
    sum += sqrt(vtkMath::Distance2BetweenPoints(endP, startP));
    i++;

  } while(sum < s);

  idMin = i-1;
  idMax = i;
  sFromIdMin = s - lastSum;

}

void mafGizmoDebugger::ComputeLocalPointPositionBetweenVertices( double distP0s, int idP0, int idP1, double pOut[3] )
{
	// get the points from the two vertexes
  double p0[3] = {0,0,0};
  double p1[3] = {0,0,0};

  vtkPoints *pts = vtkPolyData::SafeDownCast(m_ConstraintPolyline->GetOutput()->GetVTKData())->GetPoints();
  if(pts == NULL) return;

  pts->GetPoint(idP0, p0);
  pts->GetPoint(idP1, p1);
  
  // creates versor from two points
  double p0p1[3] = {0,0,0};
  BuildVector(p0,p1,p0p1);
  vtkMath::Normalize(p0p1);

  double p0s[3] = {0,0,0};
  BuildVector(distP0s, p0p1, p0s);

  AddVectors(p0,p0s,pOut);

}

void mafGizmoDebugger::BuildVector( double p0[3],double p1[3],double vOut[3] )
{
	for (int i = 0;i < 3; i++)
	{
    vOut[i] = p1[i] - p0[i];
	}
}

//----------------------------------------------------------------------------
void mafGizmoDebugger::BuildVector(double coeff, const double *inVector, double *outVector)
//----------------------------------------------------------------------------
{
  if (outVector)
  {
    outVector[0] = coeff * inVector[0];
    outVector[1] = coeff * inVector[1];
    outVector[2] = coeff * inVector[2];
  }
}

void mafGizmoDebugger::AddVectors( double p0[3],double p1[3],double sum[3] )
{
  for (int i = 0;i < 3; i++)
  {
    sum[i] = p0[i] + p1[i];
  }
}

void mafGizmoDebugger::ComputeLocalNormal( int idP0, int idP1, double viewUp[3], double normal[3] )
{
  // get the points from the two vertexes
  double p0[3] = {0,0,0};
  double p1[3] = {0,0,0};

  vtkPoints *pts = vtkPolyData::SafeDownCast(m_ConstraintPolyline->GetOutput()->GetVTKData())->GetPoints();
  if(pts == NULL) return;

  pts->GetPoint(idP0, p0);
  pts->GetPoint(idP1, p1);

  double p0p1[3] = {0,0,0};

  BuildVector(p0, p1, p0p1);
  vtkMath::Normalize(p0p1);

  // cross product with the normal
  double viewup_x_p0p1[3] = {0,0,0};
  vtkMath::Cross(viewUp, p0p1, normal);

  vtkMath::Normalize(normal);
}

void mafGizmoDebugger::SetVersor( int axis, double versor[3], mafMatrix &matrix )
{
  if (0 <= axis && axis <= 2)
  {
    for (int i = 0; i < 3; i++)
    {
      matrix.SetElement(i, axis, versor[i]);
    }	
  }	
}

void mafGizmoDebugger::SetLength( double lineLength )
{
	m_LineSource->SetPoint1(0,  lineLength/2,0);
  m_LineSource->SetPoint2(0, -lineLength/2,0);
  
  // sphere on the lower end
  /*m_SphereSource->SetCenter(0,-lineLength / 2, 0);
  m_SphereSource->SetRadius(lineLength / 4);*/

  // sphere in the middle
  m_SphereSource->SetCenter(0, 0, 0);
  m_SphereSource->SetRadius(lineLength / 8);

}

void mafGizmoDebugger::CreateVMEGizmo()
{
  assert(m_VmeGizmoDebugger == NULL);

  mafNEW(m_VmeGizmoDebugger);
  m_VmeGizmoDebugger->SetName(m_Name);

  // find the root from InputVME
  mafVMERoot *root = mafVMERoot::SafeDownCast(InputVME->GetRoot());

  assert(root);

  m_VmeGizmoDebugger->ReparentTo(root);

  // ask the manager to create the pipelines
  // this his giving problems... amounted for the moment
  // mafEventMacro(mafEvent(this,VME_SHOW,m_VmeGizmoPath,true));
  
}

void mafGizmoDebugger::DestroyVMEGizmo()
{
  assert(m_VmeGizmoDebugger != NULL);

  m_VmeGizmoDebugger->SetBehavior(NULL);
  mafDEL(m_GizmoInteractor);

  m_VmeGizmoDebugger->ReparentTo(NULL);
  mafDEL(m_VmeGizmoDebugger);
}

mafMatrix* mafGizmoDebugger::GetAbsPose()
{
  return m_VmeGizmoDebugger->GetOutput()->GetAbsMatrix();

}

void mafGizmoDebugger::LogTransformEvent( mafEvent *e )
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

void mafGizmoDebugger::Move( mafEvent * e )
{
  // handle incoming transform events
  // REFACTOR THIS: extract method

  vtkMatrix4x4* startAbsPoseVTK = ((mafVME *)m_VmeGizmoDebugger)->GetOutput()->GetAbsMatrix()->GetVTKMatrix();

  mafMatrix startAbsPose;
  startAbsPose.DeepCopy(startAbsPoseVTK);
  startAbsPose.SetTimeStamp(-1);

  double startAbsPosition[3] = {0,0,0};
  mafTransform::GetPosition(startAbsPose,startAbsPosition);

  vtkTransform* transform;
  transform = vtkTransform::New();
  transform->PostMultiply();
  transform->SetMatrix(startAbsPose.GetVTKMatrix());
  transform->Concatenate(e->GetMatrix()->GetVTKMatrix());
  transform->Update();

  mafMatrix endAbsPose;
  endAbsPose.DeepCopy(transform->GetMatrix());
  endAbsPose.SetTimeStamp(-1);

  // get last move vector in wc
  double endAbsPosition[3] = {0,0,0};
  mafTransform::GetPosition(endAbsPose, endAbsPosition);

  // project vector on the constrain
  double moveVector[3] = {0,0,0};
  mafTransform::BuildVector(startAbsPosition,endAbsPosition,moveVector);
  
  mafLogMessage("Move Vector:") ;
  LogVector3(moveVector);

  // get the curvilinear abscissa 
  double sBeforeMoving = GetCurvilinearAbscissa();

  // get the constrain local versor in sBeforeMoving
  // since the gizmo is perpendicular to the constrain use its abs pose to get 
  // the constrain normal
  
  
  // from curvilinear abscissa

  double constrainVersor[3] = {0,0,0};
  mafMatrix lastAbsPose = m_VmeGizmoDebugger->GetAbsMatrixPipe()->GetMatrix();
  mafTransform::GetVersor(0, lastAbsPose, constrainVersor);
  mafLogMessage("Constrain Versor:") ;
  LogVector3(constrainVersor);

  double moveVectorOnConstrainProjection[3] = {0,0,0};

  // project moveVector on constraint versor
  double projectionValue = mafTransform::ProjectVectorOnAxis(moveVector, constrainVersor, moveVectorOnConstrainProjection);

  mafLogMessage("Move Vector On Constrain Projection") ;
  LogVector3(moveVectorOnConstrainProjection);

  // add projection to curvilinear abscissa

  // set gizmo position
  
  double newS = sBeforeMoving + projectionValue;

  SetCurvilinearAbscissa(newS);

  mafEventMacro(mafEvent(this, CAMERA_UPDATE));

  // clean up
  transform->Delete();
}

void mafGizmoDebugger::LogVector3( double *vector )
{
  std::ostringstream stringStream;
  
  stringStream << "Vector components: [" << vector[0] << " , " << vector[1] << " , " << vector[2] << " ]" << std::endl;
  stringStream << "Vector module: " << vtkMath::Norm(vector) << std::endl; 

  mafLogMessage(stringStream.str().c_str());
}

void mafGizmoDebugger::GetConstrainedAbsPose( mafVME *inputConstrain, double s, mafMatrix &moverOutputAbsPose )
{
	// get the two boundary vertexes
  int p0ID = -1, p1ID = -1;
  double distP0s = -1;

  FindBoundaryVerticesID(s,p0ID,p1ID,distP0s);

  // calculate local point position between vertexes
  double pos[3] = {0,0,0};
  double pOut[3] = {0,0,0};

  ComputeLocalPointPositionBetweenVertices(distP0s,p0ID,p1ID,pOut);

  // compute the normal
  double n[3] = {0,0,0};

  // view up is local z axi // TODO: this is not general... to be changed
  double vup[3] = {0,0,1};

  ComputeLocalNormal(p0ID, p1ID, vup, n);


  // create the local pose matrix from position and normal
  double n_x_vup[3] = {0,0,0};

  
  vtkMath::Normalize(vup);
  vtkMath::Normalize(n);
  vtkMath::Cross(n, vup, n_x_vup);

  mafMatrix localGizmoPose;
  SetVersor(0, n_x_vup, localGizmoPose);
  SetVersor(1, n, localGizmoPose);
  SetVersor(2, vup, localGizmoPose);
  localGizmoPose.SetTimeStamp(-1);
  mafTransform::SetPosition(localGizmoPose, pOut);

  // get absolute pose for the constrain
  mafMatrix constrainAbsPose; // input
  constrainAbsPose = inputConstrain->GetAbsMatrixPipe()->GetMatrix();
  
  mafSmartPointer<mafTransform> trans; // output
  trans->SetMatrix(constrainAbsPose);
  trans->Concatenate(localGizmoPose, PRE_MULTIPLY);
  trans->Update();

  moverOutputAbsPose = trans->GetMatrix();
}
