/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkALBAAdaptiveVolumeMapper.h,v $
  Language:  C++
  Date:      $Date: 
  Version:   $Revision: 


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
// .NAME vtkALBAAdaptiveVolumeMapper - direct rendering of volume data

// .SECTION Description
// vtkALBAAdaptiveVolumeMapper performs a fast volume rendering of volume data using adaptive (LOD) techniques
// This class supports both 1D and 2D transfer functions (vtkALBATransferFunction2D). Also, unlike the standard VTK volume renderers, this class supports vtkRectilinearGrid objects.
// It is currently assumed that X and Y spacing in volume data is constant. If this stops being the case, the code should be modified (see vtkALBADistanceFilter).
//
// Current limitations: the renderer supports multiresolution volumes to accelerate the rendering. This optimisation however is heavely dependent on the heuristics that are used to assign 
// level-of-detail to different blocks of the volume. The current heuristics are trivial and some work have to be done to improve them (see PrepareBlockLODTemplate). When this is done the rendering speed can
// increase dramatically..

// .SECTION see also
// vtkVolumeMapper vtkALBATransferFunction2D

#ifndef __vtkALBAAdaptiveVolumeMapper_h
#define __vtkALBAAdaptiveVolumeMapper_h

//------------------------------------------------------------------------------
// Includes:
//------------------------------------------------------------------------------
#include "albaConfigure.h"


#include "vtkMultiThreader.h" 
#include <mutex>

#include "vtkVolumeMapper.h"

/**
name space name: namespace vtkALBAAdaptiveVolumeMapperNamespace
*/
namespace vtkALBAAdaptiveVolumeMapperNamespace 
{
  const int VoxelBlockSizeLog = 3;
  const int VoxelBlockSize = 1 << VoxelBlockSizeLog; // block size should be a power of 2

  const int MaxNumOfLights    = 16;
  const int MaxNumOfRenderingPortions = 16;
};


//------------------------------------------------------------------------------
// forward declarations
//------------------------------------------------------------------------------
class vtkMatrix4x4;
class vtkVolumeProperty;
/**
class name: vtkALBAAdaptiveVolumeMapper.
*/
class ALBA_EXPORT vtkALBAAdaptiveVolumeMapper : public vtkVolumeMapper 
{
public:
  /** create an instance of the object */
  static vtkALBAAdaptiveVolumeMapper *New();
  
  /** Input should be either structured points or rectilinear grid*/
  void  SetInput(vtkDataSet *input);
  //vtkDataSet*  GetInput() { return vtkVolumeMapper::GetInput(); }

  /** The main method. It will perform all initializations and then 
  call RenderServer that distributes render portions between rendering threads.*/
  void Render(vtkRenderer *ren, vtkVolume *vol);
  
  /** Get optimizations that can affect picture quality*/
  vtkGetMacro(EnableAutoLOD, int);
  /** Set optimizations that can affect picture quality*/   
  void SetEnableAutoLOD(int val) { this->EnableAutoLOD = val; }
  /** On/Off MAcro for optimizations that can affect picture quality*/   
  vtkBooleanMacro(EnableAutoLOD, int)

  /** Retrieve interpolation of the volume mapper */
  static bool GetInterpolation() { return vtkALBAAdaptiveVolumeMapper::Interpolation; }
  /** Set interpolation of the volume mapper */
  static void SetInterpolation(bool val) { vtkALBAAdaptiveVolumeMapper::Interpolation = val; }

  /** Update the mapper */
  void Update();

  /** Check weather the input data can be rendered. The renderer supports regular and rectilinear 
  grids with unsigned/signed char or short values.*/
  bool IsDataValid(bool warnings);

  /** get range of the input data*/
  const int   *GetDataRange();
  /** get range of the gradient*/
  const double *GetGradientRange();

  /**  Pick object  */
  bool  Pick(int x, int y, vtkRenderer *renderer, vtkVolume *volume, double opacityThreshold = 0.05f);
protected:
  /** constructor */
  vtkALBAAdaptiveVolumeMapper();
  /** destructor */
  ~vtkALBAAdaptiveVolumeMapper();

  /**
  RenderServer is responsible for distributing render tasks between threads. 
  Actual rendering is performed by RenderRegion method. This method goes through the image
  performing ray casting with sub-sampling. When rendering has finished the image is copied to frame buffer
  by DrawFrameBuffer method.*/
  void RenderServer(vtkRenderer *ren, vtkVolume *vol);
  /** RenderRegion performs Actual rendering*/
  void RenderRegion(const int viewportBBox[4]);
  /**  When rendering has finished the image is copied to frame buffer  by this method*/
  void DrawFrameBuffer();
  
  /** Used by Pick in order to trace ray */
  template<typename DataType> void TraceRay(const double vxyz[3], unsigned char pixel[4], const DataType *dataPointer);

  /** Execute all initialization routines.*/
  virtual bool InitializeRender(bool setup, vtkRenderer *renderer, vtkVolume *volume);

  /** This method creates acceleration structures, defines volume-specific variables such as DataBounds
  and calculate gradients.*/
  virtual bool PrepareVolumeForRendering();
  /** create acceleration structures and compute gradients */
  template <typename DataType> void PrepareMinMaxDataTemplate(const DataType *dataPointer);

  /** Sample the transfer function into array*/
  virtual bool PrepareTransferFunctionForRendering(vtkVolumeProperty *property);

  /** Pre-compute all transformation-related variables: matrices, viewport bounding boxes, etc*/
  virtual void PrepareTransformationData(vtkRenderer *renderer, vtkVolume *volume);

  /** Compute Level-of-detail for each block. Transparent blocks are assigned lowest LOD, blocks with high gradients are assigned high LOD, etc.*/
  virtual void PrepareBlockLOD();
  /** analyze the blocks and set their LOD */
  template <typename DataType> void PrepareBlockLODTemplate(const DataType *dataPointer);

  /** Draw all non-transparent blocks to stencil buffer. This buffer is than used to skip tracing rays that does not hit visible cells.*/
  virtual void PrepareStencilBuffer(vtkRenderer *renderer);

  /** Put information about lights into structures optimized for rendering.*/
  virtual void PrepareLights(vtkRenderer *renderer, vtkVolume *volume);

  /** Call this method to make cached data invalid. This does not affect volume-related caches (min-max blocks, etc).*/
  virtual void ClearCaches() { this->ViewportAndTransformationChecksum = 0; }
  
  /** Transformation-related routine*/
  bool IsDataInViewport(double multiplier) const;
  /** Transformation-related routine*/
  bool TransformBlockToViewportCoord(int x, int y, int z, double minMaxViewportCoordinates[6]) const;
  /** Transformation-related routine*/
  bool TransformBlockToViewportCoord(const double blockDims[6], double minMaxViewportCoordinates[6]) const;
  /** Transformation-related routine*/
  double *TransformPointByTransformMatrix(const double xyz[3], double xyzCamera[3]) const;
  /** Transformation-related routine*/
  int   CalculateViewportAndTransformationChecksum(vtkRenderer *renderer, vtkVolume *volume) const;
  /** Transformation-related routine*/
  double CalculateRay(const double vxyz[3], double ray[3], double xyz[3], double dxyz[3]);

  /** This method can be used for debugging or special purposes*/
  void DrawBlock(int blockid, bool wireframe); 

  /** Get the amount of time this renderer is allowed to spend rendering its scene  */
  static double GetAllocatedRenderTime(vtkRenderer *renderer);

  /** Get data type id (VTK_CHAR, VTK_SHORT, etc).*/
  int  GetDataType();

  /** Release all allocated memory.*/
  void ReleaseData();

  //------------------------------- external parameters
  int            EnableAutoLOD;
  static bool    Interpolation;

  //------------------------------- internal parameters and helping objects
  bool           DataPreprocessed;

  // acceleration structures: min-max blocks
  int            TotalNumBlocks;
  int            NumBlocks[3];
  int            BlockToBlockIncrement[3];
  int            LastBlock[3];
  void          *BlockMin;
  void          *BlockMax;
  double         *BlockGradientMax;
  int            VoxelVertIndicesOffsets[8];


  // data-related variables
  void           *DataPointer;
  double         DataOrigin[3];
  double          DataBounds[6];
  int            DataDimensions[3];
  int            LastVoxel[3];
  double          FLastVoxel[3];
  int            VoxelToVoxelIncrement[3];
  double*         VoxelCoordinates[3];
  int*           GradientEquationIndecesR[3]; // this is used in gradient calculations
  int*           GradientEquationIndecesL[3]; // this is used in gradient calculations
  double*         GradientEquationDivisor[3];  // this is used in gradient calculations
  
  bool           NonUniformGrid;
  double          VoxelNormalizationCoefficients[3];  // 1 / VoxelSize for uniform grids. 1 for rectilinear grids
  double         (*SliceSizes)[vtkALBAAdaptiveVolumeMapperNamespace::VoxelBlockSizeLog + 1];
  double          *UniformToRectGridIndex;            // used to find indexes in rectilinear grid
  int            UniformToRectGridMaxIndex;
  double          UniformToRectGridMultiplier;

  bool           IsDataSigned;
  int            DataValueSize;
  int            DataRange[2];

  // gradients
  unsigned short *GradientIndeces; // real values should be taken from GradientTable
  double           GradientRange[2];
  double          *GradientTable;
  double          *GradientTableI;
  double           GradientToTableIndex;
  double           TableIndexToGradient;

  // optimization
  bool           EnableOptimization;
  bool           PrevEnableOptimization;
  double          TimeToDrawNotOptimized;
  int            InterpolationThresholdIndex;

  // buffers
  unsigned char (*FrameBuffer)[4];
  int            FrameBufferSize;
  bool           UseCache;    // if true and depth of the pixel has not changed, the pixel is not traced and its previous value us used.

  double          *DepthBuffer;
  double          *PrevDepthBuffer;

  unsigned char  *StencilBuffer;


  // mixed rendering
  int            NumOfVisibleProps;
  bool           MixedRendering;


  // viewport
  int            ViewportSize[2];
  int            ViewportPosition[2];
  int            ViewportPosition2[2];
  int            ViewportBBox[4];  // portion of the viewport with object projection
  int            ViewportBBoxSize[2];
  double          PixelSizeX;
  double          PixelSizeY;
  int            ViewportBBoxPortions[vtkALBAAdaptiveVolumeMapperNamespace::MaxNumOfRenderingPortions][4];
  int            NumOfRenderingPortions;
  int            NumOfRenderingPortionsLeft;

  // transformation
  vtkMatrix4x4  *ObjectToViewMatrix;
  vtkMatrix4x4  *ViewToObjectMatrix;
  vtkMatrix4x4  *WorldToObjectMatrix;
  int            ViewportAndTransformationChecksum;
  bool           TransformationNotChanged;

  double         TransformedCameraPosition[4];
  double          TransformedDataBounds[6];   // so the voxel size is 1x1x1

  double          AverageBlockProjectionSize;

  // transfer-function caches
  bool           TransferFunction2D;
  int            VisibilityThreshold;      // maximum fully transparent value
  
  unsigned int  *DataToTFLookUpTableRealPointer;
  unsigned int  *DataToTFLookUpTable;
  unsigned int  *GradientToTFLookUpTable;

  double         *TFCacheMemory;
  int            TFCacheMemorySize;

  // 1D transfer function
  
  // 2D transfer function
  double         *OpacityCache;
  unsigned char *RgbdCache;
  
  bool           PropertiesNotChanged;
  vtkTimeStamp   TransferFunctionSamplingTime;
  double          EarlyRayTerminationTransparency;
  double          TimeToCreateTFCache;
  
  vtkTimerLog   *AuxTimer;

  // level-of-detail caches
  unsigned char *BlockLOD; // level of detail for each block (from 0 to VoxelBlockSizeLog)

  // lighting
  int   NumOfLights;
  /**
    struct name : LightStructure
    */
  struct LightStructure 
  {
    // type
    bool  Positional; // only head light and directional lights are supported

    // position
    double Position[4];
    double Direction[4];
    
    //color
    double Intensity; // Kd multiplied by light color
    } Lights[vtkALBAAdaptiveVolumeMapperNamespace::MaxNumOfLights];

  // statistics
  int            NumOfTracedRays;
  int            NumOfInterpolatedRays;
  int            NumOfProcessedVoxels;
  int            NumOfSkippedVoxels;

  // multi-threading
  int                    NumProcesses;
  vtkMultiThreader      *Controller;
  std::mutex RenderingQueueCS;
  std::mutex ThreadLockCS;

  /** Render Process handling multithread */
  static VTK_THREAD_RETURN_TYPE RenderProcess(void *pThreadInfoStruct);

private:
  /** Copy Constructor , not implemented.*/
  vtkALBAAdaptiveVolumeMapper(const vtkALBAAdaptiveVolumeMapper&);
  /** operator =, not implemented */
  void operator=(const vtkALBAAdaptiveVolumeMapper&);
};
#endif

