/*=========================================================================

 Program: ALBA (Agile Library for Biomedical Applications)
 Module: vtkALBAPointCloudCutter
 Authors: Nigel McFarlane
 
 Copyright (c) BIC
 All rights reserved. See Copyright.txt or


 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkALBAPointCloudCutter.h"
#include "vtkObjectFactory.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCell.h"
#include "vtkCellArray.h"
#include "vtkCellData.h"
#include "vtkIndent.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPlane.h"
#include "vtkPolyData.h"
#include "vtkIdList.h"
#include "vtkMatrix4x4.h"

#include <ostream>
#include "albaDefines.h"
#include "vtkTransform.h"



//------------------------------------------------------------------------------
// standard macros
vtkStandardNewMacro(vtkALBAPointCloudCutter);

//------------------------------------------------------------------------------
#include "albaMemDbg.h"
#include "vtkALBASmartPointer.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"

//------------------------------------------------------------------------------
// Constructor
vtkALBAPointCloudCutter::vtkALBAPointCloudCutter()
//------------------------------------------------------------------------------
{
  CutFunction = NULL ;
  InPolydata = NULL ;
  CutTranformedNormal[0] = CutTranformedNormal[1] = CutTranformedNormal[2] = CutTranformedOrigin[0] = CutTranformedOrigin[1] = CutTranformedOrigin[2] = 0;
  CutTranformedNormal[3] = CutTranformedOrigin[3] = 1; //for point multiplication purpose 
  PlaneTolerance = 0.05;
}

//------------------------------------------------------------------------------
// Destructor
vtkALBAPointCloudCutter::~vtkALBAPointCloudCutter()
//------------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
// Overload standard modified time function. If cut function is modified,
// then this object is modified as well.
vtkMTimeType vtkALBAPointCloudCutter::GetMTime()
//------------------------------------------------------------------------------
{
  vtkMTimeType mTime = this->vtkAlgorithm::GetMTime();
  vtkMTimeType time;

  if (CutFunction != NULL )
  {
    time = CutFunction->GetMTime();
    mTime = ( time > mTime ? time : mTime );
  }

  return mTime;
}

//------------------------------------------------------------------------------
// Execute method
int  vtkALBAPointCloudCutter::RequestData(vtkInformation *vtkNotUsed(request), vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
	// get the info objects
	vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
	vtkInformation *outInfo = outputVector->GetInformationObject(0);

	// Initialize some frequently used values.
  InPolydata = vtkPolyData::SafeDownCast(inInfo->Get(vtkDataObject::DATA_OBJECT()));
  OutPolydata = vtkPolyData::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));
  
  // Make sure the cutter is cleared of previous data before you run it !
  Initialize() ;
    
  // Run the cutter
  CreateSlice();

  return 1;
}

//------------------------------------------------------------------------------
// Set cutting plane
void vtkALBAPointCloudCutter::SetCutFunction(vtkPlane *P)
{
  CutFunction = P ;
}

//------------------------------------------------------------------------------
// Get cutting plane
vtkPlane* vtkALBAPointCloudCutter::GetCutFunction()
{
  return CutFunction ;
}


//----------------------------------------------------------------------------
void vtkALBAPointCloudCutter::Initialize()
{
  // make sure the output is empty
	OutPolydata->Initialize();
}

//------------------------------------------------------------------------
//computes the intersection of the given plane with the mesh bounding box
//returns coordinates of the first intersection in pts, returns false, if the bounding box is not intersected
bool vtkALBAPointCloudCutter::GetIntersectionOfBoundsWithPlane(const double *origin, 
                                                            const double *norm)
{
  double DataBounds[3][2];
	double pts[3];
  InPolydata->GetBounds(&DataBounds[0][0]);  

  //intersect the cutting plane ax + by + cz + d = 0, where (a,b,c) is normal GlobalPlaneAxisZ
  //and d can is computed so the plane goes through GlobalPlaneOrigin with the bounding box
  const double d = -(norm[0] * origin[0] + norm[1] * origin[1] + norm[2] * origin[2]);  

  //bounding box is symmetric => we will compute intersections
  //for every rotation of the coordinate system [i, j, k]
  //i.e., for [0,1,2], [1,2,0], [2,0,1], i.e., [x,y,z],[y,z,x],[z,x,y]
  for (int i = 0; i < 3; i++) 
  {
    //check if the i-axis is not parallel to the plane
    if (fabs(norm[i]) < 1.e-10)
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
        pts[j] = DataBounds[j][jj];
        pts[k] = DataBounds[k][kk];
        pts[i] = -(d + norm[j] * pts[j] + norm[k] * pts[k]) / norm[i];

        // check that p[i] is in inside the box, with tolerance applied
        if (pts[i] < DataBounds[i][0] - PlaneTolerance || pts[i] > DataBounds[i][1] + PlaneTolerance)
          continue; //the supporting line intersects the plane but the edge does not

        return true;
      }
    }
  }

  return false;
}


//----------------------------------------------------------------------------
void vtkALBAPointCloudCutter::SlicePoints()
{
	if (!CutFunction)
		return;

  PointsMapping.clear();

  vtkPoints* outPoints = vtkPoints::New();
  vtkCellArray* outVerts = vtkCellArray::New();
  
  vtkIdType npts = InPolydata->GetNumberOfPoints();
  
  // Normalize the normal vector
  double normalLength = sqrt(CutTranformedNormal[0] * CutTranformedNormal[0] + 
                             CutTranformedNormal[1] * CutTranformedNormal[1] + 
                             CutTranformedNormal[2] * CutTranformedNormal[2]);
  
  double normalizedNormal[3];
  if (normalLength > 1.e-10)
  {
    normalizedNormal[0] = CutTranformedNormal[0] / normalLength;
    normalizedNormal[1] = CutTranformedNormal[1] / normalLength;
    normalizedNormal[2] = CutTranformedNormal[2] / normalLength;
  }
  else
  {
    return;
  }

	
  // Loop through all input points
	if (GetIntersectionOfBoundsWithPlane(CutTranformedOrigin, CutTranformedNormal))
    for (vtkIdType i = 0; i < npts; i++)
    {
      double point[3];
      InPolydata->GetPoint(i, point);

      // Calculate vector from plane origin to point
      double toPoint[3];
      toPoint[0] = point[0] - CutTranformedOrigin[0];
      toPoint[1] = point[1] - CutTranformedOrigin[1];
      toPoint[2] = point[2] - CutTranformedOrigin[2];

      // Calculate signed distance from point to plane
      double distance = toPoint[0] * normalizedNormal[0] +
        toPoint[1] * normalizedNormal[1] +
        toPoint[2] * normalizedNormal[2];

      // Check if distance is within tolerance
      if (fabs(distance) < PlaneTolerance)
      {
        // Project point onto plane
        double projectedPoint[3];
        projectedPoint[0] = point[0] - distance * normalizedNormal[0];
        projectedPoint[1] = point[1] - distance * normalizedNormal[1];
        projectedPoint[2] = point[2] - distance * normalizedNormal[2];

        // Add projected point to output
        vtkIdType outId = outPoints->InsertNextPoint(projectedPoint);

        // Add vertex cell
        outVerts->InsertNextCell(1, &outId);

        // Store mapping for scalars transfer
        PointsMapping.push_back(i);
      }
    }

  OutPolydata->SetPoints(outPoints);
  OutPolydata->SetVerts(outVerts);
  
  outPoints->Delete();
  outVerts->Delete();
}

//------------------------------------------------------------------------------
// Transfer the scalars by interpolation from input to output
void vtkALBAPointCloudCutter::TransferScalars()
//------------------------------------------------------------------------------
{
  vtkIdType i, j ;
  double tuple0[100], tuple1[100] ;
  int ncomp[100], dtype[100] ;

  // copy the structure of the point scalar arrays
  OutPolydata->GetPointData()->CopyStructure(InPolydata->GetPointData()) ;

  // allocate tuples for every point
  if (OutPolydata->GetPoints() == NULL)
    return;

  int npts = OutPolydata->GetPoints()->GetNumberOfPoints() ;
  OutPolydata->GetPointData()->SetNumberOfTuples(npts) ;

  // get no. of arrays, components and types
  int narrays = OutPolydata->GetPointData()->GetNumberOfArrays() ;
  for (i = 0 ;  i < narrays ;  i++)
  {
    ncomp[i] = OutPolydata->GetPointData()->GetArray(i)->GetNumberOfComponents() ;
    dtype[i] = OutPolydata->GetPointData()->GetArray(i)->GetDataType() ;
  }

  // interpolate the scalars for every point
  for (i = 0 ;  i < PointsMapping.size() ;  i++)
  {
    vtkIdType idIn = PointsMapping[i] ;
  
    // loop over all scalar arrays
    for (j = 0 ;  j < narrays ;  j++)
    {
      // get the scalars for the input points
      // the output point corresponds to only one input point - just copy the scalars
      InPolydata->GetPointData()->GetArray(j)->GetTuple(idIn, tuple0) ;
      OutPolydata->GetPointData()->GetArray(j)->SetTuple(i, tuple0) ;
    }
  }

  
  if (InPolydata->GetPointData()->GetScalars())
  {
    const char* scalarName = InPolydata->GetPointData()->GetScalars()->GetName();
    OutPolydata->GetPointData()->SetActiveScalars(scalarName);
  }
}




//------------------------------------------------------------------------------
// Create the polydata slice
// There must have been a call to UnstructGrid->BuildLinks() for this to work
void vtkALBAPointCloudCutter::CreateSlice()
{
  //update local cut coordinates
	CalculateLocalCutCoord();

	// compute the intersection of the cutting 
  SlicePoints();

  // interpolate the scalars
  TransferScalars() ;
}

//------------------------------------------------------------------------------
// print self
void vtkALBAPointCloudCutter::PrintSelf(ostream& os, vtkIndent indent)

{
  int i, j, ni ;

  // print cutting plane
  os << indent << "cutting plane..." << std::endl ;
  double *po = CutTranformedOrigin ;
  double *pn = CutTranformedNormal ;
  os << indent << "origin: " << po[0] << " " << po[1] << " " << po[2] << std::endl ;
  os << indent << "normal: " << pn[0] << " " << pn[1] << " " << pn[2] << std::endl ;
  os << indent << std::endl ;

  
  // print output polydata
  os << "polydata..." << std::endl ;
  OutPolydata->PrintSelf(os, indent) ;
}

void vtkALBAPointCloudCutter::ToRotationMatrix(vtkMatrix4x4 *matrix)
{
	//remove translation components
	matrix->SetElement(0,3,0);
	matrix->SetElement(1,3,0);
	matrix->SetElement(2,3,0);
}

//------------------------------------------------------------------------------
void vtkALBAPointCloudCutter::CalculateLocalCutCoord()
{
  if (CutFunction == NULL)
  {
    CutTranformedNormal[0] = CutTranformedNormal[1] = CutTranformedNormal[2] = CutTranformedOrigin[0] = CutTranformedOrigin[1] = CutTranformedOrigin[2] = 0;
    return;
  }
	//Getting Mesh coordinates
	vtkLinearTransform *trans = vtkLinearTransform::SafeDownCast(CutFunction->GetTransform());
	if (trans)
	{
		double origin[4], normal[4];
		vtkMatrix4x4 *inverse;
		vtkNEW(inverse);
		//Getting slicing transform matrix
		trans->GetMatrix(inverse);
		//Inverting matrix to obtain slice to mesh transform matrix
		inverse->Invert();

		//vtkMatrix4x4 MultiplyPoint requires a 4d vector with last element set to 1
		origin[3] = normal[3] = 1;
		CutFunction->GetNormal(normal);
		CutFunction->GetOrigin(origin);

		//Getting mesh local origin
		inverse->MultiplyPoint(origin, CutTranformedOrigin);

		//Isolate rotation matrix
		ToRotationMatrix(inverse);

		//Using rotation matrix to calculate mesh local normal
		inverse->MultiplyPoint(normal, CutTranformedNormal);
	}
	else
	{
		//No Transform found -> copy origin and normal
		CutFunction->GetNormal(CutTranformedNormal);
		CutFunction->GetOrigin(CutTranformedOrigin);
	}
}
