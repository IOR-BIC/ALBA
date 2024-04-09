/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: albaCameraTransform
 Authors: Marco Petrone
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "albaDecl.h"

#include "albaCameraTransform.h"

#include "albaMatrix.h"
#include "albaOBB.h"
#include "albaTransform.h"
#include "albaTransformFrame.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"

#include "vtkMath.h"
#include "vtkTransform.h"
#include "vtkCallbackCommand.h"

#include <assert.h>

//------------------------------------------------------------------------------
albaCxxTypeMacro(albaCameraTransform)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
albaCameraTransform::albaCameraTransform()
//------------------------------------------------------------------------------
{
  m_Renderer          = NULL;
  m_Camera            = NULL;
  m_FollowPosition    = 1;
  m_FollowOrientation = 1;
  m_FollowScale       = 1;
  m_OldViewAngle      = -1;
  m_OldDistance       = -1;

  albaNEW(m_OldViewMatrix);

  m_PositionMode      = ATTACH_TO_FOCAL_POINT;
  m_ScalingMode       = AUTO_FITTING;
  m_FittingMode       = FIT_Y;
  m_Bounds            = NULL;

  m_EventRouter = NULL;
  vtkNEW(m_EventRouter);
  m_EventRouter->SetCallback(InternalProcessEvents);
  m_EventRouter->SetClientData(this);
}

//------------------------------------------------------------------------------
albaCameraTransform::~albaCameraTransform()
//------------------------------------------------------------------------------
{
  SetRenderer(NULL);
  m_Camera=NULL;
  albaDEL(m_OldViewMatrix);

  vtkDEL(m_EventRouter);
}

//------------------------------------------------------------------------------
int albaCameraTransform::DeepCopy(albaCameraTransform *trans)
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
vtkMTimeType albaCameraTransform::GetMTime()
//------------------------------------------------------------------------------
{
	vtkMTimeType mtime = this->Superclass::GetMTime();

  if (m_Camera)
  {
		vtkMTimeType cameraMTime = m_Camera->GetMTime();
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

    if (delta_angle>.001 || !albaEquals(m_OldDistance,m_Camera->GetDistance()))
    {
      Modified();
      mtime = this->Superclass::GetMTime();
    }
  }

  if (m_Bounds)
  {
		vtkMTimeType boundsMTime = m_Bounds->GetMTime();
    if (boundsMTime > mtime)
      mtime=boundsMTime;
  }
  
  return mtime;
}

//------------------------------------------------------------------------------
void albaCameraTransform::SetRenderer(vtkRenderer *ren)
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
void albaCameraTransform::UpdatePoseMatrix(albaMatrix *matrix,albaMatrix *old_view_matrix, albaMatrix *new_view_matrix)
//------------------------------------------------------------------------------
{ 
  albaTransformFrame new_local_pose;
  new_local_pose.SetInputFrame(old_view_matrix);
  new_local_pose.SetTargetFrame(new_view_matrix);
  new_local_pose.SetInput(matrix);
  
  matrix->DeepCopy(new_local_pose.GetMatrixPointer());
}

//------------------------------------------------------------------------------
void albaCameraTransform::AutoPosition(albaMatrix *matrix,vtkRenderer *ren, int mode)
//------------------------------------------------------------------------------
{
   assert(ren);
   vtkCamera *camera=ren->GetActiveCamera();
   assert(camera);
   switch (mode)
   {
   case ATTACH_TO_FOCAL_POINT:
     albaTransform::SetPosition(*matrix,camera->GetFocalPoint());
     break;
   case ATTACH_TO_CAMERA:
     albaTransform::SetPosition(*matrix,camera->GetPosition());
     break;
   case ATTACH_TO_CLIPPING_PLANE:
     assert (false); // not supported yet
     break;
   };
}

//------------------------------------------------------------------------------
void albaCameraTransform::AutoOrientation(albaMatrix *matrix,vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  double *campos, *vup;
  double Rx[3], Ry[3], Rz[3];
  int i;
  double distance;
  double position[3];

  assert(matrix);
  assert(ren);

  vtkCamera *camera=ren->GetActiveCamera();
  assert(camera);

  albaTransform::GetPosition(*matrix,position);

  // do the rotation
  // first rotate y 
  campos = camera->GetPosition();
  vup = camera->GetViewUp();

  if (camera->GetParallelProjection())
  {
    camera->GetDirectionOfProjection(Rz);
    Rz[0] = -Rz[0];
    Rz[1] = -Rz[1];
    Rz[2] = -Rz[2];
  }
  else
  {
    distance = sqrt(
      (campos[0] - position[0])*(campos[0] - position[0]) +
      (campos[1] - position[1])*(campos[1] - position[1]) +
      (campos[2] - position[2])*(campos[2] - position[2]));
    for (i = 0; i < 3; i++)
    {
      Rz[i] = (campos[i] - position[i])/distance;
    }
  }

  vtkMath::Cross(vup,Rz,Rx);
  vtkMath::Normalize(Rx);
  vtkMath::Cross(Rz,Rx,Ry);

  // set the rotation matrix

  matrix->GetElements()[0][0] = Rx[0];
  matrix->GetElements()[1][0] = Rx[1];
  matrix->GetElements()[2][0] = Rx[2];
  matrix->GetElements()[0][1] = Ry[0];
  matrix->GetElements()[1][1] = Ry[1];
  matrix->GetElements()[2][1] = Ry[2];
  matrix->GetElements()[0][2] = Rz[0];
  matrix->GetElements()[1][2] = Rz[1];
  matrix->GetElements()[2][2] = Rz[2];

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
void albaCameraTransform::ComputeScaling(albaOBB *inBox, albaOBB *outBox,double *scale,int mode)
//------------------------------------------------------------------------------
{
  double insize[3],outsize[3];

  inBox->GetDimensions(insize);
  outBox->GetDimensions(outsize);

  // ratios between sizes
  double scalex=(outsize[0])/(insize[0]);
  double scaley=(outsize[1])/(insize[1]);
  double scalez=(outsize[2])/(insize[2]);
  double aspect=(insize[1])/(insize[0]);

  switch (mode)
  {
  case MIN_SCALE:
    {
      double minscale=GetMinScale(scalex,scaley);
      scale[0]=minscale;
      scale[1]=minscale;
      scale[2]=minscale;
    }
    break;
  case MAX_SCALE:
    {
	    double maxscale=GetMaxScale(scalex,scaley);
	    scale[0]=maxscale;
	    scale[1]=maxscale;
	    scale[2]=maxscale;
    }
    break;
    case ANISOTROPIC:
      {
	      scale[0]=scalex;
	      scale[1]=scaley;
	      scale[2]=scalez;
      }
    break;
    case FIT_X:
    {
	    scale[0]=scalex;
	    scale[1]=scalex;
	    scale[2]=scalex;
    }
    break;
    case FIT_Y:
    {
	    scale[0]=scaley;
	    scale[1]=scaley;
	    scale[2]=scaley;
    }
    break;
  }
}


//------------------------------------------------------------------------------
void albaCameraTransform::AutoFitting(albaMatrix *matrix,albaOBB *tracked_bounds,vtkRenderer *ren,int mode)
//------------------------------------------------------------------------------
{
  vtkCamera *camera=ren->GetActiveCamera();

  double vangle;
  vangle=camera->GetViewAngle();

  double fpoint[3];
  camera->GetFocalPoint(fpoint);

  double center[3];
  tracked_bounds->GetCenter(center);

  //transform->Translate(matrix,fpoint[0]-center[0], fpoint[1]-center[1], fpoint[2]-center[2]);

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

    //dims[1] = 1.9*crange[0]*tan((vangle/2)*(3.1415/180));
    dims[1] = 1.9*distance*tan((vangle/2)*(3.1415/180));

    float wsize[2];
  
    wsize[0] = (float)ren->GetRenderWindow()->GetSize()[0];
    wsize[1] = (float)ren->GetRenderWindow()->GetSize()[1];
  
    dims[0] =(wsize[0]/wsize[1])*(dims[1]);
    dims[2] = dims[0];
  }

  albaOBB out_bounds;
  out_bounds.SetDimensions(dims);
  
  double scale[3];
  ComputeScaling(tracked_bounds,&out_bounds,scale,mode);

  albaTransform::Scale(*matrix,scale[0],scale[1],scale[2],PRE_MULTIPLY);
}

//------------------------------------------------------------------------------
void albaCameraTransform::AutoFitting2(albaMatrix *matrix,albaOBB *tracked_bounds,vtkRenderer *ren,int mode)
//------------------------------------------------------------------------------
{
  // We want to scale the tracked box to make the upper bound fall
  // on the frustum upper plane. Being 'd' the focal distance 
  // Zw the coordinate of the the front scaled box along the
  // Z axis with origin in the focal point and directed toward
  // the camera.
  //               \ Yw  
  //                 \
  //                 | \
  //                 |   \
  //                 |     \
  //                 |       \
  //                 |         \
  //                 |           \
  //   o-------------+-------------o
  //   F=0           Zw            d=camera to focal point distance
  //
  //
  // This imposes the following
  //   Yw = c * (d - Zw)
  // 
  // where Yw and Zw are the scaled Y and Z dimensions of the tracked box
  // to make it fit the fov Y direction
  //
  // Since we want to try keeping the original aspect of the tracked box
  // we also impose
  //
  // Yw = k * Zw
  //
  // where k = Yt / Zt
  //
  // and where Yt and Zt are the original tracked box dimensions
  // 
  //
  // This couple of equations lead to the solution:
  //
  // Zw = c * d / ( k + c )
  // Yw = k * c * d / ( k + c)
  //
  // We want also to clamp the Zw to be comprised between (.2 * d) and (0.8 * d)
  // and this leads to the following:
  // 
  // c < 4 * K   and   K < 4 * c
  // 
  // therefore 
  // if (k > c/4) c = 4 * k
  // if (k < 4*c) k = 4 * c
  
  double tdims[3];
  tracked_bounds->GetDimensions(tdims);

  // aspect ratio between Y and Z dimensions of the tracked box
  double k=tdims[1]/tdims[2];

  double w=tdims[1]/tdims[0]; // tracked box YX aspect ration

  vtkCamera *camera=ren->GetActiveCamera();

  double vangle;
  vangle=camera->GetViewAngle(); // field of view Y Angle
  
  double c=tan(vtkMath::RadiansFromDegrees(vangle/2));

  double d=camera->GetDistance();
  
  if (k > c/4) c = 4 * k;
  if (k < 4*c) k = 4 * c;

  double dims[3];

  dims[1] = k * c * d / (k + c);
  dims[2] = c * d / (k + c);
  dims[0] = dims[1] / w ; // keep the YX aspect ration

  albaOBB out_bounds;
  out_bounds.SetDimensions(dims);

  double scale[3];
  ComputeScaling(tracked_bounds,&out_bounds,scale,mode);

  albaTransform::Scale(*matrix,scale[0],scale[1],scale[2],PRE_MULTIPLY);
}
//----------------------------------------------------------------------------
void albaCameraTransform::RecomputeAll()
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


  if (m_FollowPosition)
    AutoPosition();

  if (m_FollowOrientation)
    AutoOrientation();

  if (m_FollowScale&&m_Bounds->IsValid())
  {
    switch (m_ScalingMode)
    {
    case AUTO_FITTING:
      //AutoFitting2();
      AutoFitting();
      break;
    case PRESERVE_SIZE:
      // not yet implemented
      assert(false);
      break;
    }
  }
  Modified();
}

//----------------------------------------------------------------------------
void albaCameraTransform::InternalUpdate()
//----------------------------------------------------------------------------
{
  if (!m_Renderer)
    return;
  
  vtkCamera *camera=m_Renderer->GetActiveCamera();
  if (!m_Camera)
    return;

  // try to not recompute everything if the camera has only moved
  // in space.
  double delta_angle = fabs(m_OldViewAngle - m_Camera->GetViewAngle());    

  if (delta_angle>.001 || !albaEquals(m_OldDistance,m_Camera->GetDistance()))
  {
    RecomputeAll();    
  }
  else
  {
    // simply follow view's transform changes    
    albaMatrix view_trans=camera->GetViewTransformMatrix();
    UpdatePoseMatrix(m_Matrix,m_OldViewMatrix,&view_trans);

    m_OldViewMatrix->DeepCopy(camera->GetViewTransformMatrix());
  }

  InvokeEvent(this, MATRIX_UPDATED);
}

//------------------------------------------------------------------------------
void albaCameraTransform::InternalProcessEvents(vtkObject* sender, unsigned long id, void* clientdata, void* calldata)
//------------------------------------------------------------------------------
{
  albaCameraTransform* self = reinterpret_cast<albaCameraTransform *>( clientdata );
  if (sender==self->m_Renderer)
  {
    switch (id)
    {
    case vtkCommand::ResetCameraEvent:
      self->RecomputeAll();
      break;
//    case vtkCommand::ResetCameraClippingRangeEvent:
//      self->RecomputeAll();
      break;
    case vtkCommand::StartEvent:
      break;
    case vtkCommand::EndEvent:
      break;
    }


  }
}
