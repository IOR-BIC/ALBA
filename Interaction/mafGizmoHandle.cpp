/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoHandle.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-06 13:50:23 $
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
mafGizmoHandle::mafGizmoHandle(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  IsaComp = NULL;
  Cube = NULL;

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
  BoxGizmo->ReparentTo(InputVme);
  BoxGizmo->SetData(RotateBoxPDF->GetOutput());
  
  // set come gizmo material property and initial color to red
  this->SetColor(1, 0, 0);

  // hide gizmo at creation
  this->Show(false);

  //-----------------
  // create isa stuff
  //-----------------
  // create isa compositor and assign behaviors to IsaGen ivar
  IsaComp  = mmiCompositorMouse::New();

  // default behavior is activated by mouse left and is constrained to X axis,
  // default ref sys is input vme abs matrix
  IsaGen = IsaComp->CreateBehavior(MOUSE_LEFT);

  //isa will send events to this
  IsaGen->SetListener(this);  
  IsaGen->SetVME(BoxGizmo);
  IsaGen->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal(InputVme);
  IsaGen->EnableTranslation(true);

  // assign isa to cylinder and cone
  BoxGizmo->SetBehavior(IsaComp);

   // default axis is X
  SetType(mafGizmoHandle::XMIN);

  InputVme->GetOutput()->Update();
  double b[6];
  InputVme->GetOutput()->GetBounds(b);
  SetBBCenters(b);
  SetTranslationIntervals(b);

  // add the gizmo to the tree
  mafEventMacro(mafEvent(this, VME_ADD, BoxGizmo));
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
	
  //----------------------
	// No leaks so somebody is performing this...
	// wxDEL(GizmoData );
	//----------------------
  vtkDEL(IsaComp); 

  mafEventMacro(mafEvent(this, VME_REMOVE, BoxGizmo));  
  BoxGizmo->Delete();

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
  double cubeSize = min_dim / 8;

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
  if (highlight == true)
  {
     // Highlight the box by setting its color to yellow 
     this->SetColor(1, 1, 0);
  } 
  else
  {   
     // set box col to red
     this->SetColor(1, 0, 0);
  }
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
  double opacity = ((show == TRUE) ? 1 : 0);
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
  TranslationIntervals[0][0] = bounds[0];
  TranslationIntervals[0][1] = bounds[1];
  
  TranslationIntervals[1][0] = bounds[2];
  TranslationIntervals[1][1] = bounds[3];
  
  TranslationIntervals[2][0] = bounds[4];
  TranslationIntervals[2][1] = bounds[5];
  
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
      // set the constrain
      IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::BOUNDS, vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK);
      IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::X, TranslationIntervals[0]);
      // place the gizmo
      BoxGizmo->SetPose(BBCenters[0], rot, InputVme->GetTimeStamp());   
      mafTransform::SetPosition(PivotMatrix, BBCenters[0]);
    }
    break;
    
    case XMAX:
    {
      RotateBoxTr->RotateZ(180);
      IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::BOUNDS, vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK);
      IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::X, TranslationIntervals[0]);
      BoxGizmo->SetPose(BBCenters[1], rot, InputVme->GetTimeStamp());
      mafTransform::SetPosition(PivotMatrix, BBCenters[1]);
    }
    break;
    
    case YMIN:
    {
      RotateBoxTr->RotateZ(90);
      IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::BOUNDS, vtkDOFMatrix::LOCK);
      IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::Y, TranslationIntervals[1]);
      BoxGizmo->SetPose(BBCenters[2], rot, InputVme->GetTimeStamp());
      mafTransform::SetPosition(PivotMatrix, BBCenters[2]);
    }
    break;
    
    case YMAX:
    {
      RotateBoxTr->RotateZ(-90);
      IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::BOUNDS, vtkDOFMatrix::LOCK);
      IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::Y, TranslationIntervals[1]);
      BoxGizmo->SetPose(BBCenters[3], rot, InputVme->GetTimeStamp());
      mafTransform::SetPosition(PivotMatrix, BBCenters[3]);
    }
    break;
    
    case ZMIN:
    {
      RotateBoxTr->RotateY(-90);
      IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK, vtkDOFMatrix::BOUNDS);
      IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::Z, TranslationIntervals[2]);
      BoxGizmo->SetPose(BBCenters[4], rot, InputVme->GetTimeStamp());
      mafTransform::SetPosition(PivotMatrix, BBCenters[4]);
    }
    break;
    
    case ZMAX:
    {
      RotateBoxTr->RotateY(90);
      IsaGen->GetTranslationConstraint()->SetConstraintModality(vtkDOFMatrix::LOCK, vtkDOFMatrix::LOCK, vtkDOFMatrix::BOUNDS);
      IsaGen->GetTranslationConstraint()->SetBounds(vtkDOFMatrix::Z, TranslationIntervals[2]);
      BoxGizmo->SetPose(BBCenters[5], rot, InputVme->GetTimeStamp());
      mafTransform::SetPosition(PivotMatrix, BBCenters[5]);
    }
    break;

    default:
    {
    }
  }
}
