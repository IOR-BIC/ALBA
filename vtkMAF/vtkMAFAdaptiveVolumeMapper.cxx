/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMAFAdaptiveVolumeMapper.cxx,v $
  Language:  C++
  Date:      $Date: 2008-03-04 17:04:19 $
  Version:   $Revision: 1.2 $


Copyright (c) 1993-2001 Ken Martin, Will Schroeder, Bill Lorensen 
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

 * Neither name of Ken Martin, Will Schroeder, or Bill Lorensen nor the names
   of any contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.

 * Modified source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
#include <assert.h>
#include <vector>
#include <stdlib.h>
#include <search.h>

#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkMath.h"
#include "vtkCamera.h"
#include "vtkVolume.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkTimerLog.h"
#include "vtkPointData.h"
#include "vtkCullerCollection.h"
#include "vtkLight.h"
#include "vtkLightCollection.h"
#include "vtkVolumeProperty.h"
#include "vtkTransferFunction2D.h"

#include "vtkContourVolumeMapper.h"
#include "vtkMAFAdaptiveVolumeMapper.h"

////////////////////////////////////////// constant expressions
const int VoxelBlockMask  = (~0) << vtkMAFAdaptiveVolumeMapperNamespace::VoxelBlockSizeLog; 
const int VoxelBlockMaskI = ~VoxelBlockMask; 
const int VoxelsInBlock   = vtkMAFAdaptiveVolumeMapperNamespace::VoxelBlockSize * vtkMAFAdaptiveVolumeMapperNamespace::VoxelBlockSize * vtkMAFAdaptiveVolumeMapperNamespace::VoxelBlockSize; 
const int VoxelMRMasks[vtkMAFAdaptiveVolumeMapperNamespace::VoxelBlockSizeLog + 1] = { ~0, ~0 << 1, ~0 << 2, ~0 << 3 };

static double UnsignedCharToFloatTable[256];
static double UnsignedCharToFloatTable255[256];

static const unsigned char stencilBufferValue = 17;

bool vtkMAFAdaptiveVolumeMapper::Interpolation = false;

////////////////////////////////////////// inline functions
#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))
template<typename type> static inline type clip(type x, type xmin, type xmax) { if (x < xmin) return xmin; if (x > xmax) return xmax; return x; }

vtkStandardNewMacro(vtkMAFAdaptiveVolumeMapper);

//------------------------------------------------------------------------------
vtkMAFAdaptiveVolumeMapper::vtkMAFAdaptiveVolumeMapper() 
//------------------------------------------------------------------------------
{
  // initialize static variables
  if (UnsignedCharToFloatTable[1] == 0) 
  {
    for (int i = 0; i < 256; i++) 
    {
      UnsignedCharToFloatTable[i] = double(i) / 255.f;
      UnsignedCharToFloatTable255[i] = double(i);
    }
  }

  this->EnableAutoLOD  = true;

  this->BlockMin = this->BlockMax = NULL;
  this->BlockGradientMax = NULL;
  this->TimeToDrawNotOptimized = this->TimeToDraw = 0;
  this->VoxelCoordinates[0] = this->VoxelCoordinates[1] = this->VoxelCoordinates[2] = NULL;
  this->GradientEquationIndecesR[0] = NULL;
  this->GradientEquationDivisor[0] = NULL;

  this->SliceSizes = NULL;
  this->UniformToRectGridIndex = NULL;

  this->ObjectToViewMatrix = vtkMatrix4x4::New();
  this->ViewToObjectMatrix = vtkMatrix4x4::New();
  this->WorldToObjectMatrix = vtkMatrix4x4::New();
  this->ViewportAndTransformationChecksum = 0;

  this->VisibilityThreshold = VTK_INT_MIN;
  this->InterpolationThresholdIndex = 0.f;

  this->FrameBuffer = NULL;
  this->PrevDepthBuffer = this->DepthBuffer = NULL;
  this->FrameBufferSize = 0;

  this->StencilBuffer = NULL;

  this->GradientIndeces = NULL;
  this->GradientTable   = NULL;

  this->BlockLOD = NULL;

  this->TFCacheMemory = NULL;
  this->DataToTFLookUpTableRealPointer = NULL;
  this->GradientToTFLookUpTable = NULL;
  this->TimeToCreateTFCache = 0;

  // multi-processing
  this->NumProcesses = 0;
  this->Controller = (vtkMultiThreader::GetGlobalDefaultNumberOfThreads() > 1) ? vtkMultiThreader::New() : NULL;
  this->RenderingQueueCS = (this->Controller == NULL) ? NULL : vtkCriticalSection::New();
  this->ThreadLockCS = this->Controller ? vtkCriticalSection::New() : NULL;

  this->AuxTimer = vtkTimerLog::New();
  this->DataPreprocessed = false;
}

//------------------------------------------------------------------------------
vtkMAFAdaptiveVolumeMapper::~vtkMAFAdaptiveVolumeMapper() 
//------------------------------------------------------------------------------
{
  this->ReleaseData();
  
  this->ObjectToViewMatrix->Delete();
  this->ViewToObjectMatrix->Delete();
  this->WorldToObjectMatrix->Delete();

  delete [] this->TFCacheMemory;
  delete [] this->DataToTFLookUpTableRealPointer;
  delete [] this->GradientToTFLookUpTable;

  if (this->ThreadLockCS) 
  {
    this->NumProcesses = 0; // flag to stop the process
    this->ThreadLockCS->Unlock();
    this->Controller->TerminateThread(0);
    this->ThreadLockCS->Delete();
  }
  
  if (this->RenderingQueueCS)
    this->RenderingQueueCS->Delete();

  if (this->Controller)
    this->Controller->Delete();

  this->AuxTimer->Delete();
}

//------------------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::ReleaseData() 
//------------------------------------------------------------------------------
{
  delete [] this->BlockMin;
  this->BlockMin = this->BlockMax = NULL;
  delete [] this->BlockGradientMax;
  this->BlockGradientMax = NULL;

  delete [] this->VoxelCoordinates[0];
  delete [] this->VoxelCoordinates[1];
  delete [] this->VoxelCoordinates[2];
  this->VoxelCoordinates[0] = this->VoxelCoordinates[1] = this->VoxelCoordinates[2] = NULL;
  delete [] this->GradientEquationIndecesR[0];
  this->GradientEquationIndecesR[0] = this->GradientEquationIndecesR[1] = this->GradientEquationIndecesR[2] = NULL;
  this->GradientEquationIndecesL[0] = this->GradientEquationIndecesL[1] = this->GradientEquationIndecesL[2] = NULL;
  delete [] this->GradientEquationDivisor[0];
  this->GradientEquationDivisor[0] = this->GradientEquationDivisor[1] = this->GradientEquationDivisor[2] = NULL;

  delete [] this->SliceSizes;
  this->SliceSizes = NULL;
  delete [] this->UniformToRectGridIndex;
  this->UniformToRectGridIndex = NULL;

  delete [] this->FrameBuffer;
  this->FrameBuffer = NULL;
  delete [] this->DepthBuffer;
  this->DepthBuffer = NULL;
  delete [] this->PrevDepthBuffer;
  this->PrevDepthBuffer = NULL;
  delete [] this->StencilBuffer;
  this->StencilBuffer = NULL;

  delete [] this->BlockLOD;
  this->BlockLOD = NULL;

  delete [] this->GradientIndeces;
  this->GradientIndeces = NULL;

  delete [] this->GradientTable;
  this->GradientTable = NULL;

  delete [] this->TFCacheMemory;
  this->TFCacheMemory = NULL;

  this->DataPreprocessed = false;
}

//------------------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::Render(vtkRenderer *renderer, vtkVolume *volume) 
//------------------------------------------------------------------------------
{
  if (this->GetInput() == NULL || this->GetInput()->GetPointData() == NULL || this->GetInput()->GetPointData()->GetScalars() == NULL) 
  {
    vtkErrorMacro(<< "No data for rendering");
    return;
  }

  this->UpdateProgress(0.f);
  if (this->Controller && this->NumProcesses == 0) 
  {
    this->ThreadLockCS->Lock(); // pause other processes
    this->Controller->SpawnThread(&RenderProcess, this);
    this->NumProcesses++;
  }
  
  this->RenderServer(renderer, volume);
  this->UpdateProgress(1.f);
}

//------------------------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::RenderServer(vtkRenderer *renderer, vtkVolume *volume) 
//------------------------------------------------------------------------------
{
  // prepare data for rendering
  if (!this->PrepareVolumeForRendering())
    return;
  if (!this->PrepareTransferFunctionForRendering(volume->GetProperty()))
    return;

  //----- should we use level-of-detail?
  this->PrevEnableOptimization = this->EnableOptimization;
  this->EnableOptimization = this->EnableAutoLOD && this->TimeToDrawNotOptimized > this->GetAllocatedRenderTime(renderer);

  // reset statistics
  this->NumOfTracedRays = this->NumOfInterpolatedRays = 0;
  this->NumOfProcessedVoxels = this->NumOfSkippedVoxels = 0;

  this->Timer->StartTimer();
  if (!this->InitializeRender(true, renderer, volume)) 
  {
    this->Timer->StopTimer();
    return;
  }

  this->PrepareStencilBuffer(renderer);
  this->PrepareLights(renderer, volume);

  // actual rendering
  if (this->NumOfRenderingPortions > 1) 
  {
    this->NumOfRenderingPortionsLeft = this->NumOfRenderingPortions;

    // resume the other thread
    this->ThreadLockCS->Unlock();

    while (this->NumOfRenderingPortionsLeft >= 0) 
    {
      this->RenderingQueueCS->Lock();

      const int portion = this->NumOfRenderingPortions - this->NumOfRenderingPortionsLeft;
      this->NumOfRenderingPortionsLeft--;
      this->RenderingQueueCS->Unlock();
      
      if (portion < this->NumOfRenderingPortions)
        this->RenderRegion(this->ViewportBBoxPortions[portion]);
    }
    
    // wait for other processes to finish the job and stop them
    this->ThreadLockCS->Lock();
  }
  else 
  {
    this->RenderRegion(this->ViewportBBox);
  }
  this->DrawFrameBuffer();

  this->InitializeRender(false, renderer, volume);

  // statistics
  this->Timer->StopTimer();
  this->TimeToDraw = (double)this->Timer->GetElapsedTime();
  if (this->TimeToDraw < 0.0001f)
    this->TimeToDraw = 0.0001f;

  if (!this->EnableOptimization)
    this->TimeToDrawNotOptimized = this->TimeToDraw;
  
  if (!this->UseCache)
    printf("\rTime (%d): %.2f%s. %.0f%% pixels processed, %.0f%% of those interpolated. %d (%d) voxels \r", int(UseCache), this->TimeToDraw, this->TransformationNotChanged ? "c" : " ", 100.f * (this->NumOfTracedRays + this->NumOfInterpolatedRays) / (this->ViewportSize[0] * this->ViewportSize[1]),
           100.f * this->NumOfInterpolatedRays / (this->NumOfTracedRays + this->NumOfInterpolatedRays), this->NumOfProcessedVoxels, this->NumOfSkippedVoxels);
}

//---------------------------------------------------------------------------
double vtkMAFAdaptiveVolumeMapper::GetAllocatedRenderTime(vtkRenderer *renderer) 
//------------------------------------------------------------------------------
{
  vtkVolumeCollection *volumes = renderer->GetVolumes();
  vtkProp*   propArray[256];
  int numOfVolumes = 0;
  volumes->InitTraversal();
  for (vtkVolume *volume = volumes->GetNextVolume(); volume != NULL; volume = volumes->GetNextVolume()) 
  {
    if (!volume->GetVisibility() || vtkMAFAdaptiveVolumeMapper::SafeDownCast(volume->GetMapper()) == NULL)
      continue;
    propArray[numOfVolumes++] = volume;
    if (numOfVolumes >= 255)
      break;
  }
  
  vtkCullerCollection *cullers  = renderer->GetCullers();
  vtkCuller           *culler;
  int initialized = 0;
  for (cullers->InitTraversal(); culler = cullers->GetNextItem(); )
    culler->Cull(renderer, propArray, numOfVolumes, initialized);
  
  return renderer->GetAllocatedRenderTime() / (numOfVolumes + 0.1f);
}

//---------------------------------------------------------------------------  
bool vtkMAFAdaptiveVolumeMapper::Pick(int x, int y, vtkRenderer *renderer, vtkVolume *volume, double opacityThreshold) 
//------------------------------------------------------------------------------
{
  if (this->GetInput() == NULL || this->GetInput()->GetMTime() > this->MTime)
    return false; // cannot afford preprocessing here
  
  if (!this->PrepareTransferFunctionForRendering(volume->GetProperty()))
    return false;
  this->NumOfLights = 0;
  this->PrepareTransformationData(renderer, volume);
  
  double vxyz[3] = {x, y, 1};
  unsigned char rgba[4];
  switch (this->GetDataType()) 
  {
    case VTK_UNSIGNED_SHORT: 
      this->TraceRay(vxyz, (rgba), (const unsigned short*)this->DataPointer); 
    break;
    case VTK_SHORT:          
      this->TraceRay(vxyz, (rgba), (const short*)this->DataPointer); 
    break;
    case VTK_UNSIGNED_CHAR:  
      this->TraceRay(vxyz, (rgba), (const unsigned char*)this->DataPointer); 
    break;
    case VTK_CHAR:           
      this->TraceRay(vxyz, (rgba), (const char*)this->DataPointer); 
    break;
  }
  return (rgba[3] / 255.f) > opacityThreshold;
}

//---------------------------------------------------------------------------  
void vtkMAFAdaptiveVolumeMapper::RenderRegion(const int viewportBBox[4]) 
//------------------------------------------------------------------------------
{
#define Trace(x, y, z, prevz, rgba) {\
  double vxyz[3] = {double(x), double(y), this->MixedRendering ? (2.f * double(z) - 1.f) : 1.f};\
  if (!this->UseCache || (this->MixedRendering && z != prevz))\
  switch (dataType) {\
    case VTK_UNSIGNED_SHORT: this->TraceRay(vxyz, (rgba), (const unsigned short*)this->DataPointer); break;\
    case VTK_SHORT:          this->TraceRay(vxyz, (rgba), (const short*)this->DataPointer); break;\
    case VTK_UNSIGNED_CHAR:  this->TraceRay(vxyz, (rgba), (const unsigned char*)this->DataPointer); break;\
    case VTK_CHAR:           this->TraceRay(vxyz, (rgba), (const char*)this->DataPointer); break;\
    }\
  }

  const int dataType = this->GetDataType();
  const int supersamplingThreshold = this->UseCache ? 1 : (this->EnableOptimization ? 24 : 6);
  this->EarlyRayTerminationTransparency = this->EnableOptimization ? 0.03 : 0.005;
  
  // offset between pixels
  const int width  = viewportBBox[1] - viewportBBox[0] + 1;
  const int height = viewportBBox[3] - viewportBBox[2] + 1;

  static const int subsamplingStep = 4;
  static const int numberOfSamples = subsamplingStep * subsamplingStep - 1;
  static const int subsamplingScheme[numberOfSamples][3] = {
    {-2, -2},
    {-2, -4}, {-4, -2},
    {-3, -3}, {-3, -5}, { -5, -3}, {-5, -5},
    {-3, -4}, {-4, -3}, { -4, -5}, {-5, -4},
    {-3, -6}, {-6, -3}, { -5, -6}, {-6, -5}
    };
  static const int subsamplingDistances[numberOfSamples] = { 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static const int subsamplingNeighbours[numberOfSamples] = {1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1}; // see neightbourOffsets below

  int subsamplingSchemeOffsets[numberOfSamples], subsamplingSchemeOffsets4[numberOfSamples];
  int si;
  for (si = 0; si < numberOfSamples; si++) {
    subsamplingSchemeOffsets[si]  = subsamplingScheme[si][0] + subsamplingScheme[si][1] * width;
    subsamplingSchemeOffsets4[si] = 4 * subsamplingSchemeOffsets[si];
  }

  const int neighbourOffsets[2][4] = { {-4 * width, -4, 4, 4 * width}, {-4 * width - 4, -4 * width + 4, 4 * width - 4, 4 * width + 4}};
  
  const int yMax = height + 2 * subsamplingStep;
  const int xMax = width  + 2 * subsamplingStep;

  // main processing loop: trace or interpolate all pixels
  for (int y = 0; y < yMax; y += subsamplingStep) {
    const int absY = y + viewportBBox[2];
    const int yOffset = (absY - this->ViewportBBox[2]) * this->ViewportBBoxSize[0];
    const unsigned char * const yStencil = this->StencilBuffer + yOffset;
    unsigned char * const yPixel   = (unsigned char*)this->FrameBuffer + (yOffset << 2);
    const double * const yDepth     = this->DepthBuffer + yOffset;
    const double * const yDepthPrev = this->PrevDepthBuffer + yOffset;

    for (int x = 0; x < xMax; x += subsamplingStep) {
      const int absX = x + viewportBBox[0];
      const unsigned char * const stencil = yStencil + x;
      unsigned char * const pixel = yPixel + (x << 2);
      const double * const depth = yDepth + x;
      const double * const prevDepth = yDepthPrev + x;

      // trace ray
      if (y < height && x < width && stencil[0] != stencilBufferValue)
        Trace(absX, absY, depth[0], prevDepth[0], pixel);

      if (y == 0 || x == 0)
        continue; // border

      // sub sampling
      for (si = 0; si < numberOfSamples; si++) {
        const int sx = x + subsamplingScheme[si][0], sy = y + subsamplingScheme[si][1];
        if (sx < 0 || sx >= width || sy < 0 || sy >= height || stencil[subsamplingSchemeOffsets[si]] == stencilBufferValue ||
            this->UseCache && (!this->MixedRendering || depth[subsamplingSchemeOffsets[si]] == prevDepth[subsamplingSchemeOffsets[si]]))
          continue; // skip empty and outside pixels
        
        // compare with neighbors and interpolate if possible
        unsigned char *spixel = pixel + subsamplingSchemeOffsets4[si];
        const int shift = subsamplingDistances[si];
        int ni, ci;
		if (sx > 0 && sy > 0 && (sx + 1 + shift) < width && (sy + 1 + shift) < height) {
          // calculate average
          unsigned int r = 0, g = 0, b = 0, a = 0;
          for (ni = 0; ni < 4; ni++) {
            unsigned char *npixel = spixel + (neighbourOffsets[subsamplingNeighbours[si]][ni] << shift);
            r += unsigned(npixel[0]); g += unsigned(npixel[1]); b += unsigned(npixel[2]); a += unsigned(npixel[3]);
          }
          spixel[0] = r >> 2; spixel[1] = g >> 2; spixel[2] = b >> 2; spixel[3] = a >> 2;
          
          // compare with threshold
          for (ni = 0; ni < 4; ni++) {
            unsigned char *npixel = spixel + (neighbourOffsets[subsamplingNeighbours[si]][ni] << shift);
            for (ci = 0; ci < 4; ci++) {
              int difference = int(npixel[ci]) - spixel[ci];
              if (difference > supersamplingThreshold || -difference > supersamplingThreshold)
                break;
            }
            if (ci < 4)
              break;
          }
          if (ni == 4) {
            this->NumOfInterpolatedRays++;
            continue;
          }
        } // interpolation

        Trace(absX + subsamplingScheme[si][0], absY + subsamplingScheme[si][1], depth[subsamplingSchemeOffsets[si]], prevDepth[subsamplingSchemeOffsets[si]], spixel);
      }
    }
  }
}
#undef Trace

//------------------------------------------------------------------------------
template<typename DataType> void vtkMAFAdaptiveVolumeMapper::TraceRay(const double vxyz[3], unsigned char pixel[4], const DataType *dataPointer) 
//------------------------------------------------------------------------------
{
  double ray[3], xyz[3], dxyz[3];
  const double rayLength = this->CalculateRay(vxyz, ray, xyz, dxyz);
  if (rayLength <= 0.f)
    return;

  const DataType * const   blockMax = (DataType *)this->BlockMax;
  const unsigned char * blockLOD = this->BlockLOD;
  const DataType visibilityThreshold = (DataType)this->VisibilityThreshold;

  // calculate z for non-uniform grid. use binary search
  const bool nonUniformGrid = this->NonUniformGrid;

  const double x = xyz[0], y = xyz[1], z = xyz[2];
  int blockIndex = (int(x) >> VoxelBlockSizeLog) + (int(y) >> VoxelBlockSizeLog) * this->BlockToBlockIncrement[1] + (int(z) >> VoxelBlockSizeLog) * this->BlockToBlockIncrement[2];
  int prevBlockIndex = blockIndex;
  int currentLOD = nonUniformGrid ? 0 : blockLOD[blockIndex];
  int prevLOD = currentLOD;
  int vi[3] = {(int(x) >> currentLOD) << currentLOD, (int(y) >> currentLOD) << currentLOD, (int(z) >> currentLOD) << currentLOD};
  int bordervi[3] = {vi[0] & VoxelBlockMask, vi[1] & VoxelBlockMask, vi[2] & VoxelBlockMask};
  int voxelIndex = vi[0] + vi[1] * this->VoxelToVoxelIncrement[1] + vi[2] * this->VoxelToVoxelIncrement[2];

  const int   bInc[3] = {ray[0] > 0 ? 1 : -1, ray[1] > 0 ? 1 : -1, ray[2] > 0 ? 1 : -1};
  const int   incPositive[3] = {bInc[0] > 0, bInc[1] > 0, bInc[2] > 0};
  const int   prevMask[3] = {incPositive[0] ? ~0 : VoxelBlockMask, incPositive[1] ? ~0 : VoxelBlockMask, incPositive[2] ? ~0 : VoxelBlockMask};
  const unsigned int maxIndex[3] = {(unsigned)this->DataDimensions[0] - 1, (unsigned)this->DataDimensions[1] - 1, (unsigned)this->DataDimensions[2] - 1};

  int inc[3] = {bInc[0] << currentLOD, bInc[1] << currentLOD, bInc[2] << currentLOD};

  // array increments
  const int blockArrayIncrement[3] = {bInc[0], bInc[1] * this->BlockToBlockIncrement[1], bInc[2] * this->BlockToBlockIncrement[2]};
  const int absVoxelArrayIncrement[3] = {1, this->VoxelToVoxelIncrement[1], this->VoxelToVoxelIncrement[2]};
  const int voxelArrayIncrement[3] = {bInc[0], bInc[1] * absVoxelArrayIncrement[1], bInc[2] * absVoxelArrayIncrement[2]};

  double distanceAlongTheRay = 0;
  const double dx = dxyz[0], dy = dxyz[1], dz = dxyz[2];
  const double dLOD[VoxelBlockSizeLog + 1][3] = { { dx, dy, dz}, {2.f * dx, 2.f * dy, 2.f * dz}, { 4.f * dx, 4.f * dy, 4.f * dz}, { 8.f * dx, 8.f * dy, 8.f * dz}};
  const double *pdLOD = dLOD[currentLOD];
  double l[3] = {dx * (incPositive[0] ? ((vi[0] + inc[0]) - x) : (x - vi[0])), dy * (incPositive[1] ? ((vi[1] + inc[1]) - y) : (y - vi[1])), dz * (incPositive[2] ? ((vi[2] + inc[2]) - z) : (z - vi[2]))};
  double prevl[3] = {dx * (incPositive[0] ? (bordervi[0] - x) : x - (bordervi[0] + VoxelBlockSize)), dy * (incPositive[1] ? (bordervi[1] - y) : y - (bordervi[1] + VoxelBlockSize)), dz * (incPositive[2] ? (bordervi[2] - z) : z - (bordervi[2] + VoxelBlockSize))};
  if (nonUniformGrid) 
  {
    l[2] *= this->SliceSizes[vi[2]][currentLOD];
    prevl[2] *= this->SliceSizes[vi[2]][currentLOD];
  }
    
  //---------------- actual traverse
  const double * const opacityCache      = this->OpacityCache;
  const unsigned char * const rgbdCache = this->RgbdCache;

  double r = 0.f, g = 0.f, b = 0.f, a = 1.f;

  bool isBlockTransparent = (blockMax[blockIndex] < visibilityThreshold);
  for ( ; (distanceAlongTheRay < rayLength) && (a > this->EarlyRayTerminationTransparency); ) 
  {
    const int prevVoxelIndex = voxelIndex, prevvi[3] = {vi[0], vi[1], vi[2]};

    // traverse
    const int ii = (l[0] <= l[1] && l[0] <= l[2]) ? 0 : ((l[1] <= l[2]) ? 1 : 2);
    const double distance = l[ii] - distanceAlongTheRay;
    distanceAlongTheRay  = l[ii];

    vi[ii] += inc[ii];
    if ((unsigned)vi[ii] <= maxIndex[ii]) 
    {
      voxelIndex += voxelArrayIncrement[ii];
 
      // new block
      if ((vi[ii] ^ bordervi[ii]) & VoxelBlockMask) 
      {
        prevBlockIndex = blockIndex;
        blockIndex += blockArrayIncrement[ii];
        prevLOD = currentLOD;
        currentLOD = blockLOD[blockIndex];
        isBlockTransparent = (blockMax[blockIndex] < visibilityThreshold) && (blockMax[prevBlockIndex] < visibilityThreshold);

        if (prevLOD != currentLOD) 
        { // correct the position
          pdLOD = dLOD[currentLOD];
          inc[0] = bInc[0] << currentLOD;
          inc[1] = bInc[1] << currentLOD;
          inc[2] = bInc[2] << currentLOD;
          for (int j = 0; j < 3; j++) 
          {
            if (ii == j)
              continue;

            if (nonUniformGrid && j == 2) 
            {
              vi[j] = bordervi[j] + (!incPositive[j] ? VoxelBlockSize : 0);
              l[j] = prevl[j] + (incPositive[j] ? (dz * this->SliceSizes[bordervi[j]][currentLOD]) : 0.f);
              while (l[j] < l[ii])
                vi[j] += inc[j], l[j] += dz * this->SliceSizes[vi[j]][currentLOD];
              continue;
            }

            l[j] = prevl[j] + (incPositive[j] ? pdLOD[j] : 0.f);
            vi[j] = bordervi[j] + (!incPositive[j] ? VoxelBlockSize : 0);
            while (l[j] < l[ii])
              l[j] += pdLOD[j], vi[j] += inc[j];
          }
        }
        if (!incPositive[ii])
          vi[ii] = bordervi[ii] + inc[ii];
        bordervi[ii] = vi[ii] & prevMask[ii];
        prevl[ii] = l[ii];
        voxelIndex = vi[0] + vi[1] * absVoxelArrayIncrement[1] + vi[2] * absVoxelArrayIncrement[2];
      }

      if (nonUniformGrid && ii == 2)
        l[2] += dz * this->SliceSizes[vi[2]][currentLOD];
      else
        l[ii] += pdLOD[ii];

      //hack
      if ((unsigned)vi[0] > maxIndex[0] || (unsigned)vi[1] > maxIndex[1] || (unsigned)vi[2] > maxIndex[2])
        distanceAlongTheRay = VTK_DOUBLE_MAX; // stop processing

    }
    else 
    { // data bounds reached
      distanceAlongTheRay = VTK_DOUBLE_MAX; // stop processing
    }

    //------------------------------------------- lighting calculations
    if (isBlockTransparent) // skip transparent blocks
      continue;

    const DataType *voxelPointer    = dataPointer + prevVoxelIndex;
    const DataType  dataValue       = *voxelPointer;
    int             gradientIndex   = int(this->GradientIndeces[prevVoxelIndex]);
    const bool      noInterpolation = prevLOD != 0 || gradientIndex < this->InterpolationThresholdIndex;
    if (dataValue < visibilityThreshold && noInterpolation)
      continue; // transparent

    double opacity, color[3], diffuse, normal[3];
    if (noInterpolation) 
    {
      int index = this->GradientToTFLookUpTable[gradientIndex] + this->DataToTFLookUpTable[dataValue];
      opacity = opacityCache[index];
      if (opacity <= 0.f)
        continue;
      const unsigned char *packed = rgbdCache + (index << 2);
      color[0] = UnsignedCharToFloatTable255[packed[0]];
      color[1] = UnsignedCharToFloatTable255[packed[1]];
      color[2] = UnsignedCharToFloatTable255[packed[2]];
      diffuse  = UnsignedCharToFloatTable[packed[3]];
      if (packed[3] != 0) 
      {
        const double inorm = this->GradientTableI[gradientIndex];
        for (int j = 0; j < 3; j++) 
        {
          const int index = prevvi[j];
          normal[j] = double(int(voxelPointer[this->GradientEquationIndecesR[j][index]]) - voxelPointer[this->GradientEquationIndecesL[j][index]]) * this->GradientEquationDivisor[j][index] * inorm;
        }
      }
    }
    else 
    { // interpolation
      static const int indexInc[3][8] = {{0, 1, 1, 0, 0, 1, 1, 0}, {0, 0, 1, 1, 0, 0, 1, 1}, {0, 0, 0, 0, 1, 1, 1, 1} };
      opacity = color[0] = color[1] = color[2] = diffuse = 0.f;
      int maxGradientIndex = gradientIndex, maxGradientIndexSI = 0;
      for (int si = 0; si < 8; si++) 
      {
        const int x = prevvi[0] + indexInc[0][si], y = prevvi[1] + indexInc[1][si], z = prevvi[2] + indexInc[2][si];
        if (x > maxIndex[0] || y > maxIndex[1] || z > maxIndex[2])
          continue;
        
        const DataType *vPtr = voxelPointer + this->VoxelVertIndicesOffsets[si];
        const DataType  tmpDataValue     = *vPtr;
        const int       tmpGradientIndex = int(this->GradientIndeces[prevVoxelIndex + this->VoxelVertIndicesOffsets[si]]);
        if (tmpGradientIndex > maxGradientIndex)
          maxGradientIndex = tmpGradientIndex, maxGradientIndexSI = si;
        int index = this->GradientToTFLookUpTable[tmpGradientIndex] + this->DataToTFLookUpTable[tmpDataValue];
        const double weight = opacityCache[index];
        if (weight <= 0.f)
          continue;
        opacity  += weight;
        const unsigned char *packed = rgbdCache + (index << 2);
        color[0] += UnsignedCharToFloatTable255[packed[0]] * weight;
        color[1] += UnsignedCharToFloatTable255[packed[1]] * weight;
        color[2] += UnsignedCharToFloatTable255[packed[2]] * weight;
        diffuse  += UnsignedCharToFloatTable[packed[3]]    * weight;
      }
      if (opacity <= 0.f)
        continue;

      const double iopacity = 1.f / opacity;
      color[0] *= iopacity;
      color[1] *= iopacity;
      color[2] *= iopacity;
      diffuse  *= iopacity;
      opacity  *= 0.125f;
      if (diffuse > 0.f) 
      { // normalize the normal
        const double inorm = this->GradientTableI[maxGradientIndex];
        for (int j = 0; j < 3; j++) 
        {
          const DataType *vPtr = voxelPointer + this->VoxelVertIndicesOffsets[maxGradientIndexSI];
          const int index = prevvi[j] + indexInc[j][maxGradientIndexSI];
          normal[j] = double(int(vPtr[this->GradientEquationIndecesR[j][index]]) - vPtr[this->GradientEquationIndecesL[j][index]]) * this->GradientEquationDivisor[j][index] * inorm;
        }
      }
    }

    const double opacityXdistance1 = opacity * distance;
    const double opacityXdistance2 = opacityXdistance1 * (0.5f * (opacityXdistance1 - opacity));
    //const double opacityXdistance3 = opacityXdistance2 * (0.3333333f * (opacityXdistance1 - 2.f * sampleOpacity));
    double newOpacity = a * (opacityXdistance1 - opacityXdistance2);
    if (newOpacity <= 0.f)
      newOpacity = a * (1.f - pow(1.f - opacity, distance));

    // lighting
    double emissionAndScattering = 1.f - diffuse;
    for (int li = 0; diffuse > 0.f && li < this->NumOfLights; li++) 
    {
      struct LightStructure &light = this->Lights[li];
      const double *L  = light.Positional ? ray : light.Direction;
      
      // diffuse (lambert) shading
      const double dot = fabs(normal[0] * L[0] + normal[1] * L[1] + normal[2] * L[2]);
      emissionAndScattering += dot * light.Intensity * diffuse; // * a?
    } // for (EVERY LIGHT)

    emissionAndScattering *= newOpacity;
    r += color[0] * emissionAndScattering;
    g += color[1] * emissionAndScattering;
    b += color[2] * emissionAndScattering;
    a -= newOpacity;
  } // for ( ; ; )
  
  
  //this->NumOfProcessedVoxels += ti;
  //this->NumOfSkippedVoxels   += numberOfVoxels - ti;
  //--------------------------------------- convert to byte format
  int val = int(r);
  pixel[0] = val < 0 ? 0 : (val > 255 ? 255 : unsigned char(val));
  val = int(g);
  pixel[1] = val < 0 ? 0 : (val > 255 ? 255 : unsigned char(val));
  val = int(b);
  pixel[2] = val < 0 ? 0 : (val > 255 ? 255 : unsigned char(val));
  val = int((1.f - a) * 255.f);
  pixel[3] = val < 0 ? 0 : (val > 255 ? 255 : unsigned char(val));
  //pixel[0] = pixel[1] = pixel[2] = pixel[3]; //show alpha-buffer only
  this->NumOfTracedRays++;
}

//---------------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::DrawFrameBuffer() 
//---------------------------------------------------------------------------
{
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);

  // prepare gl state
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // setup viewport
  glViewport(this->ViewportBBox[0], this->ViewportBBox[2], this->ViewportBBoxSize[0], this->ViewportBBoxSize[1]);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glRasterPos3i(-1, -1, 0);

  glDrawPixels(this->ViewportBBoxSize[0], this->ViewportBBoxSize[1], GL_RGBA, GL_UNSIGNED_BYTE, this->FrameBuffer);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
}

//--------------------------------------------------------------------------------------
VTK_THREAD_RETURN_TYPE vtkMAFAdaptiveVolumeMapper::RenderProcess(void *pThreadInfoStruct) 
//---------------------------------------------------------------------------
{
  vtkMAFAdaptiveVolumeMapper *mapper = (vtkMAFAdaptiveVolumeMapper *)((ThreadInfoStruct*)pThreadInfoStruct)->UserData;
  
  while (mapper->NumProcesses) 
  {
    mapper->ThreadLockCS->Lock(); // continue the thread

    while (mapper->NumOfRenderingPortionsLeft > 0) 
    {
      mapper->RenderingQueueCS->Lock();
      const int portion = mapper->NumOfRenderingPortions - mapper->NumOfRenderingPortionsLeft;
      if (mapper->NumOfRenderingPortionsLeft > 0)
        mapper->NumOfRenderingPortionsLeft--;
      mapper->RenderingQueueCS->Unlock();
      
      if (portion < mapper->NumOfRenderingPortions)
        mapper->RenderRegion(mapper->ViewportBBoxPortions[portion]);
    }

    mapper->ThreadLockCS->Unlock();
  }
  return VTK_THREAD_RETURN_VALUE;
}

//---------------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::DrawBlock(int block, bool wireframe) 
//---------------------------------------------------------------------------
{
  glColor3d(0.2f, 0.f, 0.f);
  const int bzi = block / (this->NumBlocks[0] * this->NumBlocks[1]);
  const int byi = (block - bzi * (this->NumBlocks[0] * this->NumBlocks[1])) / this->NumBlocks[0];
  const int bxi = block - bzi * this->NumBlocks[0] * this->NumBlocks[1] - byi * this->NumBlocks[0];

  const int z = bzi << VoxelBlockSizeLog, z1 = (bzi + 1) << VoxelBlockSizeLog;
  const int y = byi << VoxelBlockSizeLog, y1 = (byi + 1) << VoxelBlockSizeLog;
  const int x = bxi << VoxelBlockSizeLog, x1 = (bxi + 1) << VoxelBlockSizeLog;
  
  glBegin(wireframe ? GL_LINE_STRIP : GL_QUADS);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
  glEnd();

  glBegin(wireframe ? GL_LINE_STRIP: GL_QUADS);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);
  glEnd();

  glBegin(wireframe ? GL_LINE_STRIP : GL_QUADS);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);
  glEnd();

  glBegin(wireframe ? GL_LINE_STRIP : GL_QUADS);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);
  glEnd();

  glBegin(wireframe ? GL_LINE_STRIP : GL_QUADS);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);
  glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);
  glEnd();

  glBegin(wireframe ? GL_LINE_STRIP : GL_QUADS);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);
  glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);
  glEnd();
}

//---------------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::PrepareTransformationData(vtkRenderer *renderer, vtkVolume *volume) 
//---------------------------------------------------------------------------
{
  // check whether we can use the transformation cache
  const int prevChecksum = this->ViewportAndTransformationChecksum;
  this->ViewportAndTransformationChecksum = this->CalculateViewportAndTransformationChecksum(renderer, volume);
  this->TransformationNotChanged = (prevChecksum == this->ViewportAndTransformationChecksum && prevChecksum != 0);
  this->UseCache = this->TransformationNotChanged && this->PropertiesNotChanged &&(this->PrevEnableOptimization == this->EnableOptimization || this->EnableOptimization);
  if (this->TransformationNotChanged)
    return;

  // identify viewport dimensions
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  this->ViewportSize[0] = viewport[2];
  this->ViewportSize[1] = viewport[3];
  this->PixelSizeX = 2.f / (this->ViewportSize[0] - 1);
  this->PixelSizeY = 2.f / (this->ViewportSize[1] - 1);

  // transformation
  double aspectRatio[2];
  renderer->ComputeAspect();
  renderer->GetAspect(aspectRatio);
  vtkCamera *camera = renderer->GetActiveCamera();

  // create transform matrix
  // NOTE: do not use composite transform from camera as it does not take into account stereo
  vtkMatrix4x4::Multiply4x4(camera->GetPerspectiveTransformMatrix((double)aspectRatio[0] / aspectRatio[1], -1, 1), camera->GetViewTransformMatrix(), this->ObjectToViewMatrix);
  vtkMatrix4x4::Multiply4x4(this->ObjectToViewMatrix, volume->GetMatrix(), this->ObjectToViewMatrix);
  vtkMatrix4x4::Invert(this->ObjectToViewMatrix, this->ViewToObjectMatrix);
  vtkMatrix4x4::Invert(volume->GetMatrix(), this->WorldToObjectMatrix);

  camera->GetPosition(this->TransformedCameraPosition);
  if (renderer->GetRenderWindow()->GetStereoRender()) 
  {
    double stereoMatrix[4][4], stereoMatrixI[4][4];
    
    //vtkMatrix4x4::Invert((double*)camera->GetCompositePerspectiveTransformMatrix((double)aspectRatio[0] / aspectRatio[1], -1, 1)->Element, (double*)originalMatrixI);
    vtkMatrix4x4::Multiply4x4((double*)camera->GetPerspectiveTransformMatrix((double)aspectRatio[0] / aspectRatio[1], -1, 1)->Element, (double*)camera->GetViewTransformMatrix()->Element, (double*)stereoMatrix);
    vtkMatrix4x4::Invert((double*)stereoMatrix, (double*)stereoMatrixI);

    // calculate the new direction
    double direction[4] = {0, 0, 1, 1};
    vtkMatrix4x4::MultiplyPoint((double*)stereoMatrixI, direction, direction);
    direction[0] = direction[0] / direction[3] - stereoMatrixI[0][3] / stereoMatrixI[3][3];
    direction[1] = direction[1] / direction[3] - stereoMatrixI[1][3] / stereoMatrixI[3][3];
    direction[2] = direction[2] / direction[3] - stereoMatrixI[2][3] / stereoMatrixI[3][3];
    const double multiplier = camera->GetDistance() / vtkMath::Norm(direction);
    
    // calculate the new position
    camera->GetFocalPoint(this->TransformedCameraPosition);
    this->TransformedCameraPosition[0] -= direction[0] * multiplier;
    this->TransformedCameraPosition[1] -= direction[1] * multiplier;
    this->TransformedCameraPosition[2] -= direction[2] * multiplier;
    printf("\n%f %f %f\n", this->TransformedCameraPosition[0], this->TransformedCameraPosition[1], this->TransformedCameraPosition[2]);
  }

  // transform camera position to object's coordinate system
  this->TransformedCameraPosition[3] = 1.f;
  this->WorldToObjectMatrix->MultiplyPoint(this->TransformedCameraPosition, this->TransformedCameraPosition);
  double norm =  (fabs(this->TransformedCameraPosition[3]) > 0.0001) ? (1.f / this->TransformedCameraPosition[3]) : 1.f;
  this->TransformedCameraPosition[0] *=norm;
  this->TransformedCameraPosition[1] *=norm;
  this->TransformedCameraPosition[2] *=norm;
    
  this->TransformedDataBounds[0] = this->VoxelNormalizationCoefficients[0] * (this->DataBounds[0] - this->TransformedCameraPosition[0]);
  this->TransformedDataBounds[1] = this->VoxelNormalizationCoefficients[0] * (this->DataBounds[1] - this->TransformedCameraPosition[0]);
  this->TransformedDataBounds[2] = this->VoxelNormalizationCoefficients[1] * (this->DataBounds[2] - this->TransformedCameraPosition[1]);
  this->TransformedDataBounds[3] = this->VoxelNormalizationCoefficients[1] * (this->DataBounds[3] - this->TransformedCameraPosition[1]);
  this->TransformedDataBounds[4] = this->VoxelNormalizationCoefficients[2] * (this->DataBounds[4] - this->TransformedCameraPosition[2]);
  this->TransformedDataBounds[5] = this->VoxelNormalizationCoefficients[2] * (this->DataBounds[5] - this->TransformedCameraPosition[2]);

  // find 2D bounding box
  double minMaxViewportCoordinates[6];
  TransformBlockToViewportCoord(this->DataBounds, minMaxViewportCoordinates);
  this->ViewportBBox[0] = max(0.5f * (minMaxViewportCoordinates[0] + 1.f) * (ViewportSize[0] - 1) - 2, 0);
  this->ViewportBBox[1] = min(0.5f * (minMaxViewportCoordinates[1] + 1.f) * (ViewportSize[0] - 1) + 2, (ViewportSize[0] - 1));
  if (((this->ViewportBBox[1] - this->ViewportBBox[0]) % 2) == 0)
    this->ViewportBBox[1]++; // odd number of rows
  this->ViewportBBox[2] = max(0.5f * (minMaxViewportCoordinates[2] + 1.f) * (ViewportSize[1] - 1) - 2, 0);
  this->ViewportBBox[3] = min(0.5f * (minMaxViewportCoordinates[3] + 1.f) * (ViewportSize[1] - 1) + 2, (ViewportSize[1] - 1)) + 1; // allow for extra row
  if (((this->ViewportBBox[3] - this->ViewportBBox[2]) % 2) == 0)
    this->ViewportBBox[3]++; // odd number of rows
  this->ViewportBBoxSize[0] = max(this->ViewportBBox[1] - this->ViewportBBox[0] + 1, 0);
  this->ViewportBBoxSize[1] = max(this->ViewportBBox[3] - this->ViewportBBox[2] + 1, 0);

  if (this->ViewportBBoxSize[0] == 0 || this->ViewportBBoxSize[0] == 0)
    return; // empty

  const int newFrameBufferSize = (this->ViewportBBoxSize[0] + 1) * (this->ViewportBBoxSize[1] + 1);
  if (this->FrameBuffer == NULL || this->FrameBufferSize < newFrameBufferSize) 
  {
    assert(!this->UseCache);

    this->FrameBufferSize = newFrameBufferSize;
    delete [] this->FrameBuffer;
    this->FrameBuffer = (unsigned char (*)[4])new unsigned char [4 * this->FrameBufferSize];
    
    delete [] this->StencilBuffer;
    this->StencilBuffer = new unsigned char[this->ViewportBBoxSize[0] * this->ViewportBBoxSize[1]];
  }

  // calculate block projection size
  this->AverageBlockProjectionSize = 0;
  for (int bzi = 0; bzi < this->NumBlocks[2]; bzi += ((this->NumBlocks[2] >> 3) + 1)) 
  {
    for (int byi = 0; byi < this->NumBlocks[1]; byi += ((this->NumBlocks[1] >> 3) + 1)) 
    {
      for (int bxi = 0; bxi < this->NumBlocks[0]; bxi += ((this->NumBlocks[0] >> 3) + 1)) 
      {
        double minMax[6];
        this->TransformBlockToViewportCoord(bxi, byi, bzi, minMax);
        double x = fabs(minMax[1] - minMax[0]) * this->ViewportSize[0];
        double y = fabs(minMax[3] - minMax[2]) * this->ViewportSize[1];
        x = max(x, y);
        if (x > this->AverageBlockProjectionSize)
          this->AverageBlockProjectionSize = x;
      }
    }
  }
  this->AverageBlockProjectionSize *= 0.5f;  // viewport is from -1 to 1
}

//---------------------------------------------------------------------------
bool vtkMAFAdaptiveVolumeMapper::PrepareVolumeForRendering() 
//---------------------------------------------------------------------------
{
  // Is data the same?
  if (this->GetInput() != NULL && this->GetInput()->GetMTime() > this->MTime)
    this->ReleaseData();
  else if (this->DataPreprocessed)
    return true; // nothing to do

  // check the data
  if (this->GetInput() && this->GetInput()->GetDataReleased())
    this->GetInput()->Update(); // ensure that the data is loaded
  if (!this->IsDataValid(true))
    return false;

  vtkImageData       *imageData = vtkImageData::SafeDownCast(this->GetInput());
  vtkRectilinearGrid *gridData  = vtkRectilinearGrid::SafeDownCast(this->GetInput());
  
  // find extent
  this->NonUniformGrid = false;
  if (imageData) 
  {
    double dataSpacing[3];
    imageData->GetDimensions(this->DataDimensions);
    imageData->GetOrigin(this->DataOrigin);
    imageData->GetSpacing(dataSpacing);
    int i;
	for (int axis = 0; axis < 3; axis++) 
    {
      delete [] this->VoxelCoordinates[axis];
      this->VoxelCoordinates[axis] = new double [this->DataDimensions[axis] + VoxelBlockSize + 1];
      double f = this->DataOrigin[axis];
      for (i = 0; i < this->DataDimensions[axis]; i++, f += dataSpacing[axis])
        this->VoxelCoordinates[axis][i] = f;
      for (i = 0; i < VoxelBlockSize; i++)
        this->VoxelCoordinates[axis][this->DataDimensions[axis] + i] = f;
    }
  }
  else 
  {
    gridData->GetDimensions(this->DataDimensions);
    this->DataOrigin[0] = gridData->GetXCoordinates()->GetTuple(0)[0];
    this->DataOrigin[1] = gridData->GetYCoordinates()->GetTuple(0)[0];
    this->DataOrigin[2] = gridData->GetZCoordinates()->GetTuple(0)[0];

    for (int axis = 0; axis < 3; axis++) 
    {
      delete [] this->VoxelCoordinates[axis];
      this->VoxelCoordinates[axis] = new double [this->DataDimensions[axis] + VoxelBlockSize + 1];
      
      vtkDataArray *coordinates = (axis == 2) ? gridData->GetZCoordinates() : (axis == 1 ? gridData->GetYCoordinates() : gridData->GetXCoordinates());
      const double spacing = *(coordinates->GetTuple(1)) - *(coordinates->GetTuple(0));
      const double blockSpacingThreshold = 0.01f * spacing + 0.001f;
      int i;
	  for (i = 0; i < this->DataDimensions[axis]; i++) 
      {
        this->VoxelCoordinates[axis][i] = *(coordinates->GetTuple(i));
        if (i > 0 && fabs(this->VoxelCoordinates[axis][i] - this->VoxelCoordinates[axis][i - 1] - spacing) > blockSpacingThreshold) 
        {
          // try to correct the coordinates
          if (i < (this->DataDimensions[axis] - 1) && fabs(*(coordinates->GetTuple(i + 1)) - this->VoxelCoordinates[axis][i - 1] - 2.f * spacing) < blockSpacingThreshold) 
          {
            this->VoxelCoordinates[axis][i]     = this->VoxelCoordinates[axis][i - 1] + spacing;
            this->VoxelCoordinates[axis][i + 1] = this->VoxelCoordinates[axis][i] + spacing;
            i++;
          }
          else 
          {
            this->NonUniformGrid = true;
          }
        }
      }
      for (i = 0; i < VoxelBlockSize; i++)
        this->VoxelCoordinates[axis][this->DataDimensions[axis] + i] = this->VoxelCoordinates[axis][this->DataDimensions[axis] - 1];
    }
  }

  this->FLastVoxel[0] = this->DataDimensions[0] - 0.01f;
  this->FLastVoxel[1] = this->DataDimensions[1] - 0.01f;
  this->FLastVoxel[2] = this->DataDimensions[2] - 0.01f;
  this->LastVoxel[0] = this->DataDimensions[0] - 1;
  this->LastVoxel[1] = this->DataDimensions[1] - 1;
  this->LastVoxel[2] = this->DataDimensions[2] - 1;
  
  this->DataBounds[0] = min(this->VoxelCoordinates[0][0], this->VoxelCoordinates[0][this->DataDimensions[0] - 1]);
  this->DataBounds[1] = max(this->VoxelCoordinates[0][0], this->VoxelCoordinates[0][this->DataDimensions[0] - 1]);
  this->DataBounds[2] = min(this->VoxelCoordinates[1][0], this->VoxelCoordinates[1][this->DataDimensions[1] - 1]);
  this->DataBounds[3] = max(this->VoxelCoordinates[1][0], this->VoxelCoordinates[1][this->DataDimensions[1] - 1]);
  this->DataBounds[4] = min(this->VoxelCoordinates[2][0], this->VoxelCoordinates[2][this->DataDimensions[2] - 1]);
  this->DataBounds[5] = max(this->VoxelCoordinates[2][0], this->VoxelCoordinates[2][this->DataDimensions[2] - 1]);
  
  // init offsets
  const int sliceDimension = this->DataDimensions[0] * this->DataDimensions[1];
  this->VoxelVertIndicesOffsets[0] = 0;
  this->VoxelVertIndicesOffsets[1] = 1;
  this->VoxelVertIndicesOffsets[2] = this->DataDimensions[0] + 1;
  this->VoxelVertIndicesOffsets[3] = this->DataDimensions[0];
  this->VoxelVertIndicesOffsets[4] = this->VoxelVertIndicesOffsets[0] + sliceDimension;
  this->VoxelVertIndicesOffsets[5] = this->VoxelVertIndicesOffsets[1] + sliceDimension;
  this->VoxelVertIndicesOffsets[6] = this->VoxelVertIndicesOffsets[2] + sliceDimension;
  this->VoxelVertIndicesOffsets[7] = this->VoxelVertIndicesOffsets[3] + sliceDimension;
  
  for (int xyz = 0; xyz < 3; xyz++) 
  {
    this->NumBlocks[xyz] = ((this->DataDimensions[xyz] - 1) >> VoxelBlockSizeLog) + 1;
    this->LastBlock[xyz] = this->NumBlocks[xyz] - 1;
  }
  this->TotalNumBlocks = this->NumBlocks[0] * this->NumBlocks[1] * this->NumBlocks[2];
  this->BlockToBlockIncrement[0] = 1;
  this->BlockToBlockIncrement[1] = this->NumBlocks[0];
  this->BlockToBlockIncrement[2] = this->NumBlocks[0] * this->NumBlocks[1];
  this->VoxelToVoxelIncrement[0] = 1;
  this->VoxelToVoxelIncrement[1] = this->DataDimensions[0];
  this->VoxelToVoxelIncrement[2] = this->DataDimensions[0] * this->DataDimensions[1];

  this->DataValueSize = (this->GetDataType() == VTK_CHAR || this->GetDataType() == VTK_UNSIGNED_CHAR) ? sizeof(char) : sizeof(short);
  this->IsDataSigned  = (this->GetDataType() == VTK_CHAR || this->GetDataType() == VTK_SHORT);

  this->VoxelNormalizationCoefficients[0] = this->LastVoxel[0] / (this->DataBounds[1] - this->DataBounds[0]);
  this->VoxelNormalizationCoefficients[1] = this->LastVoxel[1] / (this->DataBounds[3] - this->DataBounds[2]);
  this->VoxelNormalizationCoefficients[2] = this->LastVoxel[2] / (this->DataBounds[5] - this->DataBounds[4]);
  
  // calculate slice sizes for non-uniform grid
  //this->UniformGrid = false;
  if (this->NonUniformGrid) 
  {
    this->VoxelNormalizationCoefficients[2] = 1.f;
    
    // allocate memory for helping arrays
    this->UniformToRectGridMaxIndex = min(this->DataDimensions[2] * 50, 16000);
    delete [] this->SliceSizes;
    this->SliceSizes = (double (*)[VoxelBlockSizeLog + 1])new double [(VoxelBlockSizeLog + 1) * this->DataDimensions[2]];
    delete [] this->UniformToRectGridIndex;
    this->UniformToRectGridIndex = new double [this->UniformToRectGridMaxIndex + 16];

    this->UniformToRectGridMultiplier = double(this->UniformToRectGridMaxIndex) / (this->DataBounds[5] - this->DataBounds[4]);
    const double iUniformToRectGridMultiplier = 1.f / this->UniformToRectGridMultiplier;

    // SliceSize array stores inter-slice distances at several levels of detail
    // UniformToRectGrid arrays help to find cell index by double z-coordinate
    const double *zcoordinates = this->VoxelCoordinates[2];
    int zi, ri;
	for (zi = 0, ri = 1; zi < this->DataDimensions[2]; zi++) 
    {
      // SliceSize
      for (int bzi = 0; bzi <= VoxelBlockSizeLog; bzi++)
        this->SliceSizes[zi][bzi] = (zcoordinates[((zi >> bzi) + 1) << bzi] - zcoordinates[(zi >> bzi) << bzi]);

      // UniformToRectGrid
      if (zi > 0) 
      {
        const double riFromZi = (zcoordinates[zi] - this->DataOrigin[2]) * this->UniformToRectGridMultiplier;
        const int   riFromZiInt = min(int(riFromZi), this->UniformToRectGridMaxIndex);
        const double isegmentLength = 1.f / (zcoordinates[zi] - zcoordinates[zi - 1]);
        const double segmentStart   = zcoordinates[zi - 1] - this->DataOrigin[2];
        const double indexOffset    = double(zi - 1);
        if (isegmentLength > VTK_DOUBLE_MAX)
          continue;

        for ( ; ri <= riFromZiInt; ri++)
          this->UniformToRectGridIndex[ri] = indexOffset + (double(ri) * iUniformToRectGridMultiplier - segmentStart) * isegmentLength;
      }
    }
    
    // special cases
    this->UniformToRectGridIndex[0] = 0.f;
    for ( ; ri <= this->UniformToRectGridMaxIndex; ri++)
      this->UniformToRectGridIndex[ri] = double(this->LastVoxel[2]);
  }

  // prepare min-max arrays (allocate extra memory to avoid "border cases" checks)
  delete [] this->BlockMin;
  this->BlockMin = new char [2 * this->DataValueSize * this->TotalNumBlocks + 16];
  this->BlockMax = (char*)this->BlockMin + this->DataValueSize * this->TotalNumBlocks + 8;
  delete [] this->BlockGradientMax;
  this->BlockGradientMax = new double [this->TotalNumBlocks];

  int ii = this->TotalNumBlocks - 1;
  switch (this->GetDataType()) 
  {
    case VTK_CHAR:
      for ( ; ii >=0; ii--)
        ((char *)BlockMin)[ii] = VTK_CHAR_MAX, ((char *)this->BlockMax)[ii] = VTK_CHAR_MIN;
      break;
    case VTK_UNSIGNED_CHAR:
      for ( ; ii >=0; ii--)
        ((unsigned char *)BlockMin)[ii] = VTK_UNSIGNED_CHAR_MAX, ((unsigned char *)this->BlockMax)[ii] = 0;
      break;
    case VTK_SHORT:
      for ( ; ii >=0; ii--)
        ((short *)BlockMin)[ii] = VTK_SHORT_MAX, ((short *)this->BlockMax)[ii] = VTK_SHORT_MIN;
      break;
    case VTK_UNSIGNED_SHORT:
      for ( ; ii >=0; ii--)
        ((unsigned short *)BlockMin)[ii] = VTK_UNSIGNED_SHORT_MAX, ((unsigned short *)this->BlockMax)[ii] = 0;
      break;
  }
  memset(this->BlockGradientMax, 0, this->TotalNumBlocks * sizeof(double));

  this->DataPointer = this->GetInput()->GetPointData()->GetScalars()->GetVoidPointer(0);
  delete [] this->GradientIndeces;
  this->GradientIndeces = new unsigned short[this->DataDimensions[0] * this->DataDimensions[1] * this->DataDimensions[2]];
  if (this->GradientTable == NULL) 
  {
    this->GradientTable  = new double [2 * (VTK_UNSIGNED_SHORT_MAX + 1)];
    this->GradientTableI = this->GradientTable + (VTK_UNSIGNED_SHORT_MAX + 1);
  }
  switch (this->GetDataType()) 
  {
    case VTK_CHAR:
      this->PrepareMinMaxDataTemplate((const char*)this->DataPointer);
    break;
    case VTK_UNSIGNED_CHAR:
      this->PrepareMinMaxDataTemplate((const unsigned char*)this->DataPointer);
    break;
    case VTK_SHORT:
      this->PrepareMinMaxDataTemplate((const short*)this->DataPointer);
    break;
    case VTK_UNSIGNED_SHORT:
      this->PrepareMinMaxDataTemplate((const unsigned short*)this->DataPointer);
    break;
  }

  this->MTime.Modified();

  this->ClearCaches();

  this->DataPreprocessed = true;

  return true;
}

//---------------------------------------------------------------------------
template <typename DataType> void vtkMAFAdaptiveVolumeMapper::PrepareMinMaxDataTemplate(const DataType *dataPointer) 
//---------------------------------------------------------------------------
{
  DataType *blockMin = (DataType *)this->BlockMin;
  DataType *blockMax = (DataType *)this->BlockMax;
  
  double * const gradientBuffer = new double [this->DataDimensions[0] * this->DataDimensions[1] * this->DataDimensions[2]];
  double *gradientPointer = gradientBuffer;
  double *gradientMax     = this->BlockGradientMax;

  // optimize gradient calculations
  delete [] this->GradientEquationIndecesR[0];
  this->GradientEquationIndecesR[0] = new int [2 * (this->DataDimensions[0] + this->DataDimensions[1] + this->DataDimensions[2])];
  this->GradientEquationIndecesL[0] = this->GradientEquationIndecesR[0] + this->DataDimensions[0] + this->DataDimensions[1] + this->DataDimensions[2];
  delete [] this->GradientEquationDivisor[0];
  this->GradientEquationDivisor[0] = new double [this->DataDimensions[0] + this->DataDimensions[1] + this->DataDimensions[2]];
  int i;
  for (i = 0; i < 3; i++) 
  {
    if (i > 0) 
    {
      this->GradientEquationIndecesR[i] = this->GradientEquationIndecesR[i - 1] + this->DataDimensions[i - 1];
      this->GradientEquationIndecesL[i] = this->GradientEquationIndecesL[i - 1] + this->DataDimensions[i - 1];
      this->GradientEquationDivisor[i]  = this->GradientEquationDivisor[i - 1]  + this->DataDimensions[i - 1];
    }

    int j;
	for (j = 1; j < (this->DataDimensions[i] - 1); j++) 
    {
      this->GradientEquationIndecesR[i][j] = this->VoxelToVoxelIncrement[i];
      this->GradientEquationIndecesL[i][j] = -this->VoxelToVoxelIncrement[i];
      this->GradientEquationDivisor[i][j]  = 1.f / (VoxelCoordinates[i][j + 1] - VoxelCoordinates[i][j - 1]);
    }
    this->GradientEquationIndecesR[i][0] = this->VoxelToVoxelIncrement[i];
    this->GradientEquationIndecesL[i][0] = 0;
    this->GradientEquationDivisor[i][0]  = 1.f / (VoxelCoordinates[i][1] - VoxelCoordinates[i][0]);
    
    this->GradientEquationIndecesR[i][j] = 0;
    this->GradientEquationIndecesL[i][j] = -this->VoxelToVoxelIncrement[i];
    this->GradientEquationDivisor[i][this->DataDimensions[i] - 1] = 1.f / (VoxelCoordinates[i][this->DataDimensions[i] - 1] - VoxelCoordinates[i][this->DataDimensions[i] - 2]);
  }

  // create acceleration structures and compute gradients
  const int bzi = this->NumBlocks[0] * this->NumBlocks[1], byi = this->NumBlocks[0];
  const double *idz = this->GradientEquationDivisor[2];
  for (int zi = 0; zi < this->DataDimensions[2]; zi++, idz++) 
  {
    const int zblock = (zi >> VoxelBlockSizeLog) * this->NumBlocks[0] * this->NumBlocks[1];
    const int zIterations = ((zi & VoxelBlockMaskI) == 0 && zi != 0);

    if ((zi % 4) == 0) // progress
      this->UpdateProgress(0.6f * double(zi) / this->DataDimensions[2]);

    const double *idy = this->GradientEquationDivisor[1];
    for (int yi = 0; yi < this->DataDimensions[1]; yi++, idy++) 
    {
      const int yzblock = (yi >> VoxelBlockSizeLog) * this->NumBlocks[0] + zblock;
      const int yIterations = ((yi & VoxelBlockMaskI) == 0 && yi != 0);

      const double *idx = this->GradientEquationDivisor[0];
      for (int xi = 0; xi < this->DataDimensions[0]; xi++, idx++, dataPointer++, gradientPointer++) 
      {
        const int xIterations = ((xi & VoxelBlockMaskI) == 0 && xi != 0);
        
        DataType dataValue = *dataPointer;
        // gradient
        const double fx = double(int(dataPointer[this->GradientEquationIndecesR[0][xi]]) - dataPointer[this->GradientEquationIndecesL[0][xi]]) * idx[0];
        const double fy = double(int(dataPointer[this->GradientEquationIndecesR[1][yi]]) - dataPointer[this->GradientEquationIndecesL[1][yi]]) * idy[0];
        const double fz = double(int(dataPointer[this->GradientEquationIndecesR[2][zi]]) - dataPointer[this->GradientEquationIndecesL[2][zi]]) * idz[0];
        double gradientValue = sqrt(fx * fx + fy * fy + fz * fz);
        *gradientPointer = gradientValue;

        int zBlockIndex = (xi >> VoxelBlockSizeLog) + yzblock;
        for (int zz = 0; zz <= zIterations; zz++, zBlockIndex -= bzi) 
        {
          int yBlockIndex = zBlockIndex;
          for (int yy = 0; yy <= yIterations; yy++, yBlockIndex -= byi) 
          {
            int blockIndex = yBlockIndex;
            for (int xx = 0; xx <= xIterations; xx++, blockIndex--) 
            {
              DataType &minB = blockMin[blockIndex];
              DataType &maxB = blockMax[blockIndex];
              if (minB > dataValue) // min
                minB = dataValue;
              if (maxB < dataValue) // max
                maxB = dataValue;
              
              if (gradientMax[blockIndex] < gradientValue) // max
                gradientMax[blockIndex] = gradientValue;
            }
          }
        } // for (zz)
        
      } //for (x)
    } //for (y)
  } //for (z)

  // data and gradient range
  this->DataRange[0] = VTK_INT_MAX;
  this->DataRange[1] = VTK_INT_MIN;
  this->GradientRange[0] = this->GradientRange[1] = 0;
  for (int ii = this->TotalNumBlocks - 1; ii >=0; ii--) 
  {
    if (this->DataRange[0] > blockMin[ii])
      this->DataRange[0] = blockMin[ii];
    if (this->DataRange[1] < blockMax[ii])
      this->DataRange[1] = blockMax[ii];
    if (this->GradientRange[1] < gradientMax[ii])
      this->GradientRange[1] = gradientMax[ii];
  }
  if (this->GradientRange[1] <= 0.01f) // to avoid exceptions
    this->GradientRange[1] = 0.01f;
  
  // avoid overflow
  const double maxGVal = this->GradientRange[1];
  double gEpsilon = 0.001f;
  while (int(maxGVal * double(VTK_UNSIGNED_SHORT_MAX) / this->GradientRange[1]) > VTK_UNSIGNED_SHORT_MAX) 
  {
    while ((this->GradientRange[1] + gEpsilon) == this->GradientRange[1])
      gEpsilon *= 2.f;
    
    this->GradientRange[1] += gEpsilon;
  }

  // compress gradient
  gradientPointer = gradientBuffer;
  unsigned short *gradientIndexPointer = this->GradientIndeces;
  const double gradientToIndex = double(VTK_UNSIGNED_SHORT_MAX) / this->GradientRange[1];
  this->GradientToTableIndex  = gradientToIndex;
  const double indexToGradient = this->GradientRange[1] / double(VTK_UNSIGNED_SHORT_MAX);
  this->TableIndexToGradient = indexToGradient;
  for (i = this->DataDimensions[0] * this->DataDimensions[1] * this->DataDimensions[2] - 1; i >= 0; i--)
    gradientIndexPointer[i] = unsigned short(gradientToIndex * gradientPointer[i]); // overflow is already handled
    

  // create gradient look-up table
  for (i = VTK_UNSIGNED_SHORT_MAX; i > 0; i--) 
  {
    this->GradientTable[i]  = double(i) * indexToGradient;
    this->GradientTableI[i] = 1.f / this->GradientTable[i];
  }
  this->GradientTable[0]  = 0.f;
  this->GradientTableI[0] = 0.f;
  
  this->UpdateProgress(1.f);
  delete [] gradientBuffer;
}

//----------------------------------------------------------------------
bool vtkMAFAdaptiveVolumeMapper::InitializeRender(bool setup, vtkRenderer *renderer, vtkVolume *volume) 
//---------------------------------------------------------------------------
{
  if (setup) 
  {
    // check whether there are any surfaces in the scene
    vtkVolumeCollection *volumes = renderer->GetVolumes();
    vtkActorCollection  *actors  = renderer->GetActors();
    vtkProp*   propArray[256];
    this->NumOfVisibleProps = 0;
    volumes->InitTraversal();
    for (vtkVolume *vol = volumes->GetNextVolume(); vol != NULL; vol = volumes->GetNextVolume()) 
    {
      if (vol->GetVisibility() && vol->GetMapper() != this && this->NumOfVisibleProps < 255)
        propArray[this->NumOfVisibleProps++] = vol;
    }
    actors->InitTraversal();
    for (vtkProp *actor = actors->GetNextProp(); actor != NULL; actor = actors->GetNextProp()) 
    {
      if (actor->GetVisibility() && this->NumOfVisibleProps < 255)
        propArray[this->NumOfVisibleProps++] = actor;
    }
    // cull the props
    if (this->NumOfVisibleProps < 255) 
    {
      vtkCullerCollection *cullers = renderer->GetCullers();
      vtkCuller           *culler  = NULL;
      int initialized = 0;
      cullers->InitTraversal();
      for (int num_cullers = 0; num_cullers < cullers->GetNumberOfItems(); num_cullers++)
        culler = (vtkCuller *)cullers->GetItemAsObject(num_cullers);
        culler->Cull(renderer, propArray, this->NumOfVisibleProps, initialized);
    }
    this->MixedRendering = this->NumOfVisibleProps > 0;

    // precompute some transformation related data
    this->PrepareTransformationData(renderer, volume);
    if (this->ViewportBBoxSize[0] <= 0 || this->ViewportBBoxSize[0] <= 0)
      return false; // just in case
    this->PrepareBlockLOD();

    if (!this->UseCache)
      memset(this->FrameBuffer, 0, 4 * this->FrameBufferSize);
    // copy depth buffer
    if (this->MixedRendering) 
    {
      delete [] this->PrevDepthBuffer;
      this->PrevDepthBuffer = this->TransformationNotChanged ? this->DepthBuffer : NULL;
      if (this->PrevDepthBuffer == NULL)
        this->UseCache = false;
      if (!this->TransformationNotChanged)
        delete [] this->DepthBuffer;

      // read zbuffer
      this->DepthBuffer = new double [this->ViewportBBoxSize[0] * this->ViewportBBoxSize[1]];
      glPixelStorei(GL_PACK_ALIGNMENT, 1);
      glDisable( GL_TEXTURE_2D );
      glDisable( GL_SCISSOR_TEST );
      glEnable(GL_DEPTH_TEST);
      glReadPixels(this->ViewportBBox[0], this->ViewportBBox[2], this->ViewportBBoxSize[0], this->ViewportBBoxSize[1], GL_DEPTH_COMPONENT, GL_FLOAT, this->DepthBuffer);
    }
    else 
    {
      this->UseCache = this->UseCache && this->DepthBuffer == NULL;
      delete [] this->DepthBuffer;
      delete [] this->PrevDepthBuffer;
      this->DepthBuffer = this->PrevDepthBuffer = NULL;
    }

    // init OpenGL
    double volumeMatrix[16];
    vtkMatrix4x4::Transpose((double*)volume->GetMatrix()->Element, volumeMatrix);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixd(volumeMatrix);

    // calculate viewports for render-processes
    if (this->NumProcesses == 0 || (this->ViewportBBox[3] - this->ViewportBBox[2]) < 10) 
    {
      this->NumOfRenderingPortions = 1;
      memcpy(this->ViewportBBoxPortions[0], this->ViewportBBox, sizeof(this->ViewportBBox));
    }
    else 
    {
      this->NumOfRenderingPortions = min(6 * this->NumProcesses, vtkMAFAdaptiveVolumeMapperNamespace::MaxNumOfRenderingPortions);
      const int height = ((((this->ViewportBBox[3] - this->ViewportBBox[2]) / this->NumOfRenderingPortions) >> 1) << 1); // even number
      for (int pi = 0, y = this->ViewportBBox[2]; pi < this->NumOfRenderingPortions; pi++) 
      {
        // x is the same
        this->ViewportBBoxPortions[pi][0] = this->ViewportBBox[0];
        this->ViewportBBoxPortions[pi][1] = this->ViewportBBox[1];
        // split y
        this->ViewportBBoxPortions[pi][2] = y;
        this->ViewportBBoxPortions[pi][3] = (pi < (this->NumOfRenderingPortions - 1))  ? (y + height) : y + (((this->ViewportBBox[3] - y) >> 1) << 1);
        y += height + 1;
      }
    }
  }
  else 
  { // restore the settings
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }
  return true;
}

//----------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::PrepareStencilBuffer(vtkRenderer *renderer) 
//---------------------------------------------------------------------------
{
  if (this->TransformationNotChanged && this->PropertiesNotChanged)
    return;

  // prepare gl state
  int viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glDisable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_TEXTURE_2D);
  
  GLint bufferId;
  glGetIntegerv(GL_DRAW_BUFFER, &bufferId);
  glReadBuffer(bufferId);
  
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // backup image if necessary
  bool backupFrameBuffer = this->NumOfVisibleProps > 0;
  unsigned char *frameBuffer = NULL;
  if (backupFrameBuffer) 
  {
    frameBuffer = new unsigned char[3 * this->ViewportBBoxSize[0] * this->ViewportBBoxSize[1]];
    glReadPixels(this->ViewportBBox[0], this->ViewportBBox[2], this->ViewportBBoxSize[0], this->ViewportBBoxSize[1], GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);
  }
  // clear the viewport
  glEnable(GL_SCISSOR_TEST);
  glScissor(this->ViewportBBox[0], this->ViewportBBox[2], this->ViewportBBoxSize[0], this->ViewportBBoxSize[1]);
  double bkgColor[4];
  //glGetFloatv(GL_COLOR_CLEAR_VALUE, bkgColor);
  glGetDoublev(GL_COLOR_CLEAR_VALUE, bkgColor);
  glClearColor(stencilBufferValue / 255.f, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(bkgColor[0], bkgColor[1], bkgColor[2], bkgColor[3]);
  glDisable(GL_SCISSOR_TEST);
  
  // traverse the blocks
  const int dataType = this->GetDataType();
  glBegin(GL_QUADS);
  for (int bzi = 0; bzi < this->NumBlocks[2]; bzi++) 
  {
    const int z = bzi << VoxelBlockSizeLog, z1 = (bzi + 1) << VoxelBlockSizeLog;
    
    for (int byi = 0; byi < this->NumBlocks[1]; byi++) 
    {
      const int y = byi << VoxelBlockSizeLog, y1 = (byi + 1) << VoxelBlockSizeLog;

      for (int bxi = 0; bxi < this->NumBlocks[0]; bxi++) 
      {
        const int block = bxi + this->NumBlocks[0] * (byi + bzi * this->NumBlocks[1]);
        const int x = bxi << VoxelBlockSizeLog, x1 = (bxi + 1) << VoxelBlockSizeLog;

        // skip transparent blocks
        switch (dataType) 
        {
          case VTK_UNSIGNED_SHORT: 
            if (((unsigned short *)this->BlockMax)[block] < this->VisibilityThreshold)
              continue;
          break;
          case VTK_SHORT:
            if (((short *)this->BlockMax)[block] < this->VisibilityThreshold)
              continue;
          break;
          case VTK_UNSIGNED_CHAR:  
            if (((unsigned char *)this->BlockMax)[block] < this->VisibilityThreshold)
              continue;
          break;
          case VTK_CHAR:           
            if (((char *)this->BlockMax)[block] < this->VisibilityThreshold)
              continue;
          break;
        }
  
        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);

        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);
        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);

        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);
        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);

        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);
        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);

        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);
        glVertex3d(this->VoxelCoordinates[0][x],  this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);

        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y1], this->VoxelCoordinates[2][z1]);
        glVertex3d(this->VoxelCoordinates[0][x1], this->VoxelCoordinates[1][y],  this->VoxelCoordinates[2][z1]);
      }
    }
  }
  glEnd();
  // read "stencil" buffer
  glReadPixels(this->ViewportBBox[0], this->ViewportBBox[2], this->ViewportBBoxSize[0], this->ViewportBBoxSize[1], GL_RED, GL_UNSIGNED_BYTE, this->StencilBuffer);
  
  // restore framebuffer
  glViewport(this->ViewportBBox[0], this->ViewportBBox[2], this->ViewportBBoxSize[0], this->ViewportBBoxSize[1]);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glRasterPos3i(-1, -1, 0);

  if (backupFrameBuffer)
    glDrawPixels(this->ViewportBBoxSize[0], this->ViewportBBoxSize[1], GL_RGB, GL_UNSIGNED_BYTE, frameBuffer);
  else
    glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
  glEnable(GL_DEPTH_TEST);

  delete [] frameBuffer;
}

//----------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::PrepareLights(vtkRenderer *renderer, vtkVolume *volume) 
//----------------------------------------------------------------------
{
  this->NumOfLights = 0;

  // initialize lights
  vtkLightCollection *lightsCollection = renderer->GetLights();
  vtkLight *light;
  for (lightsCollection->InitTraversal(); (light = lightsCollection->GetNextItem()) != NULL && this->NumOfLights < vtkMAFAdaptiveVolumeMapperNamespace::MaxNumOfLights; ) 
  {
    if (!light->GetSwitch() || light->GetIntensity() < 0.001f)
      continue;
    
    // get params
    struct LightStructure &lightInfo = this->Lights[this->NumOfLights];
    
    // position
    if (light->GetPositional() && !light->LightTypeIsHeadlight()) // ignore light
      continue;

    lightInfo.Positional = light->GetPositional() != 0;
    light->GetTransformedPosition(lightInfo.Position);
    lightInfo.Position[3] = 1.f;
    this->WorldToObjectMatrix->MultiplyPoint(lightInfo.Position, lightInfo.Position);
    double norm = (fabs(lightInfo.Position[3]) > 0.0001f) ? (1.f / lightInfo.Position[3]) : 1.f;
    lightInfo.Position[0] *= norm;
    lightInfo.Position[1] *= norm;
    lightInfo.Position[2] *= norm;
    lightInfo.Position[3] = 1.f;

    light->GetTransformedFocalPoint(lightInfo.Direction);
    lightInfo.Direction[3] = 1.f;
    this->WorldToObjectMatrix->MultiplyPoint(lightInfo.Direction, lightInfo.Direction);
    norm = (fabs(lightInfo.Direction[3]) > 0.0001f) ? (1.f / lightInfo.Direction[3]) : 1.f;
    lightInfo.Direction[0] = lightInfo.Direction[0] * norm - lightInfo.Position[0];
    lightInfo.Direction[1] = lightInfo.Direction[1] * norm - lightInfo.Position[1];
    lightInfo.Direction[2] = lightInfo.Direction[2] * norm - lightInfo.Position[2];
    lightInfo.Direction[3] = 1.f;

    lightInfo.Intensity = light->GetIntensity();
    
    // normalize direction
    if (!lightInfo.Positional && vtkMath::Norm(lightInfo.Direction) < 0.001f)
      continue; // ignore the light
    vtkMath::Normalize(lightInfo.Direction);
    this->NumOfLights++;
  }
}

//------------------------------------------------------------------------------
double *vtkMAFAdaptiveVolumeMapper::TransformPointByTransformMatrix(const double xyz[3], double xyzCamera[3]) const 
//----------------------------------------------------------------------
{
  const double (*m)[4] = this->ObjectToViewMatrix->Element;

  const double x = xyz[0], y = xyz[1], z = xyz[2];
  double normComponent = 1. / (x * m[3][0] + y * m[3][1] + z * m[3][2] + m[3][3]);
  if (normComponent > double(VTK_DOUBLE_MAX) || normComponent < double(VTK_DOUBLE_MIN))
    normComponent = 1.;
  xyzCamera[0] = double((x * m[0][0] + y * m[0][1] + z * m[0][2] + m[0][3]) * normComponent);
  xyzCamera[1] = double((x * m[1][0] + y * m[1][1] + z * m[1][2] + m[1][3]) * normComponent);
  xyzCamera[2] = double((x * m[2][0] + y * m[2][1] + z * m[2][2] + m[2][3]) * normComponent);
  
  return xyzCamera;
}

//---------------------------------------------------------------------------
double vtkMAFAdaptiveVolumeMapper::CalculateRay(const double vxyz[3], double ray[3], double xyz[3], double dxyz[3]) 
//----------------------------------------------------------------------
{
  const double (*m)[4] = this->ViewToObjectMatrix->Element;
  const double *m0 = m[0], *m1 = m[1], *m2 = m[2], *m3 = m[3];

  const double vx = -1.f + this->PixelSizeX * vxyz[0], vy = -1.f + this->PixelSizeY * vxyz[1], vz = vxyz[2];

  double normComponent = 1.f / (vx * m3[0] + vy * m3[1] + vz * m3[2] + m3[3]);
  if (normComponent > VTK_DOUBLE_MAX || normComponent < VTK_DOUBLE_MIN)
    normComponent = 1.f; // handle overflow
  ray[0] = (vx * m0[0] + vy * m0[1] + vz * m0[2] + m0[3]) * normComponent - this->TransformedCameraPosition[0];
  ray[1] = (vx * m1[0] + vy * m1[1] + vz * m1[2] + m1[3]) * normComponent - this->TransformedCameraPosition[1];
  ray[2] = (vx * m2[0] + vy * m2[1] + vz * m2[2] + m2[3]) * normComponent - this->TransformedCameraPosition[2];

  const double rayLength  = sqrt(ray[0] * ray[0] + ray[1] * ray[1] + ray[2] * ray[2]);
  const double rayLengthI = (rayLength > 0.0001f) ? 1.f / rayLength : 1.f;
  ray[0] *= rayLengthI;
  ray[1] *= rayLengthI;
  ray[2] *= rayLengthI;

  const double nray[3] = {ray[0] * this->VoxelNormalizationCoefficients[0], ray[1] * this->VoxelNormalizationCoefficients[1], ray[2] * this->VoxelNormalizationCoefficients[2]}; 
  const double iray[3] = {1.f / nray[0], 1.f / nray[1], 1.f / nray[2]}; 

  // find starting point
  double tmin = VTK_DOUBLE_MIN, tmax = VTK_DOUBLE_MAX;
  for (int i = 0; i < 3; i++) 
  {
    const double t0 = this->TransformedDataBounds[i << 1] * iray[i];
    const double t1 = this->TransformedDataBounds[(i << 1) + 1] * iray[i];
    if (t0 > t1)
      tmin = max(tmin, t1), tmax = min(tmax, t0);
    else
      tmin = max(tmin, t0), tmax = min(tmax, t1);
  }
  tmin = max(tmin, 0.f);
  if (tmin > tmax)
    return -1.f; // no intersection

  xyz[0] = clip(tmin * nray[0] - this->TransformedDataBounds[0], (double)0.f, this->FLastVoxel[0]);
  xyz[1] = clip(tmin * nray[1] - this->TransformedDataBounds[2], (double)0.f, this->FLastVoxel[1]);
  if (this->NonUniformGrid) 
  {
    int uIndex = int((tmin * nray[2] - this->TransformedDataBounds[4]) * this->UniformToRectGridMultiplier);
    clip(uIndex, 0, UniformToRectGridMaxIndex);
    xyz[2] = this->UniformToRectGridIndex[uIndex];
  }
  else 
  {
    xyz[2] = clip(tmin * nray[2] - this->TransformedDataBounds[4], (double)0.f, this->FLastVoxel[2]);
  }

  static const double maxD = 1.e20f;
  dxyz[0] = fabs(iray[0]) < maxD ? fabs(iray[0]) : maxD;
  dxyz[1] = fabs(iray[1]) < maxD ? fabs(iray[1]) : maxD;
  dxyz[2] = fabs(iray[2]) < maxD ? fabs(iray[2]) : maxD;
  
  return (rayLength - tmin);
}

//---------------------------------------------------------------------------
bool vtkMAFAdaptiveVolumeMapper::IsDataInViewport(double multiplier) const 
//----------------------------------------------------------------------
{
  // find whether the data are in the viewport
  double minMaxViewportCoordinates[6];
  
  TransformBlockToViewportCoord(this->DataBounds, minMaxViewportCoordinates);
  return (minMaxViewportCoordinates[0] < -multiplier || minMaxViewportCoordinates[1] > multiplier ||
          minMaxViewportCoordinates[2] < -multiplier || minMaxViewportCoordinates[3] > multiplier ||
          minMaxViewportCoordinates[4] < -0.1f);
}

//---------------------------------------------------------------------------
bool vtkMAFAdaptiveVolumeMapper::TransformBlockToViewportCoord(int bxi, int byi, int bzi, double minMaxViewportCoordinates[6]) const 
//----------------------------------------------------------------------
{
  const double blockDims[6]     = {this->VoxelCoordinates[0][bxi << VoxelBlockSizeLog], this->VoxelCoordinates[0][(bxi + 1) << VoxelBlockSizeLog],
                                  this->VoxelCoordinates[1][byi << VoxelBlockSizeLog], this->VoxelCoordinates[1][(byi + 1) << VoxelBlockSizeLog],
                                  this->VoxelCoordinates[2][bzi << VoxelBlockSizeLog], this->VoxelCoordinates[2][(bzi + 1) << VoxelBlockSizeLog]};
  return TransformBlockToViewportCoord(blockDims, minMaxViewportCoordinates);
}

//------------------------------------------------------------------------------
bool vtkMAFAdaptiveVolumeMapper::TransformBlockToViewportCoord(const double blockDims[6], double minMaxViewportCoordinates[6]) const 
//----------------------------------------------------------------------
{
  static const int pointIndexToDimIndex[8][3] = {{0, 2, 4}, {1, 2, 4}, {1, 3, 4}, {0, 3, 4}, {0, 2, 5}, {1, 2, 5}, {1, 3, 5}, {0, 3, 5}};
  const double (*m)[4] = this->ObjectToViewMatrix->Element;
  
  // init coordinates
  minMaxViewportCoordinates[0] = minMaxViewportCoordinates[2] = minMaxViewportCoordinates[4] = VTK_DOUBLE_MAX;
  minMaxViewportCoordinates[1] = minMaxViewportCoordinates[3] = minMaxViewportCoordinates[5] = VTK_DOUBLE_MIN;

  for (int pi = 0; pi < 8; pi++) 
  {
    const double x = blockDims[pointIndexToDimIndex[pi][0]], y = blockDims[pointIndexToDimIndex[pi][1]], z = blockDims[pointIndexToDimIndex[pi][2]];
    double normComponent = 1. / (x * m[3][0] + y * m[3][1] + z * m[3][2] + m[3][3]);
    if (normComponent > double(VTK_DOUBLE_MAX) || normComponent < double(VTK_DOUBLE_MIN))
      normComponent = 1.;
    const double xv = double((x * m[0][0] + y * m[0][1] + z * m[0][2] + m[0][3]) * normComponent);
    const double yv = double((x * m[1][0] + y * m[1][1] + z * m[1][2] + m[1][3]) * normComponent);
    const double zv = double((x * m[2][0] + y * m[2][1] + z * m[2][2] + m[2][3]) * normComponent);

    if (xv < minMaxViewportCoordinates[0])
      minMaxViewportCoordinates[0] = xv;
    if (xv > minMaxViewportCoordinates[1])
      minMaxViewportCoordinates[1] = xv;
    
    if (yv < minMaxViewportCoordinates[2])
      minMaxViewportCoordinates[2] = yv;
    if (yv > minMaxViewportCoordinates[3])
      minMaxViewportCoordinates[3] = yv;
    
    if (zv < minMaxViewportCoordinates[4])
      minMaxViewportCoordinates[4] = zv;
    if (zv > minMaxViewportCoordinates[5])
      minMaxViewportCoordinates[5] = zv;
  }

  return (minMaxViewportCoordinates[0] <= 1.1f && minMaxViewportCoordinates[1] >= -1.1f &&
          minMaxViewportCoordinates[2] <= 1.1f && minMaxViewportCoordinates[3] >= -1.1f &&
          minMaxViewportCoordinates[5] >= -0.1f);
}

//------------------------------------------------------------------------------
int vtkMAFAdaptiveVolumeMapper::CalculateViewportAndTransformationChecksum(vtkRenderer *renderer, vtkVolume *volume) const 
//----------------------------------------------------------------------
{
  int checksum = 0;
  
  if (renderer == NULL || renderer->GetActiveCamera() == NULL || renderer->GetRenderWindow() == NULL || volume == NULL)
    return 0;

  renderer->ComputeAspect();
  unsigned char *mPtr = (unsigned char *)volume->GetMatrix()->Element[0];
  int ii;
  for (ii = 0; ii < (16 * sizeof(double)); ii++)
    checksum += int(mPtr[ii] * 3 + 1) * (151 + ii) + int(mPtr[ii] + 7) * (31 * ii + 3);
  if (renderer->GetRenderWindow()->GetStereoRender()) 
  {
    mPtr = (unsigned char *)renderer->GetActiveCamera()->GetPerspectiveTransformMatrix((double)renderer->GetAspect()[0] / renderer->GetAspect()[1], -1, 1)->Element[0];
    for (ii = 0; ii < (16 * sizeof(double)); ii++)
      checksum += int(mPtr[ii] * 7 + 3) * (15 + ii) + int(mPtr[ii] + 9) * (33 * ii + 2);
  }
  mPtr = (unsigned char *)renderer->GetActiveCamera()->GetViewTransformMatrix()->Element[0];
  for (ii = 0; ii < (16 * sizeof(double)); ii++)
    checksum += int(mPtr[ii] * 17 + 31) * (11 + ii) + int(mPtr[ii] + 2) * (3 * ii + 1);

  if (renderer->GetRenderWindow()) 
  {
    if (renderer->GetRenderWindow()->GetStereoRender()) 
    {
      checksum += renderer->GetRenderWindow()->GetStereoType() * 37;
      checksum += int(renderer->GetActiveCamera()->GetEyeAngle() * 10000.f);
    }
    checksum += int(renderer->GetActiveCamera()->GetViewAngle() * 111111.f);
    checksum += int(renderer->GetActiveCamera()->GetUseHorizontalViewAngle());
    mPtr = (unsigned char*)renderer->GetActiveCamera()->GetViewShear();
    for (ii = 0; ii < (3 * sizeof(double)); ii++)
      checksum += int(mPtr[ii] * 2 + 3) * (15 + ii);
    checksum += renderer->GetRenderWindow()->GetSize()[0] * 17 + renderer->GetRenderWindow()->GetSize()[1] * 341;
  }
  // take data dimensions into account
  checksum += this->DataDimensions[0] * 71;

  checksum += this->DataDimensions[1] * 711;
  checksum += this->DataDimensions[2] * 7111;

  //checksum += renderer->GetMTime();
  return checksum;
}

//------------------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::SetInput(vtkDataSet *input) 
//------------------------------------------------------------------------------
{
  if (input == this->GetInput())
    return;
  this->ReleaseData();
  this->ClearCaches();
  this->vtkProcessObject::SetNthInput(0, input);
  this->MTime.Modified();
  this->DataPreprocessed = false;
}

//------------------------------------------------------------------------------
int vtkMAFAdaptiveVolumeMapper::GetDataType() 
//------------------------------------------------------------------------------
{
  if (this->GetInput() && this->GetInput()->GetPointData())
    return this->GetInput()->GetPointData()->GetScalars()->GetDataType();

  return VTK_VOID;
}

//-------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::Update() 
//------------------------------------------------------------------------------
{
  if (vtkImageData::SafeDownCast(this->GetInput()) != NULL || 
      vtkRectilinearGrid::SafeDownCast(this->GetInput()) != NULL) 
  {
    this->GetInput()->UpdateInformation();
    this->GetInput()->SetUpdateExtentToWholeExtent();
    this->GetInput()->Update();
    this->PrepareVolumeForRendering();
  }
}

//------------------------------------------------------------------------------
bool vtkMAFAdaptiveVolumeMapper::IsDataValid(bool warnings) 
//------------------------------------------------------------------------------
{
  vtkDataSet         *inputData = this->GetInput();
  vtkImageData       *imageData = vtkImageData::SafeDownCast(inputData);
  vtkRectilinearGrid *gridData  = vtkRectilinearGrid::SafeDownCast(inputData);
  
  // check the data
  if (inputData == NULL) 
  {
    if (warnings)
      vtkErrorMacro(<< "Volume mapper: No data to render.");
    return false;
  }
  if (imageData == NULL && gridData == NULL) 
  {
    if (warnings)
      vtkErrorMacro(<< "Volume mapper: this data format is not supported");
    return false;
  }

  int dataType = this->GetDataType();
  if (dataType != VTK_SHORT && dataType != VTK_UNSIGNED_SHORT &&
      dataType != VTK_CHAR && dataType != VTK_UNSIGNED_CHAR) 
  {
    if (warnings)
      vtkErrorMacro(<< "Volume mapper: the data type is not supported");
    return false;
  }

  // check image data
  if (imageData) 
  {
    int extent[6];
    imageData->GetExtent(extent);
    if (extent[4] >= extent[5]) 
    {
      if (warnings)
        vtkErrorMacro(<< "Volume mapper: 2D datasets are not supported");
      return false;
    }

    if (imageData->GetNumberOfScalarComponents() > 1) 
    {
      if (warnings)
        vtkErrorMacro(<< "Volume mapper: only monochrome images are supported.");
      return false;
    }

    return true;
  }


  if (gridData) 
  {
    if (gridData->GetPointData() == NULL || gridData->GetPointData()->GetScalars() == NULL ||
      gridData->GetXCoordinates()->GetNumberOfTuples() < 2 ||
      gridData->GetYCoordinates()->GetNumberOfTuples() < 2 ||
      gridData->GetZCoordinates()->GetNumberOfTuples() < 2) 
    {
      if (warnings)
        vtkErrorMacro(<< "Volume mapper: the dataset is empty.");
      return false;
    }
  }

  return true;
}

//------------------------------------------------------------------------------
void vtkMAFAdaptiveVolumeMapper::PrepareBlockLOD() 
//------------------------------------------------------------------------------
{
  const void *dataPointer = this->GetInput()->GetPointData()->GetScalars()->GetVoidPointer(0);
  switch (this->GetDataType()) 
  {
    case VTK_CHAR:
      this->PrepareBlockLODTemplate((const char*)dataPointer);
    break;
    case VTK_UNSIGNED_CHAR:
      this->PrepareBlockLODTemplate((const unsigned char*)dataPointer);
    break;
    case VTK_SHORT:
      this->PrepareBlockLODTemplate((const short*)dataPointer);
    break;
    case VTK_UNSIGNED_SHORT:
      this->PrepareBlockLODTemplate((const unsigned short*)dataPointer);
    break;
  }
}

//------------------------------------------------------------------------------    
template <typename DataType> void vtkMAFAdaptiveVolumeMapper::PrepareBlockLODTemplate(const DataType *dataPointer)
//------------------------------------------------------------------------------    
{
  const DataType *blockMin = (DataType *)this->BlockMin;
  const DataType *blockMax = (DataType *)this->BlockMax;
  const DataType  visibilityThreshold = (DataType)(min((int)this->VisibilityThreshold, this->DataRange[1]));

  this->InterpolationThresholdIndex = vtkMAFAdaptiveVolumeMapper::Interpolation ? int(GradientToTableIndex * (this->EnableOptimization ? 0.2f : 0.05f) * this->GradientRange[1]) : (VTK_UNSIGNED_SHORT_MAX - 1);

  if (this->BlockLOD == NULL)
    this->BlockLOD = new unsigned char [this->TotalNumBlocks];
  unsigned char *blockLOD = this->BlockLOD;

  // additional constants
  const double voxelsInPixel = clip(((double)VoxelBlockSize / (this->AverageBlockProjectionSize + 0.1f)), (double)0.3f, (double)2.f) * (this->EnableOptimization ? 2.f : 1.f);
  const int   wholeRange = int((this->DataRange[1] - visibilityThreshold) * voxelsInPixel + 16);
  const int   lodThresholds[VoxelBlockSizeLog + 1] = { wholeRange >> 5, wholeRange >> 4, wholeRange >> 3, VTK_INT_MAX };
  
  // process every block
  for (int bzi = 0; bzi < this->NumBlocks[2]; bzi++) 
  {
    const bool incompleteBlockZ = (bzi == this->LastBlock[2]) && (this->DataDimensions[2] & VoxelBlockMaskI);
    
    for (int byi = 0; byi < this->NumBlocks[1]; byi++) {
      const bool incompleteBlockY = incompleteBlockZ || ((byi == this->LastBlock[1]) && (this->DataDimensions[1] & VoxelBlockMaskI));

      for (int bxi = 0; bxi < this->NumBlocks[0]; bxi++, blockMin++, blockMax++, blockLOD++) 
      {
        const bool incompleteBlockX = incompleteBlockY || ((bxi == this->LastBlock[0]) && (this->DataDimensions[0] & VoxelBlockMaskI));
        if (incompleteBlockX) 
        {
          *blockLOD = 0;
          continue;
        }

        //////////////// analyze the block and set its LOD
        // special case: transparent block
        if (*blockMax < visibilityThreshold) 
        {
          *blockLOD = VoxelBlockSizeLog;
          continue;
        }

        // analyse the range and gradients !!!
        const DataType range = *blockMax - *blockMin;
        for (int lod = 0; lodThresholds[lod] < range; lod++)
        { ; }
        *blockLOD = 0;//VoxelBlockSizeLog - lod;
      }
    }
  }
}

//------------------------------------------------------------------------------
bool vtkMAFAdaptiveVolumeMapper::PrepareTransferFunctionForRendering(vtkVolumeProperty *property) 
//------------------------------------------------------------------------------
{
  if (property == NULL)
    return false;

  this->PropertiesNotChanged = true;
  if (property->GetMTime() < this->TransferFunctionSamplingTime &&
      this->GetMTime() < this->TransferFunctionSamplingTime &&
      TFCacheMemory != NULL)
    return true; // cache is up to date
  this->PropertiesNotChanged = false;

  this->AuxTimer->StartTimer();

  // sample the function
  vtkVolumeProperty2 *property2 = vtkVolumeProperty2::SafeDownCast(property);
  this->TransferFunction2D = property2 != NULL;
  if (property2) 
  {
    vtkTransferFunction2D *function = property2->GetTransferFunction2D();
    if (!function)
      return false;

    const double *_valueRange = function->GetRange();
    const double *_gradientRange = function->GetGradientRange();
    if (_valueRange[0] > _valueRange[1] || _gradientRange[0] > _gradientRange[1])
      return false;

    const int valueRange[2] = { max(int(_valueRange[0]), this->DataRange[0]), min(int(_valueRange[1]), this->DataRange[1]) };
    const int gradientRange[2] = { max(int(_gradientRange[0] * this->GradientToTableIndex), 0), min(int(_gradientRange[1] * this->GradientToTableIndex), VTK_UNSIGNED_SHORT_MAX) };
    
    const int vsize = valueRange[1] - valueRange[0] + 1;
    const int gsize = min(gradientRange[1] - gradientRange[0] + 1, 255);

    if (vsize < 1 || gsize < 1)
      return false;

    this->VisibilityThreshold = valueRange[0] + 1;

    // create look-up tables
    delete [] this->DataToTFLookUpTableRealPointer;
    this->DataToTFLookUpTableRealPointer = new unsigned int [this->DataRange[1] - this->DataRange[0] + 16];
    this->DataToTFLookUpTable = this->DataToTFLookUpTableRealPointer - this->DataRange[0];
    if (this->GradientToTFLookUpTable == NULL)
      this->GradientToTFLookUpTable = new unsigned int [VTK_UNSIGNED_SHORT_MAX + 1];
    // temp tables
    double * const vTable = new double [vsize + gsize];
    double * const gTable = vTable + vsize;
    
    // value table
    int vi;
	for (vi = this->DataRange[0]; vi <= valueRange[0]; vi++)
      this->DataToTFLookUpTable[vi] = 0;
    for ( ; vi <= valueRange[1]; vi++)
      this->DataToTFLookUpTable[vi] = vi - valueRange[0];
    for ( ; vi <= DataRange[1]; vi++)
      this->DataToTFLookUpTable[vi] = vsize - 1;
    
    for (vi = 0; vi < vsize; vi++)
      vTable[vi] = vi + valueRange[0];
    
    // gradient table
    const double gradientCompression  = double(gsize - 1) / (gradientRange[1] - gradientRange[0]);
    const double gradientCompressionI = 1.f / gradientCompression;
    int gi;
	for (gi = 0; gi <= gradientRange[0]; gi++)
      this->GradientToTFLookUpTable[gi] = 0;
    for ( ; gi <= gradientRange[1]; gi++)
      this->GradientToTFLookUpTable[gi] = clip(int(double(gi - gradientRange[0]) * gradientCompression), 0, gsize - 1) * vsize;
    for ( ; gi <= VTK_UNSIGNED_SHORT_MAX; gi++)
      this->GradientToTFLookUpTable[gi] = (gsize - 1) * vsize;

    for (gi = 0; gi < gsize; gi++)
      gTable[gi] = this->GradientTable[clip(int(double(gi) * gradientCompressionI) + gradientRange[0], 0, VTK_UNSIGNED_SHORT_MAX)];

    // allocate memory for TF cache
    const int memSize = 2 * vsize * gsize;

    if (this->TFCacheMemory == NULL || this->TFCacheMemorySize < memSize) 
    {
      delete [] this->TFCacheMemory;
      this->TFCacheMemory = new double [memSize];
      this->TFCacheMemorySize = memSize;
    }
    this->OpacityCache = this->TFCacheMemory;
    this->RgbdCache    = (unsigned char*)(this->OpacityCache + vsize * gsize);

    // sample function
    function->GetTable(vsize, vTable, gsize, gTable, this->OpacityCache, this->RgbdCache);
    delete [] vTable;
  }
  else 
  {
    assert(false); // not implemented yet
  }

  this->AuxTimer->StopTimer();
  this->TimeToCreateTFCache = (double)this->AuxTimer->GetElapsedTime();
  printf("Transfer function sampling time: %.2f\n", this->TimeToCreateTFCache);

  this->TransferFunctionSamplingTime.Modified();
  return true;
}

//------------------------------------------------------------------------------
const int *vtkMAFAdaptiveVolumeMapper::GetDataRange() 
//------------------------------------------------------------------------------
{
  if (!this->PrepareVolumeForRendering()) 
  {
    this->DataRange[0] = VTK_INT_MIN;
    this->DataRange[1] = VTK_INT_MAX;
  }
  return this->DataRange;  
}

//------------------------------------------------------------------------------
const double *vtkMAFAdaptiveVolumeMapper::GetGradientRange() 
//------------------------------------------------------------------------------
{
  if (!this->PrepareVolumeForRendering()) 
  {
    this->GradientRange[0] = 0;
    this->GradientRange[1] = VTK_DOUBLE_MAX;
  }
  return this->GradientRange;  
}
