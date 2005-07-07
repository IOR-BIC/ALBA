/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoScaleAxis.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-07 15:14:36 $
  Version:   $Revision: 1.2 $
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


#include "mafGizmoScaleAxis.h"
#include "mafDecl.h"

// isa stuff
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "mafMatrix.h"
#include "mafVME.h"
#include "mafVMEGizmo.h"
#include "mafVMEOutput.h"

// vtk stuff
#include "vtkCubeSource.h"
#include "vtkCylinderSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
mafGizmoScaleAxis::mafGizmoScaleAxis(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  IsaComp[0] = IsaComp[1] = NULL;

  m_Listener = listener;
  InputVme = input;
  
  // default axis is X
  Axis = X;
  
  // create pipeline stuff
  CreatePipeline();

  // create isa stuff
  CreateISA();

  //-----------------
  // create vme gizmos stuff
  //-----------------

  // cylinder gizmo
  CylGizmo = mafVMEGizmo::New();
  CylGizmo->SetName("CylGizmo");
  CylGizmo->ReparentTo(mafVME::SafeDownCast(InputVme->GetRoot()));
  CylGizmo->SetData(RotatePDF[CYLINDER]->GetOutput());
  
  // cube gizmo
  CubeGizmo = mafVMEGizmo::New();  
  CubeGizmo->SetName("CubeGizmo");
  CubeGizmo->ReparentTo(mafVME::SafeDownCast(InputVme->GetRoot()));
  CubeGizmo->SetData(RotatePDF[CUBE]->GetOutput());

  // assign isa to cylinder and cube
  CylGizmo->SetBehavior(IsaComp[0]);
  CubeGizmo->SetBehavior(IsaComp[1]);

  InputVme->Update();
  SetAbsPose(InputVme->GetOutput()->GetAbsMatrix());
  
  // set come gizmo material property and initial color to red
  this->SetColor(1, 0, 0, 1, 0, 0);

  // hide gizmos at creation
  this->Show(false);

  // add the gizmo to the tree, this should increase reference count 
  mafEventMacro(mafEvent(this, VME_ADD, CylGizmo));
  mafEventMacro(mafEvent(this, VME_ADD, CubeGizmo));
}
//----------------------------------------------------------------------------
mafGizmoScaleAxis::~mafGizmoScaleAxis() 
//----------------------------------------------------------------------------
{
  CylGizmo->SetBehavior(NULL);
  CubeGizmo->SetBehavior(NULL);
   
  vtkDEL(Cube);
  vtkDEL(Cylinder);
  
  // clean up
  for (int i = 0; i < 2; i++)
  {
    vtkDEL(TranslateTr[i]);
    vtkDEL(TranslatePDF[i]);
    vtkDEL(RotationTr);
    vtkDEL(RotatePDF[i]);
	//----------------------
	// No leaks so somebody is performing this...
	// wxDEL(GizmoData[i]);
	//----------------------
    vtkDEL(IsaComp[i]); 
  }
  mafEventMacro(mafEvent(this, VME_REMOVE, CylGizmo));
  mafEventMacro(mafEvent(this, VME_REMOVE, CubeGizmo));  

//  CylGizmo->Delete();
//  CubeGizmo->Delete();
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // create pipeline for cube-cylinder gizmo along global X axis
  // calculate diagonal of InputVme space bounds 
  double b[6],p1[3],p2[3],d;
  InputVme->GetOutput()->GetBounds(b);
  p1[0] = b[0];
  p1[1] = b[2];
  p1[2] = b[4];
  p2[0] = b[1];
  p2[1] = b[3];
  p2[2] = b[5];
  d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
  
  // create the cylinder
  Cylinder = vtkCylinderSource::New();
  Cylinder->SetRadius(d / 200);
  
  //-----------------
  // rotate the cylinder on the X axis (default axis is Z)
  //-----------------
	
  vtkTransform *cylInitTr = vtkTransform::New();
  cylInitTr->RotateZ(-90);	
  
  vtkTransformPolyDataFilter *cylInitTrPDF = vtkTransformPolyDataFilter::New();
  cylInitTrPDF->SetInput(Cylinder->GetOutput());
  cylInitTrPDF->SetTransform(cylInitTr);

  /*
   vtk coord

      y
      ^
      |
      __
     || |
     |z-|-------> x
     |  |
      --
   */
  
  //-----------------

  // create the cube
  Cube = vtkCubeSource::New();
  //Cube->SetRadius(InputVme->GetCurrentData()->GetLength() / 30);

  // create the translation transform
  TranslateTr[CUBE] = vtkTransform::New();
  TranslateTr[CYLINDER] = vtkTransform::New();

  // create cube translation transform pdf
  TranslatePDF[CUBE] = vtkTransformPolyDataFilter::New();
  TranslatePDF[CUBE]->SetInput(Cube->GetOutput());
  
  // create cylinder translation transform
  TranslatePDF[CYLINDER] = vtkTransformPolyDataFilter::New();
  TranslatePDF[CYLINDER]->SetInput(cylInitTrPDF->GetOutput());

  //-----------------
  // update translate transform
  //-----------------

  // place the cube; default cube length is 1/16 of vme bb diagonal
  this->SetCubeLength(d / 16);

  // place the cylinder before the cube; default cylinder length is 7/16 of vme bb diagonal
  this->SetCylinderLength(d * 7 / 16);

  //-----------------

  // translate transform setting
  TranslatePDF[CUBE]->SetTransform(TranslateTr[CUBE]);
  TranslatePDF[CYLINDER]->SetTransform(TranslateTr[CYLINDER]);

/*
  ^           ________          
  |          |        |
  |----------|        |
  |          |        |------>X
  |----------|        |
  |          |________|
  O          
  
  <---------><-------->
     cylLen    cubeLen
  */  

  // create rotation transform and rotation TPDF 
  RotatePDF[CYLINDER] = vtkTransformPolyDataFilter::New();
  RotatePDF[CUBE] = vtkTransformPolyDataFilter::New();
  RotationTr = vtkTransform::New();
  RotationTr->Identity(); 

  RotatePDF[CYLINDER]->SetTransform(RotationTr);
  RotatePDF[CUBE]->SetTransform(RotationTr);

  RotatePDF[CYLINDER]->SetInput(TranslatePDF[CYLINDER]->GetOutput());
  RotatePDF[CUBE]->SetInput(TranslatePDF[CUBE]->GetOutput());

  RotatePDF[CYLINDER]->Update();
  RotatePDF[CYLINDER]->Update();

  //clean up
  cylInitTr->Delete();
  cylInitTrPDF->Delete();
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::CreateISA()
//----------------------------------------------------------------------------
{
  // create isa compositor and assign behaviors to IsaGen ivar
  for (int i = 0; i < 2; i++)
  {
    IsaComp[i] = mmiCompositorMouse::New();

    // default behavior is activated by mouse left and is constrained to X axis,
    // default ref sys is input vme abs matrix
    IsaGen[i] = IsaComp[i]->CreateBehavior(MOUSE_LEFT);

    IsaGen[i]->SetVME(InputVme);
    IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK);
  	IsaGen[i]->GetTranslationConstraint()->GetRefSys()->SetTypeToLocal();
      
    //isa will send events to this
    IsaGen[i]->SetListener(this);
  }
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::SetAxis(int axis) 
//----------------------------------------------------------------------------
{
  // this should be called when the translation gizmo
  // is created; gizmos are not highlighted
  
  // register the axis
  Axis = axis;
  
  // rotate the cylinder and the cube to match given axis
  if (Axis == X)
  {
    // reset cyl and cube rotation
    RotationTr->Identity();
  
    // set cyl and cube color to red
    this->SetColor(1, 0, 0, 1, 0, 0);

    // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::FREE, mmiConstraint::LOCK, mmiConstraint::LOCK);
    }
  }
  else if (axis == Y)
  {
    // set rotation to move con and cyl on Y 
    RotationTr->Identity();
    RotationTr->RotateZ(90);
   
    // set cyl and cube color to green
    this->SetColor(0, 1, 0, 0, 1, 0);

    // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::FREE, mmiConstraint::LOCK);
    }
  }  
  else if (axis == Z)
  {
    // set rotation to move con and cyl on Z
    RotationTr->Identity();
    RotationTr->RotateY(-90);
    
    // set cyl and cube color to blue
     this->SetColor(0, 0, 1, 0, 0, 1);

     // change the axis constrain
    for (int i = 0; i < 2; i++)
    {
      IsaGen[i]->GetTranslationConstraint()->SetConstraintModality(mmiConstraint::LOCK, mmiConstraint::LOCK, mmiConstraint::FREE);
    }
  }  
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::Highlight(bool highlight)
//----------------------------------------------------------------------------
{
  if (highlight == true)
  {
   // Highlight the cylinder by setting its color to yellow 
   this->SetColor(CYLINDER, 1, 1, 0);
  } 
  else
  {
   // deactivate the cylinder 
   if (Axis == X)
   {
     // set cyl col to red
     this->SetColor(CYLINDER, 1, 0, 0);
   } 
   else if (Axis == Y)
   {
     // set cyl col to green
     this->SetColor(CYLINDER, 0, 1, 0);
   }
   else if (Axis == Z)
   {
     // set cyl col to blue
     this->SetColor(CYLINDER, 0, 0, 1);
   } 
  }
}

//----------------------------------------------------------------------------
void  mafGizmoScaleAxis::SetCubeLength(double cubeLength)
//----------------------------------------------------------------------------
{
  /*
  ^           ________          
  |          |        |
  |----------|        |
  |          |        |------>X
  |----------|        |
  |          |________|
  O          
  
  <---------><-------->
     cylLen    cubeLen
  */  

  // set the cube length
  Cube->SetXLength(cubeLength);
  Cube->SetYLength(cubeLength);
  Cube->SetZLength(cubeLength);

  // translate the cube in (cylLen + cubeLen/2)
  TranslateTr[CUBE]->Identity();
  TranslateTr[CUBE]->Translate(Cylinder->GetHeight() + cubeLength / 2, 0, 0);
}

//----------------------------------------------------------------------------
double mafGizmoScaleAxis::GetCubeLength() const
//----------------------------------------------------------------------------
{
  return Cube->GetXLength();
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::SetCylinderLength(double cylLength)
//----------------------------------------------------------------------------
{
  /*
  ^           ________          
  |          |        |
  |----------|        |
  |          |        |------>X
  |----------|        |
  |          |________|
  O          
  
  <---------><-------->
     cylLen    cubeLen
  */  

  // set cylLen to cylLength
  Cylinder->SetHeight(cylLength);

  // translate the cyl in (cylLen / 2)
  TranslateTr[CYLINDER]->Identity();
  TranslateTr[CYLINDER]->Translate(cylLength / 2, 0, 0);

  // translate the cube in (cylLen + (cubeLen / 2)) 
  TranslateTr[CUBE]->Identity();
  TranslateTr[CUBE]->Translate(cylLength + Cube->GetXLength() / 2, 0, 0);
}

//----------------------------------------------------------------------------
double mafGizmoScaleAxis::GetCylinderLength() const
//----------------------------------------------------------------------------
{
  return Cylinder->GetHeight();
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  // forward events to the listener
	maf_event->SetSender(this);
  mafEventMacro(*maf_event);
}

/** Gizmo color */
//----------------------------------------------------------------------------
void mafGizmoScaleAxis::SetColor(int part, double col[3])
//----------------------------------------------------------------------------
{
  if (part == CYLINDER)
  {
    CylGizmo->GetMaterial()->m_Prop->SetColor(col);
    CylGizmo->GetMaterial()->m_Prop->SetAmbient(0);
    CylGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
    CylGizmo->GetMaterial()->m_Prop->SetSpecular(0);
  }
  else
  {
    CubeGizmo->GetMaterial()->m_Prop->SetColor(col);
    CubeGizmo->GetMaterial()->m_Prop->SetAmbient(0);
    CubeGizmo->GetMaterial()->m_Prop->SetDiffuse(1);
    CubeGizmo->GetMaterial()->m_Prop->SetSpecular(0);
  }
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::SetColor(int part, double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(part, col);
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::SetColor(double cylCol[3], double coneCol[3])
//----------------------------------------------------------------------------
{
  this->SetColor(CYLINDER, cylCol);
  this->SetColor(CUBE, coneCol);
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::SetColor(double cylR, double cylG, double cylB, double coneR, double coneG, double coneB)
//----------------------------------------------------------------------------
{
  this->SetColor(CYLINDER, cylR, cylG, cylB);
  this->SetColor(CUBE, coneR, coneG, coneB);
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::Show(bool show)
//----------------------------------------------------------------------------
{
  double opacity = ((show == TRUE) ? 1 : 0);

  CylGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
  CubeGizmo->GetMaterial()->m_Prop->SetOpacity(opacity);
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::SetAbsPose(mafMatrix *absPose)
//----------------------------------------------------------------------------
{
  CubeGizmo->SetAbsMatrix(*absPose); 
  CylGizmo->SetAbsMatrix(*absPose); 
  SetRefSysMatrix(absPose);
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::SetRefSysMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{  
  for (int i = 0; i < 2; i++)
  {
    IsaGen[i]->GetTranslationConstraint()->GetRefSys()->SetTypeToCustom(matrix);
    IsaGen[i]->GetPivotRefSys()->SetTypeToCustom(matrix);
  } 
}

//----------------------------------------------------------------------------
mafMatrix *mafGizmoScaleAxis::GetAbsPose()
//----------------------------------------------------------------------------
{
  return CylGizmo->GetOutput()->GetAbsMatrix();
}

//----------------------------------------------------------------------------
void mafGizmoScaleAxis::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
  this->InputVme = vme; 
  SetAbsPose(vme->GetOutput()->GetAbsMatrix()); 
  SetRefSysMatrix(vme->GetOutput()->GetAbsMatrix());
}
