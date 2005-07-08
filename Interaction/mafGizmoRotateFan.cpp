/*=========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafGizmoRotateFan.cpp,v $
  Language:  C++
  Date:      $Date: 2005-07-08 13:27:46 $
  Version:   $Revision: 1.3 $
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


#include "mafGizmoRotateFan.h"
// wxwin stuff
#include "wx/string.h"

#include "mafDecl.h"

// isa stuff
#include "mmiCompositorMouse.h"
#include "mmiGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "mafMatrix.h"
#include "mafRefSys.h"
#include "mafVME.h"
#include "mafVMEOutput.h"
#include "mafVMEGizmo.h"

// vtk stuff
#include "vtkSphereSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
mafGizmoRotateFan::mafGizmoRotateFan(mafVME *input, mafObserver *listener)
//----------------------------------------------------------------------------
{
  StartTheta = EndTheta = 0;
  MirrorStatus = false;

  // default axis is X
  ActiveAxis = X;

  m_Listener = listener;
  InputVme = input;

  RefSys = new mafRefSys(InputVme->GetOutput()->GetAbsMatrix()->GetVTKMatrix());

  // create pipeline stuff
  CreatePipeline();

  //-----------------
  // create vme gizmo stuff
  //-----------------
  
  // the circle gizmo
  Gizmo = mafVMEGizmo::New();
  Gizmo->SetName("fan");
  Gizmo->SetData(ChangeFanAxisTPDF->GetOutput());

  // set the default axis to X axis
  this->SetAxis(ActiveAxis);

  // set gizmo color to yellow
  this->SetColor(1, 1, 0);

  SetAbsPose(InputVme->GetOutput()->GetAbsMatrix());
  
  // hide the gizmo after creation
  this->Show(false);

  Gizmo->ReparentTo(mafVME::SafeDownCast(InputVme->GetRoot()));
  // add the gizmo to the tree, this should increase reference count  
//  mafEventMacro(mafEvent(this, VME_ADD, Gizmo));
}
//----------------------------------------------------------------------------
mafGizmoRotateFan::~mafGizmoRotateFan() 
//----------------------------------------------------------------------------
{   
  cppDEL(RefSys);

  vtkDEL(Sphere);
  vtkDEL(RotateFanTPDF);
  vtkDEL(RotateFanTr);
  vtkDEL(MirrorTPDF);
  vtkDEL(MirrorTr);
  vtkDEL(BufferTr);
  vtkDEL(ChangeFanAxisTr);
  vtkDEL(ChangeFanAxisTPDF);
  
	//----------------------
	// No leaks so somebody is performing this...
	// cppDEL(GizmoData[i]);
	//----------------------
  mafEventMacro(mafEvent(this, VME_REMOVE, Gizmo));
  //vtkDEL(Gizmo);
}

//----------------------------------------------------------------------------
void mafGizmoRotateFan::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // buffer transfrom for storing mirror transform
  vtkNEW(BufferTr);
  BufferTr->PostMultiply();

  InputVme->Update();
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
  
  // create sphere
  vtkNEW(Sphere);
	Sphere->SetRadius(d/2);
	Sphere->SetThetaResolution(50);
	Sphere->SetStartTheta(StartTheta);
	Sphere->SetEndTheta(EndTheta);

  // create the rotate fan transform
  RotateFanTr = vtkTransform::New();

  RotateFanTPDF = vtkTransformPolyDataFilter::New();
  RotateFanTPDF->SetTransform(RotateFanTr);
  RotateFanTPDF->SetInput(Sphere->GetOutput());

  // create the mirroring transform
  MirrorTr = vtkTransform::New();
  vtkNEW(MirrorTPDF);
  MirrorTPDF->SetTransform(MirrorTr);
  MirrorTPDF->SetInput(RotateFanTPDF->GetOutput());

  // create the transform for the sphere
  ChangeFanAxisTr = vtkTransform::New();
  ChangeFanAxisTr->PostMultiply();

  // update transform to set the gizmo normal to X
  this->SetAxis(X);

	// transform the sphere
  ChangeFanAxisTPDF = vtkTransformPolyDataFilter::New();
  ChangeFanAxisTPDF->SetInput(MirrorTPDF->GetOutput());
	ChangeFanAxisTPDF->SetTransform(ChangeFanAxisTr);
}

//----------------------------------------------------------------------------
void mafGizmoRotateFan::SetAxis(int axis) 
//----------------------------------------------------------------------------
{
  // this should be called when the gizmo
  // is created; gizmos are not highlighted
  
  // register the axis
  ActiveAxis = axis;
  
  // rotate the gizmo components to match the specified axis
  
    /**

      z               x              y
      ^               ^              ^
      |               |              |
      |   /|          |  /|          |   /|
      | /  |          |/  |          | /  |
      x-------> y     y-------> z    z-------> x
  
          X               Y            Z
  */

  ChangeFanAxisTr->Identity();
  
  // splat the sphere on XY plane; the
  // gizmo us normal to the Z axis
  ChangeFanAxisTr->Scale(1, 1, 0);

  if (ActiveAxis == X)
  {
    // set rotation to move gizmo normal to X
    ChangeFanAxisTr->RotateY(90);
    ChangeFanAxisTr->RotateX(90);
  }
  else if (ActiveAxis == Y)
  {
    // set rotation to move gizmo normal to Y 
    ChangeFanAxisTr->RotateX(-90);
    ChangeFanAxisTr->RotateY(-90);
  }
}

//----------------------------------------------------------------------------
void  mafGizmoRotateFan::SetRadius(double radius)
//----------------------------------------------------------------------------
{
  Sphere->SetRadius(radius);
}

//----------------------------------------------------------------------------
void mafGizmoRotateFan::OnEvent(mafEventBase *maf_event)
//----------------------------------------------------------------------------
{
  if (mafEvent *e = mafEvent::SafeDownCast(maf_event))
  {
    // mouse down change gizmo selection status
    switch(e->GetId()) 
    {
      case ID_TRANSFORM:
      {
        if (e->GetArg() == mmiGenericMouse::MOUSE_DOWN)
        {
          //----------------------------------------
          // compute start theta for the sphere
          //----------------------------------------

          // get the picked position from the event
          double pos[3];
          //mafTransform::GetPosition(vtkMatrix4x4::SafeDownCast(e->GetVtkObj()), pos);
          mafTransform::GetPosition(*mafMatrix::SafeDownCast(e->GetMafObject()),pos);

          // get the start theta
          double offsetAngle = PointPickedToStartTheta(pos[0], pos[1], pos[2]);

          //----------------------------------------

          // rotate the fan of theta around its axis 
          RotateFanTr->Identity();
          RotateFanTr->RotateZ(offsetAngle);

          // build the rotation axis for mirroring
          double axis[3];
          axis[0] = cos(vtkMath::DegreesToRadians() * offsetAngle);
          axis[1] = sin(vtkMath::DegreesToRadians() * offsetAngle);
          axis[2] = 0;

          // create the transform for mirroring the fan
          BufferTr->Identity();
          BufferTr->RotateWXYZ(180, axis);

          // change the sender and forward the event
          e->SetSender(this);
          mafEventMacro(*e);
          mafEventMacro(mafEvent(this, CAMERA_UPDATE));
        }
        else if (e->GetArg() == mmiGenericMouse::MOUSE_MOVE)
        {
          double dTheta = e->GetDouble();
          if (MirrorStatus == ON)
          {
            EndTheta -= dTheta;     
          }
          else
          {
            EndTheta += dTheta;
          }

          if (EndTheta > 360)
          {
            EndTheta -= 360;
          }

          if (EndTheta < 0)
          {
            // toggle mirror status
            if (MirrorStatus == ON)
            {
              MirrorStatus = OFF;
              MirrorTr->Identity();
            }
            else
            {
              MirrorStatus = ON;
              MirrorTr->Identity();
              MirrorTr->Concatenate(BufferTr);
            }
            EndTheta = -EndTheta;
          }

          // update the sphere EndTheta ivar
          Sphere->SetEndTheta(EndTheta);

          // change the sender and forward the event
          e->SetSender(this);
          mafEventMacro(*e);
          mafEventMacro(mafEvent(this, CAMERA_UPDATE));
        }
        else if (e->GetArg() == mmiGenericMouse::MOUSE_UP)
        {

          // reset StartTheta and EndTheta
          StartTheta = 0;
          EndTheta = 0;

          // update the sphere EndTheta ivar
          Sphere->SetEndTheta(EndTheta);

          // change the sender and forward the event
          e->SetSender(this);
          mafEventMacro(*e);

          mafEventMacro(mafEvent(this, CAMERA_UPDATE));
        }
      }
      break;
      default:
      {
        mafEventMacro(*e);
      }
    }
  }
} 

/** Gizmo color */
//----------------------------------------------------------------------------
void mafGizmoRotateFan::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  Gizmo->GetMaterial()->m_Prop->SetColor(col);
	Gizmo->GetMaterial()->m_Prop->SetAmbient(0);
	Gizmo->GetMaterial()->m_Prop->SetDiffuse(1);
	Gizmo->GetMaterial()->m_Prop->SetSpecular(0);
}

//----------------------------------------------------------------------------
void mafGizmoRotateFan::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}

//----------------------------------------------------------------------------
void mafGizmoRotateFan::Show(bool show)
//----------------------------------------------------------------------------
{
  if (show == TRUE)
  {  
    Gizmo->GetMaterial()->m_Prop->SetOpacity(0.2);
  }
  else
  {
    Gizmo->GetMaterial()->m_Prop->SetOpacity(0);
  }
}

//----------------------------------------------------------------------------
double mafGizmoRotateFan::PointPickedToStartTheta(double xp, double yp, double zp)
//----------------------------------------------------------------------------
{

  // Get the abs matrix
  mafMatrix invParentAbsMat;
  invParentAbsMat.Invert(*RefSys->GetMatrix(), invParentAbsMat);

  double pAbs[4], pLoc[4];
  pAbs[0] = xp;
  pAbs[1] = yp;
  pAbs[2] = zp;
  pAbs[3] = 1;

  /*
    Get the picked point in local coordinate
    pAbs = M_abs * pLoc => pLoc = M_abs^-1 * pAbs
  */
  invParentAbsMat.MultiplyPoint(pAbs, pLoc);

  // compute the start theta
  double startThetaRad = 0;
  switch (ActiveAxis)
  {
	  case (X):
	  {
	    startThetaRad = atan2( pLoc[2], pLoc[1]);
	  }
	  break;
	  case (Y):
	  {
	    startThetaRad = atan2( pLoc[0], pLoc[2]);
	  }
	  break;
	  case (Z):
	  {
	    startThetaRad = atan2( pLoc[1], pLoc[0]);
	  }
	  break;
  }

  double startThetaDeg = startThetaRad * vtkMath::RadiansToDegrees();
  return ((startThetaDeg > 0) ? startThetaDeg : (360 + startThetaDeg));  
}

//----------------------------------------------------------------------------
void mafGizmoRotateFan::SetAbsPose(mafMatrix *absPose )
//----------------------------------------------------------------------------
{
  Gizmo->SetAbsMatrix(*absPose);
  SetRefSysMatrix(absPose);
}

//----------------------------------------------------------------------------
void mafGizmoRotateFan::SetInput(mafVME *vme)
//----------------------------------------------------------------------------
{
 this->InputVme = vme;
 SetAbsPose(vme->GetOutput()->GetAbsMatrix());
}

//----------------------------------------------------------------------------
void mafGizmoRotateFan::SetRefSysMatrix(mafMatrix *matrix)
//----------------------------------------------------------------------------
{  
  RefSys->SetTypeToCustom(matrix);
}
