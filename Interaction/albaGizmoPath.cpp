/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoPath
 Authors: Stefano Perticoni
 
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

#include "albaGizmoPath.h"
#include "albaDecl.h"
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
#include "vtkPolyData.h"
#include "vtkProperty.h"

const double defaultLineLength = 50;

//----------------------------------------------------------------------------
albaGizmoPath::albaGizmoPath(albaVME* imputVme, albaObserver *listener, const char* name, int textVisibility) 
//----------------------------------------------------------------------------
{
  m_TextVisibility = textVisibility;
  Constructor(imputVme, listener, name);
}
//----------------------------------------------------------------------------
void albaGizmoPath::CreateInteractor()
{
  albaNEW(m_GizmoInteractor);
  m_LeftMouseInteractor = m_GizmoInteractor->CreateBehavior(MOUSE_LEFT);
  m_LeftMouseInteractor->SetListener(this);
  m_LeftMouseInteractor->SetVME(m_VmeGizmoPath);
  m_LeftMouseInteractor->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal(m_InputVME);
  m_LeftMouseInteractor->EnableTranslation(true);
  m_LeftMouseInteractor->ResultMatrixConcatenationOn();
}
void albaGizmoPath::Constructor(albaVME *imputVme, albaObserver *listener, const char* name)
//----------------------------------------------------------------------------
{

  m_Name = name;
  m_Listener = listener;
  m_ConstraintPolyline = NULL;
  m_VmeGizmoPath = NULL;

  m_LineSource = vtkLineSource::New();

  SetLineLength(defaultLineLength);

  // register the input vme
  m_InputVME = imputVme;

  m_ConstraintModality = FREE;

  CreateVMEGizmo();
  
  m_VmeGizmoPath->SetDataConnection(m_LineSource->GetOutputPort());

  assert(m_VmeGizmoPath->GetData()->GetNumberOfPoints()==2);


  CreateInteractor();
  m_VmeGizmoPath->SetBehavior(m_GizmoInteractor);

  m_TextSidePosition = ID_LEFT_TEXT_SIDE;
}
//----------------------------------------------------------------------------
void albaGizmoPath::Destructor()
//----------------------------------------------------------------------------
{
  m_LineSource->Delete();

  m_VmeGizmoPath->SetBehavior(NULL);
  albaDEL(m_GizmoInteractor);

  m_VmeGizmoPath->ReparentTo(NULL);
  albaDEL(m_VmeGizmoPath);
}
//----------------------------------------------------------------------------
albaGizmoPath::~albaGizmoPath()
//----------------------------------------------------------------------------
{
  Destructor();
}
void albaGizmoPath::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_VmeGizmoPath->GetMaterial()->m_Diffuse[0] = col[0];
  m_VmeGizmoPath->GetMaterial()->m_Diffuse[1] = col[1];
  m_VmeGizmoPath->GetMaterial()->m_Diffuse[2] = col[2];
  m_VmeGizmoPath->GetMaterial()->UpdateProp();
}
//----------------------------------------------------------------------------
void albaGizmoPath::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
	{
    switch(e->GetId())
    {
      case ID_TRANSFORM:
      {
        // coming from the interactor
        // nothing is done for the moment since this needs to be implemented at interactor level
      }
      break;
      default:
        albaEventMacro(*e);
      break; 
    }
	}
}

//----------------------------------------------------------------------------
void albaGizmoPath::Show(bool show)
//----------------------------------------------------------------------------
{
  assert(m_VmeGizmoPath);

  // can not use this since it's too slow... this requires destroying and creating
  // the pipeline each time...
  
  // ... instead I am using vtk opacity to speed up the render
  double opacity = show ? 1 : 0;
  m_VmeGizmoPath->GetMaterial()->m_Prop->SetOpacity(opacity);
}
//----------------------------------------------------------------------------
void albaGizmoPath::SetInput( albaVME *vme )
//----------------------------------------------------------------------------
{
  if (m_VmeGizmoPath != NULL)
  {
    Destructor();
    Constructor(vme, m_Listener, m_Name);
  }
}

void albaGizmoPath::SetCurvilinearAbscissa( double s )
{
  FindGizmoAbsPose(s);
  m_CurvilinearAbscissa = s;

  double pos[3], rot[3];
  m_VmeGizmoPath->GetOutput()->GetPose(pos, rot);
  
  albaMatrix *matrix;
  matrix = m_VmeGizmoPath->GetOutput()->GetMatrix();
  
  //calculate position of the text
  double point1[3], point2[3];
  m_LineSource->GetPoint1(point1);
  m_LineSource->GetPoint2(point2);
  double halfLength;
  halfLength = sqrt(vtkMath::Distance2BetweenPoints(point1,point2))/2.;

  double versorY[3];
  matrix->GetVersor(1, versorY);

  if(m_TextSidePosition == ID_LEFT_TEXT_SIDE)
  {
    pos[0] = pos[0] - halfLength * (versorY[0]); 
    pos[1] = pos[1] - halfLength * (versorY[1]);
    pos[2] = pos[2] - halfLength * (versorY[2]);
  }
  else
  {
    pos[0] = pos[0] + halfLength * (versorY[0]); 
    pos[1] = pos[1] + halfLength * (versorY[1]);
    pos[2] = pos[2] + halfLength * (versorY[2]);
  }

  m_VmeGizmoPath->SetTextPosition(pos);
  
  m_VmeGizmoPath->SetTextValue(albaString::Format("%.1f", m_CurvilinearAbscissa>0?albaRoundToPrecision(m_CurvilinearAbscissa,1):0.0));
  
}

double albaGizmoPath::GetCurvilinearAbscissa()
{
  return m_CurvilinearAbscissa;
}

void albaGizmoPath::FindGizmoAbsPose( double s )
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

  double vup[3] = {0,0,1};

  ComputeLocalNormal(p0ID, p1ID, vup, n);


  // create the local pose matrix from position and normal
  double n_x_vup[3] = {0,0,0};

  
  vtkMath::Normalize(vup);
  vtkMath::Normalize(n);
  vtkMath::Cross(n, vup, n_x_vup);

  albaMatrix localGizmoPose;
  SetVersor(0, n_x_vup, localGizmoPose);
  SetVersor(1, n, localGizmoPose);
  SetVersor(2, vup, localGizmoPose);
  localGizmoPose.SetTimeStamp(-1);
  albaTransform::SetPosition(localGizmoPose, pOut);

  // get absolute pose for the constrain
  albaMatrix constrainAbsPose;
  constrainAbsPose = m_ConstraintPolyline->GetAbsMatrixPipe()->GetMatrix();
  
  albaSmartPointer<albaTransform> trans;
  trans->SetMatrix(constrainAbsPose);
  trans->Concatenate(localGizmoPose, PRE_MULTIPLY);
  trans->Update();
  
  // set the new pose to the Gizmo
  m_VmeGizmoPath->SetPose(trans->GetMatrix(), -1);
 
  albaSmartPointer<albaMatrix> mat2Send;
  mat2Send->DeepCopy(&(trans->GetMatrix()));

  SendTransformMatrix(mat2Send, ID_TRANSFORM, albaGizmoPath::ABS_POSE );
}
  

void albaGizmoPath::FindBoundaryVerticesID( double s, int &idMin, int &idMax, double &sFromIdMin )
{
  idMin = idMax = -1;
  
  albaVMEOutputPolyline *outPolyline = albaVMEOutputPolyline::SafeDownCast(m_ConstraintPolyline->GetOutput());
  assert(outPolyline);

  double lineLenght = outPolyline->CalculateLength();

  if (s <  0)
  {
    s = 0;
    albaLogMessage("out of constrain polyline: setting s to 0") ;
  }
  else if (s  > lineLenght)
  {
    s = lineLenght;
    albaLogMessage("out of constrain polyline: setting s to constraint length") ;
  }

  vtkPoints *pts = vtkPolyData::SafeDownCast(m_ConstraintPolyline->GetOutput()->GetVTKData())->GetPoints();
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

void albaGizmoPath::ComputeLocalPointPositionBetweenVertices( double distP0s, int idP0, int idP1, double pOut[3] )
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

void albaGizmoPath::BuildVector( double p0[3],double p1[3],double vOut[3] )
{
	for (int i = 0;i < 3; i++)
	{
    vOut[i] = p1[i] - p0[i];
	}
}

//----------------------------------------------------------------------------
void albaGizmoPath::BuildVector(double coeff, const double *inVector, double *outVector)
//----------------------------------------------------------------------------
{
  if (outVector)
  {
    outVector[0] = coeff * inVector[0];
    outVector[1] = coeff * inVector[1];
    outVector[2] = coeff * inVector[2];
  }
}

void albaGizmoPath::AddVectors( double p0[3],double p1[3],double sum[3] )
{
  for (int i = 0;i < 3; i++)
  {
    sum[i] = p0[i] + p1[i];
  }
}

void albaGizmoPath::ComputeLocalNormal( int idP0, int idP1, double viewUp[3], double normal[3] )
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

void albaGizmoPath::SetVersor( int axis, double versor[3], albaMatrix &matrix )
{
  if (0 <= axis && axis <= 2)
  {
    for (int i = 0; i < 3; i++)
    {
      matrix.SetElement(i, axis, versor[i]);
    }	
  }	
}

void albaGizmoPath::SetLineLength( double lineLength )
{

	m_LineSource->SetPoint1(0,  lineLength/2,0);
  m_LineSource->SetPoint2(0, -lineLength/2,0);
  m_LineSource->Update();

}

void albaGizmoPath::CreateVMEGizmo()
{
  assert(m_VmeGizmoPath == NULL);

  albaNEW(m_VmeGizmoPath);
  m_VmeGizmoPath->SetName(m_Name);
  m_VmeGizmoPath->SetTextVisibility(m_TextVisibility);

  // find the root from InputVME
  albaVMERoot *root = albaVMERoot::SafeDownCast(m_InputVME->GetRoot());

  assert(root);

  m_VmeGizmoPath->ReparentTo(root);

  // ask the manager to create the pipelines
  // this his giving problems... amounted for the moment
  
}

void albaGizmoPath::DestroyVMEGizmo()
{
  assert(m_VmeGizmoPath != NULL);

  m_VmeGizmoPath->SetBehavior(NULL);
  albaDEL(m_GizmoInteractor);

  m_VmeGizmoPath->ReparentTo(NULL);
  albaDEL(m_VmeGizmoPath);
}

albaMatrix* albaGizmoPath::GetAbsPose()
{
  return m_VmeGizmoPath->GetOutput()->GetAbsMatrix();

}
//----------------------------------------------------------------------------
