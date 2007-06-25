/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoHandle.cpp,v $
  Language:  C++
  Date:      $Date: 2007-06-25 10:02:15 $
  Version:   $Revision: 1.6 $
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


#include "mafGizmoHandle.h"
#include "mafDecl.h"
#include "mmaMaterial.h"

// isa stuff
#include "mmiCompositorMouse.h"
#include "mmiGenericInterface.h"

// vme stuff
#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafVMEOutput.h"
#include "mafTransform.h"

// vtk stuff
#include "vtkProperty.h"
#include "vtkCubeSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkDOFMatrix.h"

#include <vector>
#include <algorithm>

using namespace std;

//----------------------------------------------------------------------------
mafGizmoHandle::mafGizmoHandle(mafVME *input, mafObserver *listener, int constraintModality,mafVME *parent)
//----------------------------------------------------------------------------
{
	m_ConstraintModality = constraintModality;
  IsaComp = NULL;
  Cube = NULL;

	for(int i=0;i<6;i++)
		for(int j=0;j<3;j++)
			BBCenters[i][j]=0;

  m_Listener = listener;
  InputVme = input;

  ////-----------------
  // create pipeline stuff
  ////-----------------
  CreatePipeline();

  //-----------------
  // create vme gizmos stuff
  //-----------------
  // cone gizmo
  BoxGizmo = mafVMEGizmo::New();  
  BoxGizmo->SetName("BoxGizmo");
  
  // since i'm working in local mode i reparent to input vme the gizmo
  BoxGizmo->SetData(TranslateBoxPDFEnd->GetOutput());
	if(parent)
		BoxGizmo->ReparentTo(parent);
	else
		BoxGizmo->ReparentTo(InputVme);

  // set come gizmo material property and initial color to red
  this->SetColor(1, 0, 0);

  // hide gizmo at creation
  this->Show(false);

  // ask the manager to create the pipeline
  mafEventMacro(mafEvent(this,VME_SHOW,BoxGizmo,true));
  
  //-----------------
  // create isa stuff
  //-----------------
  // create isa compositor and assign behaviors to IsaGen ivar
  IsaComp  = mmiCompositorMouse::New();

  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  IsaGen = IsaComp->CreateBehavior(MOUSE_LEFT);

  //isa will send events to this
	if(m_ConstraintModality==BOUNDS)
	{
		IsaGen->SetListener(this);  
		IsaGen->SetVME(BoxGizmo);
		IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToGlobal();
		IsaGen->EnableTranslation(true);
	}
	else if(m_ConstraintModality==FREE)
	{
		IsaGen->SetListener(this);  
		IsaGen->SetVME(BoxGizmo);
		IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal();
		IsaGen->EnableTranslation(true);
	}

  // assign isa to cylinder and cone
  BoxGizmo->SetBehavior(IsaComp);

   // default axis is X
  SetType(mafGizmoHandle::XMIN);

  InputVme->GetOutput()->Update();
  double b[6];
  InputVme->GetOutput()->GetVTKData()->GetBounds(b);
  SetBBCenters(b);
	if(m_ConstraintModality==BOUNDS)
		SetTranslationIntervals(b);
}
//----------------------------------------------------------------------------
mafGizmoHandle::~mafGizmoHandle() 
//----------------------------------------------------------------------------
{
  BoxGizmo->SetBehavior(NULL);

  vtkDEL(TranslateBoxTr);
  vtkDEL(TranslateBoxPDF);
  vtkDEL(RotateBoxTr);
  vtkDEL(RotateBoxPDF);
	
  mafDEL(IsaComp); 

  mafEventMacro(mafEvent(this, VME_REMOVE, BoxGizmo));  
  vtkDEL(Cube);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // create pipeline for XMIN box gizmo 
  // calculate diagonal of InputVme space bounds 
  double b[6];
  InputVme->GetOutput()->GetBounds(b);
  
  vector<double> dim;
  dim.push_back(b[1] - b[0]);
  dim.push_back(b[3] - b[2]);
  dim.push_back(b[5] - b[4]);

  vector<double>::iterator result;
  result = min_element(dim.begin(), dim.end());

  double min_dim = *result;
  cubeSize = min_dim / 8;

  // create box
  Cube = vtkCubeSource::New();
  Cube->SetXLength(cubeSize / 2);
  Cube->SetYLength(cubeSize);
  Cube->SetZLength(cubeSize);

  // create box translation transform
  TranslateBoxTr = vtkTransform::New();

  // create box translation transform pdf
  TranslateBoxPDF = vtkTransformPolyDataFilter::New();
  TranslateBoxPDF->SetInput(Cube->GetOutput());
  TranslateBoxPDF->SetTransform(TranslateBoxTr);

  // box rotation transform
  RotateBoxTr = vtkTransform::New();
  
  // create rotation transform and rotation TPDF 
  RotateBoxPDF = vtkTransformPolyDataFilter::New();
  RotateBoxPDF->SetInput(TranslateBoxPDF->GetOutput());
  RotateBoxPDF->SetTransform(RotateBoxTr);

	TranslateBoxTrEnd = vtkTransform::New();

	TranslateBoxPDFEnd = vtkTransformPolyDataFilter::New();
	TranslateBoxPDFEnd->SetInput(RotateBoxPDF->GetOutput());
	TranslateBoxPDFEnd->SetTransform(TranslateBoxTrEnd);

  SetLength(cubeSize);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetType(int type) 
//----------------------------------------------------------------------------
{
  assert(IsaGen && RotateBoxTr && BoxGizmo);
  
  // register the type
  GizmoType = type;
  Update();
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetLength(double length)
//----------------------------------------------------------------------------
{
  // set the length of the box
  Cube->SetXLength(length / 2);
  Cube->SetYLength(length);
  Cube->SetZLength(length);
  
  // translate box to match its right side with world y axis
  TranslateBoxTr->Identity();
  TranslateBoxTr->Translate(- length / 4, 0,0);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  double hl = highlight ? 1 : 0;
  this->SetColor(1, hl, 0);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // forward events to the listener
	maf_event->SetSender(this);
  mafEventMacro(*maf_event);
}

/** Gizmo color */
//----------------------------------------------------------------------------
void mafGizmoHandle::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  BoxGizmo->GetMaterial()->m_Prop->SetColor(col);
	BoxGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	BoxGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	BoxGizmo->GetMaterial()->m_Prop->SetSpecular(0);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::Show(bool show)
//----------------------------------------------------------------------------
{
  // use VTK opacity instead of VME_SHOW to speed up the render
  double opacity = show ? 1 : 0;
  BoxGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetConstrainRefSys(mafMatrix *constrain)
//----------------------------------------------------------------------------
{  
  IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(constrain);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  BoxGizmo->SetAbsMatrix(*absPose);
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoHandle::GetAbsPose()
//----------------------------------------------------------------------------
{
  return BoxGizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetPose(mafMatrix *pose)
//----------------------------------------------------------------------------
{
  BoxGizmo->SetMatrix(*pose);
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoHandle::GetPose()
//----------------------------------------------------------------------------
{
  return BoxGizmo->GetOutput()->GetMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{ 
  this->InputVme = vme; 
  SetType(GizmoType); 
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetBBCenters(double bounds[6])
//----------------------------------------------------------------------------
{
    // calculate the center of each face
  
/**
        5                   5
    _________           _________  _zmax
   |         |         |         |
   |         |         |         |
  0|    2    |1       3|    1    |2 
   |         |         |         |   
   |_________|         |_________| _zmin 
        4                   4
   |         |         |         |
  xmin      xmax      ymin     ymax

  XZ                  YZ

  -----------------------------------------

        3
    _________  _ymax
   |         |
   |         |
  0|    5    |1
   |         |
   |_________| _ymin
        2
   |         |
  xmin      xmax
   
  XY

*/

  
  // fill BBCenters ivar
  double xmin = bounds[0];
  double xmax = bounds[1];
  double xmed = (xmin + xmax) / 2;
  
  double ymin = bounds[2];
  double ymax = bounds[3];
  double ymed = (ymin + ymax) / 2;

  double zmin = bounds[4];
  double zmax = bounds[5];
  double zmed = (zmin + zmax) / 2;

  BBCenters[0][0] = xmin;
  BBCenters[0][1] = ymed;
  BBCenters[0][2] = zmed;

  BBCenters[1][0] = xmax;
  BBCenters[1][1] = ymed;
  BBCenters[1][2] = zmed;

  BBCenters[2][0] = xmed;
  BBCenters[2][1] = ymin;
  BBCenters[2][2] = zmed;

  BBCenters[3][0] = xmed;
  BBCenters[3][1] = ymax;
  BBCenters[3][2] = zmed;

  BBCenters[4][0] = xmed;
  BBCenters[4][1] = ymed;
  BBCenters[4][2] = zmin;

  BBCenters[5][0] = xmed;
  BBCenters[5][1] = ymed,
  BBCenters[5][2] = zmax;

  Update();
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetTranslationIntervals(double bounds[6])
//----------------------------------------------------------------------------
{
  // fill TranslationIntervals ivar
  
	TranslationIntervals[0][0] = 0.0;
	TranslationIntervals[0][1] = bounds[1]-bounds[0];

	TranslationIntervals[1][0] = -(bounds[1]-bounds[0]);
	TranslationIntervals[1][1] = 0.0;

	TranslationIntervals[2][0] = 0.0;
	TranslationIntervals[2][1] = bounds[3]-bounds[2];

	TranslationIntervals[3][0] = -(bounds[3]-bounds[2]);
	TranslationIntervals[3][1] = 0.0;

	TranslationIntervals[4][0] = 0.0;
	TranslationIntervals[4][1] = bounds[5]-bounds[4];

	TranslationIntervals[5][0] = -(bounds[5]-bounds[4]);
	TranslationIntervals[5][1] = 0.0;

  Update();
}

//----------------------------------------------------------------------------
void mafGizmoHandle::Update()
//----------------------------------------------------------------------------
{
  // reset the rotation transform
  RotateBoxTr->Identity();
  double rot[3] = {0,0,0};

  switch(GizmoType) 
  {
    case XMIN:
    {
			// place the gizmo
			if(m_ConstraintModality==BOUNDS)
			{
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::BOUNDS, vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK);
				IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::X, TranslationIntervals[0]);
			}
			else if(m_ConstraintModality==FREE)
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::FREE, vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK);
			
			TranslateBoxTrEnd->Identity();
			TranslateBoxTrEnd->Translate(BBCenters[0]);
      mafTransform::SetPosition(PivotMatrix, BBCenters[0]);
    }
    break;
    
    case XMAX:
    {
      RotateBoxTr->RotateZ(180);

			if(m_ConstraintModality==BOUNDS)
			{
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::BOUNDS, vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK);
				IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::X, TranslationIntervals[1]);
			}
			else if(m_ConstraintModality==FREE)
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::FREE, vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK);

			TranslateBoxTrEnd->Identity();
			TranslateBoxTrEnd->Translate(BBCenters[1]);
      mafTransform::SetPosition(PivotMatrix, BBCenters[1]);
    }
    break;
    
    case YMIN:
    {
      RotateBoxTr->RotateZ(90);

			if(m_ConstraintModality==BOUNDS)
			{
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::BOUNDS, vtkDOFMatrix::LOCK);
				IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::Y, TranslationIntervals[2]);
			}
			else if(m_ConstraintModality==FREE)
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::FREE, vtkDOFMatrix::LOCK);

			TranslateBoxTrEnd->Identity();
			TranslateBoxTrEnd->Translate(BBCenters[2]);
      mafTransform::SetPosition(PivotMatrix, BBCenters[2]);
    }
    break;
    
    case YMAX:
    {
      RotateBoxTr->RotateZ(-90);

			if(m_ConstraintModality==BOUNDS)
			{
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::BOUNDS, vtkDOFMatrix::LOCK);
				IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::Y, TranslationIntervals[3]);
			}
			else if(m_ConstraintModality==FREE)
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::FREE, vtkDOFMatrix::LOCK);
			
			TranslateBoxTrEnd->Identity();
			TranslateBoxTrEnd->Translate(BBCenters[3]);
      mafTransform::SetPosition(PivotMatrix, BBCenters[3]);
    }
    break;
    
    case ZMIN:
    {
      RotateBoxTr->RotateY(-90);

			if(m_ConstraintModality==BOUNDS)
			{
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK, vtkDOFMatrix::BOUNDS);
				IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::Z, TranslationIntervals[4]);
			}
			else if(m_ConstraintModality==FREE)
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK, vtkDOFMatrix::FREE);

			TranslateBoxTrEnd->Identity();
			TranslateBoxTrEnd->Translate(BBCenters[4]);
      mafTransform::SetPosition(PivotMatrix, BBCenters[4]);
    }
    break;
    
    case ZMAX:
    {
      RotateBoxTr->RotateY(90);

			if(m_ConstraintModality==BOUNDS)
			{
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK, vtkDOFMatrix::BOUNDS);
				IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::Z, TranslationIntervals[5]);
			}
			else if(m_ConstraintModality==FREE)
				IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK, vtkDOFMatrix::FREE);

			TranslateBoxTrEnd->Identity();
			TranslateBoxTrEnd->Translate(BBCenters[5]);
      mafTransform::SetPosition(PivotMatrix, BBCenters[5]);
    }
    break;

    default:
    {
    }
  }
}
//----------------------------------------------------------------------------
void mafGizmoHandle::GetHandleCenter(int type,double HandleCenter[6])
//----------------------------------------------------------------------------
{
	for(int i=0;i<3;i++)
		HandleCenter[i]=BBCenters[type][i];
}
