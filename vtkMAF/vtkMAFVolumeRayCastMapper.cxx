/*========================================================================= 
Program: Multimod Application Framework RELOADED 
Module: $RCSfile: vtkMAFVolumeRayCastMapper.cxx,v $ 
Language: C++ 
Date: $Date: 2011-05-26 08:33:31 $ 
Version: $Revision: 1.1.2.3 $ 
Authors: Josef Kohout (Josef.Kohout *AT* beds.ac.uk)
========================================================================== 
Copyright (c) 2008 University of Bedfordshire (www.beds.ac.uk)
See the COPYINGS file for license details 
=========================================================================
*/

#include "vtkMAFVolumeRayCastMapper.h"
#include "vtkObjectFactory.h"
#include "vtkPointData.h"
#include "vtkTimerLog.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkTransform.h"
#include "vtkMatrix4x4.h"
#include "vtkMultiThreader.h"
#include "vtkImageData.h"
#include "vtkVolumeProperty.h"
#include "gl/gl.h"

vtkCxxRevisionMacro(vtkMAFVolumeRayCastMapper, "$Revision: 1.1.2.3 $");
vtkStandardNewMacro(vtkMAFVolumeRayCastMapper);

#include "mafMemDbg.h"

//#define __PROFILING__
//#include <atlbase.h>
//#include <BSGenLib.h>
//
//__PROFILING_DECLARE_DEFAULT_PROFILER(false);


//------------------------------------------------------------------------
// WARNING: INTERNAL METHOD - NOT INTENDED FOR GENERAL USE
// Initialize rendering for this volume.
// N.B. Code mostly adopted from vtkVolumeRayCastMapper::Render
// See the description of class in .H file for details
/*virtual*/ void vtkMAFVolumeRayCastMapper::Render( vtkRenderer *ren, vtkVolume *vol )
//------------------------------------------------------------------------
{
//  PROFILE_THIS_FUNCTION();
//  PROFILE_TIMER_START(c1);
#pragma region VTK Code (Initialization, boundary computation, camera adjustment)
  // make sure that we have scalar input and update the scalar input
  if ( this->GetInput() == NULL ) 
  {
    vtkErrorMacro(<< "No Input!");
    return;
  }
  else
  {
    this->GetInput()->UpdateInformation();
    this->GetInput()->SetUpdateExtentToWholeExtent();
    this->GetInput()->Update();
  } 


  int scalarType = this->GetInput()->GetPointData()->GetScalars()->GetDataType();
  if (scalarType != VTK_UNSIGNED_SHORT && scalarType != VTK_UNSIGNED_CHAR)
  {
    vtkErrorMacro ("Cannot volume render data of type " 
      << vtkImageScalarTypeNameMacro(scalarType) 
      << ", only unsigned char or unsigned short.");
    return;
  }

  // Start timing now. We didn't want to capture the update of the
  // input data in the times
  this->Timer->StartTimer();

  this->ConvertCroppingRegionPlanesToVoxels();

  this->UpdateShadingTables( ren, vol );

  // This is the input of this mapper
  vtkImageData *input = this->GetInput();

  // Get the camera from the renderer
  vtkCamera *cam = ren->GetActiveCamera();

  // Get the aspect ratio from the renderer. This is needed for the
  // computation of the perspective matrix
  ren->ComputeAspect();
  double *aspect = ren->GetAspect();

  // Keep track of the projection matrix - we'll need it in a couple of
  // places Get the projection matrix. The method is called perspective, but
  // the matrix is valid for perspective and parallel viewing transforms.
  // Don't replace this with the GetCompositePerspectiveTransformMatrix
  // because that turns off stereo rendering!!!
  this->PerspectiveTransform->Identity();
  this->PerspectiveTransform->Concatenate(
    cam->GetPerspectiveTransformMatrix(aspect[0]/aspect[1],0.0, 1.0 ));
  this->PerspectiveTransform->Concatenate(cam->GetViewTransformMatrix());
  this->PerspectiveMatrix->DeepCopy(this->PerspectiveTransform->GetMatrix());

  // Compute some matrices from voxels to view and vice versa based 
  // on the whole input
  this->ComputeMatrices( input, vol );


  // How big is the viewport in pixels?
  double *viewport   =  ren->GetViewport();
  int *renWinSize   =  ren->GetRenderWindow()->GetSize();

  // Save this so that we can restore it if the image is cancelled
  double oldImageSampleDistance = this->ImageSampleDistance;

  // If we are automatically adjusting the size to achieve a desired frame
  // rate, then do that adjustment here. Base the new image sample distance 
  // on the previous one and the previous render time. Don't let
  // the adjusted image sample distance be less than the minimum image sample 
  // distance or more than the maximum image sample distance.
  if ( this->AutoAdjustSampleDistances )
  {
    float oldTime = this->RetrieveRenderTime( ren, vol );
    float newTime = vol->GetAllocatedRenderTime();
    this->ImageSampleDistance *= sqrt(oldTime / newTime);
    this->ImageSampleDistance = 
      (this->ImageSampleDistance>this->MaximumImageSampleDistance)?
      (this->MaximumImageSampleDistance):(this->ImageSampleDistance);
    this->ImageSampleDistance = 
      (this->ImageSampleDistance<this->MinimumImageSampleDistance)?
      (this->MinimumImageSampleDistance):(this->ImageSampleDistance);
  }

  // The full image fills the viewport. First, compute the actual viewport
  // size, then divide by the ImageSampleDistance to find the full image
  // size in pixels
  int width, height;
  ren->GetTiledSize(&width, &height);
  this->ImageViewportSize[0] = 
    static_cast<int>(width/this->ImageSampleDistance);
  this->ImageViewportSize[1] = 
    static_cast<int>(height/this->ImageSampleDistance);

//  PROFILE_TIMER_STOP(c1);
//  PROFILE_TIMER_START(c2);

  // Compute row bounds. This will also compute the size of the image to
  // render, allocate the space if necessary, and clear the image where
  // required
  if ( this->ComputeRowBounds( vol, ren ) )
  {

//    PROFILE_TIMER_START(c2_1);

    vtkVolumeRayCastStaticInfo *staticInfo = new vtkVolumeRayCastStaticInfo;
    staticInfo->ClippingPlane = NULL;
    staticInfo->Volume = vol;
    staticInfo->Renderer = ren;
    staticInfo->ScalarDataPointer = 
      this->GetInput()->GetPointData()->GetScalars()->GetVoidPointer(0);
    staticInfo->ScalarDataType = 
      this->GetInput()->GetPointData()->GetScalars()->GetDataType();

    // Do we need to capture the z buffer to intermix intersecting
    // geometry? If so, do it here
    if ( this->IntermixIntersectingGeometry && 
      ren->GetNumberOfPropsRendered() )
    {
      int x1, x2, y1, y2;

      // turn this->ImageOrigin into (x1,y1) in window (not viewport!)
      // coordinates. 
      x1 = static_cast<int> (
        viewport[0] * static_cast<double>(renWinSize[0]) +
        static_cast<double>(this->ImageOrigin[0]) * this->ImageSampleDistance );
      y1 = static_cast<int> (
        viewport[1] * static_cast<double>(renWinSize[1]) +
        static_cast<double>(this->ImageOrigin[1]) * this->ImageSampleDistance);

      // compute z buffer size
      this->ZBufferSize[0] = static_cast<int>(
        static_cast<double>(this->ImageInUseSize[0]) * this->ImageSampleDistance);
      this->ZBufferSize[1] = static_cast<int>(
        static_cast<double>(this->ImageInUseSize[1]) * this->ImageSampleDistance);

      // Use the size to compute (x2,y2) in window coordinates
      x2 = x1 + this->ZBufferSize[0] - 1;
      y2 = y1 + this->ZBufferSize[1] - 1;

      // This is the z buffer origin (in viewport coordinates)
      this->ZBufferOrigin[0] = static_cast<int>(
        static_cast<double>(this->ImageOrigin[0]) * this->ImageSampleDistance);
      this->ZBufferOrigin[1] = static_cast<int>(
        static_cast<double>(this->ImageOrigin[1]) * this->ImageSampleDistance);

      // Capture the z buffer
      this->ZBuffer = ren->GetRenderWindow()->GetZbufferData(x1,y1,x2,y2);
    }

    // This must be done before FunctionInitialize since FunctionInitialize
    // depends on the gradient opacity constant (computed in here) to 
    // determine whether to save the gradient magnitudes
    vol->UpdateTransferFunctions( ren );

    // Requires UpdateTransferFunctions to have been called first
    this->VolumeRayCastFunction->FunctionInitialize( ren, vol, 
      staticInfo );

    vol->UpdateScalarOpacityforSampleSize( ren, this->SampleDistance );

    staticInfo->CameraThickness = 
      static_cast<float>(ren->GetActiveCamera()->GetThickness());      

    // Copy the viewToVoxels matrix to 16 floats
    int i, j;
    for ( j = 0; j < 4; j++ )
    {
      for ( i = 0; i < 4; i++ )
      {
        staticInfo->ViewToVoxelsMatrix[j*4+i] = 
          static_cast<float>(this->ViewToVoxelsMatrix->GetElement(j,i));
      }
    }

    // Copy the worldToVoxels matrix to 16 floats
    for ( j = 0; j < 4; j++ )
    {
      for ( i = 0; i < 4; i++ )
      {
        staticInfo->WorldToVoxelsMatrix[j*4+i] = 
          static_cast<float>(this->WorldToVoxelsMatrix->GetElement(j,i));
      }
    }

    // Copy the voxelsToWorld matrix to 16 floats
    for ( j = 0; j < 4; j++ )
    {
      for ( i = 0; i < 4; i++ )
      {
        staticInfo->VoxelsToWorldMatrix[j*4+i] = 
          static_cast<float>(this->VoxelsToWorldMatrix->GetElement(j,i));
      }
    }

    if ( this->ClippingPlanes )
    {
      this->InitializeClippingPlanes( staticInfo, this->ClippingPlanes );
    }
    else
    {
      staticInfo->NumberOfClippingPlanes = 0;
    }

//    PROFILE_TIMER_START(c2_2);


    // Copy in the image info
    staticInfo->ImageInUseSize[0]    = this->ImageInUseSize[0];
    staticInfo->ImageInUseSize[1]    = this->ImageInUseSize[1];
    staticInfo->ImageMemorySize[0]   = this->ImageMemorySize[0];
    staticInfo->ImageMemorySize[1]   = this->ImageMemorySize[1];
    staticInfo->ImageViewportSize[0] = this->ImageViewportSize[0];
    staticInfo->ImageViewportSize[1] = this->ImageViewportSize[1];

    staticInfo->ImageOrigin[0] = this->ImageOrigin[0];
    staticInfo->ImageOrigin[1] = this->ImageOrigin[1];

    staticInfo->Image     = this->Image;
    staticInfo->RowBounds = this->RowBounds;
#pragma endregion

    unsigned long checksum = unsigned long(this->GetInput()->GetMTime());
    //NOTE: vol MTime is always changed, although nothing has changed
    checksum += unsigned long(vol->GetProperty()->GetMTime());
    checksum += CalculateChecksum((unsigned char*)staticInfo->ImageViewportSize, sizeof(staticInfo->ImageViewportSize));
    checksum += CalculateChecksum((unsigned char*)staticInfo->ImageOrigin, sizeof(staticInfo->ImageOrigin));    
    checksum += unsigned long(this->Image);

    unsigned char* pVolMatrix = (unsigned char*)&cam->GetViewTransformMatrix()->Element[0];
    checksum += CalculateChecksum(pVolMatrix, 16*sizeof(double));

    if (checksum != LastCheckSum)
    {
      LastCheckSum = checksum;
#pragma region VTK Code (Ray casting, i.e., texture computation)
      // Set the number of threads to use for ray casting,
      // then set the execution method and do it.
      this->Threader->SetNumberOfThreads( this->NumberOfThreads );
      this->Threader->SetSingleMethod( VolumeRayCastMapper_CastRays, 
        (void *)staticInfo);
      this->Threader->SingleMethodExecute();
#pragma endregion
    }

//    PROFILE_TIMER_STOP(c2_2);

    //Luigi Calori (CINECA) disabled blending for opaque objects in 
    //vtkOpenGLProperty.cxx (line 83), which unfortunately corrupts 
    //the correct visualization with MIP if some opaque model (e.g., 
    //BoundingBox) is displayed together with the volume in one view.
    //This is the patch that overcomes that problem.

    GLboolean bBlend = glIsEnabled(GL_BLEND);
    glEnable(GL_BLEND);


#pragma region VTK Code (Texture rendering)
//    PROFILE_TIMER_START(c2_3);
    if ( !ren->GetRenderWindow()->GetAbortRender() )
    {
      this->RenderTexture( vol, ren );
      this->Timer->StopTimer();
      this->TimeToDraw = this->Timer->GetElapsedTime();
      this->StoreRenderTime( ren, vol, this->TimeToDraw );
    }
    // Restore the image sample distance so that automatic adjustment
    // will work correctly
    else
    {
      this->ImageSampleDistance = oldImageSampleDistance;
    }

    if ( staticInfo->ClippingPlane )
    {
      delete staticInfo->ClippingPlane;
    }
    delete staticInfo;

    if ( this->ZBuffer )
    {
      delete [] this->ZBuffer;
      this->ZBuffer = NULL;
    }

//    PROFILE_TIMER_STOP(c2_3);
#pragma  endregion
    if (!bBlend)
      glDisable(GL_BLEND);

  } //if ( this->ComputeRowBounds( vol, ren ) )

//  PROFILE_TIMER_STOP(c2);
}
