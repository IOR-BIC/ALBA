/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoPath.cpp,v $
  Language:  C++
  Date:      $Date: 2007-02-08 16:49:27 $
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


#include "mafGizmoPath.h"
#include "mafDecl.h"
#include "mmaMaterial.h"
#include "mmgMaterialButton.h"

#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"

#include "mafTransform.h"
#include "mafSmartPointer.h"
#include "mafTagArray.h"
#include "mafVME.h"
#include "mafVMEGizmo.h"

#include "vtkMAFSmartPointer.h"
#include "vtkProperty.h"
#include "vtkPlaneSource.h"
#include "vtkOutlineFilter.h"
#include "vtkRectilinearGrid.h"
#include "vtkStructuredPoints.h"
#include "vtkPoints.h"
#include "vtkCubeSource.h"
#include "vtkAppendPolyData.h"
#include "vtkPolyData.h"
#include "vtkRenderer.h"
#include "vtkDoubleArray.h"
#include "vtkDOFMatrix.h"
#include "vtkMatrix4x4.h"

#include "mafVMEPolyline.h"
#include "mafVMEPolylineSpline.h"
#include "vtkMath.h"
#include "mafMatrix.h"
#include "vtkLineSource.h"
#include "mafAbsMatrixPipe.h"
#include "mafVMERoot.h"

const double defaultLineLength = 50;

//----------------------------------------------------------------------------
mafGizmoPath::mafGizmoPath(mafNode* imputVme, mafObserver *listener, const char* name) 
//----------------------------------------------------------------------------
{
  Constructor(imputVme, listener, name);
}
//----------------------------------------------------------------------------
void mafGizmoPath::CreateInteractor()
{
  mafNEW(m_GizmoInteractor);
  m_LeftMouseInteractor = m_GizmoInteractor->CreateBehavior(MOUSE_LEFT);
  m_LeftMouseInteractor->SetListener(this);
  m_LeftMouseInteractor->SetVME(m_VmeGizmoPath);
  m_LeftMouseInteractor->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal(InputVME);
  m_LeftMouseInteractor->EnableTranslation(true);
  m_LeftMouseInteractor->ResultMatrixConcatenationOn();
}
void mafGizmoPath::Constructor(mafNode *imputVme, mafObserver *listener, const char* name)
//----------------------------------------------------------------------------
{

  m_Name = name;
  m_Listener = listener;
  m_ConstraintPolyline = NULL;
  m_VmeGizmoPath = NULL;

  m_LineSource = vtkLineSource::New();

  SetLineLength(defaultLineLength);

  // register the input vme
  InputVME = mafVME::SafeDownCast(imputVme);

  m_ConstraintModality = FREE;

  CreateVMEGizmo();
  
  m_VmeGizmoPath->SetData(m_LineSource->GetOutput());

  assert(m_VmeGizmoPath->GetData()->GetNumberOfPoints());


  CreateInteractor();
  m_VmeGizmoPath->SetBehavior(m_GizmoInteractor);
}
//----------------------------------------------------------------------------
void mafGizmoPath::Destructor()
//----------------------------------------------------------------------------
{
  m_LineSource->Delete();

  m_VmeGizmoPath->SetBehavior(NULL);
  mafDEL(m_GizmoInteractor);

  m_VmeGizmoPath->ReparentTo(NULL);
  mafDEL(m_VmeGizmoPath);
}
//----------------------------------------------------------------------------
mafGizmoPath::~mafGizmoPath()
//----------------------------------------------------------------------------
{
  Destructor();
}
void mafGizmoPath::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_VmeGizmoPath->GetMaterial()->m_Diffuse[0] = col[0];
  m_VmeGizmoPath->GetMaterial()->m_Diffuse[1] = col[1];
  m_VmeGizmoPath->GetMaterial()->m_Diffuse[2] = col[2];
  m_VmeGizmoPath->GetMaterial()->UpdateProp();
}
//----------------------------------------------------------------------------
void mafGizmoPath::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
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
        mafEventMacro(*e);
      break; 
    }
	}
}

//----------------------------------------------------------------------------
void mafGizmoPath::Show(bool show)
//----------------------------------------------------------------------------
{
  assert(m_VmeGizmoPath);

  // can not use this since it's too slow... this requires destroying and creating
  // the pipeline each time...
  // mafEventMacro(mafEvent(this,VME_SHOW,m_VmeGizmo,show));
  
  // ... instead I am using vtk opacity to speed up the render
  double opacity = show ? 1 : 0;
  m_VmeGizmoPath->GetMaterial()->m_Prop->SetOpacity(opacity);
}
//----------------------------------------------------------------------------
void mafGizmoPath::SetInput( mafVME *vme )
//----------------------------------------------------------------------------
{
  if (m_VmeGizmoPath != NULL)
  {
    Destructor();
    Constructor(vme, m_Listener, m_Name);
  }
}

void mafGizmoPath::SetCurvilinearAbscissa( double s )
{

  FindGizmoAbsPose(s);
  
}

void mafGizmoPath::FindGizmoAbsPose( double s )
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

  mafMatrix localGizmoPose;
  SetVersor(0, n_x_vup, localGizmoPose);
  SetVersor(1, n, localGizmoPose);
  SetVersor(2, vup, localGizmoPose);
  localGizmoPose.SetTimeStamp(-1);
  mafTransform::SetPosition(localGizmoPose, pOut);

  // get absolute pose for the constrain
  mafMatrix constrainAbsPose;
  constrainAbsPose = m_ConstraintPolyline->GetAbsMatrixPipe()->GetMatrix();
  
  mafSmartPointer<mafTransform> trans;
  trans->SetMatrix(constrainAbsPose);
  trans->Concatenate(localGizmoPose, PRE_MULTIPLY);
  trans->Update();
  
  // set the new pose to the Gizmo
  m_VmeGizmoPath->SetPose(trans->GetMatrix(), -1);
 
  mafSmartPointer<mafMatrix> mat2Send;
  mat2Send->DeepCopy(&(trans->GetMatrix()));

  SendTransformMatrix(mat2Send, ID_TRANSFORM, mafGizmoPath::ABS_POSE );


}

void mafGizmoPath::FindBoundaryVerticesID( double s, int &idMin, int &idMax, double &distSIdMin )
{
  idMin = idMax = -1;
  
  mafVMEOutputPolyline *outPolyline = mafVMEOutputPolyline::SafeDownCast(m_ConstraintPolyline->GetOutput());
  assert(outPolyline);

  double lineLenght = outPolyline->CalculateLength();

  if (0 <= s && s <= lineLenght)
  {
    // if inside the line continue...
  }
  else
  {
    mafLogMessage("out of constrain polyline") ;
    return;
  }

  vtkPoints *pts = vtkPolyData::SafeDownCast(m_ConstraintPolyline->GetOutput()->GetVTKData())->GetPoints();
  if(pts == NULL) return;

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
  distSIdMin = s - lastSum;
}

void mafGizmoPath::ComputeLocalPointPositionBetweenVertices( double distP0s, int idP0, int idP1, double pOut[3] )
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

void mafGizmoPath::BuildVector( double p0[3],double p1[3],double vOut[3] )
{
	for (int i = 0;i < 3; i++)
	{
    vOut[i] = p1[i] - p0[i];
	}
}

//----------------------------------------------------------------------------
void mafGizmoPath::BuildVector(double coeff, const double *inVector, double *outVector)
//----------------------------------------------------------------------------
{
  if (outVector)
  {
    outVector[0] = coeff * inVector[0];
    outVector[1] = coeff * inVector[1];
    outVector[2] = coeff * inVector[2];
  }
}

void mafGizmoPath::AddVectors( double p0[3],double p1[3],double sum[3] )
{
  for (int i = 0;i < 3; i++)
  {
    sum[i] = p0[i] + p1[i];
  }
}

void mafGizmoPath::ComputeLocalNormal( int idP0, int idP1, double viewUp[3], double normal[3] )
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

void mafGizmoPath::SetVersor( int axis, double versor[3], mafMatrix &matrix )
{
  if (0 <= axis && axis <= 2)
  {
    for (int i = 0; i < 3; i++)
    {
      matrix.SetElement(i, axis, versor[i]);
    }	
  }	
}

void mafGizmoPath::SetLineLength( double lineLength )
{

	m_LineSource->SetPoint1(0,  lineLength/2,0);
  m_LineSource->SetPoint2(0, -lineLength/2,0);
  m_LineSource->Update();

}

void mafGizmoPath::CreateVMEGizmo()
{
  assert(m_VmeGizmoPath == NULL);

  mafNEW(m_VmeGizmoPath);
  m_VmeGizmoPath->SetName(m_Name);

  // find the root from InputVME
  mafVMERoot *root = mafVMERoot::SafeDownCast(InputVME->GetRoot());

  assert(root);

  m_VmeGizmoPath->SetVisibleToTraverse(true);
  assert(m_VmeGizmoPath->ReparentTo(root) == MAF_OK);

  // create the interactor
}

void mafGizmoPath::DestroyVMEGizmo()
{
  assert(m_VmeGizmoPath != NULL);

  m_VmeGizmoPath->SetBehavior(NULL);
  mafDEL(m_GizmoInteractor);

  m_VmeGizmoPath->ReparentTo(NULL);
  mafDEL(m_VmeGizmoPath);
}

mafMatrix* mafGizmoPath::GetAbsPose()
{
  return m_VmeGizmoPath->GetOutput()->GetAbsMatrix();

}
//----------------------------------------------------------------------------
