/*==============================================================================

Program:   Multimod Application framework RELOADED
Module:    $RCSfile: vtkContourVolumeMapper.cxx,v $
Language:  C++
Date:      $Date: 2006-11-14 10:21:10 $
Version:   $Revision: 1.9 $
Authors:   Alexander Savenko, Nigel McFarlane

================================================================================
Copyright (c) 2002/2006 University of Bedfordshire, UK (www.beds.ac.uk) 
All rights reserved.
===============================================================================*/


// CONTENTS
//
// vtkCxxRevisionMacro()
// vtkStandardNewMacro()
// vtkContourVolumeMapper()
// ~vtkContourVolumeMapper()
// PrintSelf()
// SetInput()
// EstimteRelevantVolume()
// EstimateRelevantVolumeTemplate()
// Render()
// PrepareAccelerationDataTemplate()
// EnableClipPlanes()
// Update()
// IsDataValid()
// GetDataType()
// ReleaseData()
// GetOutput()
// InitializeRender()
// DrawCache()
// RenderMCubes()
// CreateMCubes()
// EstimateTrianglesNoOpt()
// PrepareContours()
// PrepareContoursTemplate()
// DepthOfVertex()
// CalculateDepthMatrix()
// SortTriangles()
//
// Polyline2D::Reallocate()
// Polyline2D::Allocate()
// Polyline2D::Polyline2D()
// Polyline2D::AddNextLine()
// Polyline2D::Merge()
// Polyline2D::Close()
// Polyline2D::UpdateBoundingBox()
// Polyline2D::FindClosestPolyline()    - commented out
// Polyline2D::FindSubPolyline()        -     "      "
// Polyline2D::SplitPolyline()          -     "      "
// Polyline2D::Move()
// Polyline2D::IsInsideOf()
//
// ListOfPolyline2D::Clear()
// ListOfPolyline2D::IsInside()
// ListOfPolyline2D::FindContour()



#include "mafDefines.h"
#include <assert.h>
#include <vector>

#include "vtkObjectFactory.h"
#include "vtkMatrix4x4.h"
#include "vtkMath.h"
#include "vtkVolume.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkTransform.h"
#include "vtkPlaneCollection.h"
#include "vtkPlane.h"
#include "vtkDataArray.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkTimerLog.h"
#include "vtkVolumeProperty.h"
#include "vtkMarchingCubesCases.h"
#include "vtkMarchingSquaresCases.h"
#include "vtkContourVolumeMapper.h"
#include <algorithm>
#include <fstream>


static const vtkMarchingCubesTriangleCases* marchingCubesCases = vtkMarchingCubesTriangleCases::GetCases();

using namespace vtkContourVolumeMapperNamespace;

vtkCxxRevisionMacro(vtkContourVolumeMapper, "$Revision: 1.9 $");
vtkStandardNewMacro(vtkContourVolumeMapper);



//------------------------------------------------------------------------------
vtkContourVolumeMapper::vtkContourVolumeMapper()
//------------------------------------------------------------------------------
{

  m_Alpha=1.0;

  this->EnableAutoLOD = true;
  this->EnableContourAnalysis = false;

  this->BlockMinMax = NULL;
  this->TriangleCache[0] = this->TriangleCache[1] = NULL;
  this->TimeToDrawNotOptimized = this->TimeToDrawNotOptimizedCache = 0;
  this->TriangleCacheSize[0] = this->TriangleCacheSize[1] = 0;
  this->VoxelCoordinates[0] = this->VoxelCoordinates[1] = this->VoxelCoordinates[2] = NULL;

  this->TransformMatrix = vtkMatrix4x4::New();
  this->VolumeMatrix    = vtkMatrix4x4::New();

  // set initial value of no. of triangles to zero to show that values are undefined
  this->NumberOfTriangles[0] = 0 ;
  this->NumberOfTriangles[1] = 0 ;

  // sorting triangles
  this->ntriangles_previous[0] = 0 ;
  this->ntriangles_previous[1] = 0 ;
  this->ordered_vertices[0] = NULL ;
  this->ordered_vertices[1] = NULL ;

}


//------------------------------------------------------------------------------
vtkContourVolumeMapper::~vtkContourVolumeMapper()
//------------------------------------------------------------------------------
{
  // delete textures if any
  ReleaseData();

  // delete vertex order array
  delete[] ordered_vertices[0] ;
  delete[] ordered_vertices[1] ;

  this->TransformMatrix->Delete();
  this->VolumeMatrix->Delete();
}



//------------------------------------------------------------------------------
void vtkContourVolumeMapper::PrintSelf(ostream& os, vtkIndent indent)
//------------------------------------------------------------------------------
{}



//------------------------------------------------------------------------------
// The input should be either vtkImageData or vtkRectilinearGrid
void vtkContourVolumeMapper::SetInput(vtkDataSet *input)
//------------------------------------------------------------------------------
{
  this->ReleaseData();
  double b[2];
  input->GetScalarRange(b);
  m_MAXScalar=b[1];
  this->vtkProcessObject::SetNthInput(0, input);
}




//------------------------------------------------------------------------------
// This is the first function to be called before Render()
// It is used by mafPipeIsosurface::Create() to set the intial contour to an acceptable value.
// Calls EstimateRelevantVolumeTemplate() with correct scalar datatype
// Returns the fraction of blocks which contain the contour.
float vtkContourVolumeMapper::EstimateRelevantVolume(const double contourValue)
//------------------------------------------------------------------------------
{
  switch (this->GetDataType()) {
    case VTK_CHAR:
      return EstimateRelevantVolumeTemplate((char)contourValue);
    case VTK_UNSIGNED_CHAR:
      return EstimateRelevantVolumeTemplate((unsigned char)contourValue);
    case VTK_SHORT:
      return EstimateRelevantVolumeTemplate((short)contourValue);
    case VTK_UNSIGNED_SHORT:
      return EstimateRelevantVolumeTemplate((unsigned short)contourValue);
    case VTK_FLOAT:
      return EstimateRelevantVolumeTemplate((float)contourValue);
    default:
      return 0.f;
  }
  return 0.f;
}




//------------------------------------------------------------------------------
// Template function corresponding to EstimateRelevantVolume()
// Calls PrepareAccelerationDataTemplate() to set up the blocks,
// and returns the fraction of blocks which contain the contour.
template <typename DataType> float vtkContourVolumeMapper::EstimateRelevantVolumeTemplate(const DataType ContourValue)
//------------------------------------------------------------------------------
{
  if (this->GetInput() == NULL || this->GetInput()->GetPointData() == NULL || this->GetInput()->GetPointData()->GetScalars() == NULL)
    return 0.f;

  const DataType *dataPointer = (const DataType *)this->GetInput()->GetPointData()->GetScalars()->GetVoidPointer(0);
  PrepareAccelerationDataTemplate((const DataType*)dataPointer);

  // go through the blocks now
  int relevant = 0;
  const DataType (*BlockMinMax)[2] = (DataType (*)[2])this->BlockMinMax;
  for (int bzi = 0; bzi < this->NumBlocks[2]; bzi++) {
    const int zblockSize = (((bzi + 1) < this->NumBlocks[2]) ? VoxelBlockSize : (this->DataDimensions[2] - 1 - (bzi << VoxelBlockSizeLog)));

    for (int byi = 0; byi < this->NumBlocks[1]; byi++) {
      const int yblockSize = (((byi + 1) < this->NumBlocks[1]) ? VoxelBlockSize : (this->DataDimensions[1] - 1 - (byi << VoxelBlockSizeLog)));

      for (int bxi = 0; bxi < this->NumBlocks[0]; bxi++) {
        const int block = bxi + this->NumBlocks[0] * (byi + bzi * this->NumBlocks[1]);
        const int xblockSize = (((bxi + 1) < this->NumBlocks[0]) ? VoxelBlockSize : (this->DataDimensions[0] - 1 - (bxi << VoxelBlockSizeLog)));

        if (ContourValue >= BlockMinMax[block][0] && ContourValue <= BlockMinMax[block][1])
          relevant++;
      }
    }
  }

  return (float)relevant / (this->NumBlocks[0] * this->NumBlocks[1] * this->NumBlocks[2] - 1);
}




//------------------------------------------------------------------------------
// Render the isosurface
// If data has been cached for this contour value
//        Calls    DrawCache() if data has been cached for this contour value
// Else
//        Calls    PrepareAccelerationDataTemplate()
//                RenderMCubes()
void vtkContourVolumeMapper::Render(vtkRenderer *renderer, vtkVolume *volume)
//------------------------------------------------------------------------------
{
  if (this->GetInput() == NULL || this->GetInput()->GetPointData() == NULL || this->GetInput()->GetPointData()->GetScalars() == NULL) {
    vtkErrorMacro(<< "No data for rendering");
    return;
  }

  // if cache exists, use it
  if ((this->PrevContourValue[0] == this->ContourValue) && this->CacheCreated && this->TriangleCache[0] != NULL) {
    this->DrawCache(renderer, volume);
    return;
  }

  const void *dataPointer = this->GetInput()->GetPointData()->GetScalars()->GetVoidPointer(0);

  switch (this->GetDataType()) {
    case VTK_CHAR:
      if (PrepareAccelerationDataTemplate((const char*)dataPointer))
        this->RenderMCubes(renderer, volume, (const char*)dataPointer);
      break;
    case VTK_UNSIGNED_CHAR:
      if (PrepareAccelerationDataTemplate((const unsigned char*)dataPointer)){
        this->RenderMCubes(renderer, volume, (const unsigned char*)dataPointer);
      }
      break;
    case VTK_SHORT:
      if (PrepareAccelerationDataTemplate((const short*)dataPointer))
        this->RenderMCubes(renderer, volume, (const short*)dataPointer);
      break;
    case VTK_UNSIGNED_SHORT:
      if (PrepareAccelerationDataTemplate((const unsigned short*)dataPointer))
        this->RenderMCubes(renderer, volume, (const unsigned short*)dataPointer);
      break;
    case VTK_FLOAT:
      if (PrepareAccelerationDataTemplate((const float*)dataPointer))
        this->RenderMCubes(renderer, volume, (const float*)dataPointer);
      break;
    default:
      vtkErrorMacro(<< "Only 8/16 bit integers and 32 bit floats are supported.");
  }
}




//------------------------------------------------------------------------------
// Set up the acceleration structures.
// Gets data extents
// Calculates coords of every voxel, and offsets between neighbouring voxels
// Divides volume into 8x8x8 blocks and calculates the min and max of each block.
template <typename DataType> bool vtkContourVolumeMapper::PrepareAccelerationDataTemplate(const DataType *dataPointer)
//------------------------------------------------------------------------------
{
  // Is data the same?
  if (this->GetInput() != NULL && this->GetInput()->GetMTime() > this->BuildTime)
    this->ReleaseData();
  else if (this->BlockMinMax != NULL)
    return true; // nothing to do

  // check the data
  if (this->GetInput() && this->GetInput()->GetDataReleased())
    this->GetInput()->Update(); // ensure that the data is loaded
  if (!this->IsDataValid(true))
    return false;

  // cast the input to image or rectilinear grid (one will be valid, one should be NULL)
  vtkImageData       *imageData = vtkImageData::SafeDownCast(this->GetInput());
  vtkRectilinearGrid *gridData  = vtkRectilinearGrid::SafeDownCast(this->GetInput());

  // find data extent (dims, origin, spacing)
  // For each axis, calculates coords of the voxels, with padding of one VoxelBlockSize at end of array.
  // Each block is a 8x8x8 cube with VoxelBlockSizeLog = 3, VoxelBlockSize = 8, VoxelsInBlock = 8^3
  if (imageData) {
    imageData->GetDimensions(this->DataDimensions);
    imageData->GetOrigin(this->DataOrigin);
    imageData->GetSpacing(this->DataSpacing);
    for (int axis = 0; axis < 3; axis++) {
      // delete and reallocate array of coordinates
      delete [] this->VoxelCoordinates[axis];
      this->VoxelCoordinates[axis] = new float [this->DataDimensions[axis] + VoxelBlockSize + 1];    // NMcF: why do you need the extra +1 ?

      // set voxel coords, with padding of one block size
      float f = this->DataOrigin[axis];
      int i;
      for (i = 0; i < this->DataDimensions[axis]; i++, f += this->DataSpacing[axis])
        this->VoxelCoordinates[axis][i] = f;
      for (i = 0; i < VoxelBlockSize; i++)
        this->VoxelCoordinates[axis][this->DataDimensions[axis] + i] = f;
    }
  }
  else if (gridData){
    // same stuff, calculated differently for rectilinear grid
    gridData->GetDimensions(this->DataDimensions);
    this->DataOrigin[0] = gridData->GetXCoordinates()->GetTuple(0)[0];
    this->DataOrigin[1] = gridData->GetYCoordinates()->GetTuple(0)[0];
    this->DataOrigin[2] = gridData->GetZCoordinates()->GetTuple(0)[0];
    this->DataSpacing[0] = gridData->GetXCoordinates()->GetTuple(1)[0] - this->DataOrigin[0];
    this->DataSpacing[1] = gridData->GetYCoordinates()->GetTuple(1)[0] - this->DataOrigin[1];
    this->DataSpacing[2] = gridData->GetZCoordinates()->GetTuple(1)[0] - this->DataOrigin[2];

    for (int axis = 0; axis < 3; axis++) {
      // delete and reallocate array of coordinates
      delete [] this->VoxelCoordinates[axis];
      this->VoxelCoordinates[axis] = new float [this->DataDimensions[axis] + VoxelBlockSize + 1];

      // set voxel coords, with padding of one block size
      vtkDataArray *coordinates = (axis == 2) ? gridData->GetZCoordinates() : (axis == 1 ? gridData->GetYCoordinates() : gridData->GetXCoordinates());
      int i;
      for (i = 0; i < this->DataDimensions[axis]; i++)
        this->VoxelCoordinates[axis][i] = *(coordinates->GetTuple(i));
      for (i = 0; i < VoxelBlockSize; i++)
        this->VoxelCoordinates[axis][this->DataDimensions[axis] + i] = this->VoxelCoordinates[axis][this->DataDimensions[axis] - 1];
    }
  }
  else{
    // data not image or rect. grid
    vtkErrorMacro(<< "ContourVolumeMapper: vtk data is not image or rect. grid");
  }



  // calculate offsets between neighbouring voxels (used later in RenderMCubes())
  //               ___
  //     ^ y     /|3 2|
  //     |   x  / |0 1|
  //     /-->  / / ---
  //    /     /___ / /
  //   / z    |7 6| /
  //          |4 5|/
  //           ---
  const int sliceDimension = this->DataDimensions[0] * this->DataDimensions[1];
  this->VoxelVertIndicesOffsets[0] = 0;
  this->VoxelVertIndicesOffsets[1] = 1;
  this->VoxelVertIndicesOffsets[2] = this->DataDimensions[0] + 1;
  this->VoxelVertIndicesOffsets[3] = this->DataDimensions[0];
  this->VoxelVertIndicesOffsets[4] = this->VoxelVertIndicesOffsets[0] + sliceDimension;
  this->VoxelVertIndicesOffsets[5] = this->VoxelVertIndicesOffsets[1] + sliceDimension;
  this->VoxelVertIndicesOffsets[6] = this->VoxelVertIndicesOffsets[2] + sliceDimension;
  this->VoxelVertIndicesOffsets[7] = this->VoxelVertIndicesOffsets[3] + sliceDimension;



  // calculate no. of blocks in each direction
  // MMcF: is the +1 necessary if the dimension is an exact power of 2 ?
  for (int xyz = 0; xyz < 3; xyz++)
    this->NumBlocks[xyz] = ((this->DataDimensions[xyz] - 1) >> VoxelBlockSizeLog) + 1;

  // delete and reallocate array of block mins and maxes
  // declare a local BlockMinMax which is this->BlockMinMax cast to 2D array of correct type
  delete [] this->BlockMinMax;
  this->BlockMinMax = new DataType [2 * this->NumBlocks[0] * this->NumBlocks[1] * this->NumBlocks[2]];
  DataType (*BlockMinMax)[2] = (DataType (*)[2])this->BlockMinMax;    // create local variable which is 


  // set the mins and maxes to extreme values of the appropriate data type
  if (this->GetDataType() == VTK_UNSIGNED_CHAR || this->GetDataType() == VTK_UNSIGNED_SHORT) { // unsigned  
    for (int ii = 0, ilen = this->NumBlocks[0] * this->NumBlocks[1] * this->NumBlocks[2]; ii < ilen; ii++)
      BlockMinMax[ii][0] = (DataType)VTK_UNSIGNED_SHORT_MAX, BlockMinMax[ii][1] = 0;
  }
  else if (this->GetDataType() != VTK_FLOAT) { // signed
    for (int ii = 0, ilen = this->NumBlocks[0] * this->NumBlocks[1] * this->NumBlocks[2]; ii < ilen; ii++)
      BlockMinMax[ii][0] = (DataType)VTK_SHORT_MAX, BlockMinMax[ii][1] = (DataType)VTK_SHORT_MIN;
  }
  else { 
    float (*BlockMinMax)[2] = (float (*)[2])this->BlockMinMax;
    for (int ii = 0, ilen = this->NumBlocks[0] * this->NumBlocks[1] * this->NumBlocks[2]; ii < ilen; ii++)
      BlockMinMax[ii][0] = VTK_FLOAT_MAX, BlockMinMax[ii][1] = VTK_FLOAT_MIN;
  }



  // loop over all voxels, calculating the block index of the voxels.
  // voxels on the boundaries are counted as being in both blocks.
  for (int zi = 0, vi = 0; zi < this->DataDimensions[2]; zi++) {
    const int zblockI[2] = {(zi >> VoxelBlockSizeLog) * this->NumBlocks[0] * this->NumBlocks[1],
      zi ? ((zi - 1) >> VoxelBlockSizeLog) * this->NumBlocks[0] * this->NumBlocks[1] : 0};
    const int zblocks = (zblockI[0] == zblockI[1]) ? 1 : 2;

    for (int yi = 0; yi < this->DataDimensions[1]; yi++) {
      const int yblockI[2] = {(yi >> VoxelBlockSizeLog) * this->NumBlocks[0],
        yi ? ((yi - 1) >> VoxelBlockSizeLog) * this->NumBlocks[0] : 0};
      const int yblocks = (yblockI[0] == yblockI[1]) ? 1 : 2;

      for (int xi = 0; xi < this->DataDimensions[0]; xi++, vi++) {
        const int xblockI[2] = {xi >> VoxelBlockSizeLog, 
          xi ? ((xi - 1) >> VoxelBlockSizeLog) : 0};
        const int xblocks = (xblockI[0] == xblockI[1]) ? 1 : 2;

        const DataType val = dataPointer[vi];    // pointer to scalar values was passed as an argument to this function

        // update the min/max for each block which the voxel is in.
        for (int zz = 0; zz < zblocks; zz++) {
          for (int yy = 0; yy < yblocks; yy++) {
            for (int xx = 0; xx < xblocks; xx++) {
              DataType (&minMax)[2] = BlockMinMax[xblockI[xx] + yblockI[yy] + zblockI[zz]];    // minMax is a just a reference for BlockMinMax[i]
              if (minMax[0] > val) // min
                minMax[0] = val;
              if (minMax[1] < val) // max
                minMax[1] = val;
            }
          }
        }

      } //for (x)
    } //for (y)
  } //for (z)

  // clear caches
  this->PrevContourValue[0] = this->PrevContourValue[1] = VTK_FLOAT_MAX;
  this->CacheCreated = false;

  this->BuildTime.Modified();
  return true;
}



//------------------------------------------------------------------------------
void vtkContourVolumeMapper::EnableClipPlanes(bool enable)
//------------------------------------------------------------------------------
{
  if (enable) {
    if (this->ClippingPlanes) {
      int numClipPlanes = this->ClippingPlanes->GetNumberOfItems();
      if (numClipPlanes > 6)
        vtkErrorMacro(<< "OpenGL guarantees only 6 additional clipping planes");

      for (int i = 0; i < this->ClippingPlanes->GetNumberOfItems(); i++) {
        glEnable((GLenum)(GL_CLIP_PLANE0+i));

        vtkPlane *plane = (vtkPlane *)this->ClippingPlanes->GetItemAsObject(i);

        double planeEquation[4];
        planeEquation[0] = plane->GetNormal()[0]; 
        planeEquation[1] = plane->GetNormal()[1]; 
        planeEquation[2] = plane->GetNormal()[2];
        planeEquation[3] = -(planeEquation[0] * plane->GetOrigin()[0] + planeEquation[1] * plane->GetOrigin()[1]+
          planeEquation[2] * plane->GetOrigin()[2]);
        glClipPlane((GLenum)(GL_CLIP_PLANE0+i),planeEquation);
      }
    }
  }
  else {
    if (this->ClippingPlanes) {
      for (int i = 0; i < this->ClippingPlanes->GetNumberOfItems(); i++)
        glDisable((GLenum)(GL_CLIP_PLANE0+i));
    }
  }
}



//-------------------------------------------------------------------
void vtkContourVolumeMapper::Update()
//------------------------------------------------------------------------------
{
  if (vtkImageData::SafeDownCast(this->GetInput()) != NULL || 
    vtkRectilinearGrid::SafeDownCast(this->GetInput()) != NULL) {
      this->GetInput()->UpdateInformation();
      this->GetInput()->SetUpdateExtentToWholeExtent();
      this->GetInput()->Update();
    }
}



//------------------------------------------------------------------------------
bool vtkContourVolumeMapper::IsDataValid(bool warnings)
//------------------------------------------------------------------------------
{
  vtkDataSet         *inputData = this->GetInput();
  vtkImageData       *imageData = vtkImageData::SafeDownCast(inputData);
  vtkRectilinearGrid *gridData  = vtkRectilinearGrid::SafeDownCast(inputData);

  // check the data
  if (inputData == NULL) {
    if (warnings)
      vtkErrorMacro(<< "Contour mapper: No data to render.");
    return false;
  }
  if (imageData == NULL && gridData == NULL) {
    if (warnings)
      vtkErrorMacro(<< "Contour mapper: this data format is not supported");
    return false;
  }

  int dataType = this->GetDataType();
  if (dataType != VTK_SHORT && dataType != VTK_UNSIGNED_SHORT &&
    dataType != VTK_CHAR && dataType != VTK_UNSIGNED_CHAR && dataType != VTK_FLOAT) {
      if (warnings)
        vtkErrorMacro(<< "Only 8/16 bit integers and 32 bit floats are supported.");
      return false;
    }

    // check image data
    if (imageData) {
      int extent[6];
      imageData->GetExtent(extent);
      if (extent[4] >= extent[5]) {
        if (warnings)
          vtkErrorMacro(<< "Contour mapper: 2D datasets are not supported");
        return false;
      }

      if (imageData->GetNumberOfScalarComponents() > 1) {
        if (warnings)
          vtkErrorMacro(<< "Contour mapper: only monochrome images are currently supported.");
        return false;
      }

      return true;
    }


    if (gridData) {
      if (gridData->GetPointData() == NULL || gridData->GetPointData()->GetScalars() == NULL ||
        gridData->GetXCoordinates()->GetNumberOfTuples() < 2 ||
        gridData->GetYCoordinates()->GetNumberOfTuples() < 2 ||
        gridData->GetZCoordinates()->GetNumberOfTuples() < 2) {
          if (warnings)
            vtkErrorMacro(<< "Contour mapper: the dataset is empty.");
          return false;
        }
    }

    return true;
}




//------------------------------------------------------------------------------
// Return datatype of input scalars
int vtkContourVolumeMapper::GetDataType()
//------------------------------------------------------------------------------
{
  if (this->GetInput() && this->GetInput()->GetPointData())
    return this->GetInput()->GetPointData()->GetScalars()->GetDataType();

  return VTK_VOID;
}



//------------------------------------------------------------------------------
void vtkContourVolumeMapper::ReleaseData()
//------------------------------------------------------------------------------
{
  delete [] this->BlockMinMax;
  this->BlockMinMax = NULL;
  delete [] this->TriangleCache[0];
  delete [] this->TriangleCache[1];
  this->TriangleCache[0] = this->TriangleCache[1] = NULL;
  delete [] this->VoxelCoordinates[0];
  delete [] this->VoxelCoordinates[1];
  delete [] this->VoxelCoordinates[2];
  this->VoxelCoordinates[0] = this->VoxelCoordinates[1] = this->VoxelCoordinates[2] = NULL;
}



//------------------------------------------------------------------------------
// Return isosurface as polydata
// Similar to Render() but calls CreateMCubes() instead of RenderMCubes()
vtkPolyData *vtkContourVolumeMapper::GetOutput(int level, vtkPolyData *data)
//------------------------------------------------------------------------------
{
  if (level != 0 && level != 1)
    return NULL;
  if (this->GetInput() == NULL || this->GetInput()->GetPointData() == NULL || this->GetInput()->GetPointData()->GetScalars() == NULL) {
    vtkErrorMacro(<< "No data");
    return NULL;
  }

  vtkPolyData *polydata = (data == NULL) ? vtkPolyData::New() : data;

  const void *dataPointer = this->GetInput()->GetPointData()->GetScalars()->GetVoidPointer(0);

  switch (this->GetDataType()) {
    case VTK_CHAR:
      if (!PrepareAccelerationDataTemplate((const char*)dataPointer))
        return NULL;
      this->CreateMCubes(level, polydata, (const char*)dataPointer);
      break;
    case VTK_UNSIGNED_CHAR:
      if (!PrepareAccelerationDataTemplate((const unsigned char*)dataPointer))
        return NULL;
      this->CreateMCubes(level, polydata, (const unsigned char*)dataPointer);
      break;
    case VTK_SHORT:
      if (!PrepareAccelerationDataTemplate((const short*)dataPointer))
        return NULL;
      this->CreateMCubes(level, polydata, (const short*)dataPointer);
      break;
    case VTK_UNSIGNED_SHORT:
      if (!PrepareAccelerationDataTemplate((const unsigned short*)dataPointer))
        return NULL;
      this->CreateMCubes(level, polydata, (const unsigned short*)dataPointer);
      break;
    case VTK_FLOAT:
      if (!PrepareAccelerationDataTemplate((const float*)dataPointer))
        return NULL;
      this->CreateMCubes(level, polydata, (const float*)dataPointer);
      break;
    default:
      vtkErrorMacro(<< "Only 8/16 bit integers and 32 bit floats are supported.");
  }

  // create polydata
  return polydata;
}






//------------------------------------------------------------------------------
// Initialize OpenGL rendering, called by DrawCache() and RenderMCubes()
void vtkContourVolumeMapper::InitializeRender(bool setup, vtkRenderer *renderer, vtkVolume *volume)
//------------------------------------------------------------------------------
{
  glGetError(); // reset error flag
  if (setup) {
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    EnableClipPlanes(true);

    //Matteo
    glEnable(GL_BLEND);
    //End

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_TEXTURE_2D);

    glShadeModel(GL_SMOOTH);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);

    // disable any antialiasing
    bool multisamplingExt = (strstr((const char *)glGetString(GL_EXTENSIONS), "GL_EXT_multisample") != NULL ||
      strstr((const char *)glGetString(GL_EXTENSIONS), "GL_SGIS_multisample") != NULL);
    if (multisamplingExt)
      glDisable(0x809D);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
    glDisable(GL_POLYGON_SMOOTH);

    glPixelTransferf(GL_RED_SCALE, 1.f);
    glPixelTransferf(GL_GREEN_SCALE, 1.f);
    glPixelTransferf(GL_BLUE_SCALE, 1.f);

    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // ----------- initialize state variables 

    // viewport
    float viewport[4];
    glGetFloatv(GL_VIEWPORT, viewport);
    this->ViewportDimensions[0] = viewport[2];
    this->ViewportDimensions[1] = viewport[3];

    // transformation
    this->TransformMatrix->DeepCopy(renderer->GetActiveCamera()->GetCompositePerspectiveTransformMatrix((double)viewport[2] / viewport[3], 0, 1));
    volume->GetMatrix(this->VolumeMatrix);
    vtkMatrix4x4::Multiply4x4(this->TransformMatrix, this->VolumeMatrix, this->TransformMatrix);
    this->VolumeMatrix->Transpose();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glMultMatrixd(this->VolumeMatrix->Element[0]);
    volume->GetMatrix(this->VolumeMatrix); // get the matrix again

    // set up materials
    glDisable(GL_COLOR_MATERIAL);
    glColor3f(1.f, 1.f, 1.f);
    float ambientColor  = volume->GetProperty()->GetAmbient();
    float diffuseColor  = volume->GetProperty()->GetDiffuse();
    float specularColor = volume->GetProperty()->GetSpecular();
    float ambientColor3[4] = { ambientColor, ambientColor, ambientColor, m_Alpha};
    float diffuseColor3[4] = { diffuseColor, diffuseColor, diffuseColor, m_Alpha};
    float specularColor3[4] = { specularColor, specularColor, specularColor, m_Alpha};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,  ambientColor3);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,  diffuseColor3);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularColor3);
  }
  else { // restore the settings
    glPopMatrix();

    glPopAttrib();

    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
  }
  assert(glGetError() == 0);
}




//------------------------------------------------------------------------------
// OpenGL code which initializes OpenGL and renders the cached triangles.
// Draws either normal or LOD cache
void vtkContourVolumeMapper::DrawCache(vtkRenderer *renderer, vtkVolume *volume)
//------------------------------------------------------------------------------
{
  // use cache if contour value not changed and cache exists
  const bool useCache = (this->PrevContourValue[0] == this->ContourValue) && this->CacheCreated;
  assert(useCache);

  // set flag to use LOD cache if LOD enabled and LOD cache is valid
  const bool enableOptimization = 
    this->EnableAutoLOD && 
    this->TimeToDrawNotOptimizedCache > renderer->GetAllocatedRenderTime() && 
    this->NumberOfTriangles[1] > 1000 && 
    this->TriangleCache[1] != NULL;

  this->Timer->StartTimer();

  int lod = enableOptimization ? 1 : 0 ;
  int nvert = 3*this->NumberOfTriangles[lod] ;
  InitializeRender(true, renderer, volume);

  /* if (this->m_Alpha < 1.0){
  // transparency enabled so sort triangles and render
  SortTriangles(lod) ;

  if (this->NumberOfTriangles[lod] > 0){
  glInterleavedArrays(GL_N3F_V3F, 0, this->TriangleCache[lod]);
  if (this->NumberOfTriangles[lod] > 0)
  glDrawElements(GL_TRIANGLES, nvert, GL_UNSIGNED_INT, ordered_vertices[lod]);
  }
  } */


  // draw the triangles
  if (this->NumberOfTriangles[lod] > 0){
    glInterleavedArrays(GL_N3F_V3F, 0, this->TriangleCache[lod]);
    if (this->NumberOfTriangles[lod] > 0)
      glDrawArrays(GL_TRIANGLES, 0, nvert);
  }


  InitializeRender(false);

  this->Timer->StopTimer();

  // if not LOD, note time taken to draw non-LOD triangles
  this->TimeToDraw = (float)this->Timer->GetElapsedTime();
  if (!enableOptimization)
    this->TimeToDrawNotOptimizedCache = this->TimeToDraw;

  //  float time_per_triangle = this->TimeToDraw / (float)this->NumberOfTriangles[lod] ;
  //  mafErrorMacro("draw cache: lod= " << lod << "\n" 
  //    << "\t" << " time limit= " << 60.0*renderer->GetAllocatedRenderTime() 
  //    << " time= " << 60.0*this->TimeToDraw << "\n" 
  //    << "\t" << "no. of triangles=" << this->NumberOfTriangles[lod] 
  //    << "time per triangle= " << time_per_triangle << "\n") ;

#ifdef _DEBUG
  printf("\rCache %d: %.2f %d                                   \r", int(enableOptimization), (float)this->Timer->GetElapsedTime(), this->NumberOfTriangles[enableOptimization ? 1 : 0]);
#endif
}








//------------------------------------------------------------------------------
// Marching cubes algorithm
// Constructs triangles and renders them.

// Nigel McFarlane 10.11.06 Changes to RenderMCubes()
// 1) Optimization given priority over caching, ie optim => no caching, instead of caching => no optim.
// 2) Cache is deleted if smaller than block buffer, removing danger of program crash.
// 3) LOD is allowed in z direction if resolution ok.
// 4) Gradient calculations tidied and dependence on LOD removed.
// 5) RenderMCubes estimates no. of triangles in advance, and uses LOD when no. of
//    triangles is too large or unknown (eg for new contour value).

template<typename DataType> void vtkContourVolumeMapper::RenderMCubes(vtkRenderer *renderer, vtkVolume *volume, const DataType *dataPointer)
//------------------------------------------------------------------------------
{
  const int MaxTrianglesNotOptimized = 2000000 ;  // max no. of triangles before LOD required
  int numTrianglesRunningTotal[2] = {0,0} ;       // keeps running total of triangles
  bool numTrianglesWarningFlag = false ;          // set if no. of triangles reaches warning level

  // Decide whether optimization (ie low-resolution LOD) is required
  // Optimization required if LOD enabled AND (time to draw too long OR too many triangles)
  int estTriangles = EstimateTrianglesNoOpt() ; // estimate no. of triangles in non-lod mode
  bool enableOptimization = this->EnableAutoLOD &&
    ((this->TimeToDrawNotOptimized + 0.1f > renderer->GetAllocatedRenderTime()) || 
    (estTriangles > MaxTrianglesNotOptimized)) ;

  // Decide whether caching can be enabled
  // Cache enabled if contour value same as previous, no optimization, no cache already and not too many triangles
  // Cache variable indices are 0 for normal cache and 1 for LOD cache
  bool createCache = (this->PrevContourValue[0] == this->ContourValue) && !enableOptimization && !this->CacheCreated && (this->NumberOfTriangles[0] <= MaxTrianglesNotOptimized) ;

  if (createCache) {
    // if contour value changed since LOD last cached, predict max possible no. of triangles for LOD cache
    // very unlikely that no. of triangles under LOD would be more than normal resolution
    if (this->PrevContourValue[1] != this->ContourValue){
      this->NumberOfTriangles[1] = this->NumberOfTriangles[0];
    }

    // allocate normal and LOD caches
    for (int ci = 0; ci < 2; ci++) {
      if (this->TriangleCacheSize[ci] < this->NumberOfTriangles[ci] || this->TriangleCache[ci] == NULL) {
        // if cache not big enough, or not defined, reallocate cache
        // 3 vertices per triangle, 3 normals + 3 coords per vertex = 18
        delete [] this->TriangleCache[ci];
        this->TriangleCache[ci] = new float [18 * this->NumberOfTriangles[ci]];

        // if allocation failed, set createCache back to false
        if (this->TriangleCache[ci] == NULL) {
          createCache = false;
          this->TriangleCacheSize[ci] = 0;
          vtkErrorMacro(<< "Contour mapper: not enough memory");
          break;
        }

        // cache size in no. of triangles
        this->TriangleCacheSize[ci] = this->NumberOfTriangles[ci];
      }
    }
  }
  else {
    // no caching, so clear both caches, unless cache is already right size to be reused as a block buffer
    if ((this->TriangleCacheSize[0] < 5*VoxelsInBlock) || (this->TriangleCacheSize[0] > 20*VoxelsInBlock)) {
      for (int ci = 0; ci < 2; ci++) {
        delete [] this->TriangleCache[ci];
        this->TriangleCache[ci] = NULL;
        this->TriangleCacheSize[ci] = 0;
      }
    }
  }

  this->CacheCreated = createCache;
  this->TimeToDrawNotOptimizedCache = 0;



  // start the timer and initialize OpenGL
  this->Timer->StartTimer();
  InitializeRender(true, renderer, volume);

  // if cache 0 is not defined, allocate it as a buffer for block triangles
  // (only 0 is used if no caching because each block is rendered immediately, regardless of LOD)
  if (this->TriangleCache[0] == NULL) {
    this->TriangleCacheSize[0] = 5 * VoxelsInBlock ; // 5 triangles per voxel
    this->TriangleCache[0] = new float [18 * this->TriangleCacheSize[0]];
    this->CacheCreated = false;
    createCache = false;
  }

  // OpenGL: attach triangle cache 0 to GL array with 3 normals + 3 vertices per item
  // Only need TriangleCache[0] because if caching, only render normal level; if not caching, render from block buffer.
  glInterleavedArrays(GL_N3F_V3F, 0, this->TriangleCache[0]);

  // process blocks and voxels
  this->SkippedVoxelBlocks = 0;
  this->VoxelsRendered = this->VoxelsSkipped = 0;

  // define local variables to reduce overheads and improve readability
  const DataType (*BlockMinMax)[2] = (DataType (*)[2])this->BlockMinMax;
  DataType ContourValue = (DataType)this->ContourValue;
  float *verticeArray[2] = {this->TriangleCache[0] + 3, this->TriangleCache[1] + 3};
  float *normalArray[2]  = {this->TriangleCache[0], this->TriangleCache[1]};


  // indices of LOD caches to use (can be (0,0), (0,1) or (1,1))
  // if optimization enabled, only use lod=1
  // if no optimization and no caching, only use lod=0
  // if no optimization, but caching enabled, got time to cache lod=0 and lod=1 (but obviously don't render both !)
  const int firstLOD = enableOptimization ? 1 : 0;
  const int lastLOD  = (enableOptimization || createCache) ? 1 : 0;


  // these offsets define a LOD cube of neighbours twice the size of the cube in PrepareAccelerationDataTemplate()
  const int VoxelVertIndicesOffsetsLOD[8] = {0, 2, 
    this->VoxelVertIndicesOffsets[2] << 1, this->VoxelVertIndicesOffsets[3] << 1,
    this->VoxelVertIndicesOffsets[4], this->VoxelVertIndicesOffsets[4] + 2, 
    this->VoxelVertIndicesOffsets[4] + (this->VoxelVertIndicesOffsets[2] << 1), 
    this->VoxelVertIndicesOffsets[4] + (this->VoxelVertIndicesOffsets[3] << 1)};

  // initialization
  this->PrevContourValue[firstLOD] = this->ContourValue;
  this->PrevContourValue[lastLOD]  = this->ContourValue;
  this->NumberOfTriangles[firstLOD] = 0;
  this->NumberOfTriangles[lastLOD] = 0;

  // no. of voxels per slice and per row
  const int rowSize   = this->DataDimensions[0];
  const int sliceSize = this->DataDimensions[0] * this->DataDimensions[1];

  // create array of ListOfPolyLine2D pointers and set them all to zero
  ListOfPolyline2D *polylines[2000];
  memset(polylines, 0, sizeof(polylines));

  if (EnableContourAnalysis) {
    for (int z = 0; z < this->DataDimensions[2]; z++) {
      polylines[z] = new ListOfPolyline2D();
      this->PrepareContours(z, dataPointer + z * sliceSize, *polylines[z]);
    }
  }


  //----------------------------
  // loop through all the blocks
  //----------------------------
  for (int bzi = 0; bzi < this->NumBlocks[2]; bzi++) {
    // calculate block size in z (all blocks are VoxelBlockSize except last, which is remainder - 1)
    const int zblockSize = (((bzi + 1) < this->NumBlocks[2]) ? VoxelBlockSize : (this->DataDimensions[2] - 1 - (bzi << VoxelBlockSizeLog)));

    for (int byi = 0; byi < this->NumBlocks[1]; byi++) {
      // calculate block size in y
      const int yblockSize = (((byi + 1) < this->NumBlocks[1]) ? VoxelBlockSize : (this->DataDimensions[1] - 1 - (byi << VoxelBlockSizeLog)));

      for (int bxi = 0; bxi < this->NumBlocks[0]; bxi++) {
        // calculate block size in x
        const int xblockSize = (((bxi + 1) < this->NumBlocks[0]) ? VoxelBlockSize : (this->DataDimensions[0] - 1 - (bxi << VoxelBlockSizeLog)));

        // calculate block no.
        const int block = bxi + this->NumBlocks[0] * (byi + bzi * this->NumBlocks[1]);

        // skip the block if contour value is outside range min-max
        if ((ContourValue < BlockMinMax[block][0]) || (ContourValue > BlockMinMax[block][1])) {
          this->SkippedVoxelBlocks += 1.f;
          continue;
        }


        //--------------------------------
        // Process the voxels in the block
        //--------------------------------

        // set up pointLocator array float* pointLocator[9][9][9][3], ie 3 float pointers for every voxel in block.
        // pointLocator is an array of pointers to every edge in the block, 
        // so it will become an index of the triangle vertices.
        float* pointLocator[VoxelBlockSize + 1][VoxelBlockSize + 1][VoxelBlockSize + 1][3]; // three edges per voxel
        memset(pointLocator, 0, sizeof(pointLocator));

        // index of first voxel in block
        const int voxelIBlock = VoxelBlockSize * (bzi * sliceSize + byi * rowSize + bxi);


        //---------------------------
        // loop over levels of detail
        //---------------------------
        for (int lod = firstLOD; lod <= lastLOD; lod++) {
          // Voxel increment is 1 for normal, 2 for LOD, which happens to be lod+1
          // However, z resolution betwen slices may already be poor,
          // so set z to 1 if inter-slice spacing is greater than within-slice.
          int lodxy = lod + 1;
          int lodz = lod + 1 ;
          if (this->DataSpacing[2] > this->DataSpacing[0])
            lodz = 1 ;

          // select offsets corresponding to LOD (0 is normal, 1 is LOD)
          const int *VoxelOffsets = ((lod == 0) ? this->VoxelVertIndicesOffsets : VoxelVertIndicesOffsetsLOD);

          // last xyz in volume
          const int lastIndex[3] = { this->DataDimensions[0] - lodxy, this->DataDimensions[1] - lodxy, this->DataDimensions[2] - lodz};


          // set arrays for vertices and normals
          int numberOfTrianglesInBlock = 0;
          if (!createCache) {
            verticeArray[lod] = this->TriangleCache[0] + 3;
            normalArray[lod]  = this->TriangleCache[0];
          }
          else if (this->TriangleCache[lod] == NULL) {
            continue; // no memory do not create this cache
          }

          float* (&vertices) = verticeArray[lod];
          float* (&normals)  = normalArray[lod];


          //---------------------------------------------
          // loop over voxels in block
          //---------------------------------------------
          for (int zi = 0; zi < zblockSize; zi += lodz) {
            const int   z  = (bzi << VoxelBlockSizeLog) + zi;                       // voxel z index relative to volume
            const float fz = this->VoxelCoordinates[2][z];                          // voxel z coord
            const float fzSize = this->VoxelCoordinates[2][z + lodz] - fz;          // size of LOD voxel                    
            const float fzSizeNorm = 1.f / (this->VoxelCoordinates[2][z + 1] - fz); // normalization factor : size of normal voxel                        
            if ((z + lodz) >= this->DataDimensions[2])                              // skip if volume dimension exceeded
              continue;
            if (fzSize < 0.0001f) // arbitrary minimum size
              continue;

            for (int yi = 0; yi < yblockSize; yi += lodxy) {
              const int   y  = (byi << VoxelBlockSizeLog) + yi;
              const float fy = this->VoxelCoordinates[1][y];
              const float fySize     = this->VoxelCoordinates[1][y + lodxy] - fy;
              const float fySizeNorm = 1.f / (this->VoxelCoordinates[1][y + 1] - fy);
              if ((y + lodxy) >= this->DataDimensions[1])
                continue;

              for (int xi = 0; xi < xblockSize; xi += lodxy) {
                const int x = (bxi << VoxelBlockSizeLog) + xi;
                const float fx = this->VoxelCoordinates[0][x];
                const float fxSize = this->VoxelCoordinates[0][x + lodxy] - fx;
                const float fxSizeNorm = 1.f / (this->VoxelCoordinates[0][x + 1] - fx);
                if ((x + lodxy) >= this->DataDimensions[0])
                  continue;

                const int voxelI = voxelIBlock + zi * sliceSize + yi * rowSize + xi;    // index of current voxel
                const DataType * const voxelPtr = dataPointer + voxelI;                 // pointer to current voxel
                const float fxyzSize[3] = { fxSize, fySize, fzSize };                   // size (LOD size) of current voxel


                // Find the marching cubes case

                // get value of voxel and neighbours (VoxelOffsets was set earlier to normal or LOD sized cube)
                const DataType voxelVals[8] = {voxelPtr[0], voxelPtr[VoxelOffsets[1]], voxelPtr[VoxelOffsets[2]], voxelPtr[VoxelOffsets[3]],
                  voxelPtr[VoxelOffsets[4]], voxelPtr[VoxelOffsets[5]], voxelPtr[VoxelOffsets[6]], voxelPtr[VoxelOffsets[7]]};

                // calculate case, each bit of caseIndex is set if corner > contour value
                int caseIndex = voxelVals[0] > ContourValue; 
                caseIndex |= (voxelVals[1] > ContourValue) << 1;
                caseIndex |= (voxelVals[2] > ContourValue) << 2;
                caseIndex |= (voxelVals[3] > ContourValue) << 3;
                caseIndex |= (voxelVals[4] > ContourValue) << 4;
                caseIndex |= (voxelVals[5] > ContourValue) << 5;
                caseIndex |= (voxelVals[6] > ContourValue) << 6;
                caseIndex |= (voxelVals[7] > ContourValue) << 7;

                // get list of edges
                const EDGE_LIST *edge = marchingCubesCases[caseIndex].edges;
                if (*edge < 0)
                  continue;

                if (EnableContourAnalysis) {
                  // remove internal parts
                  if (polylines[z]->IsInside(x, y, 50) && polylines[z + 1]->IsInside(x, y, 50))
                    continue;

                  // remove noise
                  if (polylines[z]->FindContour(x, y, 50) == NULL &&
                    polylines[z]->FindContour(x, y, 75, 4) == NULL && 
                    !polylines[z + 1]->IsInside(x, y, 60) && (z == 0 || !polylines[z - 1]->IsInside(x, y, 60)))
                    continue;

                }

                // loop through all edges in list
                while(*edge >= 0) {
                  // check that memory is not about to be exceeded
                  if (createCache && (numTrianglesRunningTotal[lod] >= this->TriangleCacheSize[lod])){
                    mafErrorMacro("no. of triangles exceeded allocated cache size\n") ;
                    break ;
                  }
                  //if ((!numTrianglesWarningFlag) && numTrianglesRunningTotal[lod] > MaxTrianglesNotOptimized){
                  // mafErrorMacro("warning: no. of triangles exceeded lod limit") ;
                  //  numTrianglesWarningFlag = true ;
                  //}

                  // loop through next three edges
                  for (int ii = 0; ii < 3; ii++, edge++) {
                    // get corners of cube at ends of this edge
                    // vert0 and vert1 are cube indices of corners (see static consts in .h)
                    // vertIndeces points to offsets
                    // eg if *edge = 5, vert0 = 5, vert1 = 6, vertIndeces ={{0,0,1}, {1,0,1}}
                    const int vert0 = edgeOffsets[*edge][0];
                    const int vert1 = edgeOffsets[*edge][1];
                    const int *vertIndeces[2] = {voxelVertIndicesXYZ[vert0], voxelVertIndicesXYZ[vert1] };

                    // was this edge created?
                    // define a reference to pointLocator for the current edge
                    // reminder: float* pointLocator[9][9][9][3], allocated, but all pointers zero
                    float *(&pointLocatorRef) = pointLocator[xi + vertIndeces[0][0]][yi + vertIndeces[0][1]][zi + vertIndeces[0][2]][edgeAxis[*edge]];

                    if (pointLocatorRef) {
                      // if edge already created, just read normals and vertices
                      normals[0] = pointLocatorRef[0];
                      normals[1] = pointLocatorRef[1];
                      normals[2] = pointLocatorRef[2];
                      vertices[0] = pointLocatorRef[3];
                      vertices[1] = pointLocatorRef[4];
                      vertices[2] = pointLocatorRef[5];
                    }
                    else {
                      // edge not created, so calculate normals and vertices

                      // calculate coords of vertex, ie intersection of contour with this edge
                      vertices[0] = fx + vertIndeces[0][0] * fxSize;
                      vertices[1] = fy + vertIndeces[0][1] * fySize;
                      vertices[2] = fz + vertIndeces[0][2] * fzSize;
                      float edgeRatio = (float)(ContourValue - voxelVals[vert0]) / (voxelVals[vert1] - voxelVals[vert0]);
                      vertices[edgeAxis[*edge]] += edgeRatio * fxyzSize[edgeAxis[*edge]];

                      // estimate gradients at vert0 and vert1
                      // use central difference except at edges
                      // reminder: x, y and z are indices of voxel in volume
                      int gradient[2][3];
                      for (int vi = 0; vi < 2; vi++) {
                        const DataType * const verticePtr = voxelPtr + VoxelOffsets[vi == 0 ? vert0 : vert1];    // pointer to vert0 or vert1

                        // gradient in x
                        const int vx = x + vertIndeces[vi][0];    // x index of vertex
                        if (vx == 0)
                          gradient[vi][0] = (int)(verticePtr[1] - verticePtr[0]) ;
                        else if (vx == lastIndex[0])
                          gradient[vi][0] = (int)(verticePtr[0] - verticePtr[-1]) ;
                        else
                          gradient[vi][0] = ((int)(verticePtr[1] - verticePtr[-1])) >> 1 ;

                        // gradient in y
                        const int vy = y + vertIndeces[vi][1];    // y index of vertex
                        if (vy == 0)
                          gradient[vi][1] = (int)(verticePtr[rowSize] - verticePtr[0]) ;
                        else if (vy == lastIndex[1])
                          gradient[vi][1] = (int)(verticePtr[0] - verticePtr[-rowSize]) ;
                        else
                          gradient[vi][1] = ((int)(verticePtr[rowSize] - verticePtr[-rowSize])) >> 1 ;

                        // gradient in z
                        const int vz = z + vertIndeces[vi][2];    // z index of vertex
                        if (vz == 0)
                          gradient[vi][2] = (int)(verticePtr[sliceSize] - verticePtr[0]) ;
                        else if (vz == lastIndex[2])
                          gradient[vi][2] = (int)(verticePtr[0] - verticePtr[-sliceSize]) ;
                        else
                          gradient[vi][2] = ((int)(verticePtr[sliceSize] - verticePtr[-sliceSize])) >> 1 ;

                      }

                      // calculate normals by linear interpolation
                      normals[0] = (gradient[0][0] + edgeRatio * (gradient[1][0] - gradient[0][0])) * fxSizeNorm;
                      normals[1] = (gradient[0][1] + edgeRatio * (gradient[1][1] - gradient[0][1])) * fySizeNorm;
                      normals[2] = (gradient[0][2] + edgeRatio * (gradient[1][2] - gradient[0][2])) * fzSizeNorm;

                      const float normalLenI = -1.f / sqrt(0.0001f + normals[0] * normals[0] + normals[1] * normals[1] + normals[2] * normals[2]);
                      normals[0] *= normalLenI;
                      normals[1] *= normalLenI;
                      normals[2] *= normalLenI;

                      // finally set the pointLocator
                      // (remember that normals and vertices are pointers to the same interleaved array)
                      pointLocatorRef = normals; // normals precede vertices
                    }

                    vertices += 6;    
                    normals += 6;

                  } // for ii (next edge in triangle)

                  numberOfTrianglesInBlock++;

                  // note running total of triangles
                  numTrianglesRunningTotal[lod]++ ;


                } // while(*edge) (next triangle in edge list)

                this->VoxelsRendered++;

              } // next xi
            } // next yi
          } // next zi

          // if no caching, render the block now, else wait till whole volume is cached
          // n.b. loop does both lods only if caching enabled, so can't draw both lods
          if (!createCache)
            glDrawArrays(GL_TRIANGLES, 0, 3 * numberOfTrianglesInBlock);
          this->NumberOfTriangles[lod] += numberOfTrianglesInBlock;

        } // next LOD

      } // nextbxi
    } // next byi
  } // next bzi


  // render the cached triangles (normal level of detail)
  // the arrays were attached to openGL earlier with glInterleavedArrays()
  if (createCache)
    glDrawArrays(GL_TRIANGLES, 0, 3 * this->NumberOfTriangles[0]);

  assert(glGetError() == GL_NO_ERROR);

  for (int z = 0; z < this->DataDimensions[2]; z++)
    delete polylines[z];

  // this measures the percentage of empty blocks
  this->SkippedVoxelBlocks = 100.f * this->SkippedVoxelBlocks / (this->NumBlocks[2] * this->NumBlocks[1] * this->NumBlocks[0]);

  InitializeRender(false);

  this->Timer->StopTimer();
  this->TimeToDraw = (float)this->Timer->GetElapsedTime();
  if (!enableOptimization)
    this->TimeToDrawNotOptimized = this->TimeToDraw;

  if (this->TimeToDraw < 0.0001f)
    this->TimeToDraw = 0.0001f;


  //  if (firstLOD == lastLOD){
  //    float time_per_triangle0 = this->TimeToDraw / (float)this->NumberOfTriangles[firstLOD] ;
  //    mafErrorMacro("RenderMCubes:" << "\n" 
  //      << "\t" << "optim= " << enableOptimization << " caching= " << createCache << " lod= " << firstLOD << "\n" 
  //     << "\t" << " time limit= " << 60.0*renderer->GetAllocatedRenderTime() 
  //      << " time= " << 60.0*this->TimeToDraw << "\n" 
  //      << "\t" << "no. of triangles=" << this->NumberOfTriangles[firstLOD] 
  //      << "time per triangle= " << time_per_triangle0 << "\n") ;
  //  }
  //  else{
  //    float time_per_triangle0 = this->TimeToDraw / (float)this->NumberOfTriangles[firstLOD] ;
  //   float time_per_triangle1 = this->TimeToDraw / (float)this->NumberOfTriangles[lastLOD] ; 
  //    mafErrorMacro("RenderMCubes:" << "\n" 
  //     << "\t" << "optim= " << enableOptimization << " caching= " << createCache << " lods= " << firstLOD << " " << lastLOD << "\n" 
  //      << "\t" << " time limit= " << 60.0*renderer->GetAllocatedRenderTime() 
  //      << " time= " << 60.0*this->TimeToDraw << "\n" 
  //     << "\t" << "no. of triangles=" << this->NumberOfTriangles[firstLOD] << " " << this->NumberOfTriangles[lastLOD] 
  //     << "times per triangle= " << time_per_triangle0 << " " << time_per_triangle1 << "\n") ;
  //  }



#if 0
  printf("\rTime: %.2f (%.0f%% blocks + %.0f%% voxels). %d triangles.   \r", this->TimeToDraw, float(this->SkippedVoxelBlocks), 100.f * this->VoxelsSkipped / (this->VoxelsSkipped + this->VoxelsRendered), this->NumberOfTriangles[enableOptimization ? 1 : 0]);
#endif

}





//------------------------------------------------------------------------------
// Marching cubes with result as vtkPolyData
// Similar to RenderMCubes(), but doesn't render or cache
// Called by GetOutput()
template<typename DataType> void vtkContourVolumeMapper::CreateMCubes(int level, vtkPolyData *polydata, const DataType *dataPointer) {
  //------------------------------------------------------------------------------
  const int estimatedNumberOfTriangles = (this->PrevContourValue[level] == this->ContourValue) ? (this->NumberOfTriangles[level] + 100) : 1000000;
  this->CreatedTriangles = 0;

  // prepare polydata
  vtkPoints *points = vtkPoints::New();
  polydata->SetPoints(points);
  points->UnRegister(NULL);
  points->Allocate(estimatedNumberOfTriangles, estimatedNumberOfTriangles / 2);

  vtkCellArray *triangles = vtkCellArray::New();
  polydata->SetPolys(triangles);
  triangles->UnRegister(NULL);
  triangles->Allocate(4 * estimatedNumberOfTriangles, estimatedNumberOfTriangles);

  vtkFloatArray *normals = vtkFloatArray::New();
  normals->SetNumberOfComponents(3);
  //  polydata->GetPointData()->SetNormals(normals);
  //  normals->UnRegister(NULL);
  normals->Allocate(6 * estimatedNumberOfTriangles, 3 * estimatedNumberOfTriangles);

  const DataType (* const BlockMinMax)[2] = (DataType (*)[2])this->BlockMinMax;
  const DataType ContourValue = (DataType)this->ContourValue;

  const int VoxelOffsets[8] = {0, 1 << level, this->VoxelVertIndicesOffsets[2] << level, this->VoxelVertIndicesOffsets[3] << level,
    this->VoxelVertIndicesOffsets[4], this->VoxelVertIndicesOffsets[4] + (1 << level), this->VoxelVertIndicesOffsets[4] + (this->VoxelVertIndicesOffsets[2] << level), this->VoxelVertIndicesOffsets[4] + (this->VoxelVertIndicesOffsets[3] << level)};

  const int rowSize   = this->DataDimensions[0];
  const int sliceSize = this->DataDimensions[0] * this->DataDimensions[1];

  // point locator
  const int plDims[2]    = {this->DataDimensions[0] + 1, this->DataDimensions[1] + 1};
  const int plOffsets[3] = {3, 3 * plDims[0], 3 * plDims[0] * plDims[1]};
  vtkIdType * const pointLocator = new vtkIdType[2 * plOffsets[2]];
  //Modified by Matteo 27/06/06
  memset(pointLocator, -1, 2 * sizeof(vtkIdType) * plOffsets[2]);

  ListOfPolyline2D polylines[2];

  const int lastIndex[3] = { this->DataDimensions[0] - level - 1, this->DataDimensions[1] - level - 1, this->DataDimensions[2] - 1};

  for (int z = 0; z < (this->DataDimensions[2] - 1); z++) {
    const int bzi = z >> VoxelBlockSizeLog;
    const float fz = this->VoxelCoordinates[2][z];
    const float fzSize = this->VoxelCoordinates[2][z + 1] - fz;
    const float fzSizeNorm = 1.f / fzSize;

    if (fzSize < 0.0001f)
      continue;

    this->CreatedTriangles = triangles->GetNumberOfCells();
    this->UpdateProgress(float(z) / (this->DataDimensions[2] - 1));

    // prepare contours
    if (EnableContourAnalysis) {
      if (z == 0)
        this->PrepareContours(z, dataPointer + z * sliceSize, polylines[0]);
      this->PrepareContours(z + 1, dataPointer + (z + 1) * sliceSize, polylines[(z + 1) % 2]);
    }

    // reset locator: copy the last slice to the top and reset all other slices
    if (z != 0) {
      memcpy(pointLocator, pointLocator + plOffsets[2], sizeof(vtkIdType) * plOffsets[2]);
      //Modified by Matteo 27/06/06
      memset(pointLocator + plOffsets[2], -1, sizeof(vtkIdType) * plOffsets[2]);
    }

    for (int byi = 0; byi < this->NumBlocks[1]; byi++) {
      const int yblockSize = (((byi + 1) < this->NumBlocks[1]) ? VoxelBlockSize : (this->DataDimensions[1] - 1 - (byi << VoxelBlockSizeLog)));

      for (int bxi = 0; bxi < this->NumBlocks[0]; bxi++) {
        const int block = bxi + this->NumBlocks[0] * (byi + bzi * this->NumBlocks[1]);
        const int xblockSize = (((bxi + 1) < this->NumBlocks[0]) ? VoxelBlockSize : (this->DataDimensions[0] - 1 - (bxi << VoxelBlockSizeLog)));
        if (ContourValue > BlockMinMax[block][1] || ContourValue < BlockMinMax[block][0])
          continue; // skip the block

        //---------------------------- process the voxels in the block
        const int voxelIBlock = z * sliceSize + VoxelBlockSize * (byi * rowSize + bxi);

        for (int yi = 0; yi < yblockSize; yi += (level + 1)) {
          const int   y  = (byi << VoxelBlockSizeLog) + yi;
          const float fy = this->VoxelCoordinates[1][y];
          const float fySize = this->VoxelCoordinates[1][y + level + 1] - fy;
          const float fySizeNorm = 1.f / (this->VoxelCoordinates[1][y + 1] - fy);

          for (int xi = 0; xi < xblockSize; xi += (level + 1)) {
            const int voxelI = voxelIBlock + yi * rowSize + xi;
            const DataType * const voxelPtr = dataPointer + voxelI;

            const int x = (bxi << VoxelBlockSizeLog) + xi;
            const float fx = this->VoxelCoordinates[0][x];
            const float fxSize = this->VoxelCoordinates[0][x + level + 1] - fx;
            const float fxSizeNorm = 1.f / (this->VoxelCoordinates[0][x + 1] - fx);
            const float fxyzSize[3] = { fxSize, fySize, fzSize };

            if (x >= lastIndex[0] || y >= lastIndex[1])
              continue;

            // find the case
            const DataType voxelVals[8] = {voxelPtr[0], voxelPtr[VoxelOffsets[1]], voxelPtr[VoxelOffsets[2]], voxelPtr[VoxelOffsets[3]],
              voxelPtr[VoxelOffsets[4]], voxelPtr[VoxelOffsets[5]], voxelPtr[VoxelOffsets[6]], voxelPtr[VoxelOffsets[7]]};
            //Check if the scalar value is greter or equal then the threshold
            int caseIndex;
            //Modified by Matteo 27/06/06
            if(m_MAXScalar==ContourValue)
            {
              caseIndex = voxelVals[0] != ContourValue; 
              caseIndex |= (voxelVals[1] != ContourValue) << 1;
              caseIndex |= (voxelVals[2] != ContourValue) << 2;
              caseIndex |= (voxelVals[3] != ContourValue) << 3;
              caseIndex |= (voxelVals[4] != ContourValue) << 4;
              caseIndex |= (voxelVals[5] != ContourValue) << 5;
              caseIndex |= (voxelVals[6] != ContourValue) << 6;
              caseIndex |= (voxelVals[7] != ContourValue) << 7;
            }//End
            else
            {
              caseIndex = voxelVals[0] > ContourValue; 
              caseIndex |= (voxelVals[1] > ContourValue) << 1;
              caseIndex |= (voxelVals[2] > ContourValue) << 2;
              caseIndex |= (voxelVals[3] > ContourValue) << 3;
              caseIndex |= (voxelVals[4] > ContourValue) << 4;
              caseIndex |= (voxelVals[5] > ContourValue) << 5;
              caseIndex |= (voxelVals[6] > ContourValue) << 6;
              caseIndex |= (voxelVals[7] > ContourValue) << 7;
            }
            const EDGE_LIST *edge = marchingCubesCases[caseIndex].edges;
            if (*edge < 0)
              continue;
            if (EnableContourAnalysis) {
              if (polylines[0].IsInside(x, y, 50) && polylines[1].IsInside(x, y, 50))
                continue;
            }

            while (*edge >= 0) {
              vtkIdType pointIds[3];

              for (int ii = 0; ii < 3; ii++, edge++) {
                const int vert0 = edgeOffsets[*edge][0], vert1 = edgeOffsets[*edge][1];
                const int *vertIndeces[2] = {voxelVertIndicesXYZ[vert0], voxelVertIndicesXYZ[vert1] };

                // was this edge created?
                vtkIdType (&pointLocatorRef) = pointLocator[plOffsets[0] * (x + vertIndeces[0][0]) + plOffsets[1] * (y + vertIndeces[0][1]) + plOffsets[2] * vertIndeces[0][2] + edgeAxis[*edge]];
                if (pointLocatorRef != -1) {
                  pointIds[ii] = pointLocatorRef;
                  continue;
                }

                float point[3] = { fx + vertIndeces[0][0] * fxSize, fy + vertIndeces[0][1] * fySize, fz + vertIndeces[0][2] * fzSize};
                const float edgeRatio = (float)(ContourValue - voxelVals[vert0]) / (voxelVals[vert1] - voxelVals[vert0]);
                point[edgeAxis[*edge]] += edgeRatio * fxyzSize[edgeAxis[*edge]];
                // add new point
                pointIds[ii] = points->InsertNextPoint(point);

                // estimate gradient
                int gradient[2][3];
                for (int vi = 0; vi < 2; vi++) {//!!!
                  const DataType * const verticePtr = voxelPtr + VoxelOffsets[vi == 0 ? vert0 : vert1];

                  const int vx = x + vertIndeces[vi][0];
                  gradient[vi][0] = vx > level ? (vx < lastIndex[0] ? ((int(verticePtr[1]) - int(verticePtr[-1])) >> 1) : (int(verticePtr[0]) - verticePtr[-1]))
                    : (int(verticePtr[1]) - verticePtr[0]);
                  const int vy = y + vertIndeces[vi][1];
                  gradient[vi][1] = vy > level ? (vy < lastIndex[1] ? ((int(verticePtr[rowSize]) - int(verticePtr[-rowSize])) >> 1) : (int(verticePtr[0]) - verticePtr[-rowSize]))
                    : (int(verticePtr[rowSize]) - verticePtr[0]);
                  const int vz = z + vertIndeces[vi][2];
                  gradient[vi][2] = vz > level ? (vz < lastIndex[2] ? ((int(verticePtr[sliceSize]) - int(verticePtr[-sliceSize])) >> 1) : (int(verticePtr[0]) - verticePtr[-sliceSize]))
                    : (int(verticePtr[sliceSize]) - verticePtr[0]);
                }

                // calculate normals
                float normal[3] = {(gradient[0][0] + edgeRatio * (gradient[1][0] - gradient[0][0])) * fxSizeNorm,
                  (gradient[0][1] + edgeRatio * (gradient[1][1] - gradient[0][1])) * fySizeNorm,
                  (gradient[0][2] + edgeRatio * (gradient[1][2] - gradient[0][2])) * fzSizeNorm};
                const float normalLen = -1.f / sqrt(0.0001f + normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);

                normal[0] *= normalLen;
                normal[1] *= normalLen;
                normal[2] *= normalLen;

                normals->InsertNextTuple(normal);
                pointLocatorRef = pointIds[ii];
              }
              // add triangle
              triangles->InsertNextCell(3, pointIds);
            } //for each triangle

          }
        }

      } // for (bxi)
    } // for (byi)
  } // for (z)

  delete [] pointLocator;

  // free extra memory
  points->Squeeze();
  normals->Squeeze();
  triangles->Squeeze();

  this->UpdateProgress(1.f);
} // CreateMCubes()




//------------------------------------------------------------------------------
// This helper function returns an estimate of the expected number of triangles 
// in the normal (non-LOD) mode, given the current number of normal and LOD triangles.
// A zero value means that the number is undefined (see constructor)
// Only use this to decide whether to use LOD or not.
int vtkContourVolumeMapper::EstimateTrianglesNoOpt()
//------------------------------------------------------------------------------
{
  if ((this->NumberOfTriangles[0] != 0) && (this->PrevContourValue[0] == this->ContourValue)){
    // no. of normal triangles is valid for this contour value, so just return it
    return this->NumberOfTriangles[0] ;
  }
  else if ((this->NumberOfTriangles[1] != 0) && (this->PrevContourValue[1] == this->ContourValue)){
    // no. of lod triangles is valid, so use this to estimate normal triangles
    if (this->DataSpacing[2] <= this->DataSpacing[0])
      return 8*this->NumberOfTriangles[1] ;
    else
      return 4*this->NumberOfTriangles[1] ;
  }
  else{
    // if both numbers undefined, return a very large number
    return 100000000 ;
  }
}




//------------------------------------------------------------------------------
static const vtkMarchingSquaresLineCases* marchingSquaresCases = vtkMarchingSquaresLineCases::GetCases();



//------------------------------------------------------------------------------
void vtkContourVolumeMapper::PrepareContours(const int slice, const void *imageData, ListOfPolyline2D& polylines) {
  this->Polylines = &polylines;

  switch (this->GetDataType()) {
    case VTK_CHAR:
      this->PrepareContoursTemplate(slice, (const char*)imageData);
      break;
    case VTK_UNSIGNED_CHAR:
      this->PrepareContoursTemplate(slice, (const unsigned char*)imageData);
      break;
    case VTK_SHORT:
      this->PrepareContoursTemplate(slice, (const short*)imageData);
      break;
    case VTK_UNSIGNED_SHORT:
      this->PrepareContoursTemplate(slice, (const unsigned short*)imageData);
      break;
  }
}



//------------------------------------------------------------------------------
template<typename DataType> void vtkContourVolumeMapper::PrepareContoursTemplate(const int slice, const DataType *imageData) {
  const DataType (* const BlockMinMax)[2] = (DataType (*)[2])((DataType *)this->BlockMinMax + 2 * (slice >> VoxelBlockSizeLog) * this->NumBlocks[0] * this->NumBlocks[1]);
  const DataType ContourValue = (DataType)this->ContourValue;
  const int lastXBlock = this->NumBlocks[0] - 1, lastYBlock = this->NumBlocks[1] - 1;
  const int lastXBlockSize = this->DataDimensions[0] - 1 - (lastXBlock << VoxelBlockSizeLog), lastYBlockSize = this->DataDimensions[1] - 1 - (lastYBlock << VoxelBlockSizeLog);

  ListOfPolyline2D& polylines = *this->Polylines;
  polylines.clear();

  int statCounter = 0;

  for (int byi = 0, yblock = 0; byi < this->NumBlocks[1]; byi++, yblock += this->NumBlocks[0]) {
    const int yblockSize = (byi < lastYBlock) ? VoxelBlockSize : lastYBlockSize;
    for (int bxi = 0, block = yblock; bxi < this->NumBlocks[0]; bxi++, block++) {
      if (ContourValue > BlockMinMax[block][1] || ContourValue < BlockMinMax[block][0])
        continue; // skip the block
      const int xblockSize = (bxi < lastXBlock) ? VoxelBlockSize : lastXBlockSize;

      const int xBlock = (bxi << VoxelBlockSizeLog);
      const int yBlock = (byi << VoxelBlockSizeLog);
      const DataType *imageBlock = imageData + yBlock * this->DataDimensions[0] + xBlock;

      // build the contours
      for (int yi = 0; yi < yblockSize; yi++, imageBlock += this->DataDimensions[0]) {
        for (int xi = 0; xi < xblockSize; xi++) {
          const DataType * const voxelPtr = imageBlock + xi;

          const DataType voxelVals[4] = {voxelPtr[0], voxelPtr[1], voxelPtr[this->DataDimensions[0] + 1], voxelPtr[this->DataDimensions[0]]};
          int caseIndex = voxelVals[0] > ContourValue; 
          caseIndex |= (voxelVals[1] > ContourValue) << 1;
          caseIndex |= (voxelVals[2] > ContourValue) << 2;
          caseIndex |= (voxelVals[3] > ContourValue) << 3;
          const EDGE_LIST * edge = marchingSquaresCases[caseIndex].edges;
          if (*edge < 0)
            continue;

          const int x = (xBlock + xi) << 1;
          const int y = (yBlock + yi) << 1;

          Polyline2D::Point line[2];

          while(*edge >= 0) {
            static const int edgeToOffset[4][2] = {{1, 0}, {2, 1}, {1, 2}, {0, 1}};
            // point 1
            line[0].xy[0] = x + edgeToOffset[*edge][0];
            line[0].xy[1] = y + edgeToOffset[*edge][1];
            edge++;
            // point 2
            line[1].xy[0] = x + edgeToOffset[*edge][0];
            line[1].xy[1] = y + edgeToOffset[*edge][1];
            edge++;

            // try to add line to polylines
            const int numOfPolylines = polylines.size();
            int pi;
            for (pi = numOfPolylines - 1; pi >= 0; pi--) {
              if (polylines[pi]->AddNextLine(line))
                break;
            }
            // try to merge polylines
            if (pi >= 0) {
              for (int pj = numOfPolylines - 1; pj >= 0; pj--) {
                if (pi != pj && polylines[pi]->Merge(*polylines[pj])) {
                  delete polylines[pj];
                  polylines[pj] = polylines[numOfPolylines - 1];
                  polylines.pop_back();
                  break;
                }
              }
              continue;
            }
            polylines.push_back(new Polyline2D(line));
          } // edge loop

        } // for (xi)
      } // for (yi)

    }  // for (bxi)
  } // for (byi)

  for (int pi = polylines.size() - 1; pi >= 0 ; pi--) {
    if (polylines[pi]->Length() < 30) {
      delete polylines[pi];
      polylines[pi] = polylines[polylines.size() - 1];
      polylines.pop_back();
      statCounter++;
    }
    else {
      polylines[pi]->Close();
    }
  }

  // eliminate inside polygons
#if 0
  for (pi = 0; pi < polylines.size(); pi++) {
    for (int pj = polylines.size() - 1; pj >= 0; pj--) {
      if (pi != pj && polylines[pi]->IsInsideOf(polylines[pj])) {
        delete polylines[pi];
        polylines[pi] = polylines[polylines.size() - 1];
        polylines.pop_back();
        pi--;
        break;
      }
    }
  }
#endif
}



//------------------------------------------------------------------------------
// calculate depth of vertex from screen
// matrix MP is in openGL format
float vtkContourVolumeMapper::DepthOfVertex(float *v, float *MP) const
//------------------------------------------------------------------------------
{
  float zhomo, whomo, z ;

  // transform in homogeneous coords v*M
  zhomo = v[0]*MP[8] + v[1]*MP[9] + v[2]*MP[10] + MP[11] ;
  whomo = v[0]*MP[12] + v[1]*MP[13] + v[2]*MP[14] + MP[15] ;

  // perspective division
  z = zhomo/whomo ;
  return -z ;
}




//------------------------------------------------------------------------------
// Calculate matrix required for depth transform
// Return product MP of modelview and projection matrices
void vtkContourVolumeMapper::CalculateDepthMatrix(float *MP)
//------------------------------------------------------------------------------
{
  float M[16], P[16] ;

  glMatrixMode(GL_MODELVIEW);
  glPushMatrix() ;

  glGetFloatv(GL_MODELVIEW_MATRIX, M) ;
  glGetFloatv(GL_PROJECTION_MATRIX, P) ;

  glLoadIdentity() ;
  glLoadMatrixf(P) ;
  glMultMatrixf(M) ;
  glGetFloatv(GL_MODELVIEW_MATRIX, MP) ;

  glPopMatrix() ;
}





//------------------------------------------------------------------------------
// Sort triangles into back-to-front order
// lod is cache 0 or 1 
void vtkContourVolumeMapper::SortTriangles(int lod)
//------------------------------------------------------------------------------
{

  // check that we have some triangles to draw
  if (this->TriangleCache[lod] == NULL)
    return ;


  // Get depth transform matrix
  float MP[16] ;
  CalculateDepthMatrix(MP) ;

  // allocate list of depth values
  int ntri = this->NumberOfTriangles[lod] ;
  int nvert = 3*ntri ;
  Idepth *depthlist = new Idepth[ntri] ;

  // allocate vertex order array, if necessary
  // (deallocated in the destructor)
  if ((this->ordered_vertices == NULL) || (this->NumberOfTriangles[lod] != this->ntriangles_previous[lod])){
    // allocate if undefined or no. of triangles has changed
    delete[] ordered_vertices[lod] ;
    this->ordered_vertices[lod] = new unsigned int[nvert] ;
    this->ntriangles_previous[lod] = this->NumberOfTriangles[lod] ;

    // put array into default order
    for (int j = 0 ;  j < nvert ; j++)
      this->ordered_vertices[lod][j] = j ;
  }

  this->Timer->StopTimer() ;
  this->Timer->StartTimer() ;

  // loop through triangles and calculate depth of 1st vertex of each triangle
  // store triangles in order of previous sort
  int i, j, jj ;
  float *v, depth, mindepth, maxdepth ;
  for (i = 0, j = 0, mindepth = 10000.0, maxdepth = -10000.0 ;  i < ntri ;  i++, j+=3){
    jj = ordered_vertices[lod][j] ;             // get index jj of next vertex in depth order
    v = this->TriangleCache[lod] + 6*jj + 3 ;   // get pointer to vertex jj
    depth = DepthOfVertex(v, MP) ;              // calculate depth
    depthlist[i].depth = depth ;                // store depth and 1st vertex of triangle i
    depthlist[i].index = jj ;

    mindepth = std::min(depth, mindepth) ;
    maxdepth = std::max(depth, maxdepth) ;
  }

  this->Timer->StopTimer() ;
  float t1 = 60.0*this->Timer->GetElapsedTime() ;
  this->Timer->StartTimer() ;

  // sort triangles into order
  std::sort<Idepth*>(depthlist, depthlist+ntri-1) ;

  this->Timer->StopTimer() ;
  float t2 = 60.0*this->Timer->GetElapsedTime() ;


  // create ordered list of all vertices
  int index ;
  unsigned int *ov = this->ordered_vertices[lod] ;
  for (i = 0, j = 0 ;  i < ntri ;  i++, j += 3){
    index = depthlist[i].index ;
    ov[j] = index ;
    ov[j+1] = index+1 ;
    ov[j+2] = index+2 ;
  }

  std::cout << "sort: " << " triangles = " << this->NumberOfTriangles[lod] << " " << t1 << " " << t2 << "\n";
  //mafErrorMacro("sort: " << " triangles = " << this->NumberOfTriangles[lod] << " " << t1 << " " << t2 << "\n") ;

  delete[] depthlist ;
}




///////////////////////////////////////////////////////////////////////////////////
//                             class Polyline2D

//---------------------------------------------------------------------------------
void Polyline2D::Reallocate() {
  const int newsize = 2 * (size + 1);
  Point *buffer = new Point[newsize];
  const int newstart = int(0.5f * size + 1);
  const int newend   = newstart + this->end - this->start;
  memcpy(buffer + newstart, this->vertices + this->start, sizeof(Point) * (this->end - this->start + 1));
  if (this->vertices != this->verticesBuffer)
    delete [] vertices;
  this->vertices = buffer;
  this->start = newstart;
  this->end   = newend;
  this->size = newsize;
  assert(this->start > 1 && (this->end + 2) < newsize);
}


//---------------------------------------------------------------------------------
void Polyline2D::Allocate(int newsize) {
  if (this->vertices != NULL && (this->end - this->start) > newsize)
    return;
  if (this->vertices != this->verticesBuffer)
    delete [] vertices;
  this->vertices = new Point[newsize];
  this->start = -1;
  this->end   = -1;
  this->size  = newsize;
  this->closestPolyline[0] = this->closestPolyline[1] = -1;
  this->minDistance[0] = this->minDistance[1] = 0;
}


//---------------------------------------------------------------------------------
Polyline2D::Polyline2D(const Point *line) {
  assert(sizeof(Point) == (2 * sizeof(short)));
  this->size = VERTICES_BUFFER_SIZE;
  this->start = this->end = VERTICES_BUFFER_SIZE >> 1;
  this->end++;
  this->vertices = this->verticesBuffer;
  this->vertices[this->start] = line[0];
  this->vertices[this->end]   = line[1];
  this->closestPolyline[0] = this->closestPolyline[1] = -1;
  this->minDistance[0] = this->minDistance[1] = 0;
  this->updateBoundingBox = true;
}


//---------------------------------------------------------------------------------
bool Polyline2D::AddNextLine(const Point *newLine) {
  if (newLine[0] == this->vertices[this->start]) {
    if (this->start == 0)
      Reallocate();
    this->vertices[--this->start] = newLine[1];
  }
  else if (newLine[0] == this->vertices[this->end]) {
    if ((this->end + 1) == size)
      Reallocate();
    this->vertices[++this->end] = newLine[1];
  }
  else if (newLine[1] == this->vertices[this->start]) {
    if (this->start == 0)
      Reallocate();
    this->vertices[--this->start] = newLine[0];
  }
  else if (newLine[1] == this->vertices[this->end]) {
    if ((this->end + 1) == size)
      Reallocate();
    this->vertices[++this->end] = newLine[0];
  }
  else {
    return false;
  }
  assert(this->end < this->size && this->start >= 0);
  this->updateBoundingBox = true;
  return true;
}


//---------------------------------------------------------------------------------
bool Polyline2D::Merge(Polyline2D &polyline) {
  if (polyline.vertices[polyline.start] == this->vertices[start]) {
    while (this->start <= (polyline.end - polyline.start))
      Reallocate();
    for (int i = polyline.start + 1; i <= polyline.end; i++)
      this->vertices[--this->start] = polyline.vertices[i];
  }
  else if (polyline.vertices[polyline.start] == this->vertices[end]) {
    while ((this->size - this->end) <= (polyline.end - polyline.start))
      Reallocate();
    memcpy(this->vertices + end + 1, polyline.vertices + polyline.start + 1, sizeof(Point) * (polyline.end - polyline.start));
    this->end += polyline.end - polyline.start;
  }
  else if (polyline.vertices[polyline.end] == this->vertices[start]) {
    while (this->start <= (polyline.end - polyline.start))
      Reallocate();
    memcpy(this->vertices + start - (polyline.end - polyline.start), 
      polyline.vertices + polyline.start, sizeof (Point) * (polyline.end - polyline.start));
    this->start -= polyline.end - polyline.start;
  }
  else if (polyline.vertices[polyline.end] == this->vertices[end]) {
    while ((this->size - this->end) <= (polyline.end - polyline.start))
      Reallocate();
    for (int i = polyline.end - 1; i >= polyline.start; i--)
      this->vertices[++this->end] = polyline.vertices[i];
  }
  else {
    return false;
  }

  this->updateBoundingBox = true;
  assert(this->end < this->size && this->start >= 0);
  return true;
}


//---------------------------------------------------------------------------------
void Polyline2D::Close() {
  if (this->vertices[this->start] == this->vertices[this->end]) {
    if ((this->end + 1) == this->size)
      this->Reallocate();
    this->vertices[++this->end] = this->vertices[this->start];
  }
}



//---------------------------------------------------------------------------------
void Polyline2D::UpdateBoundingBox() const {
  this->bbox[0] = this->bbox[2] = VTK_SHORT_MAX;
  this->bbox[1] = this->bbox[3] = VTK_SHORT_MIN;
  for (int i = this->start; i <= this->end; i++) {
    const short * const xy = this->vertices[i].xy;
    if (xy[0] > this->bbox[1])
      this->bbox[1] = xy[0];
    if (xy[0] < this->bbox[0])
      this->bbox[0] = xy[0];
    if (xy[1] > this->bbox[3])
      this->bbox[3] = xy[1];
    if (xy[1] < this->bbox[2])
      this->bbox[2] = xy[1];
  }
  this->updateBoundingBox = false;
}


/*
//------------------------------------------------------------------------------------------------
void Polyline2D::FindClosestPolyline(int index, int numOfPolylines, Polyline2D* polylines) {
assert(index == 0 || index == 1);

this->minDistance[index] = VTK_FLOAT_MAX;
this->closestPolyline[index] = -1;

const int numPoints = (this->end - this->start + 1);

//const float distanceThreshold = 4.f * (this->dataSpacing[0] + this->dataSpacing[1]);

for (int pi = 0; pi < numOfPolylines; pi++) {
// compare two polylines
float distance = 0;

if (this->bbox[0] > polylines[pi].bbox[1] || this->bbox[1] < polylines[pi].bbox[0] ||
this->bbox[2] > polylines[pi].bbox[3] || this->bbox[3] < polylines[pi].bbox[2])
continue; // bounding boxes do not intersect

for (int i = this->start; i <= this->end; i++) {
float *iPoint = this->pointArray->GetPoint(this->vertices[i]);

// find the best match
float localMindistance = VTK_FLOAT_MAX;
for (int j = polylines[pi].start; j <= polylines[pi].end; j++) {
float *jPoint = polylines[pi].pointArray->GetPoint(polylines[pi].vertices[j]);
float locdistance = fabs(jPoint[0] - iPoint[0]) + fabs(jPoint[1] - iPoint[1]);
if (locdistance < localMindistance)
localMindistance = locdistance;
}
distance += localMindistance;
}
distance *= (fabs(numPoints - polylines[pi].end + polylines[pi].start - 1) / numPoints + 1) / numPoints;

if (distance < this->minDistance[index]) {
this->minDistance[index] = distance;
this->closestPolyline[index] = pi;
}
}
if (this->closestPolyline[index] == -1)
this->minDistance[index] = 0;
}


//----------------------------------------------------------------------------------------------------
int Polyline2D::FindSubPolyline(int numOfPolylines, Polyline2D* polylines, float &minDistance) {
const int numPoints = (this->end - this->start + 1);
minDistance = VTK_FLOAT_MAX;
int bestMatch = -1;

for (int pi = 0; pi < numOfPolylines; pi++) {
// compare two polylines
float distance = 0;

if (this->bbox[0] > polylines[pi].bbox[1] || this->bbox[1] < polylines[pi].bbox[0] ||
this->bbox[2] > polylines[pi].bbox[3] || this->bbox[3] < polylines[pi].bbox[2])
continue; // bounding boxes do not intersect
if ((polylines[pi].end - polylines[pi].start) > numPoints)
continue;

for (int j = polylines[pi].start; j <= polylines[pi].end; j++) {
float *jPoint = polylines[pi].pointArray->GetPoint(polylines[pi].vertices[j]);

// find the best match
float localMindistance = VTK_FLOAT_MAX;
for (int i = this->start; i <= this->end; i++) {  
float *iPoint = this->pointArray->GetPoint(this->vertices[i]);
float locdistance = fabs(jPoint[0] - iPoint[0]) + fabs(jPoint[1] - iPoint[1]);
if (locdistance < localMindistance)
localMindistance = locdistance;
}
distance += localMindistance;
}
distance /= (polylines[pi].end - polylines[pi].start);
if (distance < minDistance) {
bestMatch = pi;
minDistance = distance;
}
}
return bestMatch;
}




//---------------------------------------------------------------------------------
bool Polyline2D::SplitPolyline(Polyline2D& subpoly, Polyline2D& newpoly) {
const int polyLength = this->end - this->start + 1;
const int subpolyLength = subpoly.end - subpoly.start + 1;

float avgLineLength = 0;
for (int si = subpoly.start; si < subpoly.end; si++) {
float *iPoint = subpoly.pointArray->GetPoint(subpoly.vertices[si]);
float *jPoint = subpoly.pointArray->GetPoint(subpoly.vertices[si + 1]);
avgLineLength += fabs(jPoint[0] - iPoint[0]) + fabs(jPoint[1] - iPoint[1]);
}
avgLineLength /= (subpoly.end - subpoly.start);
const float pointDistanceThreshold = 30.f * avgLineLength;

// find the best match between two polylines
static char *hitBuffer = NULL;
static int hitBufferLength = 0;
if (hitBuffer == NULL || hitBufferLength <= (polyLength + 1)) {
delete [] hitBuffer;
hitBuffer = new char [polyLength + 1];
hitBufferLength = polyLength + 1;
}
memset(hitBuffer, 0, polyLength + 1);

bool anyHit = false;
for (si = subpoly.start; si < subpoly.end; si++) {
float *iPoint = subpoly.pointArray->GetPoint(subpoly.vertices[si + 1]);

// fin best match between points
float bestDistance = VTK_FLOAT_MAX;
int bestIndex;
for (int j = this->start; j <= this->end; j++) {
float *jPoint = this->pointArray->GetPoint(this->vertices[j]);
float distance = fabs(jPoint[0] - iPoint[0]) + fabs(jPoint[1] - iPoint[1]);
if (distance < bestDistance) {
bestDistance = distance;
bestIndex = j;
}
}
if (bestDistance < pointDistanceThreshold) {
hitBuffer[bestIndex - this->start] = 1;
anyHit = true;
}
} // for (i)

if (!anyHit)
return false;

// analyze the hits
int bestIndex = -1;
int bestLength = 0;
for (int i = 0; i < polyLength; i++) {
const int startI = i;
while (hitBuffer[i] == 0) {
i++;
if (i >= polyLength)
i = 0;
}
const int length = (i >= startI) ? (i - startI) : (i + polyLength - startI);
if (length > bestLength) {
bestLength = length;
bestIndex = startI;
}
if (i < startI)
break;
}

// avoid problems with border cases
if (bestIndex == 0)
bestIndex++;
if ((bestIndex + bestLength) == polyLength)
bestLength--;
if (bestLength < 40)
return false; // the subpolyline is too small

// identify borders of polylines
bool newPolyLineOverlap = ((bestIndex + bestLength) >= polyLength);
int  newPolyStart = bestIndex;
int  newPolyEnd =  (bestIndex + bestLength) % polyLength;

// find the narrowest point
float bestDistance = VTK_FLOAT_MAX;
int   bestS, bestE;
for (si = (newPolyStart > 15) ? (newPolyStart - 15) : 0; si < (polyLength - 1) && si < (newPolyStart + 15); si++) {
float *sPoint = this->pointArray->GetPoint(this->vertices[si + this->start]);
for (int ei = (newPolyEnd > 15) ? (newPolyEnd - 15) : 0; ei < (polyLength - 1) && ei < (newPolyEnd + 15); ei++) {
float *ePoint = this->pointArray->GetPoint(this->vertices[ei + this->start]);
float distance = fabs(sPoint[0] - ePoint[0]) + fabs(sPoint[1] - ePoint[1]);
if (distance < bestDistance) {
bestS = si;
bestE = ei;
bestDistance = distance;
}
}
}
newPolyStart = bestS;
newPolyEnd   = bestE;
const int newPolyLength = (newPolyEnd >= newPolyStart) ? (newPolyEnd - newPolyStart) : (newPolyEnd + polyLength - newPolyStart);

// test if the polylines are OK
if (newPolyLength > 1.2 * (polyLength - subpolyLength) ||
newPolyLength < 0.8 * (polyLength - subpolyLength))
return false; // deviation is too big
// check the distance
const float *sPoint = this->pointArray->GetPoint(this->vertices[this->start + newPolyStart]);
const float *ePoint = this->pointArray->GetPoint(this->vertices[this->start + newPolyEnd]);
if ((fabs(sPoint[0] - ePoint[0]) + fabs(sPoint[1] - ePoint[1])) > pointDistanceThreshold)
return false;

// create new polyline
newpoly.Allocate(newPolyLength + 3);
if (newPolyLineOverlap) {
memcpy(newpoly.vertices, this->vertices + this->start + newPolyStart, sizeof (vtkIdType) * (polyLength - newPolyStart));
memcpy(newpoly.vertices + polyLength - newPolyStart, this->vertices + this->start, sizeof (vtkIdType) * (newPolyEnd + 1));
}
else {
memcpy(newpoly.vertices, this->vertices + this->start + newPolyStart, sizeof (vtkIdType) * (newPolyLength + 1));
}
newpoly.start = 1;
newpoly.end   = newPolyLength;
newpoly.vertices[newpoly.end] = newpoly.vertices[newpoly.start];
newpoly.SetPointArray(this->pointArray);
newpoly.UpdateBoundingBox();

// modify the original
if (newPolyLineOverlap) {
this->end    = this->start + newPolyStart - 1;
this->start += newPolyEnd + 1;
this->vertices[this->end] = this->vertices[this->start];
}
else {
memcpy(this->vertices + this->start + newPolyStart - 1, this->vertices + this->start + newPolyEnd, sizeof (vtkIdType) * (polyLength - newPolyEnd));
this->end -= (newPolyLength + 1);
}

this->UpdateBoundingBox();

return true;
}
*/


//------------------------------------------------------------------------------
void Polyline2D::Move(Polyline2D &polyline) {
  if (this->vertices != this->verticesBuffer)
    delete [] vertices;
  *this = polyline; // copy all members

  this->updateBoundingBox = true;

  // reset the source
  polyline.vertices = NULL;
}


//------------------------------------------------------------------------------
// check if one polyline is inside another       
bool Polyline2D::IsInsideOf(const Polyline2D *outerPolyline) const {
  if (this->updateBoundingBox)
    this->UpdateBoundingBox();
  if (outerPolyline->updateBoundingBox)
    outerPolyline->UpdateBoundingBox();

  // check bounding boxes
  if (outerPolyline->bbox[0] > this->bbox[1] ||
    outerPolyline->bbox[2] > this->bbox[3] ||
    outerPolyline->bbox[1] < this->bbox[0] ||
    outerPolyline->bbox[3] < this->bbox[2] || outerPolyline->Length() < 16)
    return false;

  // get sample point
  int intersection = 0;
  const short sx = this->vertices[this->start].xy[0];
  const short sy = this->vertices[this->start].xy[1];

  for (int p = outerPolyline->start + 1; p < outerPolyline->end; p++) {
    const short pointX   = outerPolyline->vertices[p].xy[0];
    const short pointY   = outerPolyline->vertices[p].xy[1];
    if (pointY != sy || pointX <= sx)
      continue;
    const short prevPointY   = outerPolyline->vertices[p - 1].xy[1];
    p++;
    while ((p < outerPolyline->end) && (outerPolyline->vertices[p].xy[1] == pointY))
      p++; // skip parallel lines
    if (p < outerPolyline->end && outerPolyline->vertices[p].xy[1] != prevPointY)
      intersection++;
  }

  // check
  if ((intersection % 2) == 1 && this->Length() > 32) {
    intersection = 0;

    for (int p = outerPolyline->start + 1; p < outerPolyline->end; p++) {
      const short pointX   = outerPolyline->vertices[p].xy[0];
      const short pointY   = outerPolyline->vertices[p].xy[1];
      if (pointY != sy || pointX >= sx)
        continue;
      const short prevPointY   = outerPolyline->vertices[p - 1].xy[1];
      p++;
      while ((p < outerPolyline->end) && (outerPolyline->vertices[p].xy[1] == pointY))
        p++; // skip parallel lines
      if (p < outerPolyline->end && outerPolyline->vertices[p].xy[1] != prevPointY)
        intersection++;
    }
  }

  return ((intersection % 2) == 1);
}



//------------------------------------------------------------------------------
// Clear the list of polylines
void ListOfPolyline2D::clear() {
  for (int pj = size() - 1; pj >= 0; pj--)
    delete at(pj);
  erase(begin(), end());
}


//------------------------------------------------------------------------------
bool ListOfPolyline2D::IsInside(int x, int y, int polylineLengthThreshold) {
  const short sx = x << 1;
  const short sy = y << 1;

  for (int pi = this->size() - 1; pi >= 0; pi--) {
    const Polyline2D * const polyline = at(pi);

    // ignore polyline if length < threshold
    if (polyline->Length() < polylineLengthThreshold)
      continue;

    // check if bounding box needs updating
    if (polyline->updateBoundingBox)
      polyline->UpdateBoundingBox();

    // check bounding boxes
    if ((polyline->bbox[0] > sx) || (polyline->bbox[1] < sx) ||
      (polyline->bbox[2]) > sy || (polyline->bbox[3] < sy))
      continue;

    int intersection = 0;
    for (int p = polyline->start + 1; p < polyline->end; p++) {
      const short pointX   = polyline->vertices[p].xy[0];
      const short pointY   = polyline->vertices[p].xy[1];
      if (pointY != sy || pointX <= sx)
        continue;
      const short prevPointY   = polyline->vertices[p - 1].xy[1];
      p++;
      while ((p < polyline->end) && (polyline->vertices[p].xy[1] == pointY))
        p++; // skip parallel lines
      if (p < polyline->end && polyline->vertices[p].xy[1] != prevPointY)
        intersection++;
    }

    // check
    if ((intersection % 2) == 1) {
      intersection = 0;
      for (int p = polyline->start + 1; p < polyline->end; p++) {
        const short pointX   = polyline->vertices[p].xy[0];
        const short pointY   = polyline->vertices[p].xy[1];
        if (pointY != sy || pointX >= sx)
          continue;
        const short prevPointY   = polyline->vertices[p - 1].xy[1];
        p++;
        while ((p < polyline->end) && (polyline->vertices[p].xy[1] == pointY))
          p++; // skip parallel lines
        if (p < polyline->end && polyline->vertices[p].xy[1] != prevPointY)
          intersection++;
      }
    }

    if ((intersection % 2) == 1) {
      // test that the point is inside, not on the contour
      int p;
      for (p = polyline->start + 1; p < polyline->end; p++) {
        const short pointX   = polyline->vertices[p].xy[0];
        const short pointY   = polyline->vertices[p].xy[1];

        if (abs(pointX - sx) <= 4 && abs(pointY - sy) <= 4)
          break;
      }

      if (p == polyline->end)
        return true;
    }
  }

  return false;
}



//------------------------------------------------------------------------------
Polyline2D *ListOfPolyline2D::FindContour(int x, int y, int polylineLengthThreshold, int distance) {
  const short sx = x << 1;
  const short sy = y << 1;
  distance = distance << 1;

  for (int pi = this->size() - 1; pi >= 0; pi--) {
    Polyline2D *polyline = at(pi);
    if (polyline->Length() < polylineLengthThreshold)
      continue;

    if (polyline->updateBoundingBox)
      polyline->UpdateBoundingBox();

    // check bounding boxes
    if (polyline->bbox[0] > (sx + distance) || polyline->bbox[1] < (sx - distance) ||
      polyline->bbox[2] > (sy + distance) || polyline->bbox[3] < (sy - distance))
      continue;

    for (int p = polyline->start + 1; p < polyline->end; p++) {
      const short pointX   = polyline->vertices[p].xy[0];
      const short pointY   = polyline->vertices[p].xy[1];

      if (abs(pointX - sx) <= distance && abs(pointY - sy) <= distance)
        return polyline;
    }
  }

  return NULL;
}