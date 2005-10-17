/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkDistanceFilter.cxx,v $
  Language:  C++
  Date:      $Date: 2005-10-17 13:09:55 $
  Version:   $Revision: 1.4 $

=========================================================================*/

#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkRectilinearGrid.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkFloatArray.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkMath.h"

#include "vtkDistanceFilter.h"

#include "assert.h"


vtkCxxRevisionMacro(vtkDistanceFilter, "$Revision: 1.4 $");
vtkStandardNewMacro(vtkDistanceFilter);

#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))
//template<typename type> static inline type clip(type x, type xmin, type xmax) { if (x < xmin) return xmin; if (x > xmax) return xmax; return x; }
template<typename type> static inline void clip(type &x, type xmin, type xmax) { if (x < xmin) x = xmin; else if (x > xmax) x = xmax; }

//----------------------------------------------------------------------------
// Constructor sets default values
vtkDistanceFilter::vtkDistanceFilter() {
  this->Threshold = 0;
  this->MaxDistance = 10;
  this->DistanceMode = VTK_SCALAR;
  this->FilterMode   = VTK_DISTANCE_MODE;
  this->OutOfBoundsDensity = VTK_FLOAT_MIN;

  this->InputMatrix = NULL;
  this->InputTransform = NULL;

  // caches
  this->UniformToRectGridIndex[0] = this->UniformToRectGridIndex[1] = this->UniformToRectGridIndex[2] = NULL;
  this->VoxelSizes[0] = this->VoxelSizes[1] = this->VoxelSizes[2] = NULL;
  }


//----------------------------------------------------------------------------
vtkDistanceFilter::~vtkDistanceFilter() {
  this->SetSource((vtkDataSet *)NULL);

  if (this->InputTransform)
    this->InputTransform->Delete();
  if (this->InputMatrix)
    this->InputMatrix->Delete();
  delete [] VoxelSizes[0];
  delete [] this->UniformToRectGridIndex[0];
  }

//----------------------------------------------------------------------------
void vtkDistanceFilter::SetSource(vtkDataSet *data)
//----------------------------------------------------------------------------
{
  this->SetNthInput(1, (vtkDataObject*)data);
}

//----------------------------------------------------------------------------
vtkDataSet *vtkDistanceFilter::GetSource()
//----------------------------------------------------------------------------
{
  if(this->GetNumberOfInputs() < 2)
    return NULL;
  return (vtkDataSet *)(this->Inputs[1]);
}
//----------------------------------------------------------------------------
unsigned long int vtkDistanceFilter::GetMTime() {
  unsigned long int time = this->MTime;
  if (this->GetSource() && this->GetSource()->GetMTime() > time)
    time = this->GetSource()->GetMTime();
  if (this->GetInput() && this->GetInput()->GetMTime() > time)
    time = this->GetInput()->GetMTime();
  if (this->InputMatrix && this->InputMatrix->GetMTime() > time)
    time = this->InputMatrix->GetMTime();
  if (this->InputTransform && this->InputTransform->GetMTime() > time)
    time = this->InputTransform->GetMTime();
  return time;
  }

//----------------------------------------------------------------------------
void vtkDistanceFilter::ComputeInputUpdateExtents(vtkDataObject *output) {
  vtkDataObject *source = this->GetSource();
  if (source)
    source->SetUpdateExtentToWholeExtent();
  }

//----------------------------------------------------------------------------
void vtkDistanceFilter::ExecuteInformation() {
  }


//----------------------------------------------------------------------------
void vtkDistanceFilter::ExecuteData(vtkDataObject *outputObject) {
  vtkImageData       *imageData = vtkImageData::SafeDownCast(this->GetSource());
  vtkRectilinearGrid *gridData  = vtkRectilinearGrid::SafeDownCast(this->GetSource());
  vtkPointSet   *input  = vtkPointSet::SafeDownCast(this->GetInput());
  vtkPointSet   *output = vtkPointSet::SafeDownCast(outputObject);

  if (input == NULL || output == NULL || imageData == NULL && gridData == NULL) {
    vtkErrorMacro(<< "Input or output is incorrect");
    return;
    }
  if (input->GetPointData()->GetNormals() == NULL && this->FilterMode == VTK_DISTANCE_MODE) {
    vtkErrorMacro(<<"No normals for input");
    return;
    }

  // prepare data for calculations
  this->PrepareVolume();
  output->CopyStructure(input);

  const int numPts = input->GetNumberOfPoints();
  vtkPoints     * const points  = input->GetPoints();
  vtkDataArray  * const normals = input->GetPointData()->GetNormals();
  vtkFloatArray * const scalars = this->DistanceMode == VTK_SCALAR || this->FilterMode == VTK_DENSITY_MODE ? vtkFloatArray::New(): NULL;
  vtkFloatArray * const vectors = scalars == NULL ? vtkFloatArray::New(): NULL;
  if (scalars) {
    scalars->SetNumberOfComponents(1);
    scalars->SetNumberOfTuples(numPts);
    output->GetPointData()->SetScalars(scalars);
    scalars->UnRegister(NULL);
    }
  else {
    vectors->SetNumberOfComponents(3);
    vectors->SetNumberOfTuples(numPts);
    output->GetPointData()->SetVectors(vectors);
    vectors->UnRegister(NULL);
    }
  output->GetPointData()->SetNormals(normals);
  
  const int dataType             = this->GetSource()->GetPointData()->GetScalars()->GetDataType();
  const void * const DataPointer = this->GetSource()->GetPointData()->GetScalars()->GetVoidPointer(0);

  // process the data
  if (this->FilterMode == VTK_DISTANCE_MODE) {
    for (int pi = 0; pi < numPts; pi++) {
      double point[4], normal[4];
      points->GetPoint(pi, point);
      normals->GetTuple(pi, normal);
      
      if(this->InputTransform)
        this->SetInputMatrix(this->InputTransform->GetMatrix());

      if (this->InputMatrix) { // transform the point into the world coordinates
        normal[0] += point[0];
        normal[1] += point[1];
        normal[2] += point[2];
        normal[3] = 1.f;
        point[3] = 1.f;
        
        this->InputMatrix->MultiplyPoint(point, point);
        float norm = (fabs(point[3]) > 0.0001) ? (1.f / point[3]) : 1.f;
        point[0] *= norm;
        point[1] *= norm;
        point[2] *= norm;
        
        this->InputMatrix->MultiplyPoint(normal, normal);
        norm = (fabs(normal[3]) > 0.0001f) ? (1.f / normal[3]) : 1.f;
        normal[0] = normal[0] * norm - point[0];
        normal[1] = normal[1] * norm - point[1];
        normal[2] = normal[2] * norm - point[2];
        vtkMath::Normalize(normal);
        }
      
      float distance = 0;
      switch (dataType) {
        case VTK_UNSIGNED_SHORT: distance = this->TraceRay(point, normal, (const unsigned short*)DataPointer); break;
        case VTK_SHORT:          distance = this->TraceRay(point, normal, (const short*)DataPointer); break;
        case VTK_UNSIGNED_CHAR:  distance = this->TraceRay(point, normal, (const unsigned char*)DataPointer); break;
        case VTK_CHAR:           distance = this->TraceRay(point, normal, (const char*)DataPointer); break;
        case VTK_FLOAT:          distance = this->TraceRay(point, normal, (const float*)DataPointer); break;
        }
      if (scalars)
        scalars->SetTuple1(pi, distance);
      else
        vectors->SetTuple3(pi, distance * normal[0], distance * normal[1], distance * normal[2]);
      }
    }
  else { // density mode
    for (int pi = 0; pi < numPts; pi++) {
      double point[4];
      points->GetPoint(pi, point);
     
      if(this->InputTransform)
        this->SetInputMatrix(this->InputTransform->GetMatrix());

      if (this->InputMatrix) {
        point[3] = 1.f;
        this->InputMatrix->MultiplyPoint(point, point);
        float norm = (fabs(point[3]) > 0.0001) ? (1.f / point[3]) : 1.f;
        point[0] *= norm;
        point[1] *= norm;
        point[2] *= norm;
        }
      
      float density = 0;
      switch (dataType) {
        case VTK_UNSIGNED_SHORT: density = this->FindDensity(point, (const unsigned short*)DataPointer); break;
        case VTK_SHORT:          density = this->FindDensity(point, (const short*)DataPointer); break;
        case VTK_UNSIGNED_CHAR:  density = this->FindDensity(point, (const unsigned char*)DataPointer); break;
        case VTK_CHAR:           density = this->FindDensity(point, (const char*)DataPointer); break;
        case VTK_FLOAT:          density = this->FindDensity(point, (const float*)DataPointer); break;
        }
      if (scalars)
        scalars->SetTuple1(pi, density);
      }
    
    }
  }


//--------------------------------------------------------------------------------------
template<typename DataType> double vtkDistanceFilter::TraceRay(const double origin[3], const double ray[3], const DataType *dataPointer) {
  // find intersection between volume and ray
  // this code can be removed if out-of-bounds points are ignored (tmin is always 0)
  static const double maxD = 1.e20f;
  double xyz[3], dxyz[3];
  double tmin = VTK_FLOAT_MIN, tmax = VTK_FLOAT_MAX;
  assert(fabs(vtkMath::Norm(ray) - 1.) < 1.e-5);
  int i;
  for (i = 0; i < 3; i++) {
    const double idistance = 1.f / ray[i];
    dxyz[i] = fabs(idistance);
    if (fabs(idistance) > maxD) {
      dxyz[i] = maxD;
      continue;
      }
    
    const double t0 = (this->DataBounds[i << 1] - origin[i]) * idistance;
    const double t1 = (this->DataBounds[(i << 1) + 1] - origin[i]) * idistance;
    if (t0 > t1)
      tmin = max(tmin, t1), tmax = min(tmax, t0);
    else
      tmin = max(tmin, t0), tmax = min(tmax, t1);
    }
  tmin = max(tmin, 0.f);
  if (tmin > tmax || tmin > this->MaxDistance)
    return this->MaxDistance; // no intersection with the volume or MaxDistance threshold is exceeded

  // convert point to index
  for (i = 0; i < 3; i++) {
    int uIndex = int((tmin * ray[i] + origin[i] - this->DataBounds[i << 1]) * this->UniformToRectGridMultiplier[i]);
    clip(uIndex, 0, this->UniformToRectGridMaxIndex[i]);
    xyz[i] = this->UniformToRectGridIndex[i][uIndex];
    }
  
  // prepare for traversing
  int vi[3] = {int(xyz[0]), int(xyz[1]), int(xyz[2])};
  dataPointer += vi[0] + this->DataDimensions[0] * (vi[1] + vi[2] * this->DataDimensions[1]);
  
  const int    inc[3] = {ray[0] > 0 ? 1 : -1, ray[1] > 0 ? 1 : -1, ray[2] > 0 ? 1 : -1};
  const int ptrInc[3] = { inc[0], inc[1] * this->DataDimensions[0], inc[2] * this->DataDimensions[0] * this->DataDimensions[1]};
  const unsigned int maxIndex[3] = {(unsigned)this->DataDimensions[0] - 1, (unsigned)this->DataDimensions[1] - 1, (unsigned)this->DataDimensions[2] - 1};
  
  double l[3] = {dxyz[0] * (inc[0] > 0 ? ((vi[0] + inc[0]) - xyz[0]) : (xyz[0] - vi[0])) * this->VoxelSizes[0][vi[0]],
                dxyz[1] * (inc[1] > 0 ? ((vi[1] + inc[1]) - xyz[1]) : (xyz[1] - vi[1])) * this->VoxelSizes[1][vi[1]],
                dxyz[2] * (inc[2] > 0 ? ((vi[2] + inc[2]) - xyz[2]) : (xyz[2] - vi[2])) * this->VoxelSizes[2][vi[2]]};
  
  
  //---------------- actual traverse
  const DataType Threshold = (DataType)this->Threshold;
  //modified by STEFY 25-6-2004(begin)
  //double distance = 0; // traversed distance
  // inizializing the distance with a negative value, so to have also negative distances (compenetration)
  double distance = -(this->MaxDistance + 1);
  //modified by STEFY 25-6-2004(end)
  for ( ; (distance < this->MaxDistance) && *dataPointer < Threshold; ) {
    const int ii = (l[0] <= l[1] && l[0] <= l[2]) ? 0 : ((l[1] <= l[2]) ? 1 : 2);
    distance = l[ii];
    
    vi[ii] += inc[ii];
    if ((unsigned)vi[ii] > maxIndex[ii])
      return this->MaxDistance;
    
    dataPointer += ptrInc[ii];
    if (*dataPointer > Threshold)
      break;
    
    l[ii] += dxyz[ii] * this->VoxelSizes[ii][vi[ii]];
    } // for ( ; ; )
  
  return distance;
  }



//------------------------------------------------------------------------
template<typename DataType> double vtkDistanceFilter::FindDensity(const double point[3], const DataType *dataPointer) {
  vtkRectilinearGrid *gridData  = vtkRectilinearGrid::SafeDownCast(this->GetSource());

  double density = 0.f, xyz[3], dxyz[3];
  int ixyz[3];
  // convert point to index
  for (int i = 0; i < 3; i++) {
    int uIndex = int((point[i] - this->DataBounds[i << 1]) * this->UniformToRectGridMultiplier[i]);
    if (uIndex < 0 || uIndex > this->UniformToRectGridMaxIndex[i])
      return this->OutOfBoundsDensity;
    xyz[i] = this->UniformToRectGridIndex[i][uIndex];
    ixyz[i] = int(xyz[i]);
    dxyz[i] = xyz[i] - double(ixyz[i]);
    }

  int vi[3] = {int(xyz[0]), int(xyz[1]), int(xyz[2])};
  dataPointer += vi[0] + this->DataDimensions[0] * (vi[1] + vi[2] * this->DataDimensions[1]);

  // tri-linear interpolation
  for (int z = 0, si = 0; z < 2; z++) {
    const double zweight = z ? 1.f - dxyz[2] : dxyz[2];
    for (int y = 0; y < 2; y++) {
      const double yzweight = (y ? 1.f - dxyz[1] : dxyz[1]) * zweight;
      for (int x = 0; x < 2; x++, si++) {
        density += dataPointer[this->InterpolationOffsets[si]] * (x ? 1.f - dxyz[0] : dxyz[0]) * yzweight;
        }
      }
    }

  return density;
  }

//--------------------------------------------------------------
void vtkDistanceFilter::PrepareVolume() {
  if (this->GetSource()->GetMTime() < this->BuildTime && this->VoxelSizes[0] != NULL)
    return; // caches are up-to-date
  vtkImageData       *imageData = vtkImageData::SafeDownCast(this->GetSource());
  vtkRectilinearGrid *gridData  = vtkRectilinearGrid::SafeDownCast(this->GetSource());

  if (imageData)
    imageData->GetDimensions(this->DataDimensions);
  else
    gridData->GetDimensions(this->DataDimensions);

  delete [] this->VoxelSizes[0];
  delete [] this->UniformToRectGridIndex[0];
  this->UniformToRectGridMaxIndex[0] = min(this->DataDimensions[0] * 50, 16000);
  this->UniformToRectGridMaxIndex[1] = min(this->DataDimensions[1] * 50, 16000);
  this->UniformToRectGridMaxIndex[2] = min(this->DataDimensions[2] * 50, 16000);

  for (int axis = 0; axis < 3; axis++) {
    // allocate data
    this->VoxelSizes[axis] = axis == 0 ? new float [this->DataDimensions[0] + this->DataDimensions[1] + this->DataDimensions[2]] :
                                         this->VoxelSizes[axis - 1] + this->DataDimensions[axis - 1];
    this->UniformToRectGridIndex[axis] = axis == 0 ? new float [this->UniformToRectGridMaxIndex[0] + this->UniformToRectGridMaxIndex[1] + this->UniformToRectGridMaxIndex[2] + 3] :
                                      this->UniformToRectGridIndex[axis - 1] + this->UniformToRectGridMaxIndex[axis - 1] + 1;
    
    if (imageData) {
      const float spacing = imageData->GetSpacing()[axis], origin = imageData->GetOrigin()[axis];
      int i;
      for (i = 0; i < this->DataDimensions[axis]; i++)
        this->VoxelSizes[axis][i] = spacing;
      this->VoxelSizes[axis][this->DataDimensions[axis] - 1] = 0;
      for (i = 0; i <= this->UniformToRectGridMaxIndex[axis]; i++)
        this->UniformToRectGridIndex[axis][i] = float(i) / this->UniformToRectGridMaxIndex[axis] * (this->DataDimensions[axis] - 1);
      for (float fs = 1.e-10f; this->UniformToRectGridIndex[axis][i - 1] >= (this->DataDimensions[axis] - 1); fs *= 2.f)
        this->UniformToRectGridIndex[axis][i - 1] -= fs;
      this->UniformToRectGridMultiplier[axis] = float(this->UniformToRectGridMaxIndex[axis]) / (spacing * (this->DataDimensions[axis] - 1));
      this->DataBounds[axis << 1] = origin;
      this->DataBounds[(axis << 1) + 1] = origin + spacing * this->DataDimensions[axis];
      }
    else {
      vtkDataArray *coordinates = (axis == 2) ? gridData->GetZCoordinates() : (axis == 1 ? gridData->GetYCoordinates() : gridData->GetXCoordinates());
      const float origin = *(coordinates->GetTuple(0));
      this->UniformToRectGridMultiplier[axis] = float(this->UniformToRectGridMaxIndex[axis]) / (*(coordinates->GetTuple(this->DataDimensions[axis] - 1)) - origin);
      int i,aj;
      for (i = 0, aj = 0; i < (this->DataDimensions[axis] - 1); i++) {
        float a = *(coordinates->GetTuple(i)), b = *(coordinates->GetTuple(i + 1));
        this->VoxelSizes[axis][i] = b - a;
        int bj = int((b - origin) * this->UniformToRectGridMultiplier[axis]);
        bj = min(bj, this->UniformToRectGridMaxIndex[axis]);
        const float dj = 1.f / float(bj - aj);
        for (int j = aj; j < bj; j++)
          this->UniformToRectGridIndex[axis][j] = i + dj * (j - aj);
        aj = bj;
        }
      for ( ; aj <= this->UniformToRectGridMaxIndex[axis]; aj++)
        this->UniformToRectGridIndex[axis][aj] = this->DataDimensions[axis] - 1;
      for (aj--; this->UniformToRectGridIndex[axis][aj] >= this->DataDimensions[axis] - 1 && aj > 0; aj--) {
        for (float fs = 1.e-10f; this->UniformToRectGridIndex[axis][aj] >= (this->DataDimensions[axis] - 1); fs *= 2.f)
          this->UniformToRectGridIndex[axis][aj] -= fs;
        }

      this->VoxelSizes[axis][this->DataDimensions[axis] - 1] = 0;

      this->DataBounds[axis << 1] = origin;
      this->DataBounds[(axis << 1) + 1] = *(coordinates->GetTuple(this->DataDimensions[axis] - 1));

      }
    }

  this->InterpolationOffsets[0] = 0;
  this->InterpolationOffsets[1] = 1;
  this->InterpolationOffsets[2] = this->DataDimensions[0];
  this->InterpolationOffsets[3] = this->DataDimensions[0] + 1;
  this->InterpolationOffsets[4] = this->InterpolationOffsets[0] + this->DataDimensions[0] * this->DataDimensions[1];
  this->InterpolationOffsets[5] = this->InterpolationOffsets[1] + this->DataDimensions[0] * this->DataDimensions[1];
  this->InterpolationOffsets[6] = this->InterpolationOffsets[2] + this->DataDimensions[0] * this->DataDimensions[1];
  this->InterpolationOffsets[7] = this->InterpolationOffsets[3] + this->DataDimensions[0] * this->DataDimensions[1];

  this->BuildTime.Modified();
  }



