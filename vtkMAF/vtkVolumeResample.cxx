/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkVolumeResample.cxx,v $
  Language:  C++
  Date:      $Date: 2006-11-23 17:16:48 $
  Version:   $Revision: 1.4 $

=========================================================================*/
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPointData.h"

#include "vtkMath.h"

#include "vtkVolumeResample.h"

#include "assert.h"

vtkCxxRevisionMacro(vtkVolumeResample, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkVolumeResample);

typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned int u_int;

inline void clip(double val, float& out) { out = val; }
inline void clip(double val, char&  out) { int x = double(VTK_CHAR_MAX) * (val - 0.5f); if (x < VTK_CHAR_MIN) out = VTK_CHAR_MIN; else if (x > VTK_CHAR_MAX) out = VTK_CHAR_MAX; else out = char(x); }
inline void clip(double val, short& out) { int x = double(VTK_SHORT_MAX) * (val - 0.5f); if (x < VTK_SHORT_MIN) out = VTK_SHORT_MIN; else if (x > VTK_SHORT_MAX) out = VTK_SHORT_MAX; else out = short(x); }
inline void clip(double val, u_char &  out) { int x = VTK_UNSIGNED_CHAR_MAX * val; if (x < VTK_UNSIGNED_CHAR_MIN) out = VTK_UNSIGNED_CHAR_MIN; else if (x > VTK_UNSIGNED_CHAR_MAX) out = VTK_UNSIGNED_CHAR_MAX; else out = u_char(x); }
inline void clip(double val, u_short & out) { int x = VTK_UNSIGNED_SHORT_MAX * val; if (x < VTK_UNSIGNED_SHORT_MIN) out = VTK_UNSIGNED_SHORT_MIN; else if (x > VTK_UNSIGNED_SHORT_MAX) out = VTK_UNSIGNED_SHORT_MAX; else out = u_short(x); }


#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))

//--------------------------------------------------------------------------
// The 'floor' function on x86 and mips is many times slower than these
// and is used a lot in this code, optimize for different CPU architectures
inline int mafVolumeResliceFloor(double x)
{
#if defined mips || defined sparc || defined __ppc__
  return (int)((u_int)(x + 2147483648.0) - 2147483648U);
#elif defined i386 || defined _M_IX86
  u_int hilo[2];
  *((double *)hilo) = x + 103079215104.0;  // (2**(52-16))*1.5
  return (int)((hilo[1]<<16)|(hilo[0]>>16));
#else
  return int(floor(x));
#endif
}

inline int mafVolumeResliceCeil(double x)
{
  return -mafVolumeResliceFloor(-x - 1.0) - 1;
}

inline int mafVolumeResliceRound(double x)
{
  return mafVolumeResliceFloor(x + 0.5);
}

static const int SamplingTableSize = 64000;
/*
void vtkVolumeResample::myDBG(const char *msg,int index)
{
  cerr<<"myDBG: ";
  if (msg)
    cerr<<msg<<" ";
  cerr<<index<<" "<<this->Num<<"\n";
}
*/
//----------------------------------------------------------------------------
// Constructor sets default values
vtkVolumeResample::vtkVolumeResample() 
{
  this->VolumeOrigin[0] = this->VolumeOrigin[1] = this->VolumeOrigin[2] = this->VolumeAxisX[1] = this->VolumeAxisX[2] = this->VolumeAxisY[0] = this->VolumeAxisY[2] = 0.f;
  this->VolumeAxisX[0]  = this->VolumeAxisY[1]  = 1.f;

  this->Window = 1.f;
  this->Level  = 0.5f;

  this->ZeroValue = 0;

  this->AutoSpacing = true;

  this->VoxelCoordinates[0] = this->VoxelCoordinates[1] = this->VoxelCoordinates[2] = NULL;
}


vtkVolumeResample::~vtkVolumeResample() {
  delete [] this->VoxelCoordinates[0];
  delete [] this->VoxelCoordinates[1];
  delete [] this->VoxelCoordinates[2];
  }

//----------------------------------------------------------------------------
void vtkVolumeResample::SetVolumeAxisX(double axis[3])
{
  if (vtkMath::Norm(axis) < 1.e-5f)
    return;

  this->VolumeAxisX[0]=axis[0];
  this->VolumeAxisX[1]=axis[1];
  this->VolumeAxisX[2]=axis[2];
    
  vtkMath::Normalize(this->VolumeAxisX);
  this->Modified();
}


//----------------------------------------------------------------------------
void vtkVolumeResample::SetVolumeAxisY(double axis[3]) {
  if (vtkMath::Norm(axis) < 1.e-5f)
    return;

  memcpy(this->VolumeAxisY, axis, sizeof(this->VolumeAxisY));
  vtkMath::Normalize(this->VolumeAxisY);
  vtkMath::Cross(this->VolumeAxisY, this->VolumeAxisX, this->VolumeAxisZ);
  vtkMath::Normalize(this->VolumeAxisZ);
  //vtkMath::Cross(this->VolumeAxisZ, this->VolumeAxisX, this->VolumeAxisY);
  //vtkMath::Normalize(this->VolumeAxisY);
  this->Modified();
}

//----------------------------------------------------------------------------
void vtkVolumeResample::ExecuteInformation() {
  for (int i = 0; i < this->GetNumberOfOutputs(); i++) {
    if (vtkImageData::SafeDownCast(this->GetOutput(i))) {
      vtkImageData *output = (vtkImageData*)this->GetOutput(i);
      
      int dims[3];
      output->GetDimensions(dims); // this is the number of pixels in each direction...
      // modified by Marco. 25-10-2003
      // now we allow output volumes, with Z!=1 for resampling the volume
      //if (dims[2] != 1) {
      //  dims[2] = 1;
      //  output->SetDimensions(dims);
      //  }
      output->SetWholeExtent(output->GetExtent());
      output->SetUpdateExtentToWholeExtent();

      if (this->AutoSpacing) { // select spacing
        this->PrepareVolume();
        const double d = (this->VolumeAxisZ[0] * this->VolumeOrigin[0] + this->VolumeAxisZ[1] * this->VolumeOrigin[1] + this->VolumeAxisZ[2] * this->VolumeOrigin[2]);
  
        // intersect plane with the bounding box
        double spacing[3] = {1.f, 1.f, 1.f};
        double t[24][2], minT = VTK_DOUBLE_MAX, maxT = VTK_DOUBLE_MIN, minS = VTK_DOUBLE_MAX, maxS = VTK_DOUBLE_MIN;
        int    numberOfPoints = 0;
        for (int i = 0; i < 3; i++) {
          const int j = (i + 1) % 3, k = (i + 2) % 3;
          
          for (int jj = 0; jj < 2; jj++) {
            for (int kk = 0; kk < 2; kk++) {
              double p[3];
              p[j] = this->DataBounds[j][jj];
              p[k] = this->DataBounds[k][kk];
              p[i] = (d + this->VolumeAxisZ[j] * p[j] + this->VolumeAxisZ[k] * p[k]);
              
              if (fabs(this->VolumeAxisZ[i]) < 1.e-10)
                continue;
              p[i] /= this->VolumeAxisZ[i];
              if (p[i] >= this->DataBounds[i][0] && p[i] <= this->DataBounds[i][1]) {
                this->CalculateTextureCoordinates(p, (int*)dims, spacing, t[numberOfPoints]);
                if (t[numberOfPoints][0] > maxT)
                  maxT = t[numberOfPoints][0];
                if (t[numberOfPoints][0] < minT)
                  minT = t[numberOfPoints][0];
                if (t[numberOfPoints][1] > maxS)
                  maxS = t[numberOfPoints][1];
                if (t[numberOfPoints][1] < minS)
                  minS = t[numberOfPoints][1];
                numberOfPoints++; // add point
                }
              }
            }
          }
        
        // find spacing now
        double maxSpacing = max(maxS - minS, maxT - minT);
        spacing[0] = spacing[1] = spacing[3] = max(maxSpacing, 1.e-8f);
        output->SetSpacing(spacing);
        if (fabs(minT) > 1.e-3 || fabs(minS) > 1.e-3) {
          this->VolumeOrigin[0] += minT * this->VolumeAxisX[0] * dims[0] + minS * this->VolumeAxisY[0] * dims[1];
          this->VolumeOrigin[1] += minT * this->VolumeAxisX[1] * dims[0] + minS * this->VolumeAxisY[1] * dims[1];
          this->VolumeOrigin[2] += minT * this->VolumeAxisX[2] * dims[0] + minS * this->VolumeAxisY[2] * dims[1];
          this->Modified();
          }
        }
      output->SetOrigin(this->VolumeOrigin);
      }
    else {
      }
    }
  }

//----------------------------------------------------------------------------
void vtkVolumeResample::ExecuteData(vtkDataObject *outputData) {
  this->PrepareVolume();

  if (vtkImageData::SafeDownCast(outputData))
    this->ExecuteData((vtkImageData*)outputData);
  
  outputData->Modified();
  }

//----------------------------------------------------------------------------
void vtkVolumeResample::PrepareVolume() {
  assert(fabs(vtkMath::Norm(this->VolumeAxisX) - 1.f) < 1.e-5);
  assert(fabs(vtkMath::Norm(this->VolumeAxisY) - 1.f) < 1.e-5);
  vtkMath::Cross(this->VolumeAxisX, this->VolumeAxisY, this->VolumeAxisZ);
  vtkMath::Normalize(this->VolumeAxisZ);

  if (PreprocessingTime > this->GetInput()->GetMTime() && PreprocessingTime > this->GetMTime())
    return;

  vtkImageData       *imageData = vtkImageData::SafeDownCast(this->GetInput());
  vtkRectilinearGrid *gridData  = vtkRectilinearGrid::SafeDownCast(this->GetInput());

  if (imageData) {
    double dataSpacing[3];
    imageData->GetDimensions(this->DataDimensions);
    imageData->GetOrigin(this->DataOrigin);
    imageData->GetSpacing(dataSpacing);
    for (int axis = 0; axis < 3; axis++) {
      delete [] this->VoxelCoordinates[axis];
      this->VoxelCoordinates[axis] = new double [this->DataDimensions[axis] + 1];
      double f = this->DataOrigin[axis];
      for (int i = 0; i <= this->DataDimensions[axis]; i++, f += dataSpacing[axis])
        this->VoxelCoordinates[axis][i] = f;
      }
    }
  else {
    gridData->GetDimensions(this->DataDimensions);
    this->DataOrigin[0] = gridData->GetXCoordinates()->GetTuple(0)[0];
    this->DataOrigin[1] = gridData->GetYCoordinates()->GetTuple(0)[0];
    this->DataOrigin[2] = gridData->GetZCoordinates()->GetTuple(0)[0];

    for (int axis = 0; axis < 3; axis++) {
      delete [] this->VoxelCoordinates[axis];
      this->VoxelCoordinates[axis] = new double [this->DataDimensions[axis] + 1];
      
      vtkDataArray *coordinates = (axis == 2) ? gridData->GetZCoordinates() : (axis == 1 ? gridData->GetYCoordinates() : gridData->GetXCoordinates());
      const double spacing = *(coordinates->GetTuple(1)) - *(coordinates->GetTuple(0));
      const double blockSpacingThreshold = 0.01f * spacing + 0.001f;
      int i;
      for (i = 0; i < this->DataDimensions[axis]; i++) {
        this->VoxelCoordinates[axis][i] = *(coordinates->GetTuple(i));
        if (i > 0 && fabs(this->VoxelCoordinates[axis][i] - this->VoxelCoordinates[axis][i - 1] - spacing) > blockSpacingThreshold) {
          // try to correct the coordinates
          if (i < (this->DataDimensions[axis] - 1) && fabs(*(coordinates->GetTuple(i + 1)) - this->VoxelCoordinates[axis][i - 1] - 2.f * spacing) < blockSpacingThreshold) {
            this->VoxelCoordinates[axis][i]     = this->VoxelCoordinates[axis][i - 1] + spacing;
            this->VoxelCoordinates[axis][i + 1] = this->VoxelCoordinates[axis][i] + spacing;
            i++;
            }
          }
        }
      this->VoxelCoordinates[axis][i] = this->VoxelCoordinates[axis][i - 1] + (i > 2 ? (this->VoxelCoordinates[axis][i - 1] - this->VoxelCoordinates[axis][i - 2]) : 0.f);
      }
    }

  this->DataBounds[0][0] = min(this->VoxelCoordinates[0][0], this->VoxelCoordinates[0][this->DataDimensions[0] - 1]);
  this->DataBounds[0][1] = max(this->VoxelCoordinates[0][0], this->VoxelCoordinates[0][this->DataDimensions[0] - 1]);
  this->DataBounds[1][0] = min(this->VoxelCoordinates[1][0], this->VoxelCoordinates[1][this->DataDimensions[1] - 1]);
  this->DataBounds[1][1] = max(this->VoxelCoordinates[1][0], this->VoxelCoordinates[1][this->DataDimensions[1] - 1]);
  this->DataBounds[2][0] = min(this->VoxelCoordinates[2][0], this->VoxelCoordinates[2][this->DataDimensions[2] - 1]);
  this->DataBounds[2][1] = max(this->VoxelCoordinates[2][0], this->VoxelCoordinates[2][this->DataDimensions[2] - 1]);

  this->SamplingTableMultiplier[0] = SamplingTableSize / (this->DataBounds[0][1] - this->DataBounds[0][0]);
  this->SamplingTableMultiplier[1] = SamplingTableSize / (this->DataBounds[1][1] - this->DataBounds[1][0]);
  this->SamplingTableMultiplier[2] = SamplingTableSize / (this->DataBounds[2][1] - this->DataBounds[2][0]);

  this->PreprocessingTime.Modified();
}


//----------------------------------------------------------------------------
void vtkVolumeResample::ComputeInputUpdateExtents(vtkDataObject *output) {
  vtkDataObject *input = this->GetInput();
  input->SetUpdateExtentToWholeExtent();
  }


//----------------------------------------------------------------------------
void vtkVolumeResample::ExecuteData(vtkImageData *outputObject) 
{
  int extent[6];
  outputObject->GetWholeExtent(extent);
  outputObject->SetExtent(extent);
  //outputObject->SetNumberOfScalarComponents(1);
  outputObject->AllocateScalars();
  
  const void *inputPointer  = this->GetInput()->GetPointData()->GetScalars()->GetVoidPointer(0);
  const void *outputPointer = outputObject->GetPointData()->GetScalars()->GetVoidPointer(0);
  
  switch (this->GetInput()->GetPointData()->GetScalars()->GetDataType()) 
  {
    case VTK_CHAR: //---------------------------------------------
      switch (outputObject->GetPointData()->GetScalars()->GetDataType()) 
      {
        case VTK_CHAR:
          this->CreateImage((const char*)inputPointer, (char*)outputPointer, outputObject);
          break;
        case VTK_UNSIGNED_CHAR:
          this->CreateImage((const char*)inputPointer, (u_char*)outputPointer, outputObject);
          break;
        case VTK_SHORT:
          this->CreateImage((const char*)inputPointer, (short*)outputPointer, outputObject);
          break;
        case VTK_UNSIGNED_SHORT:
          this->CreateImage((const char*)inputPointer, (u_short*)outputPointer, outputObject);
          break;
        case VTK_FLOAT:
          this->CreateImage((const char*)inputPointer, (float*)outputPointer, outputObject);
          break;
        default:
          vtkErrorMacro(<< "vtkVolumeResample: Scalar type is not supported");
          return;
      }
      break;
    case VTK_UNSIGNED_CHAR: //------------------------------------
      switch (outputObject->GetPointData()->GetScalars()->GetDataType()) {
        case VTK_CHAR:
          this->CreateImage((const u_char*)inputPointer, (char*)outputPointer, outputObject);
          break;
        case VTK_UNSIGNED_CHAR:
          this->CreateImage((const u_char*)inputPointer, (u_char*)outputPointer, outputObject);
          break;
        case VTK_SHORT:
          this->CreateImage((const u_char*)inputPointer, (short*)outputPointer, outputObject);
          break;
        case VTK_UNSIGNED_SHORT:
          this->CreateImage((const u_char*)inputPointer, (u_short*)outputPointer, outputObject);
          break;
        case VTK_FLOAT:
          this->CreateImage((const u_char*)inputPointer, (float*)outputPointer, outputObject);
          break;
        default:
          vtkErrorMacro(<< "vtkVolumeResample: Scalar type is not supported");
          return;
        }
      break;
    case VTK_SHORT: //--------------------------------------------
      switch (outputObject->GetPointData()->GetScalars()->GetDataType()) {
        case VTK_CHAR:
          this->CreateImage((const short*)inputPointer, (char*)outputPointer, outputObject);
          break;
        case VTK_UNSIGNED_CHAR:
          this->CreateImage((const short*)inputPointer, (u_char*)outputPointer, outputObject);
          break;
        case VTK_SHORT:
          this->CreateImage((const short*)inputPointer, (short*)outputPointer, outputObject);
          break;
        case VTK_UNSIGNED_SHORT:
          this->CreateImage((const short*)inputPointer, (u_short*)outputPointer, outputObject);
          break;
        case VTK_FLOAT:
          this->CreateImage((const short*)inputPointer, (float*)outputPointer, outputObject);
          break;
        default:
          vtkErrorMacro(<< "vtkVolumeResample: Scalar type is not supported");
          return;
      }
      break;
    case VTK_UNSIGNED_SHORT: //-----------------------------------
      switch (outputObject->GetPointData()->GetScalars()->GetDataType()) 
      {
        case VTK_CHAR:
          this->CreateImage((const u_short*)inputPointer, (char*)outputPointer, outputObject);
          break;
        case VTK_UNSIGNED_CHAR:
          this->CreateImage((const u_short*)inputPointer, (u_char*)outputPointer, outputObject);
          break;
        case VTK_SHORT:
          this->CreateImage((const u_short*)inputPointer, (short*)outputPointer, outputObject);
          break;
        case VTK_UNSIGNED_SHORT:
          this->CreateImage((const u_short*)inputPointer, (u_short*)outputPointer, outputObject);
          break;
        case VTK_FLOAT:
          this->CreateImage((const u_short*)inputPointer, (float*)outputPointer, outputObject);
          break;
        default:
          vtkErrorMacro(<< "vtkVolumeResample: Scalar type is not supported");
          return;
      }
      break;
    case VTK_FLOAT: //--------------------------------------------
      switch (outputObject->GetPointData()->GetScalars()->GetDataType()) 
      {
        case VTK_CHAR:
          this->CreateImage((const float*)inputPointer, (char*)outputPointer, outputObject);
          break;
        case VTK_UNSIGNED_CHAR:
          this->CreateImage((const float*)inputPointer, (u_char*)outputPointer, outputObject);
          break;
        case VTK_SHORT:
          this->CreateImage((const float*)inputPointer, (short*)outputPointer, outputObject);
          break;
        case VTK_UNSIGNED_SHORT:
          this->CreateImage((const float*)inputPointer, (u_short*)outputPointer, outputObject);
          break;
        case VTK_FLOAT:
          this->CreateImage((const float*)inputPointer, (float*)outputPointer, outputObject);
          break;
        default:
          vtkErrorMacro(<< "vtkVolumeResample: Scalar type is not supported");
          return;
      }
      break;
    default:
      vtkErrorMacro(<< "vtkVolumeResample: Scalar type is not supported");
      return;
  }
}

//----------------------------------------------------------------------------
template<typename InputDataType, typename OutputDataType> void vtkVolumeResample::CreateImage(const InputDataType *input, OutputDataType *output, vtkImageData *outputObject) 
{  
  // for the progress bar 
  unsigned long count = 0; 
  unsigned long target;

  // prepare data for sampling
  int dims[3];
  outputObject->GetDimensions(dims);
  const int xs = dims[0], ys = dims[1], zs = dims[2];
  const int di = outputObject->GetNumberOfScalarComponents();
  const double dx = outputObject->GetSpacing()[0], dy = outputObject->GetSpacing()[1], dz = outputObject->GetSpacing()[2];
  const double xaxis[3] = { this->VolumeAxisX[0] * dx * this->SamplingTableMultiplier[0], this->VolumeAxisX[1] * dx * this->SamplingTableMultiplier[1], this->VolumeAxisX[2] * dx * this->SamplingTableMultiplier[2]};
  const double yaxis[3] = { this->VolumeAxisY[0] * dy * this->SamplingTableMultiplier[0], this->VolumeAxisY[1] * dy * this->SamplingTableMultiplier[1], this->VolumeAxisY[2] * dy * this->SamplingTableMultiplier[2]};
  const double zaxis[3] = { this->VolumeAxisZ[0] * dz * this->SamplingTableMultiplier[0], this->VolumeAxisZ[1] * dz * this->SamplingTableMultiplier[1], this->VolumeAxisZ[2] * dz * this->SamplingTableMultiplier[2]};

  // set the offset due to the axis transform, plus the Z slice movemnet along the Y axis
  const double offset[3] = {(this->VolumeOrigin[0] - this->DataOrigin[0]) * this->SamplingTableMultiplier[0],
                         (this->VolumeOrigin[1] - this->DataOrigin[1]) * this->SamplingTableMultiplier[1],
                         (this->VolumeOrigin[2] - this->DataOrigin[2]) * this->SamplingTableMultiplier[2]};

  // prepare sampling table
  int   *stIndices[3];
  double *stOffsets[3];

  // for the progress meter
  target = (unsigned long)((dims[3]+1)*(dims[2]+1)/50.0);
  target++;

  for (int c = 0; c < 3; c++) {
    const int indexMultiplier = (c == 0) ? 1 : (c == 1 ? this->DataDimensions[0] : (this->DataDimensions[0] * this->DataDimensions[1]));
    stIndices[c] = new int   [SamplingTableSize + 1];
    stOffsets[c] = new double [SamplingTableSize + 1];

    const double *coords = this->VoxelCoordinates[c];
    const int lastIndex = this->DataDimensions[c] - 1; // number of points in input data's "c" dimension
    const double coordToIndex = double(SamplingTableSize - 1) / (coords[lastIndex] - coords[0]);
    for (int i = 0, ti0 = 0; i <= lastIndex; i++) {
      //const int ti1 = mafVolumeResliceRound((i != lastIndex) ? (coords[i] - coords[0]) * coordToIndex : (SamplingTableSize - 1));
      const int ti1 = mafVolumeResliceFloor((i != lastIndex) ? (coords[i] - coords[0]) * coordToIndex : (SamplingTableSize));
      if (ti1 <= ti0)
        continue;
      for (int ti = ti0; ti <= ti1; ti++) {
        stIndices[c][ti] = (i - 1) * indexMultiplier; // indexes map from regular grid to input grid
        stOffsets[c][ti] = double(ti1 - ti) / double(ti1 - ti0);
        }
      ti0 = ti1;
      }

    }

  const double shift = this->Window / 2.0 - this->Level;
  const double scale = 1.0 / this->Window;

//  this->Num=0;
  OutputDataType *pixel = output;

  // this is an array of pointers for retrieving sample point and sorrounding points used for trilinear interp.
  // It stores origin mem ptr and surrouding points.
  const InputDataType *samplingPtr[8] = { input, input + 1, input + this->DataDimensions[0], input + this->DataDimensions[0] + 1,
     input + this->DataDimensions[0] * this->DataDimensions[1], input + this->DataDimensions[0] * this->DataDimensions[1] + 1, input + this->DataDimensions[0] * this->DataDimensions[1] + this->DataDimensions[0], input + this->DataDimensions[0] * this->DataDimensions[1] + this->DataDimensions[0] + 1 };

  for (int zi = 0; zi < zs; zi++) 
  {
    // compute contribution of Z slice position to the point index in the input image,
    // also summing the Origin offset

    for (int yi = 0; yi < ys; yi++) 
    {
      double p[3] = {zi * zaxis[0] + yi * yaxis[0] + offset[0], \
                    zi * zaxis[1] + yi * yaxis[1] + offset[1], \
                    zi * zaxis[2] + yi * yaxis[2] + offset[2]};
  
      if (!(count%target)) 
      {
        this->UpdateProgress(count/(50.0*target));
      }
      count++;
        
      for (int xi = 0; xi < xs; xi++, p[0] += xaxis[0], p[1] += xaxis[1], p[2] += xaxis[2], pixel += di/*, this->Num++*/) 
      {
        // find index
        //const u_int pi[3] = { u_int(p[0]), u_int(p[1]), u_int(p[2])};
        const u_int pi[3] = { mafVolumeResliceFloor(p[0]), mafVolumeResliceFloor(p[1]), mafVolumeResliceFloor(p[2])};
        //const u_int pi[3] = { mafVolumeResliceRound(p[0]), mafVolumeResliceRound(p[1]), mafVolumeResliceRound(p[2])};

        if (pi[0] > SamplingTableSize || pi[1] > SamplingTableSize || pi[2] > SamplingTableSize)
        {
          /*if (pi[0] >= SamplingTableSize)
          {
            myDBG("skip X =",pi[0]);
          }
          if (pi[1] >= SamplingTableSize)
          {
            myDBG("skip Y =",pi[1]);
          }
          if (pi[2] >= SamplingTableSize)
          {
            myDBG("skip Z =",pi[2]);
          }
          */
          
          *pixel=this->ZeroValue;
          continue;
        }

        // tri-linear interpolation

        // this is the index of the sampled point in the grid
        const int   index = stIndices[0][pi[0]] + stIndices[1][pi[1]] + stIndices[2][pi[2]];

        double sample = 0.f;
        for (int z = 0, si = 0; z < 2; z++) 
        {
          const double zweight = z ? 1.f - stOffsets[2][pi[2]] : stOffsets[2][pi[2]];
          for (int y = 0; y < 2; y++) {
            const double yzweight = (y ? 1.f - stOffsets[1][pi[1]] : stOffsets[1][pi[1]]) * zweight;
            for (int x = 0; x < 2; x++, si++) 
            {
              sample += samplingPtr[si][index] * (x ? 1.f - stOffsets[0][pi[0]] : stOffsets[0][pi[0]]) * yzweight;
            }
          }
        }
        // mapping
        //clip(((sample + shift) * scale), *pixel);
				*pixel=sample;
        //myDBG(NULL,index);
        for (int i = 1; i < di; i++)
          pixel[i] = *pixel;
      }
    }
  }

  delete [] stIndices[0];
  delete [] stIndices[1];
  delete [] stIndices[2];
  delete [] stOffsets[0];
  delete [] stOffsets[1];
  delete [] stOffsets[2];

  }


//----------------------------------------------------------------------------
void vtkVolumeResample::CalculateTextureCoordinates(const double point[3], const int size[2], const double spacing[2], double ts[2]) {
  const double c[3]  = { point[0] - this->VolumeOrigin[0], point[1] - this->VolumeOrigin[1], point[2] - this->VolumeOrigin[2] };
  
  double tx = (c[0] * VolumeAxisY[1] - c[1] * VolumeAxisY[0]) / (VolumeAxisX[0] * VolumeAxisY[1] - VolumeAxisX[1] * VolumeAxisY[0]);
  double ty = (c[0] * VolumeAxisX[1] - c[1] * VolumeAxisX[0]) / (VolumeAxisX[0] * VolumeAxisY[1] - VolumeAxisX[1] * VolumeAxisY[0]);
  if (fabs(VolumeAxisX[0] * VolumeAxisY[1] - VolumeAxisX[1] * VolumeAxisY[0]) < 1.e-10f) {
    tx = (c[0] * VolumeAxisY[2] - c[2] * VolumeAxisY[0]) / (VolumeAxisX[0] * VolumeAxisY[2] - VolumeAxisX[2] * VolumeAxisY[0]);
    ty = (c[0] * VolumeAxisX[2] - c[2] * VolumeAxisX[0]) / (VolumeAxisX[0] * VolumeAxisY[2] - VolumeAxisX[2] * VolumeAxisY[0]);
    }
  ts[0] =  tx / (size[0] * spacing[0]);
  ts[1] = -ty / (size[1] * spacing[1]);
  }
