/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaBehaviorProsthesis
 Authors: Gianluigi Crimi, from albaGUITransformMouseFloatVME by Nicola Vanella

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

#include "albaBehaviorProsthesis.h"
#include "albaInteractorConstraint.h"
#include "albaRefSys.h"
#include "albaViewCompound.h"
#include "albaDecl.h"
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVect3d.h"

#include "vtkMath.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaBehaviorProsthesis);

//------------------------------------------------------------------------------
albaBehaviorProsthesis::albaBehaviorProsthesis()
{
  m_Modality2D = false;
}

//------------------------------------------------------------------------------
albaBehaviorProsthesis::~albaBehaviorProsthesis()
{
}

//------------------------------------------------------------------------------
bool albaBehaviorProsthesis::PrepareTransform(int button)
{
  // the left button with SHIFT translates, as the middle button
  if (button == ALBA_LEFT_BUTTON && m_ShiftDown)
  {
    EnableTranslationInternal(true);
    return true;
  }

  // the rotation of the left button is around the view normal in the slice views
  if (button == ALBA_LEFT_BUTTON && m_View)
  {
    GetRotationConstraint()->GetRefSys()->SetTypeToView();
    if (IsSliceView(m_View))
      GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::LOCK, albaInteractorConstraint::LOCK, albaInteractorConstraint::FREE);
    else
      GetRotationConstraint()->SetConstraintModality(albaInteractorConstraint::FREE, albaInteractorConstraint::FREE, albaInteractorConstraint::LOCK);
  }

  return Superclass::PrepareTransform(button);
}

//------------------------------------------------------------------------------
bool albaBehaviorProsthesis::IsSliceView(albaView *v)
{
  if (v == NULL)
    return false;
  if (albaViewCompound::SafeDownCast(v))
	{
		albaView *childView = albaViewCompound::SafeDownCast(v)->GetSubView();
		return IsSliceView(childView);
	}
	else if( v->IsA("albaViewRX") || v->IsA("appViewSliceBlend"))// || v->IsA("appViewSlice") )
  {
		return true;
  }
  
  return false;
}

//----------------------------------------------------------------------------
void albaBehaviorProsthesis::TrackballRotate()
{

	if (m_Modality2D)
	{
		TrackballRoll();
		return;
	}

	// mouse position
	int x = m_MousePointer2DPosition[0];
	int y = m_MousePointer2DPosition[1];

	// the pivot center
	double pivotPoint[3], newPos[4], oldPos[4];

	//Get pivot point
	albaTransform::GetPosition(*GetPivotRefSys()->GetMatrix(), pivotPoint);

	// Calculate new mouse pos
	ComputeDisplayToWorld(x, y, 0.0, newPos);

	// Calculate old mouse pos
	ComputeDisplayToWorld(m_LastX, m_LastY, 0.0, oldPos);

	albaVect3d pivotVect = pivotPoint;

	albaVect3d oldMouse = { oldPos[0], oldPos[1], oldPos[2] };
	albaVect3d newMouseX = { newPos[0], oldPos[1], oldPos[2] };
	albaVect3d newMouseY = { oldPos[0], newPos[1], oldPos[2] };

	albaVect3d oldVect = oldMouse - pivotVect;

	albaVect3d newVectX = newMouseX - pivotVect;
	albaVect3d newVectY = newMouseY - pivotVect;

	//Calculate X Angle
	double angleX = vtkMath::DegreesFromRadians(newVectX.AngleBetweenVectors(oldVect));
	if (newPos[0] - oldPos[0] > 0)
		angleX = -angleX;

	//Calculate Y Angle
	double angleY = vtkMath::DegreesFromRadians(newVectY.AngleBetweenVectors(oldVect));
	if (newPos[1] - oldPos[1] > 0)
		angleY = -angleY;

	// Create transform
	albaTransform transform;

	// translate to center of the constrain ref sys
	transform.Identity();
	transform.Translate(pivotPoint[0], pivotPoint[1], pivotPoint[2], PRE_MULTIPLY);

	// azimuth
	m_CurrentCamera->OrthogonalizeViewUp();
	double *azimuth = m_CurrentCamera->GetViewUp();

	transform.RotateWXYZ(angleX, azimuth[0], azimuth[1], azimuth[2], PRE_MULTIPLY);

	double elevation[3];
	vtkMath::Cross(m_CurrentCamera->GetDirectionOfProjection(), azimuth, elevation);

	transform.RotateWXYZ(angleY, elevation[0], elevation[1], elevation[2], PRE_MULTIPLY);

	// translate back
	transform.Translate(-pivotPoint[0], -pivotPoint[1], -pivotPoint[2], PRE_MULTIPLY);

	//send the transform matrix
	SendTransformMatrix(transform.GetMatrix());

	// update m_LastX and m_LastY
	m_LastX = x;
	m_LastY = y;
}

//----------------------------------------------------------------------------
void albaBehaviorProsthesis::TrackballRoll()
{
   // get the rotation refsys
   m_ConstrainRefSys = m_CurrentRotationConstraint->GetRefSys()->GetMatrix();  

   // the refsys center
   double pivotPoint[3];

   // get the centre of rotation from PivotRefSys
   albaTransform::GetPosition(*GetPivotRefSys()->GetMatrix(), pivotPoint); 
   
   // Get the axis to rotate around = vector from eye to origin
 
   double motion_vector[3];
   double view_point[3];
 
   if (m_CurrentCamera->GetParallelProjection())
   {
     // If parallel projection, want to get the view plane normal...
     m_CurrentCamera->ComputeViewPlaneNormal();
     m_CurrentCamera->GetViewPlaneNormal(motion_vector);
   }
   else
   {   
     // Perspective projection, get vector from eye to center of actor
     m_CurrentCamera->GetPosition(view_point);
     motion_vector[0] = view_point[0] - pivotPoint[0];
     motion_vector[1] = view_point[1] - pivotPoint[1];
     motion_vector[2] = view_point[2] - pivotPoint[2];
     vtkMath::Normalize(motion_vector);
   }
   
   double disp_refSysCenter[3];
   
   ComputeWorldToDisplay(pivotPoint[0], pivotPoint[1], pivotPoint[2], 
                               disp_refSysCenter);
   
   double newAngle = 
     atan2((double)m_MousePointer2DPosition[1] - (double)disp_refSysCenter[1],
           (double)m_MousePointer2DPosition[0] - (double)disp_refSysCenter[0]);
 
   double oldAngle = 
     atan2((double)m_LastMousePointer2DPosition[1] - (double)disp_refSysCenter[1],
           (double)m_LastMousePointer2DPosition[0] - (double)disp_refSysCenter[0]);
   
   newAngle = vtkMath::DegreesFromRadians(newAngle);
   oldAngle = vtkMath::DegreesFromRadians(oldAngle);
   
   albaTransform t;
   t.Translate(-pivotPoint[0], -pivotPoint[1], -pivotPoint[2],POST_MULTIPLY);
   t.RotateWXYZ(newAngle - oldAngle, motion_vector,POST_MULTIPLY);
   t.Translate(pivotPoint[0], pivotPoint[1], pivotPoint[2],POST_MULTIPLY);
     
   //send the transform matrix
   SendTransformMatrix(t.GetMatrix());
}
