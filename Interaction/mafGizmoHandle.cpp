/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoHandle.cpp,v $
  Language:  C++
  Date:      $Date: 2008-08-26 08:36:53 $
  Version:   $Revision: 1.12 $
  Authors:   Stefano Perticoni
==========================================================================
  Copyright (c) 2002/2004
  CINECA - Interuniversity Consortium (www.cineca.it) 
=========================================================================*/


#include "mafDefines.h"
//#include "vtkMemDbg.h" 
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
#include "vtkMAFDOFMatrix.h"
#include "vtkPlane.h"
#include "vtkPlaneSource.h"

#include <vector>
#include <algorithm>

using namespace std;

//----------------------------------------------------------------------------
mafGizmoHandle::mafGizmoHandle(mafVME *input, mafObserver *listener /* = NULL */, int constraintModality/* =BOUNDS */,mafVME *parent/* =NULL */, bool showShadingPlane /* = true */)
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
  m_InputVme = input;

  ////-----------------
  // create pipeline stuff
  ////-----------------
  CreatePipeline();

  //-----------------
  // create vme gizmos stuff
  //-----------------
  // plane
  m_ShadingPlaneGizmo = mafVMEGizmo::New();
  m_ShadingPlaneGizmo->SetName("ShadingPlaneGizmo");
  // cone gizmo
  m_BoxGizmo = mafVMEGizmo::New();  
  m_BoxGizmo->SetName("BoxGizmo");
  
  // since i'm working in local mode i reparent to input vme the gizmo
  m_BoxGizmo->SetData(m_TranslateBoxPolyDataFilterEnd->GetOutput());
	if(parent)
		m_BoxGizmo->ReparentTo(parent);
	else
		m_BoxGizmo->ReparentTo(m_InputVme);

  m_ShadingPlaneGizmo->SetData(m_TranslateShadingPlanePolyDataFilterEnd->GetOutput());
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
  mafEventMacro(mafEvent(this,VME_SHOW,m_BoxGizmo,true));

  mafEventMacro(mafEvent(this,VME_SHOW,m_ShadingPlaneGizmo,m_ShowShadingPlane));
  
  //-----------------
  // create isa stuff
  //-----------------
  // create isa compositor and assign behaviors to m_IsaGen ivar
  m_IsaComp  = mmiCompositorMouse::New();

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
  SetType(mafGizmoHandle::XMIN);

  m_InputVme->GetOutput()->Update();
  double b[6];
  m_InputVme->GetOutput()->GetVTKData()->GetBounds(b);
  SetBBCenters(b);
	if(m_ConstraintModality==BOUNDS)
		SetTranslationIntervals(b);
}
//----------------------------------------------------------------------------
mafGizmoHandle::~mafGizmoHandle() 
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
	
  mafDEL(m_IsaComp);	//m_IsaGen is released automatically

  mafEventMacro(mafEvent(this, VME_REMOVE, m_BoxGizmo)); //m_BoxGizmo is released
  mafEventMacro(mafEvent(this, VME_REMOVE, m_ShadingPlaneGizmo)); //m_ShadingPlaneGizmo is released

  vtkDEL(m_Cube);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::CreatePipeline() 
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
  m_TranslateBoxPolyDataFilter->SetInput(m_Cube->GetOutput());
  m_TranslateBoxPolyDataFilter->SetTransform(m_TranslateBoxTr);

  // box rotation transform
  m_RotateBoxTr = vtkTransform::New();
  
  // create rotation transform and rotation TPolyDataFilter 
  m_RotateBoxPolyDataFilter = vtkTransformPolyDataFilter::New();
  m_RotateBoxPolyDataFilter->SetInput(m_TranslateBoxPolyDataFilter->GetOutput());
  m_RotateBoxPolyDataFilter->SetTransform(m_RotateBoxTr);

	m_TranslateBoxTrEnd = vtkTransform::New();

	m_TranslateBoxPolyDataFilterEnd = vtkTransformPolyDataFilter::New();
	m_TranslateBoxPolyDataFilterEnd->SetInput(m_RotateBoxPolyDataFilter->GetOutput());
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
  m_TranslateShadingPlanePolyDataFilter->SetInput(m_PlaneSource->GetOutput());
  m_TranslateShadingPlanePolyDataFilter->SetTransform(m_TranslateShadingPlaneTr);

  m_RotateShadingPlaneTr = vtkTransform::New();
  m_RotateShadingPlanePolyDataFilter = vtkTransformPolyDataFilter::New();
  m_RotateShadingPlanePolyDataFilter->SetInput(m_TranslateShadingPlanePolyDataFilter->GetOutput());
  m_RotateShadingPlanePolyDataFilter->SetTransform(m_RotateShadingPlaneTr);

  m_TranslateShadingPlaneTrEnd = vtkTransform::New();
  m_TranslateShadingPlanePolyDataFilterEnd = vtkTransformPolyDataFilter::New();
  m_TranslateShadingPlanePolyDataFilterEnd->SetInput(m_RotateShadingPlanePolyDataFilter->GetOutput());
  m_TranslateShadingPlanePolyDataFilterEnd->SetTransform(m_TranslateShadingPlaneTrEnd);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetType(int type) 
//----------------------------------------------------------------------------
{
  assert(m_IsaGen && m_RotateBoxTr && m_BoxGizmo);
  
  // register the type
  m_GizmoType = type;
  Update();
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetLength(double length)
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
  m_BoxGizmo->GetMaterial()->m_Prop->SetColor(col);
	m_BoxGizmo->GetMaterial()->m_Prop->SetAmbient(0);
	m_BoxGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	m_BoxGizmo->GetMaterial()->m_Prop->SetSpecular(0);
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
  m_BoxGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
  if(m_ShowShadingPlane)
  {
    m_ShadingPlaneGizmo->GetMaterial()->m_Prop->SetOpacity(opacity*0.5);
  }
}
//----------------------------------------------------------------------------
void mafGizmoHandle::ShowShadingPlane(bool show)
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
void mafGizmoHandle::SetConstrainRefSys(mafMatrix *constrain)
//----------------------------------------------------------------------------
{  
  m_IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(constrain);
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetAbsMatrix(*absPose);
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoHandle::GetAbsPose()
//----------------------------------------------------------------------------
{
  return m_BoxGizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetPose(mafMatrix *pose)
//----------------------------------------------------------------------------
{
  m_BoxGizmo->SetMatrix(*pose);
  m_ShadingPlaneGizmo->SetMatrix(*pose);
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoHandle::GetPose()
//----------------------------------------------------------------------------
{
  return m_BoxGizmo->GetOutput()->GetMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoHandle::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{ 
  this->m_InputVme = vme; 
  SetType(m_GizmoType); 
}
//----------------------------------------------------------------------------
void mafGizmoHandle::UpdateShadingPlaneDimension(double b[6])
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
void mafGizmoHandle::SetTranslationIntervals(double bounds[6])
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
void mafGizmoHandle::Update()
//----------------------------------------------------------------------------
{
  // reset the rotation transform
  m_RotateBoxTr->Identity();
  m_RotateShadingPlaneTr->Identity();
  double rot[3] = {0,0,0};
  
	mafMatrix *matIdentity;
	mafNEW(matIdentity);
	matIdentity->Identity();
	SetPose(matIdentity);
	mafDEL(matIdentity);	//BES: 3.2.3008 - memory leaks bug fix

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
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::BOUNDS, vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::LOCK);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkMAFDOFMatrix::X, m_TranslationIntervals[0]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::FREE, vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::LOCK);

			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[0]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[0]);

      mafTransform::SetPosition(m_PivotMatrix, m_BBCenters[0]);
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
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::BOUNDS, vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::LOCK);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkMAFDOFMatrix::X, m_TranslationIntervals[1]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::FREE, vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::LOCK);

			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[1]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[1]);

      mafTransform::SetPosition(m_PivotMatrix, m_BBCenters[1]);
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
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::BOUNDS, vtkMAFDOFMatrix::LOCK);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkMAFDOFMatrix::Y, m_TranslationIntervals[2]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::FREE, vtkMAFDOFMatrix::LOCK);

			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[2]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[2]);

      mafTransform::SetPosition(m_PivotMatrix, m_BBCenters[2]);
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
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::BOUNDS, vtkMAFDOFMatrix::LOCK);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkMAFDOFMatrix::Y, m_TranslationIntervals[3]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::FREE, vtkMAFDOFMatrix::LOCK);
			
			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[3]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[3]);

      mafTransform::SetPosition(m_PivotMatrix, m_BBCenters[3]);
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
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::BOUNDS);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkMAFDOFMatrix::Z, m_TranslationIntervals[4]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::FREE);

			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[4]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[4]);

      mafTransform::SetPosition(m_PivotMatrix, m_BBCenters[4]);
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
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::BOUNDS);
				m_IsaGen->GetTranslationConstraint()->SetBounds(vtkMAFDOFMatrix::Z, m_TranslationIntervals[5]);
			}
			else if(m_ConstraintModality==FREE)
				m_IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::LOCK, vtkMAFDOFMatrix::FREE);

			m_TranslateBoxTrEnd->Identity();
			m_TranslateBoxTrEnd->Translate(m_BBCenters[5]);

      m_TranslateShadingPlaneTrEnd->Identity();
      m_TranslateShadingPlaneTrEnd->Translate(m_BBCenters[5]);

      mafTransform::SetPosition(m_PivotMatrix, m_BBCenters[5]);
    }
    break;

    default:
    {
    }
  }
}
//----------------------------------------------------------------------------
void mafGizmoHandle::GetHandleCenter(int type,double HandleCenter[3])
//----------------------------------------------------------------------------
{
	for(int i=0;i<3;i++)
		HandleCenter[i]=m_BBCenters[type][i];
}
