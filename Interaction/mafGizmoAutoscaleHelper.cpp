/*=========================================================================

 Program: MAF2
 Module: mafGizmoAutoscaleHelper
 Authors: Stefano Perticoni
 
 Copyright (c) B3C
 All rights reserved. See Copyright.txt or
 http://www.scsitaly.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "mafDecl.h"
#include "mafGizmoAutoscaleHelper.h"

#include "mafMatrix.h"
#include "mafOBB.h"
#include "mafTransform.h"
#include "mafTransformFrame.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"

#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkCallbackCommand.h"

#include <assert.h>

#include "vtkActor.h"
#include "vtkMAFSmartPointer.h"
#include "vtkDataSet.h"

#include "mafVME.h"
#include "mafGizmoInterface.h"
#include "mafVMEGizmo.h"

//------------------------------------------------------------------------------
mafCxxTypeMacro(mafGizmoAutoscaleHelper)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
mafGizmoAutoscaleHelper::mafGizmoAutoscaleHelper()
//------------------------------------------------------------------------------
{
	m_Renderer          = NULL;
	m_Camera            = NULL;
	m_FollowScale       = true;
	m_OldViewAngle      = -1;
	m_OldDistance       = -1;

	mafNEW(m_OldViewMatrix);

	double bounds[6] = {-1,1,-1,1,-1,1};

	m_Bounds = NULL;
	m_Bounds = new mafOBB;
	m_Bounds->DeepCopy(bounds);

	m_EventRouter = NULL;
	vtkNEW(m_EventRouter);
	m_EventRouter->SetCallback(InternalProcessEvents);
	m_EventRouter->SetClientData(this);

	m_Actor = NULL;
	m_VME = NULL;
	m_RenderWindowHeightPercentage = 0.3;
}

//------------------------------------------------------------------------------
mafGizmoAutoscaleHelper::~mafGizmoAutoscaleHelper()
//------------------------------------------------------------------------------
{
	SetRenderer(NULL);
	m_Camera=NULL;
	mafDEL(m_OldViewMatrix);
	vtkDEL(m_EventRouter);
	cppDEL(m_Bounds);
}

//------------------------------------------------------------------------------
int mafGizmoAutoscaleHelper::DeepCopy(mafGizmoAutoscaleHelper *trans)
//------------------------------------------------------------------------------
{
	if (trans->IsA(GetTypeId()))
	{
		SetRenderer(trans->GetRenderer());
		return VTK_OK;
	}

	return VTK_ERROR;
}


//----------------------------------------------------------------------------
// Get the MTime. Take in consideration m_Renderer, m_Camera and m_Bounds modification time
unsigned long mafGizmoAutoscaleHelper::GetMTime()
//------------------------------------------------------------------------------
{
	unsigned long mtime = this->Superclass::GetMTime();

	if (m_Camera)
	{
		unsigned long cameraMTime = m_Camera->GetMTime();
		if (cameraMTime > mtime)
			mtime=cameraMTime;
	}

	if (m_Renderer&&m_Camera!=m_Renderer->GetActiveCamera())
	{
		Modified();
		mtime = this->Superclass::GetMTime();
	}

	if (m_Camera)
	{
		double delta_angle = fabs(m_OldViewAngle - m_Camera->GetViewAngle());    

		if (delta_angle>.001 || !mafEquals(m_OldDistance,m_Camera->GetDistance()))
		{
			Modified();
			mtime = this->Superclass::GetMTime();
		}
	}

	if (m_Bounds)
	{
		unsigned long boundsMTime = m_Bounds->GetMTime();
		if (boundsMTime > mtime)
			mtime=boundsMTime;
	}

	return mtime;
}

//------------------------------------------------------------------------------
void mafGizmoAutoscaleHelper::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
	if (m_Renderer==ren)
		return;

	if (m_Renderer)
		m_Renderer->RemoveObserver(m_EventRouter);

	vtkDEL(m_Renderer);
	m_Camera=NULL;

	m_Renderer=ren;

	if (m_Renderer)
	{
		m_Renderer->Register(NULL);
		m_Renderer->AddObserver(vtkCommand::ResetCameraEvent,m_EventRouter);
		m_Renderer->AddObserver(vtkCommand::ResetCameraClippingRangeEvent,m_EventRouter);
		m_Renderer->AddObserver(vtkCommand::StartEvent,m_EventRouter);
		m_Renderer->AddObserver(vtkCommand::EndEvent,m_EventRouter);
		RecomputeAll();
	}
}

//------------------------------------------------------------------------------
void mafGizmoAutoscaleHelper::UpdatePoseMatrix(mafMatrix *matrix,mafMatrix *old_view_matrix, mafMatrix *new_view_matrix)
//------------------------------------------------------------------------------
{ 
	mafTransformFrame new_local_pose;
	new_local_pose.SetInputFrame(old_view_matrix);
	new_local_pose.SetTargetFrame(new_view_matrix);
	new_local_pose.SetInput(matrix);

	matrix->DeepCopy(new_local_pose.GetMatrixPointer());
}



//------------------------------------------------------------------------------
// Return the minimum scale among the 3 scale values for X,Y and Z axes.
inline float GetMinScale(float scalex,float scaley)
//------------------------------------------------------------------------------
{
	return (scalex<=scaley)?scalex:scaley;
}

//------------------------------------------------------------------------------
// Return the maximum scale among the 3 scale values for X,Y and Z axes.
inline float GetMaxScale(float scalex,float scaley)
//------------------------------------------------------------------------------
{
	return (scalex>=scaley)?scalex:scaley;
}

//------------------------------------------------------------------------------
void mafGizmoAutoscaleHelper::ComputeScaling(mafOBB *inBox, mafOBB *outBox,double *scale)
//------------------------------------------------------------------------------
{
	double insize[3],outsize[3];

	inBox->GetDimensions(insize);
	outBox->GetDimensions(outsize);

	// ratios between sizes
	double scaley=(outsize[1])/(insize[1]);

	scale[0]=scaley;
	scale[1]=scaley;
	scale[2]=scaley;
}

//------------------------------------------------------------------------------
void mafGizmoAutoscaleHelper::AutoFitting(mafMatrix *matrix,mafOBB *tracked_bounds,vtkRenderer *ren)
//------------------------------------------------------------------------------
{
	vtkCamera *camera=ren->GetActiveCamera();

	double vangle;
	vangle=camera->GetViewAngle();

	double fpoint[3];
	camera->GetFocalPoint(fpoint);

	double center[3];
	tracked_bounds->GetCenter(center);

	// Calculate new dimensions
	double dims[3];

	// consider both the parallel and projective cases
	if (camera->GetParallelProjection())
	{
		dims[0] = 2.2*camera->GetParallelScale();

		float wsize[2];

		wsize[0] = (float)ren->GetRenderWindow()->GetSize()[0];
		wsize[1] = (float)ren->GetRenderWindow()->GetSize()[1];

		dims[1] =(dims[0])/(wsize[0]/wsize[1]);
		dims[2] = dims[0];
	}
	else
	{
		double crange[2];
		camera->GetClippingRange(crange);

		double distance=camera->GetDistance();

		dims[1] = 1.9*distance*tan((vangle/2)*(3.1415/180));

		float wsize[2];

		wsize[0] = (float)ren->GetRenderWindow()->GetSize()[0];
		wsize[1] = (float)ren->GetRenderWindow()->GetSize()[1];

		dims[0] =(wsize[0]/wsize[1])*(dims[1]);
		dims[2] = dims[0];
	}

	mafOBB out_bounds;
	out_bounds.SetDimensions(dims);

	double scale[3];
	ComputeScaling(tracked_bounds,&out_bounds,scale);

	mafTransform::Scale(*matrix,scale[0],scale[1],scale[2],PRE_MULTIPLY);
}

//----------------------------------------------------------------------------
void mafGizmoAutoscaleHelper::RecomputeAll()
//----------------------------------------------------------------------------
{
	if (!m_Renderer)
		return;

	m_Camera=m_Renderer->GetActiveCamera(); // store active camera

	if (!m_Camera)
		return;

	m_OldViewAngle=m_Camera->GetViewAngle(); // store current view angle
	m_OldDistance=m_Camera->GetDistance(); 
	m_OldViewMatrix->DeepCopy(m_Camera->GetViewTransformMatrix());

	m_Matrix->Identity();

	if (!m_Bounds)
		return;

	if (m_FollowScale&&m_Bounds->IsValid())
	{
	  AutoFitting();
	}
	
	Modified();
}

//----------------------------------------------------------------------------
void mafGizmoAutoscaleHelper::InternalUpdate()
//----------------------------------------------------------------------------
{
	if (!m_Renderer)
		return;

	vtkCamera *camera=m_Renderer->GetActiveCamera();
	if (!m_Camera)
		return;
	
	RecomputeAll();    

	InvokeEvent(MATRIX_UPDATED);
}

//------------------------------------------------------------------------------
void mafGizmoAutoscaleHelper::InternalProcessEvents(vtkObject* sender, unsigned long id, void* clientdata, void* calldata)
//------------------------------------------------------------------------------
{

	mafGizmoAutoscaleHelper* self = reinterpret_cast<mafGizmoAutoscaleHelper *>( clientdata );

	if (sender==self->m_Renderer)
	{
		switch (id)
		{
		case vtkCommand::ResetCameraEvent:
			self->RecomputeAll();
			break;

		case vtkCommand::StartEvent:
			if (self->m_Actor)
			{
				mafVMEGizmo *vmeGizmo = NULL;
				vmeGizmo = mafVMEGizmo::SafeDownCast(self->m_VME);
				assert(vmeGizmo);

				self->InternalUpdate();
				vtkMAFSmartPointer<vtkTransform> tr;

				double scale[3];

				const mafMatrix tmp = self->m_Matrix->GetVTKMatrix();
				mafTransform::GetScale(tmp, scale);

				vtkMAFSmartPointer<vtkTransform> scaleM;
				scaleM->Scale(scale);		

				mafGizmoInterface *gizmoInterface = NULL;
				gizmoInterface = dynamic_cast<mafGizmoInterface *>(vmeGizmo->GetMediator());

				if (gizmoInterface == NULL)
				{
					return;
				}

				assert(gizmoInterface);

				double lenght = sqrt((self->m_VMEBounds[1]-self->m_VMEBounds[0])*(self->m_VMEBounds[1]-self->m_VMEBounds[0]) 
					+ (self->m_VMEBounds[3]-self->m_VMEBounds[2])*(self->m_VMEBounds[3]-self->m_VMEBounds[2]) + 
					(self->m_VMEBounds[5] - self->m_VMEBounds[4])*(self->m_VMEBounds[5] - self->m_VMEBounds[4]) );

				double yRenderWindowPercentage = gizmoInterface->GetRenderWindowHeightPercentage();

				double euristicFactor = 5;
				double dummyScale = yRenderWindowPercentage * euristicFactor / lenght;

				scaleM->Scale(dummyScale, dummyScale , dummyScale);			
				scaleM->Update();

				self->m_Actor->SetUserMatrix(scaleM->GetMatrix());
			}
			break;

		case vtkCommand::EndEvent:
			assert(true);
			break;
		}
	}
}

void mafGizmoAutoscaleHelper::SetVME( mafVME *vme )
{
	m_VME = vme;

	mafVMEGizmo *vmeGizmo = NULL;
	vmeGizmo = mafVMEGizmo::SafeDownCast(m_VME);

	if (vmeGizmo == NULL)
	{
		return;
	}

	mafGizmoInterface *gizmoMediator = NULL;
	gizmoMediator = dynamic_cast<mafGizmoInterface *>(vmeGizmo->GetMediator());

	if (gizmoMediator == NULL)
	{
		return;
	}

	mafVME *inputVME = gizmoMediator->GetInput();

	if (inputVME->GetVisibleToTraverse() == false)
	{
		// need this to compute the bounds
		inputVME->SetVisibleToTraverse(true);

		// compute the bounds
		inputVME->GetOutput()->GetBounds(m_VMEBounds);

		// roll back
		inputVME->SetVisibleToTraverse(true);

	}
	else
	{
		// simply compute the bounds
		inputVME->GetOutput()->GetBounds(m_VMEBounds);
	}
	
}
