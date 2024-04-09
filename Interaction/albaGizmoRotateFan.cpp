/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaGizmoRotateFan
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


#include "albaGizmoRotateFan.h"
// wxwin stuff
#include "wx/string.h"

#include "albaDecl.h"

// isa stuff
#include "albaInteractorCompositorMouse.h"
#include "albaInteractorGenericMouse.h"

// vme stuff
#include "mmaMaterial.h"
#include "albaMatrix.h"
#include "albaRefSys.h"
#include "albaVME.h"
#include "albaVMEOutput.h"
#include "albaVMEGizmo.h"

// vtk stuff
#include "vtkSphereSource.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkPolyData.h"
#include "vtkMath.h"
#include "vtkProperty.h"

//----------------------------------------------------------------------------
albaGizmoRotateFan::albaGizmoRotateFan(albaVME *input, albaObserver *listener)
//----------------------------------------------------------------------------
{
  m_StartTheta = m_EndTheta = 0;
  m_MirrorStatus = false;

  // default axis is X
  m_ActiveAxis  = X;
  m_Listener  = listener;
  m_InputVme    = input;

  // get the input vme abs matrix
  albaMatrix *absInputMatrix = m_InputVme->GetOutput()->GetAbsMatrix();
  
  // create refsys from abs input matrix
  m_RefSys = new albaRefSys(absInputMatrix->GetVTKMatrix());

  // create pipeline stuff
  CreatePipeline();

  //-----------------
  // create vme gizmo stuff
  //-----------------
  // the circle gizmo
  m_GizmoFan = albaVMEGizmo::New();
  m_GizmoFan->SetName("fan");
  m_GizmoFan->SetData(m_ChangeFanAxisTPDF->GetOutput());
  
  // set the default axis to X axis
  this->SetAxis(m_ActiveAxis);

  // set gizmo color to yellow
  this->SetColor(1, 1, 0);

  // the gizmo initial pose is the input vme one
  SetAbsPose(absInputMatrix);

  // add the gizmo to the tree, this should increase reference count  
  m_GizmoFan->ReparentTo(m_InputVme->GetRoot());
}
//----------------------------------------------------------------------------
albaGizmoRotateFan::~albaGizmoRotateFan() 
//----------------------------------------------------------------------------
{   
  // clean up
  cppDEL(m_RefSys);

  vtkDEL(m_Sphere);
  vtkDEL(m_RotateFanTPDF);
  vtkDEL(m_RotateFanTransform);
  vtkDEL(m_MirrorTPDF);
  vtkDEL(m_MirrorTr);
  vtkDEL(m_BufferTr);
  vtkDEL(m_ChangeFanAxisTransform);
  vtkDEL(m_ChangeFanAxisTPDF);
  
  m_GizmoFan->ReparentTo(NULL);
}
//----------------------------------------------------------------------------
void albaGizmoRotateFan::CreatePipeline() 
//----------------------------------------------------------------------------
{
  // buffer transfrom for storing mirror transform
  vtkNEW(m_BufferTr);
  m_BufferTr->PostMultiply();

  m_InputVme->Update();
  
  // calculate diagonal of m_InputVme bounding box 
  double b[6],p1[3],p2[3],d;
	if(m_InputVme->IsA("albaVMEGizmo"))
		m_InputVme->GetOutput()->GetVTKData()->GetBounds(b);
	else
		m_InputVme->GetOutput()->GetBounds(b);
  p1[0] = b[0];
  p1[1] = b[2];
  p1[2] = b[4];
	p2[0] = (b[1] != -1) ? b[1] : 100;
	p2[1] = (b[3] != -1) ? b[3] : 100;
	p2[2] = (b[5] != -1) ? b[5] : 100;

  d = sqrt(vtkMath::Distance2BetweenPoints(p1,p2));
  
  // create sphere
  vtkNEW(m_Sphere);
	m_Sphere->SetRadius(d/2);
	m_Sphere->SetThetaResolution(50);
	m_Sphere->SetStartTheta(m_StartTheta);
	m_Sphere->SetEndTheta(m_EndTheta);

  // create the rotate fan transform
  m_RotateFanTransform = vtkTransform::New();

  m_RotateFanTPDF = vtkTransformPolyDataFilter::New();
  m_RotateFanTPDF->SetTransform(m_RotateFanTransform);
  m_RotateFanTPDF->SetInputConnection(m_Sphere->GetOutputPort());

  // create the mirroring transform
  m_MirrorTr = vtkTransform::New();
  vtkNEW(m_MirrorTPDF);
  m_MirrorTPDF->SetTransform(m_MirrorTr);
  m_MirrorTPDF->SetInputConnection(m_RotateFanTPDF->GetOutputPort());

  // create the transform for the sphere
  m_ChangeFanAxisTransform = vtkTransform::New();
  m_ChangeFanAxisTransform->PostMultiply();

  // update transform to set the gizmo normal to X
  this->SetAxis(X);

  // transform the sphere
  m_ChangeFanAxisTPDF = vtkTransformPolyDataFilter::New();
  m_ChangeFanAxisTPDF->SetInputConnection(m_MirrorTPDF->GetOutputPort());
  m_ChangeFanAxisTPDF->SetTransform(m_ChangeFanAxisTransform);
}
//----------------------------------------------------------------------------
void albaGizmoRotateFan::SetAxis(int axis) 
//----------------------------------------------------------------------------
{
  // this should be called when the gizmo
  // is created; gizmos are not highlighted
  
  // register the active axis
  m_ActiveAxis = axis;
  
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
  m_ChangeFanAxisTransform->Identity();
  
  // splat the sphere on XY plane; the
  // gizmo us normal to the Z axis
  m_ChangeFanAxisTransform->Scale(1, 1, 0);

  if (m_ActiveAxis == X)
  {
    // set rotation to move gizmo normal to X
    m_ChangeFanAxisTransform->RotateY(90);
    m_ChangeFanAxisTransform->RotateX(90);
  }
  else if (m_ActiveAxis == Y)
  {
    // set rotation to move gizmo normal to Y 
    m_ChangeFanAxisTransform->RotateX(-90);
    m_ChangeFanAxisTransform->RotateY(-90);
  }
}
//----------------------------------------------------------------------------
void  albaGizmoRotateFan::SetRadius(double radius)
//----------------------------------------------------------------------------
{
  m_Sphere->SetRadius(radius);
}

double albaGizmoRotateFan::GetRadius()
{
  return m_Sphere->GetRadius();
}

//----------------------------------------------------------------------------
void albaGizmoRotateFan::OnEvent(albaEventBase *alba_event)
//----------------------------------------------------------------------------
{
  if (albaEvent *e = albaEvent::SafeDownCast(alba_event))
  {
    // mouse down change gizmo selection status
    switch(e->GetId()) 
    {
      case ID_TRANSFORM:
      {
	    // build string to notify the listener about the active gizmo component:
        albaString activeAxisStringToSend;
        if (m_ActiveAxis == X)
        {
		  // gizmo X is sending events
          activeAxisStringToSend = "X";
        }
        else if (m_ActiveAxis == Y)
        {
		  // gizmo Y is sending events
          activeAxisStringToSend = "Y";
        } 
        else if (m_ActiveAxis == Z)
        {
		  // gizmo Z is sending events
          activeAxisStringToSend = "Z";
        }

        assert(e->GetString() == NULL);
        e->SetString(&activeAxisStringToSend);

        if (e->GetArg() == albaInteractorGenericMouse::MOUSE_DOWN)
        {
          //----------------------------------------
          // compute start theta for the sphere
          //----------------------------------------

          // get the picked position from the event
          double pos[3];
          albaTransform::GetPosition(*albaMatrix::SafeDownCast(e->GetMafObject()),pos);

          // get the start theta
          double offsetAngle = PointPickedToStartTheta(pos[0], pos[1], pos[2]);

          //----------------------------------------
          // rotate the fan of theta around its axis 
          m_RotateFanTransform->Identity();
          m_RotateFanTransform->RotateZ(offsetAngle);

          // build the rotation axis for mirroring
          double axis[3];
          axis[0] = cos(vtkMath::RadiansFromDegrees(offsetAngle));
          axis[1] = sin(vtkMath::RadiansFromDegrees(offsetAngle));
          axis[2] = 0;

          // create the transform for mirroring the fan
          m_BufferTr->Identity();
          m_BufferTr->RotateWXYZ(180, axis);

          // change the sender and forward the event
          e->SetSender(this);
          albaEventMacro(*e);
        }
        else if (e->GetArg() == albaInteractorGenericMouse::MOUSE_MOVE)
        {
          double dTheta = e->GetDouble();
        
          if (m_MirrorStatus == ON)
          {
            m_EndTheta -= dTheta;     
          }
          else
          {
            m_EndTheta += dTheta;
          }

          if (m_EndTheta > 360)
          {
            m_EndTheta -= 360;
          }

          if (m_EndTheta < 0)
          {
            // toggle mirror status
            if (m_MirrorStatus == ON)
            {
              m_MirrorStatus = OFF;
              m_MirrorTr->Identity();
            }
            else
            {
              m_MirrorStatus = ON;
              m_MirrorTr->Identity();
              m_MirrorTr->Concatenate(m_BufferTr);
            }
            m_EndTheta = -m_EndTheta;
          }

          // update the sphere m_EndTheta ivar
          m_Sphere->SetEndTheta(m_EndTheta);

          // change the sender and forward the event
          e->SetSender(this);
          
          albaEventMacro(*e);
        }
        else if (e->GetArg() == albaInteractorGenericMouse::MOUSE_UP)
        {

          // reset m_StartTheta and m_EndTheta
          m_StartTheta = 0;
          m_EndTheta = 0;

          // update the sphere m_EndTheta ivar
          m_Sphere->SetEndTheta(m_EndTheta);

          // change the sender and forward the event
          e->SetSender(this);
          albaEventMacro(*e);
        }
      }
      break;
      default:
      {
        albaEventMacro(*e);
      }
    }
  }
} 
/** m_Gizmo color */
//----------------------------------------------------------------------------
void albaGizmoRotateFan::SetColor(double col[3])
//----------------------------------------------------------------------------
{
  m_GizmoFan->GetMaterial()->m_Prop->SetColor(col);
	m_GizmoFan->GetMaterial()->m_Prop->SetAmbient(0);
	m_GizmoFan->GetMaterial()->m_Prop->SetDiffuse(1);
	m_GizmoFan->GetMaterial()->m_Prop->SetSpecular(0);
	m_GizmoFan->GetMaterial()->m_Prop->SetOpacity(0.1);
}
//----------------------------------------------------------------------------
void albaGizmoRotateFan::SetColor(double colR, double colG, double colB)
//----------------------------------------------------------------------------
{
  double col[3] = {colR, colG, colB};
  this->SetColor(col);
}
//----------------------------------------------------------------------------
void albaGizmoRotateFan::Show(bool show)
//----------------------------------------------------------------------------
{
  GetLogicManager()->VmeShow(m_GizmoFan, show);
}
//----------------------------------------------------------------------------
double albaGizmoRotateFan::PointPickedToStartTheta(double xp, double yp, double zp)
//----------------------------------------------------------------------------
{
  // Get the abs matrix
  albaMatrix invParentAbsMat;
  invParentAbsMat.Invert(*m_RefSys->GetMatrix(), invParentAbsMat);

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
  switch (m_ActiveAxis)
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
  double startThetaDeg =   vtkMath::DegreesFromRadians(startThetaRad);
  return ((startThetaDeg > 0) ? startThetaDeg : (360 + startThetaDeg));  
}
//----------------------------------------------------------------------------
void albaGizmoRotateFan::SetAbsPose(albaMatrix *absPose )
//----------------------------------------------------------------------------
{
  m_GizmoFan->SetAbsMatrix(*absPose);
  SetRefSysMatrix(absPose);
}
//----------------------------------------------------------------------------
void albaGizmoRotateFan::SetInput(albaVME *vme)
//----------------------------------------------------------------------------
{
 this->m_InputVme = vme;
 SetAbsPose(vme->GetOutput()->GetAbsMatrix());
}
//----------------------------------------------------------------------------
void albaGizmoRotateFan::SetRefSysMatrix(albaMatrix *matrix)
//----------------------------------------------------------------------------
{  
  m_RefSys->SetTypeToCustom(matrix);
}


void albaGizmoRotateFan::SetMediator(albaObserver *mediator)
{
	albaGizmoInterface::SetMediator(mediator);
	m_GizmoFan->SetMediator(mediator);
}