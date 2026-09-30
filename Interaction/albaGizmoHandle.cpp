/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoHandle
 Authors: Stefano Perticoni
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "albaDefines.h"
//#include "vtkMemDbg.h" 
//----------------------------------------------------------------------------
// NOTE: Every CPP file in the ALBA must include "albaDefines.h" as first.
// This force to include Window,wxWidgets and VTK exactly in this order.
// Failing in doing this will result in a run-time error saying:
// "Failure#0: The value of ESP was not properly saved across a function call"
//----------------------------------------------------------------------------


#include "albaGizmoHandle.h"
#include "albaDecl.h"
#include "mmaMaterial.h"
#include "albaRefSys.h"

// isa stuff
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericInterface.h"

// vme stuff
#include "albaVME.h"
#include "albaVMEGizmo.h"
#include "albaVMEOutput.h"
#include "albaTransform.h"

// vtk stuff
#include "vtkProperty.h"
#include "vtkCubeSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkMatrix4x4.h"
#include "vtkALBADOFMatrix.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"

#include <vector>
#include <algorithm>

using namespace std;

//----------------------------------------------------------------------------
albaGizmoHandle::albaGizmoHandle(albaVME *input, albaObserver *listener /* = NULL */, int constraintModality/* =BOUNDS */,albaVME *parent/* =NULL */, bool showShadingPlane /* = true */)
//----------------------------------------------------------------------------
{
	m_ConstraintModality = constraintModality;
  m_IsaComp = NULL;
  m_Cube = NULL;
  
  m_ShowShadingPlane = showShadingPlane;

	for(int i=0;i<6;i++)
		for(int j=0;j<3;j++)
			m_BBCenters[i][j]=0;

  m_Listener = listener;
  assert(input != NULL);
  m_InputVme = input;

  ////-----------------
  // create pipeline stuff
  ////-----------------
  CreatePipeline();

  //-----------------
  // create vme gizmos stuff
  //-----------------
  // plane
  m_ShadingPlaneGizmo = albaVMEGizmo::New();
  m_ShadingPlaneGizmo->SetName("ShadingPlaneGizmo");
  // cone gizmo
  m_BoxGizmo = albaVMEGizmo::New();  
  m_BoxGizmo->SetName("BoxGizmo");
  
  // since i'm working in local mode i reparent to input vme the gizmo
  m_BoxGizmo->SetDataConnection(m_TranslateBoxPolyDataFilterEnd->GetOutputPort());
	if(parent)
		m_BoxGizmo->ReparentTo(parent);
	else
		m_BoxGizmo->ReparentTo(m_InputVme);

  m_ShadingPlaneGizmo->SetDataConnection(m_TranslateShadingPlanePolyDataFilterEnd->GetOutputPort());
  if(parent)
    m_ShadingPlaneGizmo->ReparentTo(parent);
  else
    m_ShadingPlaneGizmo->ReparentTo(m_InputVme);

  mmaMaterial *material = m_ShadingPlaneGizmo->GetMaterial();
  material->m_Prop->SetOpacity(0.5);
  material->m_Opacity = material->m_Prop->GetOpacity();
  material->m_Prop->SetColor(0.2,0.2,0.8);
  material->m_Prop->GetDiffuseColor(material->m_Diffuse);

  // set come gizmo material property and initial color to red
  this->SetColor(1, 0, 0);

  // hide gizmo at creation
  this->Show(false);

  // ask the manager to create the pipeline
  GetLogicManager()->VmeShow(m_BoxGizmo, true);

  GetLogicManager()->VmeShow(m_ShadingPlaneGizmo, m_ShowShadingPlane);
  
  //-----------------
  // create isa stuff
  //-----------------
  // create isa compositor and assign behaviors to m_IsaGen ivar
  m_IsaComp  = albaInteractorCompositorMouse::New();

  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  m_IsaGen = m_IsaComp->CreateBehavior(MOUSE_LEFT);

  //isa will send events to this
	if(m_ConstraintModality==BOUNDS)
	{
		m_IsaGen->SetListener(this);  
		m_IsaGen->SetVME(m_BoxGizmo);
		m_IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToGlobal();
		m_IsaGen->EnableTranslation(true);
	}
	else if(m_ConstraintModality==FREE)
	{
		m_IsaGen->SetListener(this);  
		m_IsaGen->SetVME(m_BoxGizmo);
		m_IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal();
		m_IsaGen->EnableTranslation(true);
	}

  // assign isa to cylinder and cone
  m_BoxGizmo->SetBehavior(m_IsaComp);

   // default axis is X
  SetType(albaGizmoHandle::XMIN);

  m_InputVme->GetOutput()->Update();
  double b[6];
  m_InputVme->GetOutput()->GetVTKData()->GetBounds(b);
  SetBBCenters(b);
	if(m_ConstraintModality==BOUNDS)
		SetTranslationIntervals(b);
}
//----------------------------------------------------------------------------
albaGizmoHandle::~albaGizmoHandle() 
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetBehavior(NULL);

  vtkDEL(m_TranslateBoxTr);
  vtkDEL(m_TranslateBoxPolyDataFilter);
  vtkDEL(m_RotateBoxTr);
  vtkDEL(m_RotateBoxPolyDataFilter);
  vtkDEL(m_TranslateBoxTrEnd);				//BES: 2.2.2008 - memory leaks bug fix
  vtkDEL(m_TranslateBoxPolyDataFilterEnd);	//BES: 2.2.2008 - memory leaks bug fix

  vtkDEL(m_TranslateShadingPlaneTr);
  vtkDEL(m_TranslateShadingPlanePolyDataFilter);
  vtkDEL(m_RotateShadingPlaneTr);
  vtkDEL(m_RotateShadingPlanePolyDataFilter);
  vtkDEL(m_TranslateShadingPlaneTrEnd);
  vtkDEL(m_TranslateShadingPlanePolyDataFilterEnd);

  vtkDEL(m_PlaneSource);
	
  albaDEL(m_IsaComp);	//m_IsaGen is released automatically

  GetLogicManager()->VmeRemove(m_BoxGizmo); //m_BoxGizmo is released
  GetLogicManager()->VmeRemove(m_ShadingPlaneGizmo); //m_ShadingPlaneGizmo is released

  vtkDEL(m_Cube);
}

//----------------------------------------------------------------------------
void albaGizmoHandle::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // create pipeline for XMIN box gizmo 
  // calculate diagonal of m_InputVme space bounds 
  double b[6];
  m_InputVme->GetOutput()->GetBounds(b);
  
  vector<double> dim;
  dim.push_back(b[1] - b[0]);
  dim.push_back(b[3] - b[2]);
  dim.push_back(b[5] - b[4]);

  vector<double>::iterator result;
  result = min_element(dim.begin(), dim.end());

  double min_dim = *result;
  m_CubeSize = min_dim / 8;

  // create box
  m_Cube = vtkCubeSource::New();
  m_Cube->SetXLength(m_CubeSize / 2);
  m_Cube->SetYLength(m_CubeSize);
  m_Cube->SetZLength(m_CubeSize);

  // create box translation transform
  m_TranslateBoxTr = vtkTransform::New();

  // create box translation transform PolyDataFilter
  m_TranslateBoxPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_TranslateBoxPolyDataFilter->SetInputConnection(m_Cube->GetOutputPort());
  m_TranslateBoxPolyDataFilter->SetTransform(m_TranslateBoxTr);

  // box rotation transform
  m_RotateBoxTr = vtkTransform::New();
  
  // create rotation transform and rotation TPolyDataFilter 
  m_RotateBoxPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_RotateBoxPolyDataFilter->SetInputConnection(m_TranslateBoxPolyDataFilter->GetOutputPort());
  m_RotateBoxPolyDataFilter->SetTransform(m_RotateBoxTr);

	m_TranslateBoxTrEnd = vtkTransform::New();

	m_TranslateBoxPolyDataFilterEnd = vtkTransformPolyDataFilter::New();
	m_TranslateBoxPolyDataFilterEnd->SetInputConnection(m_RotateBoxPolyDataFilter->GetOutputPort());
	m_TranslateBoxPolyDataFilterEnd->SetTransform(m_TranslateBoxTrEnd);

  SetLength(m_CubeSize);

  // create the gizmo plane on the z = 0 plane
  m_ShadingPlaneDimension[0]=dim[0];
  m_ShadingPlaneDimension[1]=dim[1];
  m_ShadingPlaneDimension[2]=dim[2];
  vtkNEW(m_PlaneSource);
  m_PlaneSource->SetPoint1(m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[1]/2, 0);
  m_PlaneSource->SetPoint2(-m_ShadingPlaneDimension[0]/2, m_ShadingPlaneDimension[1]/2, 0);
  m_PlaneSource->SetOrigin(-m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[1]/2, 0);
  m_PlaneSource->Update();

  m_TranslateShadingPlaneTr = vtkTransform::New();
  m_TranslateShadingPlanePolyDataFilter = vtkTransformPolyDataFilter::New();
  m_TranslateShadingPlanePolyDataFilter->SetInputConnection(m_PlaneSource->GetOutputPort());
  m_TranslateShadingPlanePolyDataFilter->SetTransform(m_TranslateShadingPlaneTr);

  m_RotateShadingPlaneTr = vtkTransform::New();
  m_RotateShadingPlanePolyDataFilter = vtkTransformPolyDataFilter::New();
  m_RotateShadingPlanePolyDataFilter->SetInputConnection(m_TranslateShadingPlanePolyDataFilter->GetOutputPort());
  m_RotateShadingPlanePolyDataFilter->SetTransform(m_RotateShadingPlaneTr);

  m_TranslateShadingPlaneTrEnd = vtkTransform::New();
  m_TranslateShadingPlanePolyDataFilterEnd = vtkTransformPolyDataFilter::New();
  m_TranslateShadingPlanePolyDataFilterEnd->SetInputConnection(m_RotateShadingPlanePolyDataFilter->GetOutputPort());
  m_TranslateShadingPlanePolyDataFilterEnd->SetTransform(m_TranslateShadingPlaneTrEnd);
}

//----------------------------------------------------------------------------
void albaGizmoHandle::SetType(int type) 
//----------------------------------------------------------------------------
{
  assert(m_IsaGen && m_RotateBoxTr && m_BoxGizmo);
  
  // register the type
  m_GizmoType = type;
  Update();
}

//----------------------------------------------------------------------------
void albaGizmoHandle::SetLength(double length)
//----------------------------------------------------------------------------
{
  // set the length of the box
  m_Cube->SetXLength(length / 2);
  m_Cube->SetYLength(length);
  m_Cube->SetZLength(length);
  
  // translate box to match its right side with world y axis
  m_TranslateBoxTr->Identity();
  m_TranslateBoxTr->Translate(- length / 4, 0,0);
}

double albaGizmoHandle::GetLength()
{
  return m_Cube->GetZLength();
}

//----------------------------------------------------------------------------
void albaGizmoHandle::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  double hl = highlight ? 1 : 0;
  this->SetColor(1, hl, 0);
}

//----------------------------------------------------------------------------
void albaGizmoHandle::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  // forward events to the listener
	alba_event->SetSender(this);
  albaEventMacro(*alba_event);
}

/** Gizmo color */
//----------------------------------------------------------------------------
void albaGizmoHandle::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_Color[0] = col[0];
  m_Color[1] = col[1];
  m_Color[2] = col[2];

  m_BoxGizmo->GetMaterial()->m_Prop->SetColor(col);
	m_BoxGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	m_BoxGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	m_BoxGizmo->GetMaterial()->m_Prop->SetSpecular(0);
}

//----------------------------------------------------------------------------
void albaGizmoHandle::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}

//----------------------------------------------------------------------------
void albaGizmoHandle::Show(bool show)
//----------------------------------------------------------------------------
{
  // use VTK opacity instead of vme show to speed up the render
  double opacity = show ? 1 : 0;
  m_BoxGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
  if(m_ShowShadingPlane)
  {
    m_ShadingPlaneGizmo->GetMaterial()->m_Prop->SetOpacity(opacity*0.5);
  }
  m_Show = show;
}
//----------------------------------------------------------------------------
void albaGizmoHandle::ShowShadingPlane(bool show)
//----------------------------------------------------------------------------
{
  m_ShowShadingPlane = show;
  if(m_ShowShadingPlane)
  {
    double opacity = show ? 1 : 0;
    m_ShadingPlaneGizmo->GetMaterial()->m_Prop->SetOpacity(opacity*0.5);
  }
}
//----------------------------------------------------------------------------
void albaGizmoHandle::SetConstrainRefSys(albaMatrix *constrain)
//----------------------------------------------------------------------------
{  
  m_IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(constrain);
}


albaMatrix * albaGizmoHandle::GetConstrainRefSys()
{
  return m_IsaGen->GetTranslationConstraint()->GetRefSys()->GetMatrix();
}

//----------------------------------------------------------------------------
void albaGizmoHandle::SetAbsPose(albaMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetAbsMatrix(*absPose);
}

//----------------------------------------------------------------------------
albaMatrix *albaGizmoHandle::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_BoxGizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void albaGizmoHandle::SetPose(albaMatrix *pose)
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetMatrix(*pose);
  m_ShadingPlaneGizmo->SetMatrix(*pose);
}

//----------------------------------------------------------------------------
albaMatrix *albaGizmoHandle::GetPose()
//----------------------------------------------------------------------------
{
  return m_BoxGizmo->GetOutput()->GetMatrix();
}

//----------------------------------------------------------------------------
void albaGizmoHandle::SetInput(albaVME *vme)
//----------------------------------------------------------------------------
{ 
  this->m_InputVme = vme; 
  SetType(m_GizmoType); 
}
//----------------------------------------------------------------------------
void albaGizmoHandle::UpdateShadingPlaneDimension(double b[6])
//----------------------------------------------------------------------------
{
  m_ShadingPlaneDimension[0]=b[1]-b[0];
  m_ShadingPlaneDimension[1]=b[3]-b[2];
  m_ShadingPlaneDimension[2]=b[5]-b[4];
  
  switch(m_GizmoType) 
  {
  case XMIN:
  case XMAX:
    {
      m_PlaneSource->SetPoint1(m_ShadingPlaneDimension[2]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetPoint2(-m_ShadingPlaneDimension[2]/2, m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetOrigin(-m_ShadingPlaneDimension[2]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->Update();
    }
    break;

  case YMIN:
  case YMAX:
    {
      m_PlaneSource->SetPoint1(m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[2]/2, 0);
      m_PlaneSource->SetPoint2(-m_ShadingPlaneDimension[0]/2, m_ShadingPlaneDimension[2]/2, 0);
      m_PlaneSource->SetOrigin(-m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[2]/2, 0);
      m_PlaneSource->Update();
    }
    break;

  case ZMIN:
  case ZMAX:
    {
      m_PlaneSource->SetPoint1(m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetPoint2(-m_ShadingPlaneDimension[0]/2, m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetOrigin(-m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->Update();
    }
    break;
  }
}
//----------------------------------------------------------------------------
void albaGizmoHandle::SetBBCenters(double bounds[6])
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 6; i++) 
  { 
    m_Bounds[i] = bounds[i];
  }

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
  
  
  // fill m_BBCenters ivar
  double xmin = bounds[0];
  double xmax = bounds[1];
  double xmed = (xmin + xmax) / 2;
  
  double ymin = bounds[2];
  double ymax = bounds[3];
  double ymed = (ymin + ymax) / 2;

  double zmin = bounds[4];
  double zmax = bounds[5];
  double zmed = (zmin + zmax) / 2;

  m_BBCenters[0][0] = xmin;
  m_BBCenters[0][1] = ymed;
  m_BBCenters[0][2] = zmed;

  m_BBCenters[1][0] = xmax;
  m_BBCenters[1][1] = ymed;
  m_BBCenters[1][2] = zmed;

  m_BBCenters[2][0] = xmed;
  m_BBCenters[2][1] = ymin;
  m_BBCenters[2][2] = zmed;

  m_BBCenters[3][0] = xmed;
  m_BBCenters[3][1] = ymax;
  m_BBCenters[3][2] = zmed;

  m_BBCenters[4][0] = xmed;
  m_BBCenters[4][1] = ymed;
  m_BBCenters[4][2] = zmin;

  m_BBCenters[5][0] = xmed;
  m_BBCenters[5][1] = ymed,
  m_BBCenters[5][2] = zmax;

  Update();
}

//----------------------------------------------------------------------------
void albaGizmoHandle::SetTranslationIntervals(double bounds[6])
//----------------------------------------------------------------------------
{
  // fill m_TranslationIntervals ivar
  
	m_TranslationIntervals[0][0] = 0.0;
	m_TranslationIntervals[0][1] = bounds[1]-bounds[0];

	m_TranslationIntervals[1][0] = -(bounds[1]-bounds[0]);
	m_TranslationIntervals[1][1] = 0.0;

	m_TranslationIntervals[2][0] = 0.0;
	m_TranslationIntervals[2][1] = bounds[3]-bounds[2];

	m_TranslationIntervals[3][0] = -(bounds[3]-bounds[2]);
	m_TranslationIntervals[3][1] = 0.0;

	m_TranslationIntervals[4][0] = 0.0;
	m_TranslationIntervals[4][1] = bounds[5]-bounds[4];

	m_TranslationIntervals[5][0] = -(bounds[5]-bounds[4]);
	m_TranslationIntervals[5][1] = 0.0;

  Update();
}

//----------------------------------------------------------------------------
void albaGizmoHandle::Update()
//----------------------------------------------------------------------------
{
  // reset the rotation transform
  m_RotateBoxTr->Identity();
  m_RotateShadingPlaneTr->Identity();
  double rot[3] = {0,0,0};
  
	albaMatrix *matIdentity;
	albaNEW(matIdentity);
	matIdentity->Identity();
	SetPose(matIdentity);
	albaDEL(matIdentity);	//BES: 3.2.3008 - memory leaks bug fix

  switch(m_GizmoType) 
  {
    case XMIN:
    {
      m_PlaneSource->SetPoint1(m_ShadingPlaneDimension[2]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetPoint2(-m_ShadingPlaneDimension[2]/2, m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetOrigin(-m_ShadingPlaneDimension[2]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->Update();

      m_RotateShadingPlaneTr->RotateY(90);

			// place the gizmo
			if(m_ConstraintModality==BOUNDS)
			{
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::BOUNDS, vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::LOCK);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkALBADOFMatrix::X, m_TranslationIntervals[0]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::FREE, vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::LOCK);

			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[0]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[0]);

      albaTransform::SetPosition(m_PivotMatrix, m_BBCenters[0]);
    }
    break;
    
    case XMAX:
    {
      m_PlaneSource->SetPoint1(m_ShadingPlaneDimension[2]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetPoint2(-m_ShadingPlaneDimension[2]/2, m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetOrigin(-m_ShadingPlaneDimension[2]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->Update();

      m_RotateShadingPlaneTr->RotateY(90);

      m_RotateBoxTr->RotateZ(180);

			if(m_ConstraintModality==BOUNDS)
			{
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::BOUNDS, vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::LOCK);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkALBADOFMatrix::X, m_TranslationIntervals[1]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::FREE, vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::LOCK);

			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[1]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[1]);

      albaTransform::SetPosition(m_PivotMatrix, m_BBCenters[1]);
    }
    break;
    
    case YMIN:
    {
      m_PlaneSource->SetPoint1(m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[2]/2, 0);
      m_PlaneSource->SetPoint2(-m_ShadingPlaneDimension[0]/2, m_ShadingPlaneDimension[2]/2, 0);
      m_PlaneSource->SetOrigin(-m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[2]/2, 0);
      m_PlaneSource->Update();

      m_RotateShadingPlaneTr->RotateX(90);

      m_RotateBoxTr->RotateZ(90);

			if(m_ConstraintModality==BOUNDS)
			{
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::BOUNDS, vtkALBADOFMatrix::LOCK);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkALBADOFMatrix::Y, m_TranslationIntervals[2]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::FREE, vtkALBADOFMatrix::LOCK);

			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[2]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[2]);

      albaTransform::SetPosition(m_PivotMatrix, m_BBCenters[2]);
    }
    break;
    
    case YMAX:
    {
      m_PlaneSource->SetPoint1(m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[2]/2, 0);
      m_PlaneSource->SetPoint2(-m_ShadingPlaneDimension[0]/2, m_ShadingPlaneDimension[2]/2, 0);
      m_PlaneSource->SetOrigin(-m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[2]/2, 0);
      m_PlaneSource->Update();

      m_RotateShadingPlaneTr->RotateX(90);

      m_RotateBoxTr->RotateZ(-90);

			if(m_ConstraintModality==BOUNDS)
			{
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::BOUNDS, vtkALBADOFMatrix::LOCK);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkALBADOFMatrix::Y, m_TranslationIntervals[3]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::FREE, vtkALBADOFMatrix::LOCK);
			
			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[3]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[3]);

      albaTransform::SetPosition(m_PivotMatrix, m_BBCenters[3]);
    }
    break;
    
    case ZMIN:
    {
      m_PlaneSource->SetPoint1(m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetPoint2(-m_ShadingPlaneDimension[0]/2, m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetOrigin(-m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->Update();

      //m_RotateShadingPlaneTr->RotateZ(90);

      m_RotateBoxTr->RotateY(-90);

			if(m_ConstraintModality==BOUNDS)
			{
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::BOUNDS);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkALBADOFMatrix::Z, m_TranslationIntervals[4]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::FREE);

			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[4]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[4]);

      albaTransform::SetPosition(m_PivotMatrix, m_BBCenters[4]);
    }
    break;
    
    case ZMAX:
    {
      m_PlaneSource->SetPoint1(m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetPoint2(-m_ShadingPlaneDimension[0]/2, m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->SetOrigin(-m_ShadingPlaneDimension[0]/2,-m_ShadingPlaneDimension[1]/2, 0);
      m_PlaneSource->Update();

      m_RotateBoxTr->RotateY(90);

			if(m_ConstraintModality==BOUNDS)
			{
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::BOUNDS);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkALBADOFMatrix::Z, m_TranslationIntervals[5]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::LOCK, vtkALBADOFMatrix::FREE);

			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[5]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[5]);

      albaTransform::SetPosition(m_PivotMatrix, m_BBCenters[5]);
    }
    break;

    default:
    {
    }
  }
}
//----------------------------------------------------------------------------
void albaGizmoHandle::GetHandleCenter(int type,double HandleCenter[3])
//----------------------------------------------------------------------------
{
	for(int i=0;i<3;i++)
		HandleCenter[i]=m_BBCenters[type][i];
}

void albaGizmoHandle::GetColor( double color[3] )
{
  color[0] = m_Color[0];
  color[1] = m_Color[1];
  color[2] = m_Color[2];
}


void albaGizmoHandle::SetBounds( double bounds[6] )
{
  this->SetBBCenters(bounds);
}

void albaGizmoHandle::GetBounds( double bounds[6] )
{
  for (int i = 0; i < 6; i++) 
  { 
    bounds[i] = m_Bounds[i];
  }

}
