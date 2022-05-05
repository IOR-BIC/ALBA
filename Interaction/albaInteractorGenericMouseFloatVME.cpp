/*=========================================================================
Program:   Multimod Application Framework
Module:    $RCSfile: appInteractorGenericMouse.cpp,v $
Language:  C++
Date:      $Date: 2011-05-27 07:52:12 $
Version:   $Revision: 1.1.2.2 $
Authors:   Gianluigi Crimi, Nicola Vanella
==========================================================================
Copyright (c) 2010
CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/

#include "albaInteractorGenericMouseFloatVME.h"

#include "albaDecl.h"
#include "albaDevice.h"
#include "albaRefSys.h"
#include "albaMatrix.h"
#include "albaTransform.h"
#include "albaVect3d.h"

#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"

albaCxxTypeMacro(albaInteractorGenericMouseFloatVME);

// ----------------------------------------------------------------------------
albaInteractorGenericMouseFloatVME::albaInteractorGenericMouseFloatVME() 
{
	m_Modality2D = false;
}

// ----------------------------------------------------------------------------
albaInteractorGenericMouseFloatVME::~albaInteractorGenericMouseFloatVME() 
{ }

//----------------------------------------------------------------------------
void albaInteractorGenericMouseFloatVME::TrackballRotate()
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
	double angleX = newVectX.AngleBetweenVectors(oldVect)* vtkMath::RadiansToDegrees();
	if (newPos[0] - oldPos[0] > 0)
		angleX = -angleX;

	//Calculate Y Angle
	double angleY = newVectY.AngleBetweenVectors(oldVect)* vtkMath::RadiansToDegrees();
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
void albaInteractorGenericMouseFloatVME::TrackballRoll()
{
   // get the rotation refsys
   m_ConstrainRefSys = GetRotationConstraint()->GetRefSys()->GetMatrix();  

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
   
   newAngle *= vtkMath::RadiansToDegrees();
   oldAngle *= vtkMath::RadiansToDegrees();
   
   albaTransform t;
   t.Translate(-pivotPoint[0], -pivotPoint[1], -pivotPoint[2],POST_MULTIPLY);
   t.RotateWXYZ(newAngle - oldAngle, motion_vector,POST_MULTIPLY);
   t.Translate(pivotPoint[0], pivotPoint[1], pivotPoint[2],POST_MULTIPLY);
     
   //send the transform matrix
   SendTransformMatrix(t.GetMatrix());
}