/*========================================================================
  Program:   Multimod Application Framework
  Module:    $RCSfile: mafCameraTransform.cpp,v $
  Language:  C++
  Date:      $Date: 2005-04-30 14:34:55 $
  Version:   $Revision: 1.1 $
  Authors:   Marco Petrone
==========================================================================
  Copyright (c) 2002/2004 
  CINECA - Interuniversity Consortium (www.cineca.it)
=========================================================================*/
#include "mafCameraTransform.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkMatrix4x4.h"
#include "vtkMath.h"
#include "vtkTransform.h"
#include "mflBounds.h"
#include "vtkCallbackCommand.h"

#include <assert.h>

//------------------------------------------------------------------------------
mafCameraTransform::mafCameraTransform()
//------------------------------------------------------------------------------
{
  Renderer          = NULL;
  Camera            = NULL;
  FollowPosition    = 1;
  FollowOrientation = 1;
  FollowScale       = 1;
  OldViewAngle      = -1;
  OldDistance       = -1;

  vtkNEW(CameraFrame);
  vtkNEW(OldViewMatrix);

  PositionMode      = ATTACH_TO_FOCAL_POINT;
  ScalingMode       = AUTO_FITTING;
  FittingMode       = FIT_Y;
  Bounds            = NULL;

  EventRouter = NULL;
  vtkNEW(EventRouter);
  EventRouter->SetCallback(InternalProcessEvents);
  EventRouter->SetClientData(this);
}

//------------------------------------------------------------------------------
mafCameraTransform::~mafCameraTransform()
//------------------------------------------------------------------------------
{
  SetRenderer(NULL);
  Camera=NULL;
  vtkDEL(CameraFrame);
  vtkDEL(OldViewMatrix);
  vtkDEL(EventRouter);
}

//------------------------------------------------------------------------------
int mafCameraTransform::DeepCopy(mafCameraTransform *trans)
//------------------------------------------------------------------------------
{
  if (trans->IsA(GetClassName()))
  {
    SetRenderer(trans->GetRenderer());
    return VTK_OK;
  }

  return VTK_ERROR;
}


//----------------------------------------------------------------------------
// Get the MTime. Take in consideration Renderer, Camera and Bounds modification time
unsigned long mafCameraTransform::GetMTime()
//------------------------------------------------------------------------------
{
  unsigned long mtime = this->Superclass::GetMTime();

  if (Camera)
  {
    unsigned long cameraMTime = Camera->GetMTime();
    if (cameraMTime > mtime)
      mtime=cameraMTime;
  }
  
  //if (Renderer)
  //{
  //  unsigned long renMTime = Renderer->GetMTime();
  //  if (renMTime > mtime)
  //    mtime=renMTime;
  //}

  if (Renderer&&Camera!=Renderer->GetActiveCamera())
  {
    Modified();
    mtime = this->Superclass::GetMTime();
  }

  if (Bounds)
  {
    unsigned long boundsMTime = Bounds->GetMTime();
    if (boundsMTime > mtime)
      mtime=boundsMTime;
  }
  
  return mtime;
}

//------------------------------------------------------------------------------
void mafCameraTransform::SetRenderer(vtkRenderer *ren)
//------------------------------------------------------------------------------
{
  if (Renderer)
    Renderer->RemoveObserver(EventRouter);

  vtkSetObjectBodyMacro(Renderer,vtkRenderer,ren);
  Camera=NULL;

  if (Renderer)
  {
    Renderer->AddObserver(vtkCommand::ResetCameraEvent,EventRouter);
    Renderer->AddObserver(vtkCommand::ResetCameraClippingRangeEvent,EventRouter);
    Renderer->AddObserver(vtkCommand::StartEvent,EventRouter);
    Renderer->AddObserver(vtkCommand::EndEvent,EventRouter);
    RecomputeAll();
  }
}

//------------------------------------------------------------------------------
void mafCameraTransform::UpdatePoseMatrix(vtkMatrix4x4 *matrix,vtkMatrix4x4 *old_view_matrix, vtkMatrix4x4 *new_view_matrix)
//------------------------------------------------------------------------------
{ 
  mflSmartPointer<mflTransform> new_local_pose;
  new_local_pose->SetInputFrame(old_view_matrix);
  new_local_pose->SetTargetFrame(new_view_matrix);
  new_local_pose->SetInput(matrix);
  
  matrix->DeepCopy(new_local_pose->GetMatrix());
}

//------------------------------------------------------------------------------
void mafCameraTransform::AutoPosition(vtkMatrix4x4 *matrix,vtkRenderer *ren, int mode)
//------------------------------------------------------------------------------
{
   assert(ren);
   vtkCamera *camera=ren->GetActiveCamera();
   assert(camera);
   switch (mode)
   {
   case ATTACH_TO_FOCAL_POINT:
     mflTransform::SetPosition(matrix,camera->GetFocalPoint());
     break;
   case ATTACH_TO_CAMERA:
     mflTransform::SetPosition(matrix,camera->GetPosition());
     break;
   case ATTACH_TO_CLIPPING_PLANE:
     assert (false); // not supported yet
     break;
   };
}

//------------------------------------------------------------------------------
void mafCameraTransform::AutoOrientation(vtkMatrix4x4 *matrix,vtkRenderer *ren)
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

  mflTransform::GetPosition(matrix,position);

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

  matrix->Element[0][0] = Rx[0];
  matrix->Element[1][0] = Rx[1];
  matrix->Element[2][0] = Rx[2];
  matrix->Element[0][1] = Ry[0];
  matrix->Element[1][1] = Ry[1];
  matrix->Element[2][1] = Ry[2];
  matrix->Element[0][2] = Rz[0];
  matrix->Element[1][2] = Rz[1];
  matrix->Element[2][2] = Rz[2];

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
void mafCameraTransform::ComputeScaling(mflBounds *inBox, mflBounds *outBox,double *scale,int mode)
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
void mafCameraTransform::AutoFitting(vtkMatrix4x4 *matrix,mflBounds *tracked_bounds,vtkRenderer *ren,int mode)
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

  mflBounds out_bounds;
  out_bounds.SetDimensions(dims);
  
  double scale[3];
  ComputeScaling(tracked_bounds,&out_bounds,scale,mode);

  mflTransform::Scale(matrix,scale[0],scale[1],scale[2],PRE_MULTIPLY);
}

//------------------------------------------------------------------------------
void mafCameraTransform::AutoFitting2(vtkMatrix4x4 *matrix,mflBounds *tracked_bounds,vtkRenderer *ren,int mode)
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
  
  double c=tan(vangle/2*vtkMath::DegreesToRadians());

  double d=camera->GetDistance();
  
  if (k > c/4) c = 4 * k;
  if (k < 4*c) k = 4 * c;

  double dims[3];

  dims[1] = k * c * d / (k + c);
  dims[2] = c * d / (k + c);
  dims[0] = dims[1] / w ; // keep the YX aspect ration

  mflBounds out_bounds;
  out_bounds.SetDimensions(dims);

  double scale[3];
  ComputeScaling(tracked_bounds,&out_bounds,scale,mode);

  mflTransform::Scale(matrix,scale[0],scale[1],scale[2],PRE_MULTIPLY);

  
  /*double scalex=x_dim/tdims[0];
  double scaley=y_dim/tdims[1];
  double scalez=z_dim/tdims[2];

  mflTransform::Scale(matrix,scalex,scaley,scalez);
  */
}
//----------------------------------------------------------------------------
void mafCameraTransform::RecomputeAll()
//----------------------------------------------------------------------------
{
  if (!Renderer)
    return;

  Camera=Renderer->GetActiveCamera(); // store active camera

  if (!Camera)
    return;

  OldViewAngle=Camera->GetViewAngle(); // store current view angle
  OldDistance=Camera->GetDistance(); 
  OldViewMatrix->DeepCopy(Camera->GetViewTransformMatrix());

  Matrix->Identity();

  if (!Bounds)
    return;


  if (FollowPosition)
    AutoPosition();

  if (FollowOrientation)
    AutoOrientation();

  if (FollowScale&&Bounds->IsValid())
  {
    switch (ScalingMode)
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
void mafCameraTransform::InternalUpdate()
//----------------------------------------------------------------------------
{
  if (!Renderer)
    return;
  
  vtkCamera *camera=Renderer->GetActiveCamera();
  if (!Camera)
    return;

  //double crange[2];
  //camera->GetClippingRange(crange);

  // try to not recompute everything if the camera has only moved
  // in space.
  //if (Camera!=camera||OldViewAngle!=camera->GetViewAngle()||OldDistance!=camera->GetDistance())
  if (Camera!=camera/*||fabs(OldViewAngle-camera->GetViewAngle())>.001*/)
  {
    RecomputeAll();    
  }
  else
  {
    // simply follow view's transform changes    

    UpdatePoseMatrix(Matrix,OldViewMatrix,camera->GetViewTransformMatrix());
    /*mflSmartPointer<vtkMatrix4x4> new_pose;    

    OldViewMatrix->Invert();
    vtkMatrix4x4::Multiply4x4(OldViewMatrix,camera->GetViewTransformMatrix(), \
      new_pose);
    new_pose->Invert();
    vtkMatrix4x4::Multiply4x4(Matrix,new_pose,new_pose);
    /*if (FollowPosition)
    {
      // copy the translation vector
      CopyTranslation(new_pose);
    }

    if (FollowOrientation)
    {
      // copy the 3x3 matrix. Notice the view transform doesn't scale!!!
      CopyRotation(new_pose);
    }
    */   

    //Matrix->DeepCopy(new_pose);
    OldViewMatrix->DeepCopy(camera->GetViewTransformMatrix());
  }

  InvokeEvent(UpdateEvent);
}

//------------------------------------------------------------------------------
void mafCameraTransform::InternalProcessEvents(vtkObject* sender, unsigned long id, void* clientdata, void* calldata)
//------------------------------------------------------------------------------
{
  mafCameraTransform* self = reinterpret_cast<mafCameraTransform *>( clientdata );
  if (sender==self->Renderer)
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