/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAVolumeSlicer
 Authors: Alexander Savenko, Josef Kohout (major change), Gianluigi Crimi
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/


#include "vtkALBAVolumeSlicer.h"
#include "vtkObjectFactory.h"
#include "vtkRectilinearGrid.h"
#include "vtkCellArray.h"
#include "vtkDoubleArray.h"
#include "vtkPointData.h"
#include "vtkLinearTransform.h"
#include "vtkMath.h"
#include "assert.h"
#include "albaGPU3DTextureProviderHelper.h"

vtkStandardNewMacro(vtkALBAVolumeSlicer);
vtkCxxRevisionMacro(vtkALBAVolumeSlicer, "$Revision: 1.1.2.9 $");

typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned int u_int;

#define min(x0, x1) (((x0) < (x1)) ? (x0) : (x1))
#define max(x0, x1) (((x0) > (x1)) ? (x0) : (x1))

//Defines the finest of sampling
static const int SamplingTableSize = 64000;

//----------------------------------------------------------------------------
// Constructor sets default values
vtkALBAVolumeSlicer::vtkALBAVolumeSlicer() 
//----------------------------------------------------------------------------
{
	PlaneOrigin[0] = PlaneOrigin[1] = PlaneOrigin[2] = 0.f;
  PlaneAxisX[0] = 1.f;
  PlaneAxisX[1] = PlaneAxisX[2] = 0.f;
  PlaneAxisY[0] = PlaneAxisY[2] = 0.f;
  PlaneAxisY[1] = 1.f;

  GlobalPlaneOrigin[0] = GlobalPlaneOrigin[1] = GlobalPlaneOrigin[2] = 0.f;
  GlobalPlaneAxisX[0] = 1.f;
  GlobalPlaneAxisX[1] = GlobalPlaneAxisX[2] = 0.f;
  GlobalPlaneAxisY[0] = GlobalPlaneAxisY[2] = 0.f;
  GlobalPlaneAxisY[1]  = 1.f;
	GlobalPlaneAxisZ[0] = GlobalPlaneAxisZ[1] = 0.f;
	GlobalPlaneAxisZ[2] = 1.f;

  TransformSlice = NULL;

	Window = Level = 0;
 
  this->AutoSpacing = 1;    //Autospacing is enabled by the default
  this->LastGPUEnabled = this->GPUEnabled = 1;     //GPU is enabled by the default

  for (int i = 0; i < 3; i++)
  {
    this->StIndices[i] = NULL;
    this->StOffsets[i] = NULL;
		DataOrigin[i]=0;
		DataBounds[i][0] = DataBounds[i][1] = 0;
		DataDimensions[3] = 0;
		SamplingTableMultiplier[3] = 0;
  }  

  LastPreprocessedInput = NULL; 
  BNoIntersection = false;

#ifdef _WIN32
  m_bGPUProcessing = false;
#endif

  m_TriLinearInterpolationOn = true;

	m_TextureHelper = new albaGPU3DTextureProviderHelper();
}
//----------------------------------------------------------------------------
vtkALBAVolumeSlicer::~vtkALBAVolumeSlicer() 
//----------------------------------------------------------------------------
{
  for (int i = 0; i < 3; i++)
  {
    delete[] this->StIndices[i];
    delete[] this->StOffsets[i];
  }

#ifdef _WIN32
	delete m_TextureHelper;
#endif
}
//----------------------------------------------------------------------------
void vtkALBAVolumeSlicer::SetPlaneAxisX(float axis[3]) 
//----------------------------------------------------------------------------
{
  if (vtkMath::Norm(axis) < 1.e-5f)
    return;
  memcpy(this->PlaneAxisX, axis, sizeof(this->PlaneAxisX));
  vtkMath::Normalize(this->PlaneAxisX);
  if (TransformSlice)
  {
    TransformSlice->TransformNormal(PlaneAxisX, GlobalPlaneAxisX);
  }
  else
  {
    memcpy(GlobalPlaneAxisX, PlaneAxisX, sizeof(this->PlaneAxisX));
  }
  this->Modified();
}
//----------------------------------------------------------------------------
void vtkALBAVolumeSlicer::SetPlaneAxisY(float axis[3]) 
//----------------------------------------------------------------------------
{
  if (vtkMath::Norm(axis) < 1.e-5f)
    return;
  memcpy(this->PlaneAxisY, axis, sizeof(this->PlaneAxisY));
  vtkMath::Normalize(this->PlaneAxisY);
  vtkMath::Cross(this->PlaneAxisY, this->PlaneAxisX, this->PlaneAxisZ);
  vtkMath::Normalize(this->PlaneAxisZ);
  vtkMath::Cross(this->PlaneAxisZ, this->PlaneAxisX, this->PlaneAxisY);
  vtkMath::Normalize(this->PlaneAxisY);
  if (TransformSlice)
  {
    TransformSlice->TransformNormal(PlaneAxisY, GlobalPlaneAxisY);
  }
  else
  {
    memcpy(GlobalPlaneAxisY, PlaneAxisY, sizeof(this->PlaneAxisY));
  }
  this->Modified();
}
//----------------------------------------------------------------------------
void vtkALBAVolumeSlicer::SetPlaneOrigin(double origin[3])
//----------------------------------------------------------------------------
{
	memcpy(PlaneOrigin, origin, sizeof(PlaneOrigin));
  if (TransformSlice)
  {
    TransformSlice->TransformPoint(PlaneOrigin, GlobalPlaneOrigin);
  }
  else
  {
    memcpy(GlobalPlaneOrigin, PlaneOrigin, sizeof(PlaneOrigin));
  }
  this->Modified();
}
//----------------------------------------------------------------------------
void vtkALBAVolumeSlicer::SetPlaneOrigin(double x, double y, double z)
//----------------------------------------------------------------------------
{
  double plane_origin[3];
  plane_origin[0] = x;
  plane_origin[1] = y;
  plane_origin[2] = z;
  SetPlaneOrigin(plane_origin);
}

//----------------------------------------------------------------------------
//Return this object's modified time.
/*virtual*/unsigned long int vtkALBAVolumeSlicer::GetMTime() 
//----------------------------------------------------------------------------
{
  unsigned long int time = Superclass::GetMTime();
  if (this->TransformSlice != NULL && this->TransformSlice->GetMTime() > time)
    time = this->TransformSlice->GetMTime();
  return time;
}

//----------------------------------------------------------------------------
//By default copy the output update extent to the input.
void vtkALBAVolumeSlicer::ComputeInputUpdateExtents(vtkDataObject *output) 
//----------------------------------------------------------------------------
{
  vtkDataObject *input = this->GetInput();
	if(input)
		input->SetUpdateExtentToWholeExtent();
}

//----------------------------------------------------------------------------
//By default, UpdateInformation calls this method to copy information
//unmodified from the input to the output.
/*virtual*/ void vtkALBAVolumeSlicer::ExecuteInformation() 
//----------------------------------------------------------------------------
{
  vtkDataSet* input;
  if ((input = GetInput()) == NULL || this->GetNumberOfOutputs() == 0)
    return; //nothing to cut, or we have no output -> exit
  
  this->NumComponents = input->GetPointData()->GetScalars()->GetNumberOfComponents();  

  //first, perform transformation of Plane
  if (TransformSlice)
  {
    TransformSlice->TransformPoint(PlaneOrigin, GlobalPlaneOrigin);
    TransformSlice->TransformNormal(PlaneAxisX, GlobalPlaneAxisX);
    TransformSlice->TransformNormal(PlaneAxisY, GlobalPlaneAxisY);
  }
  else
  {
    memcpy(GlobalPlaneOrigin, PlaneOrigin, sizeof(PlaneOrigin));
    memcpy(GlobalPlaneAxisX, PlaneAxisX, sizeof(PlaneAxisX));
    memcpy(GlobalPlaneAxisY, PlaneAxisY, sizeof(PlaneAxisY));
  }

  assert(fabs(vtkMath::Norm(this->GlobalPlaneAxisX) - 1.f) < 1.e-5);
  assert(fabs(vtkMath::Norm(this->GlobalPlaneAxisY) - 1.f) < 1.e-5);

  //compute normal for the plane determined by PlaneAxisX and PlaneAxisY
  vtkMath::Cross(this->GlobalPlaneAxisX, this->GlobalPlaneAxisY, this->GlobalPlaneAxisZ);
  vtkMath::Normalize(this->GlobalPlaneAxisZ);

  //now copy the important information from input, e.g., bounding box
  vtkImageData* imageData = vtkImageData::SafeDownCast(input);  
  if (imageData != NULL) 
  {
    //regular grid, it is a bit easier    
    imageData->GetDimensions(this->DataDimensions);
    imageData->GetOrigin(this->DataOrigin);

    double dataSpacing[3];
    imageData->GetSpacing(dataSpacing);
    for (int i = 0; i < 3; i++)
    {
      dataSpacing[i] *= (this->DataDimensions[i] - 1);
      this->DataBounds[i][0] = this->DataOrigin[i];
      this->DataBounds[i][1] = this->DataOrigin[i] + dataSpacing[i];
    } 
  }
  else
  {
    vtkRectilinearGrid* gridData = vtkRectilinearGrid::SafeDownCast(input);
    if (gridData == NULL)
    {
      vtkDebugMacro("Invalid input for vtkALBAVolumeSlicer");      
      return;
    }

    //rectilinear grid
    gridData->GetDimensions(this->DataDimensions);
    this->DataOrigin[0] = gridData->GetXCoordinates()->GetTuple(0)[0];
    this->DataOrigin[1] = gridData->GetYCoordinates()->GetTuple(0)[0];
    this->DataOrigin[2] = gridData->GetZCoordinates()->GetTuple(0)[0];

    this->DataBounds[0][0] = min(this->DataOrigin[0], 
      gridData->GetXCoordinates()->GetTuple(this->DataDimensions[0] - 1)[0]);
    this->DataBounds[0][1] = max(this->DataOrigin[0], 
      gridData->GetXCoordinates()->GetTuple(this->DataDimensions[0] - 1)[0]);

    this->DataBounds[1][0] = min(this->DataOrigin[1], 
      gridData->GetYCoordinates()->GetTuple(this->DataDimensions[1] - 1)[0]);
    this->DataBounds[1][1] = max(this->DataOrigin[1], 
      gridData->GetYCoordinates()->GetTuple(this->DataDimensions[1] - 1)[0]);

    this->DataBounds[2][0] = min(this->DataOrigin[2], 
      gridData->GetZCoordinates()->GetTuple(this->DataDimensions[2] - 1)[0]);
    this->DataBounds[2][1] = max(this->DataOrigin[2], 
      gridData->GetZCoordinates()->GetTuple(this->DataDimensions[2] - 1)[0]);
  }

  for (int i = 0; i < this->GetNumberOfOutputs(); i++) 
  {
    vtkImageData* output = vtkImageData::SafeDownCast(this->GetOutput(i));
    if (output != NULL) 
    {            
      int dims[3];
      output->GetDimensions(dims);
      if (dims[2] != 1) 
      {        
        dims[2] = 1;  //force it to be 2D
        output->SetDimensions(dims);
      }
      output->SetWholeExtent(output->GetExtent());
      output->SetUpdateExtentToWholeExtent();


      //if the cut should fill the whole output, we will need to get intersections
      if (this->AutoSpacing)
      { 
        //intersect the cutting plane ax + by + cz + d = 0, where (a,b,c) is normal GlobalPlaneAxisZ
        //and d can is computed so the plane goes through GlobalPlaneOrigin with the bounding box
        const float d = -(this->GlobalPlaneAxisZ[0] * this->GlobalPlaneOrigin[0] + 
          this->GlobalPlaneAxisZ[1] * this->GlobalPlaneOrigin[1] + 
          this->GlobalPlaneAxisZ[2] * this->GlobalPlaneOrigin[2]);

        //set initial spacing to 1,1,1
        double spacing[3] = {1.0f, 1.0f, 1.0f};
        float minT = VTK_FLOAT_MAX, maxT = VTK_FLOAT_MIN, minS = VTK_FLOAT_MAX, maxS = VTK_FLOAT_MIN;
        int numberOfPoints = 0;

        //bounding box is symmetric => we will compute intersections
        //for every rotation of the coordinate system [i, j, k]
        //i.e., for [0,1,2], [1,2,0], [2,0,1], i.e., [x,y,z],[y,z,x],[z,x,y]
        for (int i = 0; i < 3; i++) 
        {
          //check if the i-axis is not parallel to the plane
          if (fabs(this->GlobalPlaneAxisZ[i]) < 1.e-10)
            continue; //there is no intersection => continue

          //i-axis is the major direction where we want to compute intersections
          //the box has 4 edges parallel to this axis, 2 of them intersect j-axis and 2 k-axis,
          //thus we will compute P[i, 0, 0], P[i, 0, bbox corner on k-axis], 
          //P[i, bbox corner on j-axis, 0] and P[i, bbox corner on j-axis, bbox corner on k-axis]
          const int j = (i + 1) % 3, k = (i + 2) % 3;       
          for (int jj = 0; jj < 2; jj++) 
          {
            for (int kk = 0; kk < 2; kk++) 
            {
              //compute intersection of the bounding box edge denoted by i, j, k coordinate system and
              //the index (jj, kk) with the cutting plane
              float p[3];
              p[j] = this->DataBounds[j][jj];
              p[k] = this->DataBounds[k][kk];
              p[i] = -(d + this->GlobalPlaneAxisZ[j] * p[j] + this->GlobalPlaneAxisZ[k] * p[k]) / 
                this->GlobalPlaneAxisZ[i];

              // check that p[i] is in inside the box
              float dbi0 = ((float)(this->DataBounds[i][0]));//Added by Losi 07.15.2009:	Bug #1721 fix
              float dbi1 = ((float)(this->DataBounds[i][1]));
              //Bug #1721: No image at the boundary box extremes
              //http://bugzilla.hpc.cineca.it/show_bug.cgi?id=1721
              //cause: float, double comparsion
              if (p[i] < dbi0 || p[i] > dbi1 && (p[i] != dbi0 && p[i] != dbi1))
              {
                continue; //the supporting line intersects the plane but the edge does not
              }
              numberOfPoints++;    //some intersection detected

              float ts[2];
              this->CalculateTextureCoordinates(p, (int*)dims, spacing, ts);
              if (ts[0] > maxT)
                maxT = ts[0];
              if (ts[0] < minT)
                minT = ts[0];
              if (ts[1] > maxS)
                maxS = ts[1];
              if (ts[1] < minS)
                minS = ts[1];            
            }
          }
        }

        //RELEASE NOTE: we have 3 numberOfPoints if the plane cuts or touches one corner. The latter one
        //is not considered to be an intersection, however, it is a singular case that we will not distinguish
        if (BNoIntersection = (numberOfPoints <= 2))
          output->SetSpacing(spacing);  //spacing will be 1:1:1
        else
        {
          // find spacing now
          float maxSpacing = max(maxS - minS, maxT - minT);
          spacing[0] = spacing[1] = max(maxSpacing, 1.e-8f);
          output->SetSpacing(spacing);
          // http://bugzilla.hpc.cineca.it/show_bug.cgi?id=1427
          // Totally heuristic bug fix: magicNumber was 1.e-3 before.
          const float magicNumber = 1.e-5;
          if (fabs(minT) > magicNumber || fabs(minS) > magicNumber) 
          {
            this->GlobalPlaneOrigin[0] += minT * this->GlobalPlaneAxisX[0] * dims[0] + minS * this->GlobalPlaneAxisY[0] * dims[1];
            this->GlobalPlaneOrigin[1] += minT * this->GlobalPlaneAxisX[1] * dims[0] + minS * this->GlobalPlaneAxisY[1] * dims[1];
            this->GlobalPlaneOrigin[2] += minT * this->GlobalPlaneAxisX[2] * dims[0] + minS * this->GlobalPlaneAxisY[2] * dims[1];
            this->Modified();
          }
        }
      }

      //if !AutoSpacing, we will use the original spacing used there
      output->SetOrigin(this->GlobalPlaneOrigin);
    }
  }
}

//------------------------------------------------------------------------
//BES: 15.12.2008 - when using albaOpCrop in albaViewOrthoSlice, the input
//dimensions change between ExecuteInformation and ExecuteData 
//This routine is supposed to be called from ExecuteData and it fixes this problem
void vtkALBAVolumeSlicer::ExecuteDataHotFix(vtkDataObject *outputData)
//------------------------------------------------------------------------
{
  vtkDataSet* input = this->GetInput(); 
  vtkImageData* output = vtkImageData::SafeDownCast(outputData);
  if (input == NULL || output == NULL)
    return;


  int dims[3];
  vtkImageData* imgData = vtkImageData::SafeDownCast(input);
  if (imgData != NULL)
    imgData->GetDimensions(dims);
  else
  {
    vtkRectilinearGrid* gridData = vtkRectilinearGrid::SafeDownCast(input);
    if (gridData == NULL)
      return; //unsupported

    gridData->GetDimensions(dims);        
  }

  if (
    dims[0] != this->DataDimensions[0] || 
    dims[1] != this->DataDimensions[1] ||
    dims[2] != this->DataDimensions[2]
  )
  {
    //The previous execution of ExecuteInformation left Extent to be -1
    //this would cause troubles during the reexecution of ExecuteInformation
    int extent[6];      
    output->GetWholeExtent(extent);
    output->SetExtent(extent);

    LastPreprocessedInput = NULL; //to force PrepareVolume to reexecute
    vtkALBAVolumeSlicer::ExecuteInformation();                  
  }
}

//----------------------------------------------------------------------------
//This method is the one that should be used by subclasses, right now the 
//default implementation is to call the backwards compatibility method
/*virtual*/ void vtkALBAVolumeSlicer::ExecuteData(vtkDataObject *outputData) 
//----------------------------------------------------------------------------
{
  //BES: 15.12.2008 - when using albaOpCrop in albaViewOrthoSlice, the input
  //dimensions change between ExecuteInformation and ExecuteData
  ExecuteDataHotFix(outputData); 

  if (vtkImageData::SafeDownCast(outputData) != NULL)
    this->ExecuteData((vtkImageData*)outputData);
  else if (vtkPolyData::SafeDownCast(outputData) != NULL)
    this->ExecuteData((vtkPolyData*)outputData);
  
  outputData->Modified();
}

//----------------------------------------------------------------------------
// Create geometry for the slice
/*virtual*/ void vtkALBAVolumeSlicer::ExecuteData(vtkPolyData *output) 
//----------------------------------------------------------------------------
{
  output->Reset();

  //get the cutting plane (from associated texture)
  vtkImageData* texture = this->GetTexture();
  if (texture != NULL) 
  {
    texture->Update();
    memcpy(this->GlobalPlaneOrigin, texture->GetOrigin(), sizeof(this->GlobalPlaneOrigin));
  }
  else
  {
    vtkErrorMacro(<<"No texture specified");
    return;
  }

  //plane: ax + by + cz + d = 0, where (a,b,c) is plane normal, which is stored in GlobalPlaneAxisZ
  //plane goes through the origin => compute d
  const float d = -(this->GlobalPlaneAxisZ[0] * this->GlobalPlaneOrigin[0] + 
    this->GlobalPlaneAxisZ[1] * this->GlobalPlaneOrigin[1] + 
    this->GlobalPlaneAxisZ[2] * this->GlobalPlaneOrigin[2]);

  //intersect plane with the bounding box
  //we will have at most one intersection per one box edge
  float points[12][3];
  int   numberOfPoints = 0;
  
  //bounding box is symmetric => we will compute intersections
  //for every rotation of the coordinate system [i, j, k]
  //i.e., for [0,1,2], [1,2,0], [2,0,1], i.e., [x,y,z],[y,z,x],[z,x,y]
  for (int i = 0; i < 3; i++) 
  {
    //check if the i-axis is not parallel to the plane
    if (fabs(this->GlobalPlaneAxisZ[i]) < 1.e-10)
      continue; //there is no intersection => continue
    
    //i-axis is the major direction where we want to compute intersections
    //the box has 4 edges parallel to this axis, 2 of them intersect j-axis and 2 k-axis,
    //thus we will compute P[i, 0, 0], P[i, 0, bbox corner on k-axis], 
    //P[i, bbox corner on j-axis, 0] and P[i, bbox corner on j-axis, bbox corner on k-axis]
    const int j = (i + 1) % 3, k = (i + 2) % 3;
    for (int jj = 0; jj < 2; jj++) 
    {
      for (int kk = 0; kk < 2; kk++) 
      {
        //compute intersection of the bounding box edge denoted by i, j, k coordinate system and
        //the index (jj, kk) with the cutting plane
        float (&p)[3] = points[numberOfPoints];
        p[j] = this->DataBounds[j][jj];
        p[k] = this->DataBounds[k][kk];
        p[i] = -(d + this->GlobalPlaneAxisZ[j] * p[j] + this->GlobalPlaneAxisZ[k] * p[k]) / 
          this->GlobalPlaneAxisZ[i];

        // check that p[i] is in inside the box
        float dbi0 = ((float)(this->DataBounds[i][0]));//Added by Losi 07.15.2009:	Bug #1721 fix
        float dbi1 = ((float)(this->DataBounds[i][1]));
        //Bug #1721: No image at the boundary box extremes
        //http://bugzilla.hpc.cineca.it/show_bug.cgi?id=1721
        //cause: float, double comparsion
        if ((p[i] < dbi0 || p[i] > dbi1) && (p[i] != dbi0 && p[i] != dbi1))
        {
          continue; //the supporting line intersects the plane but the edge does not
        }
        // add point
        numberOfPoints++;
      }
    }
  }  

  //if the cutting plane touches the bounding box in one of its corner (it is computed 3x),
  //remove this singular case
  if (numberOfPoints == 3)
  {      
    if (vtkMath::Distance2BetweenPoints(points[0], points[1]) < 1.e-10)
      return;  //p and points[ii] are the same
  }

  if (numberOfPoints <= 2) {
    assert(numberOfPoints == 0);
    return;   //not enough points
  }

  // find image parameters for texture mapping
  double spacing[3];
  int size[2];
  int extent[6];

  assert(texture->GetSource() != this);
  texture->UpdateInformation();
  texture->GetWholeExtent(extent);
  if (extent[0] >= extent[1])
    texture->GetExtent(extent);
  size[0] = extent[1] - extent[0] + 1;
  size[1] = extent[3] - extent[2] + 1;
  texture->GetSpacing(spacing);

  //prepare VTK points buffer
  vtkPoints* pointsObj = output->GetPoints();
  if (pointsObj != NULL) 
    pointsObj->Reset();
  else
  {
    pointsObj = vtkPoints::New();
    pointsObj->Allocate(numberOfPoints, 1);
    output->SetPoints(pointsObj);
    pointsObj->Delete();
  }  

  //insert points
  for (int i = 0; i < numberOfPoints; i++) {
    pointsObj->InsertNextPoint(points[i]);
  }

  //prepare VTK texture coordinates buffer
  vtkDoubleArray* tsObj = vtkDoubleArray::SafeDownCast(output->GetPointData()->GetTCoords());
  if (tsObj != NULL)
    tsObj->Reset();
  else
  {    
    tsObj = vtkDoubleArray::New();
    tsObj->SetNumberOfComponents(2);  
    tsObj->Allocate(2 * numberOfPoints, 1);
    output->GetPointData()->SetTCoords(tsObj);
    tsObj->Delete();
  }

  //insert texture coordinates
  if (size[0] > 0 && size[1] > 0)
  {    
    for (int i = 0; i < numberOfPoints; i++) 
    {
      float ts[2];
      this->CalculateTextureCoordinates(points[i], size, spacing, ts);
      tsObj->InsertNextTuple(ts);
    }
  }

  //create a clockwise oriented convex polygon from the detected points  
  //we will use a modified version of Jarvis' March for convex hull computation. 
  //See: Jarvis, R. A. (1973). "On the identification of the convex hull of 
  //a finite set of points in the plane". Information Processing Letters 2: 18–21
  //http://en.wikipedia.org/wiki/Gift_wrapping_algorithm    
  vtkCellArray *polys = vtkCellArray::New();
  polys->Allocate(polys->EstimateSize(1, numberOfPoints));
  
  vtkIdType pointIds[12];
  bool processedPoints[12];  
  
  //We will start with a supporting line going through the first point (points[0])
  //and the point that is the most distant (i.e., this line is our extreme line)
  pointIds[0] = 0;  
  processedPoints[0] = true;

  double maxVectorNorm = 0.0;
  int longestVector = 1;
  for (int i = 1; i < numberOfPoints; i++) 
  {
    points[i][0] -= points[0][0];
    points[i][1] -= points[0][1];
    points[i][2] -= points[0][2];

    double norm = vtkMath::Norm(points[i]);
    if (norm > maxVectorNorm) 
    {
      maxVectorNorm = norm;
      longestVector = i;
    }
    vtkMath::Normalize(points[i]);
    processedPoints[i] = false;
  }    
	
  //we have now unit vectors in points array and we know our extreme line
  //while the convex hull is not finished (in our case all points lie on the hull)
  for (int i = 1; i < numberOfPoints; i++) 
  {    
		int nextPoint = 0;
    double minSAngle = 99999.0;

    //locate still not processed point j such that the CCW angle p_longestVector,p0, pj is smallest
		for (int j = 1; j < numberOfPoints; j++) 
    {
      if (processedPoints[j])
        continue;   //this point is already processed

      //we will exploit the cross product feature: a x b = |a|*|b|*sin(alpha)*n, where alpha is the angle
      //between vectors a and b; as our vectors (computed in previous step) are unit ones,
      //their cross product give sin(alpha)*n, thus if alpha is 0 degrees (j == longestVector),
      //we get 0*n, 45 degrees -> sqrt(2)/2*n, 90 degrees -> 1*n, 135 -> sqrt(2)/2*n, 
      //180 -> 0*n, 225 -> -sqrt(2)/2*n, 270 -> -1*n, 315 -> -sqrt(2)/2*n
			float angleVector[3];
			vtkMath::Cross(points[longestVector], points[j], angleVector);  //for j==longestVector norm is 0.0

      //vtkMath::Norm(angleVector) gives abs(sin(alpha))*|n|
      //thus additional correction is necessary, for alpha in the range 180-360 degrees,
      //angleVector is opposite to GlobalPlaneAxisZ, thus using dot product we can distinguish
      //between intervals 0-180 and 180-360 
			double norm = vtkMath::Norm(angleVector);
      if (vtkMath::Dot(angleVector, this->GlobalPlaneAxisZ) > 0.0)
        norm = -norm;   //if angle(angleVector, plane normal) > 90 degrees

      //now, norm variable ranges from -|n| to |n| for angles from -90 to 90 and 
      //from -|n| to |n| for angles from 90 to 270(= -90) degrees
      //there is, however, no point in the half plane 90-270 degrees      
      //there is no ambiguity and we are ready as the smallest norm value 
      //denotes also the smallest angle (that we were looking for)
      //Proof: ????
			if (norm <= minSAngle) 
      {
				minSAngle = norm;
				nextPoint = j;
			}
    }

		pointIds[i] = nextPoint; 
		processedPoints[nextPoint] = true;
  }    
	
  polys->InsertNextCell(numberOfPoints, pointIds);
  output->SetPolys(polys);
  polys->Delete();
}

//----------------------------------------------------------------------------
//Create texture for the slice.
/*virtual*/ void vtkALBAVolumeSlicer::ExecuteData(vtkImageData *outputObject) 
//----------------------------------------------------------------------------
{
  int extent[6];
  outputObject->GetWholeExtent(extent);
  outputObject->SetExtent(extent);
  outputObject->SetNumberOfScalarComponents(this->NumComponents);
  outputObject->AllocateScalars();  
	
  if (BNoIntersection)
  {
    //there is no intersection with data => black texture
    void *outputPointer = outputObject->GetPointData()->GetScalars()->GetVoidPointer(0);
    memset(outputPointer, 0, outputObject->GetPointData()->GetScalars()->GetSize()*
      outputObject->GetPointData()->GetScalars()->GetDataTypeSize());
    return;   //we are ready
  }

  //prepare input (preprocessing)
  vtkDataSet* input = this->GetInput();
  vtkDataArray* pScalars = input->GetPointData()->GetScalars();

  const void *inputPointer  = pScalars->GetVoidPointer(0);    
  this->PrepareVolume(input, outputObject);
	 
  switch (pScalars->GetDataType()) 
  {
  case VTK_CHAR:
    this->CreateImage((const char*)inputPointer, outputObject); 
	break;
  case VTK_UNSIGNED_CHAR:
    this->CreateImage((const unsigned char*)inputPointer, outputObject); 
	break;
  case VTK_SHORT:
    this->CreateImage((const short*)inputPointer, outputObject); 
	break;
	case VTK_UNSIGNED_SHORT:
    this->CreateImage((const unsigned short*)inputPointer, outputObject); 
	break;
	case VTK_INT:
		this->CreateImage((const int*)inputPointer, outputObject);
	break;
	case VTK_UNSIGNED_INT:
		this->CreateImage((const unsigned int*)inputPointer, outputObject);
	break;
	case VTK_FLOAT:
    this->CreateImage((const float*)inputPointer, outputObject); 
	break;
  case VTK_DOUBLE:  //NOTE: GPU is not allowed
    this->CreateImage((const double*)inputPointer, outputObject); 
	break;
  
  default:
    vtkErrorMacro(<< "vtkALBAVolumeSlicer: Scalar type is not supported");
    return;
  }
}

//----------------------------------------------------------------------------
//Calculates voxel coordinates and volume bounding box.
//This routine is called from ExecuteInformation to compute coordinates of voxels in the volume
//and to detect the bounding box - both used in ExecuteData. It also computes normal for the cutting 
//plane. NB: if input has not changed since the last call, the coordinates are not recalculated. 
/*virtual*/ void vtkALBAVolumeSlicer::PrepareVolume(vtkDataSet* input, vtkImageData* output) 
//----------------------------------------------------------------------------
{
  if (LastPreprocessedInput == input && PreprocessingTime > input->GetMTime() && LastGPUEnabled == GPUEnabled)
    return; //input is the same (and also kind of output) => return now

  m_bGPUProcessing = LastGPUEnabled = GPUEnabled;
  
  //compute coordinates in x, y and z-axis for the volume (defined as 
  //a regular grid - vtkImageData or rectilinear grid - vtkRectilinearGrid  
  vtkImageData* imageData = vtkImageData::SafeDownCast(input);  
  vtkRectilinearGrid* gridData = vtkRectilinearGrid::SafeDownCast(input);
  if (imageData != NULL) 
    PrepareVolume(imageData, output);
  else
    PrepareVolume(gridData, output);

  LastPreprocessedInput = input;
  this->PreprocessingTime.Modified();
}

//------------------------------------------------------------------------
/*virtual*/ void vtkALBAVolumeSlicer::PrepareVolume(vtkImageData* input, vtkImageData* output)
{  
#ifdef _WIN32
  if (GPUEnabled)
		m_bGPUProcessing = m_TextureHelper->InitializeTexture(input,output);

  if (!m_bGPUProcessing)
  {      
#endif
    //GPU is not available => we will use CPU
    double dataSpacing[3];
    input->GetSpacing(dataSpacing);

    float* voxelcoords[3];
    for (int axis = 0; axis < 3; axis++)
    {      
      voxelcoords[axis] = new float [this->DataDimensions[axis] + 1];

      for (int i = 0; i <= this->DataDimensions[axis]; i++)
        voxelcoords[axis][i] = i*dataSpacing[axis];       
    }

    CreateSamplingTable(voxelcoords);
    for (int axis = 0; axis < 3; axis++) {
      delete[] voxelcoords[axis];
    }
#ifdef _WIN32
  }
#endif
}


//------------------------------------------------------------------------
/*virtual*/ void vtkALBAVolumeSlicer::PrepareVolume(vtkRectilinearGrid* input, vtkImageData* output)
{  
  //rectilinear grid
#ifdef _WIN32
  m_bGPUProcessing = false; //GPU is not allowed for rectilinear grid
#endif

  float* voxelcoords[3];
  for (int axis = 0; axis < 3; axis++) 
  {      
    voxelcoords[axis] = new float [this->DataDimensions[axis] + 1];

    vtkDataArray *coordinates = (axis == 2) ? input->GetZCoordinates() : 
      (axis == 1 ? input->GetYCoordinates() : input->GetXCoordinates());

    //BES: I do not understand this Savchenko's code
    //if it is here to avoid aliasing, then it is not general enough as spacing is ill denoted
    const float spacing = *(coordinates->GetTuple(1)) - *(coordinates->GetTuple(0));
    const float blockSpacingThreshold = 0.01f * spacing + 0.001f;
    int i;
    for (i = 0; i < this->DataDimensions[axis]; i++) 
    {
      voxelcoords[axis][i] = *(coordinates->GetTuple(i));
      if (i > 0 && fabs(voxelcoords[axis][i] - voxelcoords[axis][i - 1] - spacing) > blockSpacingThreshold) 
      {
        // try to correct the coordinates
        if (i < (this->DataDimensions[axis] - 1) && 
          fabs(*(coordinates->GetTuple(i + 1)) - voxelcoords[axis][i - 1] - 2.f * spacing) < blockSpacingThreshold) 
        {
          voxelcoords[axis][i]     = voxelcoords[axis][i - 1] + spacing;
          voxelcoords[axis][i + 1] = voxelcoords[axis][i] + spacing;
          i++;
        }
      }
    }
    voxelcoords[axis][i] = voxelcoords[axis][i - 1] + 
      (i > 2 ? (voxelcoords[axis][i - 1] - voxelcoords[axis][i - 2]) : 0.f);

    //BES: the construction of sample table is faster if voxelcoords are 0 aligned
    for (i = this->DataDimensions[axis]; i >= 0; i--) {
      voxelcoords[axis][i] -= voxelcoords[axis][0];
    }
  } //end for

  CreateSamplingTable(voxelcoords);
  for (int axis = 0; axis < 3; axis++) {
    delete[] voxelcoords[axis];
  }
}

//------------------------------------------------------------------------
//Creates sampling table for the given coordinates 
//NB. coordinates must be zero aligned (i.e., the first coordinate must be 0,0,0)*/
void vtkALBAVolumeSlicer::CreateSamplingTable(float* voxelcoords[3])
//------------------------------------------------------------------------
{
  //coefficient storing number of samples / volume size => samples per one mm
  this->SamplingTableMultiplier[0] = SamplingTableSize / (this->DataBounds[0][1] - this->DataBounds[0][0]);
  this->SamplingTableMultiplier[1] = SamplingTableSize / (this->DataBounds[1][1] - this->DataBounds[1][0]);
  this->SamplingTableMultiplier[2] = SamplingTableSize / (this->DataBounds[2][1] - this->DataBounds[2][0]);

  //prepare sampling table
  //the volume is sampled using a very fine sample rate (to avoid aliasing)
  //we will prepare tables to map sample index to voxel data
  if (StIndices[0] == NULL)
  {
    for (int i = 0; i < 3; i++)
    {
      StIndices[i] = new int   [SamplingTableSize + 1];
      StOffsets[i] = new float [SamplingTableSize + 1];
    }
  }

  int increments[3];
  increments[0] = this->NumComponents;
  increments[1] = increments[0]*this->DataDimensions[0];
  increments[2] = increments[1]*this->DataDimensions[1];

  for (int c = 0; c < 3; c++) 
  {
    //indexMultiplier = number of elements in one voxel, line or slice
    const int indexMultiplier = increments[c];           

    const float* coords = voxelcoords[c];
    const int lastIndex = this->DataDimensions[c] - 1;
    const float coordToIndex = float(SamplingTableSize - 1) / coords[lastIndex];

    for (int i = 0, ti0 = 0, idxCur = -indexMultiplier; i <= lastIndex; i++, idxCur += indexMultiplier) 
    {
      const int ti1 = (i != lastIndex) ? coords[i] * coordToIndex : (SamplingTableSize);
      if (ti1 <= ti0)
        continue; //this should not happen for typical data

      //samples with indices ti0 to ti1 will be mapped to voxel with index idxCur
      //and will have offsets going from 1 to 0 (uniformly)
      int nRange = ti1 - ti0;
      float fDist = 1.0f / nRange;

      int* stIndices = &StIndices[c][ti0];
      float* stOffsets = &StOffsets[c][ti0];

      for (int j = 0; j <= nRange; j++) 
      {
        stIndices[j] = idxCur;
        stOffsets[j] = (nRange - j)*fDist;
        //j*fDist;
      }

      ti0 = ti1;
    }
  }  
}


//----------------------------------------------------------------------------
template<typename InputDataType> 
void vtkALBAVolumeSlicer::CreateImage(const InputDataType *inputPointer, vtkImageData *outputObject) 
{
 #ifdef _WIN32
	if (m_bGPUProcessing)
	{
		m_TextureHelper->CreateImage(outputObject, GlobalPlaneAxisX, GlobalPlaneAxisY, GlobalPlaneAxisZ, GlobalPlaneOrigin);
	}
	else
	{
#endif
	vtkDataArray* pScalars = outputObject->GetPointData()->GetScalars();
	void *outputPointer = pScalars->GetVoidPointer(0);

	switch (pScalars->GetDataType())
		{
			case VTK_CHAR:
				this->CreateImage(inputPointer, (char*)outputPointer, outputObject);
				break;
			case VTK_UNSIGNED_CHAR:
				this->CreateImage(inputPointer, (unsigned char*)outputPointer, outputObject);
				break;
			case VTK_SHORT:
				this->CreateImage(inputPointer, (short*)outputPointer, outputObject);
				break;
			case VTK_UNSIGNED_SHORT:
				this->CreateImage(inputPointer, (unsigned short*)outputPointer, outputObject);
				break;
			case VTK_INT:
				this->CreateImage(inputPointer, (int*)outputPointer, outputObject);
				break;
			case VTK_UNSIGNED_INT:
				this->CreateImage(inputPointer, (unsigned int*)outputPointer, outputObject);
				break;
			case VTK_FLOAT:
				this->CreateImage(inputPointer, (float*)outputPointer, outputObject);
				break;
			case VTK_DOUBLE:
				this->CreateImage(inputPointer, (double*)outputPointer, outputObject);
				break;

			default:
				vtkErrorMacro(<< "vtkALBAVolumeSlicer: Scalar type is not supported");
				return;
		}

#ifdef _WIN32
	}
#endif
}

//----------------------------------------------------------------------------
//Slices voxels from input producing image in output.
template<typename InputDataType, typename OutputDataType> 
void vtkALBAVolumeSlicer::CreateImage(const InputDataType *input, OutputDataType *output, vtkImageData *outputObject) 
//----------------------------------------------------------------------------
{
  // prepare data for sampling  
  const int xs = outputObject->GetDimensions()[0], ys = outputObject->GetDimensions()[1];
  const float dx = outputObject->GetSpacing()[0], dy = outputObject->GetSpacing()[1];
  const int numComp = this->NumComponents;  

  assert(this->NumComponents == outputObject->GetNumberOfScalarComponents());

  //the first pixel of the texture, i.e., the pixel at [0,0], has coordinates GlobalPlaneOrigin
  //the pixel [i+1,j] has coordinates equal to the coordinates of the pixel [i,j] + 
  //GlobalPlaneAxisX*dx; the pixel [i, j+1] has coordinates equal to the coordinates 
  //of the pixel [i,j] + GlobalPlaneAxisY*dy
  
  //if the scene is translated so the first voxel has coordinates [0.0, 0.0, 0.0]
  //the first pixel has coordinates GlobalPlaneOrigin - DataOrigin
  //as there is SamplingTableMultiplier samples per one mm, pixel coordinates to 
  //sample indices can be simply obtained using these variables:
  const float xaxis[3] = { 
    this->GlobalPlaneAxisX[0] * dx * this->SamplingTableMultiplier[0], 
    this->GlobalPlaneAxisX[1] * dx * this->SamplingTableMultiplier[1], 
    this->GlobalPlaneAxisX[2] * dx * this->SamplingTableMultiplier[2]};

  const float yaxis[3] = { 
    this->GlobalPlaneAxisY[0] * dy * this->SamplingTableMultiplier[0], 
    this->GlobalPlaneAxisY[1] * dy * this->SamplingTableMultiplier[1], 
    this->GlobalPlaneAxisY[2] * dy * this->SamplingTableMultiplier[2]};

  const float offset[3] = {
    (this->GlobalPlaneOrigin[0] - this->DataOrigin[0]) * this->SamplingTableMultiplier[0],
    (this->GlobalPlaneOrigin[1] - this->DataOrigin[1]) * this->SamplingTableMultiplier[1],
    (this->GlobalPlaneOrigin[2] - this->DataOrigin[2]) * this->SamplingTableMultiplier[2]};
      
  memset(output, 0, sizeof(OutputDataType) * xs * ys * numComp);
  OutputDataType* pixel = output;
  int samplingOffs[8] = { 
    0,    //voxel [i,j,k]
    1,    //voxel [i + 1,j,k]
    this->DataDimensions[0],    //voxel [i,j+1,k]
    this->DataDimensions[0] + 1,//voxel [i+1,j+1,k]
		this->DataDimensions[0] * this->DataDimensions[1],      //voxel [i,j,k+1]
    this->DataDimensions[0] * this->DataDimensions[1] + 1,  //voxel [i+1,j,k+1]
    this->DataDimensions[0] * this->DataDimensions[1] + this->DataDimensions[0],    //voxel [i,j+1,k+1]
    this->DataDimensions[0] * this->DataDimensions[1] + this->DataDimensions[0] + 1 //voxel [i+1,j+1,k+1]
  };

  for (int i = 1; i < 8; i++) {
    samplingOffs[i] *= numComp;
  }
    
  //process every pixel in the output texture
  float pl[3] = { offset[0], offset[1], offset[2] };

  for (int yi = 0; yi < ys; yi++, 
    pl[0] += yaxis[0], pl[1] += yaxis[1], pl[2] += yaxis[2]) 
  {
    //all pixels in this line has this base
    float p[3] = { pl[0], pl[1], pl[2] };
    for (int xi = 0; xi < xs; xi++, 
      p[0] += xaxis[0], p[1] += xaxis[1], p[2] += xaxis[2], pixel += numComp) 
    {
      //round p to integers
      const unsigned int pi[3] = { u_int(p[0]), u_int(p[1]), u_int(p[2])};
      if (pi[0] > SamplingTableSize || pi[1] > SamplingTableSize || pi[2] > SamplingTableSize)
        continue;   //point is outside the volume

      //get the voxel where the current pixel lies
      int index = StIndices[0][pi[0]] + StIndices[1][pi[1]] + StIndices[2][pi[2]];
      for (int comp = 0; comp < numComp; comp++) 
      {
        double sample = 0.0;
        if(m_TriLinearInterpolationOn)
        {
          // tri-linear interpolation
          
          for (int z = 0, si = 0; z < 2; z++) 
          {
            const double zweight = z ? 1.f - StOffsets[2][pi[2]] : StOffsets[2][pi[2]];
            for (int y = 0; y < 2; y++) 
            {
              const double yzweight = (y ? 1.f - StOffsets[1][pi[1]] : StOffsets[1][pi[1]]) * zweight;
              for (int x = 0; x < 2; x++, si++)
                sample += input[samplingOffs[si] + index + comp] * 
                (x ? 1.f - StOffsets[0][pi[0]] : StOffsets[0][pi[0]]) * yzweight;
            }          
          }
        }
        else
        {
          sample = input[index + comp];
        }

        pixel[comp] = sample;
      }
    }	
  }

}

//----------------------------------------------------------------------------
//Calculates the coordinates for the given point and texture denoted by its size and spacing.
//Texture is considered to have an origin at GlobalPlaneOrigin, to be oriented according to GlobalPlaneAxisX
//and GlobalPlaneAxisY and to cover area of size*spacing mm. Actually, this routine computes intersection of
//the line going through the given point and having vector GlobalPlaneAxisY with the line going through
//the origin of texture and having vector GlobalPlaneAxisY. The computed times are stored in ts.
void vtkALBAVolumeSlicer::CalculateTextureCoordinates(const float point[3], const int size[2], const double spacing[2], float ts[2]) 
//----------------------------------------------------------------------------
{ 
  //BES: 28.3.2008 - Completely rewritten - BUG fix

  //the first line p: X = Origin + AxisX*t
  //the second line q: X = Point + AxisY*s
  //intersection at [t, s]: Point - Origin = AxisY*s - AxisX*t
  //we have three equations for two variables t, s, however, one is linearly dependent
  int iX = 0, iY = 1;
  
  float dens[3], den;
  den = dens[0] = GlobalPlaneAxisX[0]*GlobalPlaneAxisY[1] - GlobalPlaneAxisY[0]*GlobalPlaneAxisX[1];
  
  dens[1] = GlobalPlaneAxisX[0]*GlobalPlaneAxisY[2] - GlobalPlaneAxisY[0]*GlobalPlaneAxisX[2];
  if (fabs(dens[1]) > fabs(den)) {
    den = dens[1]; iY = 2;
  }

  dens[2] = GlobalPlaneAxisX[1]*GlobalPlaneAxisY[2] - GlobalPlaneAxisY[1]*GlobalPlaneAxisX[2];
  if (fabs(dens[2]) > fabs(den)) {
    den = dens[2]; iX = 1; iY = 2;
  }

  assert(fabs(den) >= 1e-10);
  
  //we will use iX and iY only
  float cX = point[iX] - GlobalPlaneOrigin[iX];
  float cY = point[iY] - GlobalPlaneOrigin[iY];  

  //now we can compute t, s
  float tx = (cX*GlobalPlaneAxisY[iY] - cY*GlobalPlaneAxisY[iX]) / den;
  float ty = (cX*GlobalPlaneAxisX[iY] - cY*GlobalPlaneAxisX[iX]) / den;

  //both GlobalPlaneAxisX and GlobalPlaneAxisY are unit vectors, i.e., tx and ty are texture coordinates
  //for texture covering area 1x1 => map coordinates to the current texture area
  ts[0] =  tx / (size[0] * spacing[0]);
  ts[1] = -ty / (size[1] * spacing[1]);
}


//----------------------------------------------------------------------------
void vtkALBAVolumeSlicer::SetSliceTransform(vtkLinearTransform *trans)
//----------------------------------------------------------------------------
{
  TransformSlice = trans;
  Modified();
}



//----------------------------------------------------------------------------
void vtkALBAVolumeSlicer::SetGPUEnabled(int enable)
//----------------------------------------------------------------------------
{
	if (enable == GPUEnabled)
		return;

  GPUEnabled = enable;
  if (!enable)
  {
#ifdef _WIN32
		m_TextureHelper->UnregisterTexture();
#endif
  }

  Modified();

}